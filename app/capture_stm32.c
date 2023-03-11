/**
 * 
 * */

#include "dso.h"
#include "capture.h"
#include "board.h"

#define CAPTURE_TIMER_CLOCK         2000000UL   // Timer clock must be double due to ADC rising edge trigger
#define CAPTURE_MIN_SAMPLE_TIME     2           // 2us

static uint8_t done, triggered;
static uint32_t trigger, edge;

/**
 *
 * */
#define TIM_CCMR1_CC1S_TI1 (1<<0)
#define TIM_CCMR1_CC1S_TI2 (2<<0)
#define TIM_CCMR1_CC1S_TRC (3<<0)

#define TIM_CCMR1_CC2S_TI1 (1<<8)
#define TIM_CCMR1_CC2S_TI2 (2<<8)
#define TIM_CCMR1_CC2S_TRC (3<<8)

// Input mode bits
#define TIM_CCMR1_IC1PSC_2E (1<<2)
#define TIM_CCMR1_IC1PSC_4E (2<<2)
#define TIM_CCMR1_IC1PSC_8E (3<<2)

#define TIMxCCMR1_IC2PSC_2E (1<<10)
#define TIMxCCMR1_IC2PSC_4E (2<<10)
#define TIMxCCMR1_IC2PSC_8E (3<<10)

// Output mode bits
#define TIM_CCMR1_OC1M_OC1REF_HIGH   (1<<4)
#define TIM_CCMR1_OC1M_OC1REF_LOW    (2<<4)
#define TIM_CCMR1_OC1M_OC1REF_TOGGLE (3<<4)
#define TIM_CCMR1_OC1M_PWM_M1        (6<<4)
#define TIM_CCMR1_OC1M_PWM_M2        (7<<4)

#define TIM_CCMR1_OC2M_OC2REF_HIGH   (1<<12)
#define TIM_CCMR1_OC2M_OC2REF_LOW    (2<<12)
#define TIM_CCMR1_OC2M_OC2REF_TOGGLE (3<<12)
#define TIM_CCMR1_OC2M_PWM_M1        (6<<12)
#define TIM_CCMR1_OC2M_PWM_M2        (7<<12)

#define ADC_CR1_FAST_INTERLEAVED     (7<<16)
#define ADC_CR1_INDEPENDENT          (0<<16)
#define ADC_SMPR2_SMP0_(cy)          (cy << 0)


void CAP_Init(void){
    /** 
     * Configure DMA Channel1 to 
     * store ADC conversions on memory
     * */
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;           // Enable DMA1

    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;  // Source address ADC1 Data Register
    //DMA1_Channel1->CMAR = (uint32_t)dst;        // Destination address memory
    DMA1_Channel1->CCR =
                        DMA_CCR_PL |        // Highest priority
                        DMA_CCR_MSIZE_0 |   // 16bit Dst size
                        DMA_CCR_PSIZE_0 |   // 16bit src size
                        DMA_CCR_MINC |      // increment memory pointer after transference
                        DMA_CCR_TCIE;       // Enable end of transfer interrupt

    /** 
     * Configure ADC 1 to start a conversion on rising edge of OC4REF signal
     * Conversion is performed on a single channel
     * */
    RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;    // Enable Adc1
    RCC->APB2RSTR |= RCC_APB2ENR_ADC1EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC1EN;

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_ADCPRE) |
                (2 << RCC_CFGR_ADCPRE_Pos); // ADC clock source = PCLK /6

    ADC1->CR1 = ADC_CR1_INDEPENDENT;        // Dual ADC not ideal since 2nd conversion is 7 clocks after first
    ADC1->CR2 = ADC_CR2_ADON |              // Turn on ADC1
                ADC_CR2_EXTTRIG |           // Only the rising edge of external signal can start the conversion
                ADC_CR2_EXTSEL_2 |          // Select Tim4_CH4 as Trigger source
                ADC_CR2_EXTSEL_0 |          //
                ADC_CR2_DMA;                // Enable DMA Request
             
#if 0
    /* Configure ADC2 */
    ADC2->CR2 = ADC_CR2_EXTTRIG  |
    		    ADC_CR2_EXTSEL_2 |          // Select Ext for dual conversion
				ADC_CR2_EXTSEL_1 |
    		    ADC_CR2_EXTSEL_0 |
				ADC_CR2_ADON;
#endif

    ADC2->SQR3 = 0;							// Select AN channel 0
    ADC1->SMPR2 = ADC_SMPR2_SMP0_(0);       // CH0 sample time: 1.5 cycles

    //NVIC_SetPriority(DMA1_Channel1_IRQn, 0); // Highest priority
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    /**
     *  Configure Timer 1 as counter of Timer 4 events (counts number of ADC triggers),
     *  When count reaches half of samples, it enables TI1 signal event capture
     *  through interrupt.
     *  Upon external trigger level reached, TI1 signal is activated via PA8 pin and 
     *  count is captured to CCR1 thus registering the offset on which the trigger ocurred.
     * */
    RCC->APB2ENR  |= RCC_APB2ENR_TIM1EN;    // Enable Timer 1
    RCC->APB2RSTR |= RCC_APB2ENR_TIM1EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_TIM1EN;

    TIM1->SMCR =                            // Slave mode configuration
            (3<<TIM_SMCR_TS_Pos) |          // TS = 3 => ITR3 (Internal Trigger 3), ITR3 = TIM4_TRGO
            (7<<TIM_SMCR_SMS_Pos);          // SMS = 7 => External clock mode 1, TRGI as clock on rising edge
    TIM1->CCMR1 =                           // Compare and Capture modes configuration
            TIM_CCMR1_OC2M_OC2REF_HIGH |    // Set OC2REF on Match
            TIM_CCMR1_CC1S_TI1;             // Capture on TI1
    TIM1->PSC = 0;                          // /1 We want to count TIM4 events
    TIM1->ARR = 0xFFFF;                     // allow max count for capture

    /** 
     * Configure Timer 4 as time base
     * Each time Timer 4 matches CCR4 it toggles OC4REF signal.
     * ADC conversion is triggered on rising edge of OC4REF
     * */
    RCC->APB1ENR  |= RCC_APB1ENR_TIM4EN;    // Enable Timer 4
    RCC->APB1RSTR |= RCC_APB1ENR_TIM4EN;    // Reset timer registers
    RCC->APB1RSTR &= ~RCC_APB1ENR_TIM4EN;

    TIM4->CCMR2 = (3<<TIM_CCMR2_OC4M_Pos);  // OC4M = 3 => OC4REF toggles on CCR4 match
    TIM4->CR2 = (7<<TIM_CR2_MMS_Pos);       // MMS = 7 => TRGO = OC4REF
    TIM4->SMCR = TIM_SMCR_MSM;              // Synchronize with slaves
    TIM4->CCER = TIM_CCER_CC4E;             // Enable compare with CCR4
    TIM4->PSC = (SystemCoreClock/CAPTURE_TIMER_CLOCK) - 1; // Set Timer clock

    /**
     *  Configure PA8 as floating GPI to trigger timer1 capture
     * */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;     // Enable GPIOA

    GPIOA->CRH &= ~(0x0f<<00);
    GPIOA->CRH |= (0x04<<0);				// PA8 Floating input

    NVIC_EnableIRQ(TIM1_CC_IRQn);
    TIM1->CR1 = 1;

    done = true;
}

/**
 * @brief 
 * 
 * @param sr    2 to 2000 => 10uS to 50mS
 */
void CAP_SetSampleRate(uint32_t sr){
    if(sr < CAPTURE_MIN_SAMPLE_TIME){
        sr = CAPTURE_MIN_SAMPLE_TIME;
    }
    TIM4->ARR =  sr - 1;
    TIM4->CCR4 = sr - 1;
}

void CAP_Start(int16_t *dst, uint16_t size){
    DMA1_Channel1->CNDTR = size;
    DMA1_Channel1->CMAR = (uint32_t)dst;
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    TIM1->CCR2 = size>>2;                   // Enable trigger level capture when half of samples has been acquired.
    TIM1->CCR1 = 0;
    TIM1->CCER = TIM_CCER_CC2E;             // Capture is enable on CCR2 match
    TIM1->DIER = TIM_DIER_CC2IE;
    TIM1->SR = 0;
    TIM1->CNT = 0;

    TIM4->CNT = 0;

    done = false;
    triggered = false;

    TIM1->CR1  = TIM_CR1_CEN;
    TIM4->CR1 |= TIM_CR1_CEN;
}

void CAP_Stop(void){
    TIM4->CR1 &= ~TIM_CR1_CEN;          // Stop Timer
    DMA1_Channel1->CCR &= ~DMA_CCR_EN;  // Disable DMA
    done = true;
}

uint8_t CAP_IsEnded(void){
    return done;
}

uint8_t CAP_Triggered(void){
    return triggered;
}

void CAP_SetTriggerEdge(uint8_t rising){
    edge = (rising == 0) ? TIM_CCER_CC1E : TIM_CCER_CC1E | TIM_CCER_CC1P;
}

/**
 * @brief Get offset in samples of trigger position
 * 
 * @return uint16_t 0: no trigger, trigger position
 */
uint16_t CAP_GetTriggerOffset(){
    return trigger >> 2;
}

/**
 * Trigger interrupt handler
 * */
void TIM1_CC_IRQHandler(void){
    uint32_t sr = TIM1->SR;
    TIM1->SR = 0;   

    if(sr & TIM_SR_CC2IF){
        // half of samples have been acquired, 
        // enable trigger level capture
        trigger = TIM1->CNT;
        TIM1->CCER = edge;                  // Configure edge polarity ad enable capture on TI1
        TIM1->CR1 |= TIM_CR1_OPM;           // On capture event, stop timer
        TIM1->DIER = TIM_DIER_CC1IE;
    }
    // Trigger acquired, stop counting
    else if(sr & TIM_SR_CC1IF){
        trigger = TIM1->CCR1;               // Save trigger index
        triggered = true;
    }
    
}

/**
 * DMA transfer end (all data) interrupt handler
 * */
void DMA1_Channel1_IRQHandler(void){

    if(DMA1->ISR & DMA_ISR_TCIF1){
        CAP_Stop();
        ADC1->SR = OFF;                  // Clear ADC1 Flags        
    }
    DMA1->IFCR |= DMA_IFCR_CGIF1;  // Clear DMA Flags TODO: ADD DMA Error handling ?
}
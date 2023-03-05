/**
 * 
 * */

#include "dso.h"
#include "capture.h"
#include <board.h>

#define CAPTURE_TIMER_CLOCK 2000000UL   // Timer clock must me double due to ADC rising edge trigger
#define CAPTURE_MIN_SAMPLE_TIME 2       // 2us

uint8_t done, triggered, flag;
uint16_t ticks, samples;
uint16_t trigger;

/**
 * Trigger interrupt handler
 * */
void TIM1_CC_IRQHandler(void){
    // half of samples already acquired, enable trigger
    if(TIM1->SR & TIM_SR_CC2IF){
        TIM1->CR1 |= TIM_CR1_OPM;                       // On capture event, stop timer
        TIM1->CCER = TIM_CCER_CC1E | TIM_CCER_CC1P;     // Enable capture on TI1
    }
    // Trigger acquired, stop capture
    else if(TIM1->SR & TIM_SR_CC1IF){
        TIM1->CR1 = 0;                      // Disable Timer
        TIM1->CCER = 0;                     // Disable compare/capture
        trigger = TIM1->CCR1;               // Get trigger index
        triggered = true;
    }
    TIM1->SR = OFF; // TODO : handle this correctly
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

/**
 *
 * */
#define TIMxCCMR1_CC1S_TI1 (1<<0)
#define TIMxCCMR1_CC1S_TI2 (2<<0)
#define TIMxCCMR1_CC1S_TRC (3<<0)

#define TIMxCCMR1_CC2S_TI1 (1<<8)
#define TIMxCCMR1_CC2S_TI2 (2<<8)
#define TIMxCCMR1_CC2S_TRC (3<<8)

// Input mode bits
#define TIMxCCMR1_IC1PSC_2E (1<<2)
#define TIMxCCMR1_IC1PSC_4E (2<<2)
#define TIMxCCMR1_IC1PSC_8E (3<<2)

#define TIMxCCMR1_IC2PSC_2E (1<<10)
#define TIMxCCMR1_IC2PSC_4E (2<<10)
#define TIMxCCMR1_IC2PSC_8E (3<<10)

// Output mode bits
#define TIMxCCMR1_OC1FE              (1<<2)
#define TIMxCCMR1_OC1PE              (1<<3)
#define TIMxCCMR1_OC1M_OC1REF_HIGH   (1<<4)
#define TIMxCCMR1_OC1M_OC1REF_LOW    (2<<4)
#define TIMxCCMR1_OC1M_OC1REF_TOGGLE (3<<4)
#define TIMxCCMR1_OC1M_PWM_M1        (6<<4)
#define TIMxCCMR1_OC1M_PWM_M2        (7<<4)
#define TIMxCCMR1_OC1CE              (1<<7)

#define TIMxCCMR1_OC2FE              (1<<10)
#define TIMxCCMR1_OC2PE              (1<<11)
#define TIMxCCMR1_OC2M_OC2REF_HIGH   (1<<12)
#define TIMxCCMR1_OC2M_OC2REF_LOW    (2<<12)
#define TIMxCCMR1_OC2M_OC2REF_TOGGLE (3<<12)
#define TIMxCCMR1_OC2M_PWM_M1        (6<<12)
#define TIMxCCMR1_OC2M_PWM_M2        (7<<12)
#define TIMxCCMR1_OC2CE              (1<<15)

#define ADC_CR1_FAST_INTERLEAVED     (7<<16)
#define ADC_CR1_INDEPENDENT          (0<<16)
#define ADC_SMPR2_SMP0_(cy)          (cy << 0)


void CAP_Init(void){
    /* Configure DMA Channel1*/
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;           // Enable DMA1
    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;  // Source address ADC1 Data Register
    //DMA1_Channel1->CMAR = (uint32_t)dst;        // Destination address memory
    DMA1_Channel1->CCR =
            DMA_CCR_PL |        // Highest priority
            DMA_CCR_MSIZE_0 |   // 16bit Dst size
            DMA_CCR_PSIZE_0 |   // 16bit src size
            DMA_CCR_MINC |      // increment memory pointer after transference
            DMA_CCR_TCIE;       // Enable end of transfer interrupt

    /* Configure Timer 4 */
    RCC->APB1ENR  |= RCC_APB1ENR_TIM4EN;    // Enable Timer 4
    RCC->APB1RSTR |= RCC_APB1ENR_TIM4EN;    // Reset timer registers
    RCC->APB1RSTR &= ~RCC_APB1ENR_TIM4EN;

    TIM4->CR2 = (7<<TIM_CR2_MMS_Pos);       // OC4REF signal is used as trigger output (TRGO)
    TIM4->CCMR2 = (3<<TIM_CCMR2_OC4M_Pos);  // Toggle OC4REF on match
    TIM4->CCER = TIM_CCER_CC4E;             // Enable compare output for CCR4
    TIM4->PSC = (SystemCoreClock/CAPTURE_TIMER_CLOCK) - 1; // Set Timer clock

    /* Configure ADC 1 */
    RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;     // Enable Adc1
    RCC->APB2RSTR |= RCC_APB2ENR_ADC1EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC1EN;

    ADC1->CR1 = ADC_CR1_INDEPENDENT;        // Dual ADC not ideal since 2nd conversion is 7 clocks after first
    ADC1->CR2 = ADC_CR2_ADON |              // Turn on ADC1
             ADC_CR2_EXTTRIG |              // Only the rising edge of external signal can start the conversion
            ADC_CR2_EXTSEL_2 |              // Select Tim4_CH4 as Trigger source
            ADC_CR2_EXTSEL_0 |              //
            ADC_CR2_DMA;                    // Enable DMA Request

    ADC1->SMPR2 = ADC_SMPR2_SMP0_(0);       // CH0 sample time: 1.5 cycles
                  
#if 0
    /* Configure ADC2 */
    ADC2->CR2 = ADC_CR2_EXTTRIG  |
    		    ADC_CR2_EXTSEL_2 |          // Select Ext for dual conversion
				ADC_CR2_EXTSEL_1 |
    		    ADC_CR2_EXTSEL_0 |
				ADC_CR2_ADON;
    ADC2->SQR3 = 0;							// Select AN channel 0
#endif
    NVIC_SetPriority(DMA1_Channel1_IRQn, 0); // Highest priority
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);


    /* Configure PA8 to trigger timer1 capture */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;     // Enable GPIOA
    GPIOA->CRH &= ~(0x0f<<00);
    GPIOA->CRH |= (0x04<<0);				// PA8 Floating input


    /* Configure Timer 1 as slave */
    RCC->APB2ENR  |= RCC_APB2ENR_TIM1EN;    // Enable Timer 1
    RCC->APB2RSTR |= RCC_APB2ENR_TIM1EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_TIM1EN;
    TIM1->SMCR =
            (3<<TIM_SMCR_TS_Pos) |          // Internal Trigger 3 (TIM4_TRGO)
            (7<<TIM_SMCR_SMS_Pos);          // External clock mode 1
    TIM1->PSC = 1;                          // We want to count TIM4 events
    TIM1->CCMR1 =  TIMxCCMR1_OC2M_OC2REF_HIGH | TIMxCCMR1_CC1S_TI1;      // Capture to CCR1 on TI1, Interrupt on Match with CCR2
    TIM1->DIER = TIM_DIER_CC1IE | TIM_DIER_CC2IE;            // Enable capture interrupt
    TIM1->ARR   = 0xFFFF;                   // allow max count for capture

    NVIC_EnableIRQ(TIM1_CC_IRQn);
    TIM1->CR1 = 1;

    done = true;
}

/**
 *
 */
void CAP_SetSampleRate(uint32_t sr){
    if(sr < CAPTURE_MIN_SAMPLE_TIME){
        sr = CAPTURE_MIN_SAMPLE_TIME;
    }
    TIM4->ARR =  sr - 1;
    TIM4->CCR4 = sr - 1;
}

void CAP_Start(int16_t *dst, uint16_t size){
    done = false;
    triggered = false;

    DMA1_Channel1->CNDTR = size/2;
    DMA1_Channel1->CMAR = (uint32_t)dst;
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    TIM1->CCR2 = size>>1;                   // Enable Trigger capture
    TIM1->CCER  = TIM_CCER_CC2E;            // after half samples made
    TIM1->SR = 0;
    TIM1->CNT = 0;
    TIM4->CNT = 0;

    TIM1->CR1 |= TIM_CR1_CEN;
    TIM4->CR1 |= TIM_CR1_CEN;
}

void CAP_Stop(void){
    done = true;
    TIM4->CR1 &= ~TIM_CR1_CEN;          // Stop Timer
    DMA1_Channel1->CCR &= ~DMA_CCR_EN;  // Disable DMA

}

uint8_t CAP_IsEnded(void){
    return done;
}

uint8_t CAP_Triggered(void){
    return triggered;
}

uint16_t CAP_GetEdge(int16_t *buf, uint16_t size, int16_t level){
uint16_t index = 1024;

    while(index < DSO_MAX_SAMPLES && buf[index] > level) index++;
    while(index < DSO_MAX_SAMPLES && buf[index] < level) index++;

    return index;
}

uint16_t CAP_GetTriggerIndex(){
    if(triggered)
        return trigger;
    return false;
}


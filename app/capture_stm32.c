/**
 * 
 * */

#include "dso.h"
#include "capture.h"
#include <common.h>

#define CAPTURE_TIMER_CLOCK 2000000UL   // Timer clock must me double due to ADC rising edge trigger
#define CAPTURE_MIN_SAMPLE_TIME 2       // 2us

uint8_t done, triggered, flag;
uint16_t ticks, samples;
uint16_t trigger;

void TIM1_CC_IRQHandler(void){
    if(TIM1->SR & TIM_SR_CC2IF){
        TIM1->CR1 |= TIM_CR1_OPM;           // Stop timer on capture
        TIM1->CCER = TIM_CCER_CC1E | TIM_CCER_CC1P;     // Enable capture on TI1
    }else
        if(TIM1->SR & TIM_SR_CC1IF){
        TIM1->CR1 = 0;                      // Disable Timer
        TIM1->CCER = 0;                     // Disable compare/capture
        trigger = TIM1->CCR1;               // Get trigger index
        triggered = ON;
    }
    TIM1->SR = OFF; // TODO : handle this correctly
}

void DMA1_Channel1_IRQHandler(void){
    CAP_Stop();
    DMA1->IFCR |= DMA_IFCR_CGIF1;  // Clear DMA Flags TODO: ADD DMA Error handling ?
    ADC1->SR = OFF;                  // Clear ADC1 Flags
    done = ON;
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
    ADC1->CR2 = ADC_CR2_ADON;               // Turn on ADC1
    ADC1->CR2 |=
            ADC_CR2_EXTTRIG |               // Only the rising edge of external signal can start the conversion
            ADC_CR2_EXTSEL_2 |              // Select Tim4_CH4 as Trigger source
            ADC_CR2_EXTSEL_0 |              //
            ADC_CR2_DMA;                    // Enable DMA Request

    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0); // Highest priority
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);


    /* Configure PA8 to trigger timer1 capture */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;     // Enable GPIOA
    GPIOA->CRH &= ~(0x0f<<00);
    GPIOA->CRH |= (0x04<<0);


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

    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
    TIM1->CR1 = 1;

    done = 1;
}

/**
 *
 */
void CAP_SetSampleRate(uint32_t tb_us){
    tb_us = tb_us/DSO_SAMPLES_PER_DIVISIO;
    if(tb_us < CAPTURE_MIN_SAMPLE_TIME){
        tb_us = CAPTURE_MIN_SAMPLE_TIME;
    }
    TIM4->ARR =  tb_us - 1;
    TIM4->CCR4 = tb_us - 1;
}

void CAP_Start(int16_t *dst, uint16_t size){
    done = OFF;
    triggered = OFF;
    DMA1_Channel1->CNDTR = size;
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
    done = 1;
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
    return OFF;
}

// Testing timebase
void CAP_OutputTimeBase(void){
    /* Configure Timer 4 */
        RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;     // Enable Timer 4
        TIM4->CR1 = 0;                          // Stop timer
        TIM4->CCMR2 = TIM_CCMR2_OC4M;           // Select PWM mode 2
        TIM4->CCER = TIM_CCER_CC4E;             // enable compare output for CCR4
        TIM4->PSC = (SystemCoreClock/CAPTURE_TIMER_CLOCK); // 4MHz clk => Max Ts = 0.250ns

        /* Configure PB9 for alternate function */
        GPIOB->CRH &= ~(0x0f<<4);
        GPIOB->CRH &= ~(0x22<<4);

}

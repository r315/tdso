/*
 * softpower.c
 *
 *  Created on: 15/08/2017
 *      Author: Hugo
 */

#include "board.h"
#include "power.h"

#define VREFINT_CHANNEL	17
#define VREFINT_VALUE 	1200.0f

#if defined(BOARD_TDSO)
extern ADC_HandleTypeDef hadc2;
static float adc_res;
static float vdiv_bat;
/**
 * @brief calibrate ADC and get resolution based 
 * on 1.20V internal reference
 * */
void POWER_CalibrateADC(void)
{
    uint32_t bsqr3;

    RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;        // Enable and reset ADC1
    RCC->APB2RSTR |= RCC_APB2ENR_ADC1EN;		// VREFINT and Tem. sensor
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC1EN;		// are only available on ADC1

    ADC1->CR2 = ADC_CR2_EXTSEL | ADC_CR2_EXTTRIG | ADC_CR2_ADON;  // Enable ADC
    DelayMs(20);
    ADC1->SMPR1 = (3 << 21); // Sample time for VERFINT: 3 => 28.5 cycles.

    // Perform ADC calibration
    ADC1->CR2 |= ADC_CR2_CAL;
    while(ADC1->CR2 & ADC_CR2_CAL){               
        asm volatile("nop");
    }

    // Perform ADC conversion in order to get resolution
    bsqr3 = ADC1->SQR3;
    // Select Internal reference channel
    ADC1->SQR3 = VREFINT_CHANNEL;
    // wake up Vrefint 
    ADC1->CR2 |= ADC_CR2_TSVREFE;
    DelayMs(5);
    // Start ADC conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while(!(ADC1->SR & ADC_SR_EOC)){
        asm volatile("nop");
    }
    // Compute resolution
    adc_res = VREFINT_VALUE / ADC1->DR;
    // power down VREFINT
    ADC1->CR2 &= ~ADC_CR2_TSVREFE;
    ADC1->SQR3 = bsqr3;
}

uint16_t POWER_GetPwrBtnValue(void){
    uint32_t adcResult = 0;
    ADC2->SQR3 = 2;  //TODO...................
    HAL_ADC_Start(&hadc2);

    HAL_ADC_PollForConversion(&hadc2, 100);

    adcResult = HAL_ADC_GetValue(&hadc2);

    HAL_ADC_Stop(&hadc2);
      /* TODO: Remove adc2 reconfiguration workaround due to power button reading */
        ADC2->CR2 = ADC_CR2_EXTTRIG  |
                    ADC_CR2_EXTSEL_2 |          // Select Ext for dual conversion
                    ADC_CR2_EXTSEL_1 |
                    ADC_CR2_EXTSEL_0 |
                    ADC_CR2_ADON;
        ADC2->SQR3 = 0;							    // Select AN channel 0
    return adcResult;
}

uint8_t POWER_isPwrBtnPressed(void){
    return POWER_GetPwrBtnValue() > SOFTPOWER_PRESSED_VALUE;
}

uint8_t POWER_EnterStandby(void){
    static uint8_t state = OFF;
    static uint32_t ticks = 0;
    uint8_t pressed = POWER_isPwrBtnPressed();

    switch(state){
        case OFF:
            if(!pressed)
                break;
            state = ON;
            ticks = GetTick();
            break;

        case ON:
            if(!pressed){
                state = OFF;
                break;
            }
            if(ElapsedTicks(ticks) > SOFTPOWER_STANDBY_TICKS){
                return ON;
            }
            break;
    }
    return OFF;
}

void POWER_PowerOff(void){
    if(SOFTPOWER_PIN < GPIO_PIN_8){
        SOFTPOWER_PORT->CRL &= ~(0x0f<<(SOFTPOWER_PIN<<1));
        SOFTPOWER_PORT->CRL |= (0x06<<(SOFTPOWER_PIN<<1));  // select output open-drain
    }else{
        SOFTPOWER_PORT->CRH &= ~(0x0f<<(SOFTPOWER_PIN<<1));
        SOFTPOWER_PORT->CRH |= (0x06<<(SOFTPOWER_PIN<<1));
    }
    SOFTPOWER_PORT->BRR = SOFTPOWER_PIN;
    while(1);
}

uint16_t POWER_GetBatVoltage(void)
{
    uint32_t adcResult = 0;
    ADC2->SQR3 = 1;
    HAL_ADC_Start(&hadc2);

    HAL_ADC_PollForConversion(&hadc2, 100);

    adcResult = HAL_ADC_GetValue(&hadc2);

    HAL_ADC_Stop(&hadc2);
      /* TODO: Remove adc2 reconfiguration workaround due to power button reading */
        ADC2->CR2 = ADC_CR2_EXTTRIG  |
                    ADC_CR2_EXTSEL_2 |          // Select Ext for dual conversion
                    ADC_CR2_EXTSEL_1 |
                    ADC_CR2_EXTSEL_0 |
                    ADC_CR2_ADON;
        ADC2->SQR3 = 0;							    // Select AN channel 0

    vdiv_bat = 100.0f / (100 + 27);
    
    return (uint16_t)(adcResult * (adc_res / vdiv_bat));
}
#else
uint16_t POWER_GetPwrBtnValue(void){ return 0; }
uint8_t POWER_isPwrBtnPressed(void){ return false;}
void POWER_PowerOff(void){ }
uint8_t POWER_EnterStandby(void){return false;}
uint16_t POWER_GetBatVoltage(void) { return 0; }
#endif

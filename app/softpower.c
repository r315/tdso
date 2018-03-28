/*
 * softpower.c
 *
 *  Created on: 15/08/2017
 *      Author: Hugo
 */

#include <common.h>
#include <softpower.h>

#if defined(__TDSO__)
extern ADC_HandleTypeDef hadc2;

uint16_t SOFTPOWER_Read(void){
	uint32_t adcResult = 0;
	HAL_ADC_Start(&hadc2);

	HAL_ADC_PollForConversion(&hadc2, 100);

	adcResult = HAL_ADC_GetValue(&hadc2);

	HAL_ADC_Stop(&hadc2);
	return adcResult;
}

uint8_t SOFTPOWER_Pressed(void){
return SOFTPOWER_Read() > SOFTPOWER_PRESSED_VALUE;
}

void SOFTPOWER_PowerOff(void){
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

#else
uint16_t SOFTPOWER_Read(void){ return 0; }
uint8_t SOFTPOWER_Pressed(void){ return 0;}
void SOFTPOWER_PowerOff(void){ }
#endif

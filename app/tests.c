

#include <display.h>
#include <softpower.h>
#include <button.h>

#include "led.h"
#include "common.h"
#include "control.h"

static uint16_t vscales[] = {10, 20, 50, 100, 200, 500, 1000 , 2000, 5000};  //mV values

void TEST_BlinkLed(uint8_t times){
    while(times--){
        LED_OFF;
        DelayMs(80);
        LED_ON;
        DelayMs(10);
    }
}

/*
 * @brief test softpower
 */
void TEST_Buttons(void){
uint32_t time = GetTicks();
char *btn = "";
	while(1){
		uint32_t event = BUTTON_Read();
		uint16_t pbutton = SOFTPOWER_Read();

		if(pbutton > SOFTPOWER_PRESSED_VALUE){
					if(ElapsedTicks(time) > 500){
						DISPLAY_printf("\nGoing to power down in 1S");
						DelayMs(1000);
						SOFTPOWER_PowerOff();
					}
		}
		else{
		time = GetTicks();

		if(event == BUTTON_PRESSED){
			switch(BUTTON_GetValue()){
			case BUTTON_LEFT:
				btn = " LEFT ";
				break;
			case BUTTON_RIGHT:
				btn = " RIGHT ";
				break;
			case BUTTON_CENTER:
				btn = " CENTER";
				break;
			}
	}else{
			if(event == BUTTON_RELEASED){
				btn = "       ";
			}
		}
		}
		DelayMs(100);
		DISPLAY_Goto(0,0);
		DISPLAY_printf("Power Button Read Value %u\n", pbutton);
		DISPLAY_printf("%s", btn);
	}
}

//--------------------------------------------------------
void TEST_FrontendSelector(uint8_t *selector){
int8_t step = 0;
    BUTTON_Read();
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        switch(BUTTON_GetValue()){
            case BUTTON_LEFT:
                if((*selector - 1) > -1)
                    step = -1;
                break;
            case BUTTON_RIGHT:
                if((*selector + 1) < 8 )
                    step = 1;
                break;
            default : return;
        }
       *selector += step;
       CONTROL_SelectScale(*selector);
       DISPLAY_printf("%umV\n",vscales[*selector]);
    }
}
//--------------------------------------------------------
uint8_t cdone = 0, idx = 6;

#if defined(__TDSO__)
uint16_t buf[2048];

void __DMA1_Channel1_IRQHandler(void){
    DMA1_Channel1->CCR &= ~DMA_CCR_EN;
    DMA1->IFCR |= DMA_IFCR_CGIF1;  // TODO: ADD DMA Error handling ?
    cdone = 1;
}

void TEST_Config_DMA(void){
    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;
    DMA1_Channel1->CMAR = (uint32_t)&buf;
    //DMA1_Channel1->CNDTR = 16;
    DMA1_Channel1->CCR = DMA_CCR_PL | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_MINC | DMA_CCR_TCIE;
    //DMA1_Channel1->CCR |= DMA_CCR_EN;
}

void TEST_Enable_TIM4(uint16_t period){
#define TIM4_CLK 2000000UL

    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;      // Enable Timer 4
    TIM4->CR1 = 0;                           // Stop timer
    //TIM4->CR1 |= TIM_CR1_OPM;                // set one pulse for testing
    TIM4->CCMR2 |= TIM_CCMR2_OC4M;           // PWM mode 2
    TIM4->CCER |= TIM_CCER_CC4E;             // enable compare output for CCR4
    TIM4->PSC = (SystemCoreClock/TIM4_CLK);  // 2MHz clk, 4MHz clk => Max Ts = 0.250ns

    TIM4->ARR =  period<<1;
    TIM4->CCR4 = period;

    /* Configure PB9 for alternate function */
    GPIOB->CRH &= ~(0x0F<<4);
    GPIOB->CRH |=  (0x0A<<4);

}

void TEST_Enable_ADC(void){
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    ADC1->CR2 = ADC_CR2_ADON;
    ADC1->CR2 |= ADC_CR2_EXTTRIG | ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_0 | ADC_CR2_DMA;
}

void TEST_Capture(void){
    DMA1_Channel1->CNDTR = 128;
    DMA1_Channel1->CCR |= DMA_CCR_EN;
    TIM4->CR1 |= 1;
    cdone = 0;
    while(!cdone){
        DelayMs(200);
    }
}
#endif

void TEST_Run(void){
#if defined(__TDSO__)
    TEST_Config_DMA();
    TEST_Enable_ADC();
    TEST_Enable_TIM4(20000);
#endif
    while(1){
        //TEST_Capture();
        TEST_FrontendSelector(&idx);
        DelayMs(20);
    }
}





#include "softpower.h"
#include "board.h"
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
#define SOFT_SWITCH_Y        19
void TEST_SoftPower(void){
static uint32_t time;
uint16_t pbutton = SOFTPOWER_Read();
    if(pbutton > SOFTPOWER_PRESSED_VALUE){
        if(ElapsedTicks(time) > 500){
            DISPLAY_printf("\nGoing to power down in 1S");
            DelayMs(1000);
            SOFTPOWER_PowerOff();
        }
        DelayMs(100);
        DISPLAY_Goto(0,SOFT_SWITCH_Y);
		DISPLAY_printf("AN2: %4u\n", pbutton);		
    }else{
        time = GetTicks();
    }
}

void TEST_Buttons(void){
char *btn = "";
	uint32_t event = BUTTON_Read();
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
	}else if(event == BUTTON_RELEASED){
			btn = "       ";
	}
    DISPLAY_printf("%s", btn);
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
       //CONTROL_SetVscale(*selector);
       DISPLAY_printf("%umV\n",vscales[*selector]);
    }
}

#define SWITCH_BAR_WIDTH    100
#define SWITCH_BAR_HIGHT    20
#define SWITCH_BAR_X        ((LCD_GetWidth()/2) - (SWITCH_BAR_WIDTH / 2))
#define SWITCH_BAR_Y        20
#define SWITCH_BAR_BORDER_COLOR RED
#define SWITCH_BAR_COLOR        BLUE
#define SWITCH_BAR_BACK_COLOR   BLACK

void TEST_MultiSwitchFill(int32_t percent){
    if(percent > 100 || percent < -100){
        return;
    }

    if(percent == 0){
        LCD_FillRect(SWITCH_BAR_X + 1, SWITCH_BAR_Y + 1, SWITCH_BAR_WIDTH - 1, SWITCH_BAR_HIGHT - 1, SWITCH_BAR_BACK_COLOR);
        return;
    }

    percent = (percent * (SWITCH_BAR_WIDTH/2)) / 100;

    if(percent < 0){
        LCD_FillRect(SWITCH_BAR_X + 2 + (SWITCH_BAR_WIDTH/2) + percent, SWITCH_BAR_Y + 2, -percent, SWITCH_BAR_HIGHT - 3, SWITCH_BAR_COLOR);
        LCD_FillRect(SWITCH_BAR_X + 2, SWITCH_BAR_Y + 2, (SWITCH_BAR_WIDTH/2) + percent, SWITCH_BAR_HIGHT - 3, SWITCH_BAR_BACK_COLOR);
    }else{
        LCD_FillRect(SWITCH_BAR_X + 2 + (SWITCH_BAR_WIDTH/2), SWITCH_BAR_Y + 2, percent - 3, SWITCH_BAR_HIGHT - 3, SWITCH_BAR_COLOR);
        LCD_FillRect(SWITCH_BAR_X - 1 + (SWITCH_BAR_WIDTH/2) + percent, SWITCH_BAR_Y + 2, (SWITCH_BAR_WIDTH/2) - percent, SWITCH_BAR_HIGHT - 3, SWITCH_BAR_BACK_COLOR);
    }

}
void TEST_MultiSwitchInit(){
    LCD_Rect(SWITCH_BAR_X, SWITCH_BAR_Y, SWITCH_BAR_WIDTH, SWITCH_BAR_HIGHT, SWITCH_BAR_BORDER_COLOR);
    TEST_MultiSwitchFill(0);
}
void TEST_MultiSwitch(){

 uint32_t val;     

    if(BUTTON_Read() == BUTTON_EMPTY){
        return;
    }

    if(BUTTON_GetEvents() == BUTTON_RELEASED){
        TEST_MultiSwitchFill(0);
        return;
    }

    if(BUTTON_GetEvents() != BUTTON_PRESSED){
        return;
    }
   
    val = BUTTON_GetValue();

    DISPLAY_Goto(0, SWITCH_BAR_Y);
    DISPLAY_printf("%4X", val);

    if(val & BUTTON_LEFT2){
        TEST_MultiSwitchFill(-100);
        return;
    }

    if(val & BUTTON_LEFT){
        TEST_MultiSwitchFill(-50);
        return;
    }

    if(val & BUTTON_RIGHT2){
        TEST_MultiSwitchFill(100);
        return;
    }

    if(val & BUTTON_RIGHT){
        TEST_MultiSwitchFill(50);
        return;
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
    //TEST_Config_DMA();
    //TEST_Enable_ADC();
    //TEST_Enable_TIM4(20000);

    TEST_MultiSwitchInit();
#endif
    while(1){
        //TEST_Capture();
        //TEST_FrontendSelector(&idx);
        TEST_SoftPower();
        TEST_MultiSwitch();
        DelayMs(20);
    }
}


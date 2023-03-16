#include "board.h"
#include "power.h"
#include "control.h"
#include "tdso_test.h"
#include "lib2d.h"
#include "liblcd.h"
#include "libbutton.h"


#define SWITCH_BAR_WIDTH    100
#define SWITCH_BAR_HIGHT    20
#define SWITCH_BAR_X        ((LCD_GetWidth()/2) - (SWITCH_BAR_WIDTH / 2))
#define SWITCH_BAR_Y        20
#define SWITCH_BAR_BORDER_COLOR LCD_RED
#define SWITCH_BAR_COLOR        LCD_BLUE
#define SWITCH_BAR_BACK_COLOR   LCD_BLACK
#define SOFT_SWITCH_Y        6
#define BAT_VOLTAGE_Y        (SOFT_SWITCH_Y + 1)

static uint16_t vscales[] = {10, 20, 50, 100, 200, 500, 1000 , 2000, 5000};  //mV values
uint8_t cdone = 0, idx = 6;

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
void TEST_SoftPower(void){
    static uint32_t time;
    uint16_t pbutton = POWER_GetPwrBtnValue();

    if(pbutton > SOFTPOWER_PRESSED_VALUE){
        if(ElapsedTicks(time) > 500){
            LIB2D_Print("\nGoing to power down in 1S");
            DelayMs(1000);
            POWER_PowerOff();
        }
        DelayMs(100);
        LIB2D_SetCursor(0,SOFT_SWITCH_Y);
		LIB2D_Print("AN2: %4u\n", pbutton);		
    }else{
        time = GetTick();
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
       //CONTROL_SetVscale(*selector);
       LIB2D_Print("%umV\n",vscales[*selector]);
    }
}

void TEST_MultiSwitchSlider(int32_t percent){
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

void TEST_MultiSwitchSliderBorder(uint16_t color){
    LCD_Rect(SWITCH_BAR_X, SWITCH_BAR_Y, SWITCH_BAR_WIDTH, SWITCH_BAR_HIGHT, color);
}

void TEST_MultiSwitch(void){

    uint32_t val;     

    if(BUTTON_Read() == BUTTON_EMPTY){
        LIB2D_SetPos(SWITCH_BAR_X + SWITCH_BAR_WIDTH + 4, SWITCH_BAR_Y);
        LIB2D_Print("%04X", 0);
        return;
    }

    if(BUTTON_GetEvents() == BUTTON_RELEASED){
        TEST_MultiSwitchSlider(0);
        TEST_MultiSwitchSliderBorder(LCD_GREEN);
        return;
    }

    if(BUTTON_GetEvents() != BUTTON_PRESSED){
        return;
    }
   
    val = BUTTON_GetValue();

    LIB2D_SetPos(SWITCH_BAR_X + SWITCH_BAR_WIDTH + 4, SWITCH_BAR_Y);
    LIB2D_Print("%4X", val);

    if(val & BUTTON_LEFT){
        if(val & BUTTON_LEFT2){
            TEST_MultiSwitchSlider(-100);
        }else{
            TEST_MultiSwitchSlider(-50);
        }
        return;
    }else if(val & BUTTON_RIGHT){
        if(val & BUTTON_RIGHT2){
            TEST_MultiSwitchSlider(100);
        }else{
            TEST_MultiSwitchSlider(50);
        }
        return;
    }else if(val & BUTTON_CENTER){
        TEST_MultiSwitchSliderBorder(LCD_RED);
    }
}
//--------------------------------------------------------


#if defined(BOARD_TDSO)
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
void TEST_BatteryVoltage(void)
{
    static uint32_t time;

    if(GetTick() - time > 1000){
        uint16_t vbat = POWER_GetBatVoltage();
        LIB2D_SetCursor(0, BAT_VOLTAGE_Y);
		LIB2D_Print("BAT: %4umV\n", vbat);	
        time = GetTick();
    }
}


void TEST_Run(void){
    LIB2D_Print("Test mode\n");
    TEST_BlinkLed(3);

#if defined(__FPU_USED) && (__FPU_USED == 1U)
    volatile float a = 123.456f;
    volatile float b = 654.321f;
    volatile float c = a * b;
    if(c != 0.0f){
        LIB2D_Print("FPU ok\n");
    }
#endif

#if defined(__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1U)
    volatile uint32_t a = 0x12345678;
    volatile uint32_t b = 0x00010002;
    volatile uint32_t c;

    __ASM ("qadd16 %0, %1, %2" : "=r" (c) : "r" (a), "r" (b) );

    if(c == 0x1235567A){
        LIB2D_Print("DSP ok\n");
    }
#endif

#if defined(BOARD_TDSO)
    //TEST_Config_DMA();
    //TEST_Enable_ADC();
    //TEST_Enable_TIM4(20000);

    TEST_MultiSwitchSlider(0);
    TEST_MultiSwitchSliderBorder(LCD_GREEN);
    POWER_CalibrateADC();
#endif
    
    while(1){
        //TEST_Capture();
        //TEST_FrontendSelector(&idx);
        TEST_SoftPower();
        TEST_MultiSwitch();
        TEST_BatteryVoltage();
        DelayMs(20);
    }
}


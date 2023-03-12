#include <stdio.h>
#include "at32f4xx.h"

#define LED_PORT    GPIOB
#define LED_PIN     GPIO_Pins_3

#define STEP_DELAY_MS	500

static __IO float fac_ms;

void Delay_init()
{
  /*Config Systick*/
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
  float fac_us = (float)SystemCoreClock/(8 * 1000000);
  fac_ms = fac_us * 1000;
}

void Delay_ms(u16 nms)
{
  u32 temp;
  while(nms)
  {
    if(nms > STEP_DELAY_MS)
    {
      SysTick->LOAD = (u32)(STEP_DELAY_MS * fac_ms);
      nms -= STEP_DELAY_MS;
    }
    else
    {
      SysTick->LOAD = (u32)(nms * fac_ms);
      nms = 0;
    }

    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    do
    {
      temp = SysTick->CTRL;
    }while( (temp & 0x01) && !(temp & (1<<16)) );

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0X00;
  }
}

void RCC_Configuration(void)
{
  /* Enable peripheral clocks ------------------------------------------------*/
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);
  
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_GPIOB | RCC_APB2PERIPH_GPIOC, ENABLE);	
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure = {0};
  
  /* Enable DMA1 channel6 IRQ Channel */
  NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Channel6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

int main(void)
{
    RCC_Configuration();
       
    NVIC_Configuration();


    Delay_init();

    GPIO_InitType gpio_init;

    gpio_init.GPIO_Pins = LED_PIN;
    gpio_init.GPIO_MaxSpeed = GPIO_MaxSpeed_2MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_OUT_PP;

    GPIO_Init(LED_PORT, &gpio_init);

    // Remap Jtag pins
    RCC->APB2EN |= RCC_APB2EN_AFIOEN;
    AFIO->MAP = AFIO_MAP_SWJTAG_CONF_JTAGDISABLE;

    for(int i = 1; ;i++){
        GPIO_WriteBit(LED_PORT, LED_PIN, i & 1);
        Delay_ms(250);
    }
    return 0;
}

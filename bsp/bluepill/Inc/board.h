#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#define __BLUEPILL__

#include <lcd.h>
#include <ili9341.h>
#include <button.h>
#include <display.h>

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"

#define GPIO_Set(port, pinmask) port->BSRR = pinmask
#define GPIO_Clr(port, pinmask) port->BRR = pinmask
#define GPIO_Read(port) port->IDR
#define GPIO_Write(port) port->ODR

/**
 * HW symbols for button handling
 * */
#define BUTTON_LEFT  	(1<<15)
#define BUTTON_RIGHT 	(1<<13)
#define BUTTON_CENTER	(1<<14)
#define BUTTON_A        BUTTON_CENTER

#define BUTTON_LEFT2	(BUTTON_LEFT | (1<<9))  
#define BUTTON_RIGHT2	(BUTTON_RIGHT | (1<<12))

#define BUTTON_HW_READ (~GPIOB->IDR & BUTTON_MASK)
#define BUTTON_HW_INIT
#define BUTTON_MASK (BUTTON_LEFT | BUTTON_RIGHT | BUTTON_A | BUTTON_LEFT2 | BUTTON_RIGHT2)

#define GetTicks HAL_GetTick
#define DelayMs(_D) HAL_Delay(_D)
uint32_t ElapsedTicks(uint32_t start);


/**
* @brief Lcd Pin configuration:
*       PA4  CS
*       PB1  RST
*       PB10 C'/D
*       PA7  SI
*       PA5  SCK
*       PB11 Backlight enable
*       PA6  SO
**/

#define LCD_CS   GPIO_PIN_4
#define LCD_CD   GPIO_PIN_10
#define LCD_RST  GPIO_PIN_1
#define LCD_BKL  GPIO_PIN_11

#define LCD_CS0  GPIO_Clr(GPIOA, LCD_CS)
#define LCD_CS1  GPIO_Set(GPIOA, LCD_CS)
#define LCD_CD0  GPIO_Clr(GPIOB, LCD_CD)
#define LCD_CD1  GPIO_Set(GPIOB, LCD_CD)
#define LCD_RST0 GPIO_Clr(GPIOB, LCD_RST)
#define LCD_RST1 GPIO_Set(GPIOB, LCD_RST)
#define LCD_BKL0 GPIO_Clr(GPIOB, LCD_BKL)
#define LCD_BKL1 GPIO_Set(GPIOB, LCD_BKL)

#define LCD_PIN_INIT // handle by HAL_Init

// Bluepill LED 
#define LED_Init() // Handle by HAL init
#define LED GPIO_PIN_13
#define LED_ON  GPIO_Clr(GPIOC, LED)
#define LED_OFF GPIO_Set(GPIOC, LED)

/**
 * Analog Pins
 * */
 
 // VOUT PA0
 // IOUT PA1
 // VLOAD PA2
 // ILOAD PA3
 
 /**
 * PWM Pins
 * */
 // PB0 PWM_VOUT
 // PB1 PWM_IOUT
 // PWM_ILOAD
 // PWM_VLOAD
 
 /**
 * I2C
 * */
 // PB10 SCL
 // PB11 SDA
 
 
 
#define CH1_Pin GPIO_PIN_0
#define CH1_GPIO_Port GPIOA
#define VBAT_Pin GPIO_PIN_1
#define VBAT_GPIO_Port GPIOA
#define POWER_KEY_Pin GPIO_PIN_2
#define POWER_KEY_GPIO_Port GPIOA
#define V_POS_Pin GPIO_PIN_3
#define V_POS_GPIO_Port GPIOA
#define LCD_CS_Pin GPIO_PIN_4
#define LCD_CS_GPIO_Port GPIOA
#define LCD_SCK_Pin GPIO_PIN_5
#define LCD_SCK_GPIO_Port GPIOA
#define LCD_SDO_Pin GPIO_PIN_6
#define LCD_SDO_GPIO_Port GPIOA
#define LCD_SDI_Pin GPIO_PIN_7
#define LCD_SDI_GPIO_Port GPIOA
#define Trigger_IN_Pin GPIO_PIN_8
#define Trigger_IN_GPIO_Port GPIOA
#define TG_LEVEL_Pin GPIO_PIN_15
#define TG_LEVEL_GPIO_Port GPIOA
#define TST_SIG_Pin GPIO_PIN_0
#define TST_SIG_GPIO_Port GPIOB
#define LCD_RST_Pin GPIO_PIN_1
#define LCD_RST_GPIO_Port GPIOB
#define GAIN0_Pin GPIO_PIN_4
#define GAIN1_Pin GPIO_PIN_5
#define LCD_CD_Pin GPIO_PIN_10
#define LCD_CD_GPIO_Port GPIOB
#define LCD_LED_Pin GPIO_PIN_11
#define LCD_LED_GPIO_Port GPIOB
#define T1_Pin GPIO_PIN_9
#define T1_GPIO_Port GPIOB
#define T2_Pin GPIO_PIN_15
#define T2_GPIO_Port GPIOB
#define T3_Pin GPIO_PIN_13
#define T3_GPIO_Port GPIOB
#define T4_Pin GPIO_PIN_12
#define T4_GPIO_Port GPIOB
#define TT_Pin GPIO_PIN_14
#define TT_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_3
#define LED_GPIO_Port GPIOB

void _Error_Handler(char *, int);
void Board_Init(void);
uint16_t SPI_Send(uint16_t data);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
 
#define OFF 0
#define ON 1 
 
#ifdef __cplusplus
}
#endif

#endif

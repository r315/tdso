/**
* @file		gpio.h
* @brief	common functions for io port access
* @version	2.0
* @date		30 Out. 2016
* @modified 25 Mar. 2017
* @08-2017  New Api for STM32
* @author	Hugo Reis
**********************************************************************/

#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>
#include "common.h"

#if defined(__TDO__)
#include <stm32f103xb.h>

#define GPIO_Set(port, pinmask) port->BSRR = pinmask
#define GPIO_Clr(port, pinmask) port->BRR = pinmask
#define GPIO_Read(port) port->IDR
#define GPIO_Write(port) port->ODR

#define GPIO_DEBUG_SWD 			  \
{                                 \
			AFIO->MAPR &= (7<<24);\
			AFIO->MAPR |= (2<<24);\
}
#define GPIO_MAP_PA15_TIM2_CH1 	  \
{                                 \
			AFIO->MAPR &= (3<<8); \
			AFIO->MAPR |= (1<<8); \
}

#elif defined(__BLUEBOARD__) /* __TDO__ */

#define P0_0

#define GPIO_P0  0
#define GPIO_P1  1
#define GPIO_P2  2
#define GPIO_P3  3
#define GPIO_P4  4

#define GPIO_INT_LOW  1
#define GPIO_INT_HIGH 2

#define ON 1
#define OFF 0
#define GPIO_INPUT  0
#define GPIO_OUTPUT 1
#define GPIO_HIGH   1
#define GPIO_LOW    0

#if defined(__USE_CMSIS)
	#include <LPC17xx.h>
	#define GPIO0 LPC_GPIO0
	#define GPIO1 LPC_GPIO1
	#define GPIO2 LPC_GPIO2
	#define GPIO3 LPC_GPIO3
	#define GPIO4 LPC_GPIO4
#else
	#include <lpc1768.h>
#endif  /* __USE_CMSIS */

#define SETPIN GPIO0->FIOSET
#define CLRPIN GPIO0->FIOCLR
#define SETOUTPUT(x) GPIO0->FIODIR |= x
#define SETINPUT(x) GPIO0->FIODIR &= ~(x)

/**
* @brief Set pin of GPIO0 to high level
**/
#define GPIO_Set(pin) SETPIN = (1<<pin)

/**
* @brief Set pin of GPIO0 to low level
**/
#define GPIO_Clr(pin) CLRPIN = (1<<pin)

/**
* @brief Set pin of GPIO0 as output
**/
#define GPIO_SetOutput(pin)  SETOUTPUT((1<<pin))

/**
* @brief Set pin of GPIO0 as output
**/
#define GPIO_SetInput(pin)  SETINPUT((1<<pin))

/**
* @brief Initialyze pin and set his state
**/
void GPIO_ConfigPin(uint8_t port, uint8_t pin, uint8_t dir, uint8_t state);

/**
* @brief Set state of pin
**/
void GPIO_SetState(uint8_t port, uint8_t pin, uint8_t state);

/**
* @brief Get state of pin
**/
uint8_t GPIO_GetState(uint8_t port, uint8_t pin);

/**
* @brief configure external interrupt on pin
* on interrupt handler the int flag for pin should be cleared by seting bin in register LPC_GPIOINT->IOxIntClr;
* @param1   port  [P0,P2]
* @param2   pin   [0-31]
* @param3   level [low, high, both]
*/
void GPIO_SetInt(uint8_t port, uint8_t pin, uint8_t level);

/*
 * @brief Resets oin functions to default (GPIO)
 */
void GPIO_ResetPINSEL(void);

#elif defined(__LPC_H2106__) /* __BLUEBOARD__ */
#include <lpc2106.h>
/*
#define GPIO_Set(n) GPIO0->SET = (1 << n)  // change the state of only one pin
#define GPIO_Clr(n) GPIO0->CLR = (1 << n)  // by giving his number

#define GPIO_SetN(x) GPIO0->SET = x        // change selected pins
#define GPIO_ClrN(x) GPIO0->CLR = x        // by giving the pretended bts

#define GPIO_Write(x) GPIO0->PIN = x		//change all pins
#define GPIO_Read()   GPIO0->PIN

#define GPIO_SetOutput(n) GPIO0->DIR |= (1 << n) //affect single pin
#define GPIO_SetInput(n)  GPIO0->DIR &= ~(1 << n)

#define GPIO_SetOutputN(n) GPIO0->DIR |= n //affect multiple pin

#define GPIO_SetDir(n)  GPIO0->DIR = x //affect all pins
*/

#define GPIO_Set(n) FIO0->SET = (1 << n)        // change the state of only
#define GPIO_Clr(n) FIO0->CLR = (1 << n)        // one pin
#define GPIO_SetOutput(n) FIO0->DIR |= (1 << n) //affect single pin
#define GPIO_SetInput(n)  FIO0->DIR &= ~(1 << n)

#define GPIO_SetN(x) FIO0->SET = x              //change selected pins
#define GPIO_ClrN(x) FIO0->CLR = x
#define GPIO_Write(x) FIO0->PIN = x             //change all pins
#define GPIO_Read() FIO0->PIN
#define GPIO_SetDir(n)  FIO0->DIR = x           //affect all pins
#define GPIO_SetOutputN(n) FIO0->DIR |= n       //affect multiple pin
#define GPIO_SetInputN(n)  FIO0->DIR &= ~n      //affect multiple pin

#else /* __LPC_H2106__ */

#error "Define Board"

#endif /* error */

#endif /* _gpio_h_ */

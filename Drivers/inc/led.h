#ifndef _led_h_
#define _led_h_

#include <gpio.h>

#if defined(__BLUEBOARD__)

#define LED_BLUE (1<<29) //P1.29 (D8)
#define LED_BLUE_ON  GPIO1->FIOSET = LED_BLUE
#define LED_BLUE_OFF GPIO1->FIOCLR = LED_BLUE
#define LED_BLUE_TOGGLE GPIO1->FIOPIN ^= LED_BLUE

#define LED_RED  (1<<18) //P1.18 (D7)
#define LED_RED_OFF GPIO1->FIOSET = LED_RED
#define LED_RED_ON  GPIO1->FIOCLR = LED_RED
#define LED_RED_TOGGLE GPIO1->FIOPIN ^= LED_RED

#define LED_USB (1<<9)  //P2.9 (D1)
#define LED_USB_OFF GPIO2->FIOSET = LED_USB
#define LED_USB_ON  GPIO2->FIOCLR = LED_USB
#define LED_USB_TOGGLE GPIO2->FIOPIN ^= LED_USB

#define LED_Init()                           \
{                                            \
	GPIO1->FIODIR |= LED_BLUE | LED_RED;     \
	GPIO2->FIODIR |= LED_USB;                \
	LED_BLUE_OFF;                            \
	LED_RED_OFF;                             \
	LED_USB_OFF;                             \
}


#elif defined(__TDO__)
/* Led is connected on PB3 in common anode configuration */
/* jag option must be disable to use PB3 as GPIO*/
#define LED_Init() // Handle by HAL init

#define LED GPIO_PIN_3
#define LED_ON  HAL_GPIO_WritePin(GPIOB, LED , GPIO_PIN_RESET)
#define LED_OFF HAL_GPIO_WritePin(GPIOB, LED , GPIO_PIN_SET)

#else
#error "Define board"
#endif  /* Board selection */


#endif /* led.h */

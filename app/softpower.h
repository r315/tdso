/*
 * softpower.h
 *
 *  Created on: 15/08/2017
 *      Author: Hugo
 */

#ifndef SOFTPOWER_H_
#define SOFTPOWER_H_

#include <gpio.h>

#define SOFTPOWER_PRESSED_VALUE 3000

#if defined(__TDSO__)
#define SOFTPOWER_PORT GPIOA
#define SOFTPOWER_PIN  GPIO_PIN_2
#endif

/*
 * @brief get analog value from power button
 */
uint16_t SOFTPOWER_Read(void);

/*
 * @brief Check if power key was pressed
 */
uint8_t SOFTPOWER_Pressed(void);

/*
 * @brief powers down system when running on battery
 */
void SOFTPOWER_PowerOff(void);


#endif /* SOFTPOWER_H_ */

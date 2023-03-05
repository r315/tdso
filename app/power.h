/*
 * softpower.h
 *
 *  Created on: 15/08/2017
 *      Author: Hugo
 */

#ifndef POWER_H_
#define POWER_H_

#include <stdint.h>

#define SOFTPOWER_PRESSED_VALUE 3000
#define SOFTPOWER_STANDBY_TICKS 1500

/*
 * @brief get analog value from power button
 */
uint16_t POWER_GetPwrBtnValue(void);

/*
 * @brief Check if power key was pressed
 */
uint8_t POWER_isPwrBtnPressed(void);

/*
 * @brief powers down system when running on battery
 */
void POWER_PowerOff(void);

/**
 * @brief check if power button was pressed and handles hold for poweroff
 * */
uint8_t POWER_EnterStandby(void);

uint16_t POWER_GetBatVoltage(void);
void POWER_CalibrateADC(void);
#endif /* SOFTPOWER_H_ */

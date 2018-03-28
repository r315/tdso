/*
 * control.h
 *
 *  Created on: Aug 19, 2017
 *      Author: hmr
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#if defined(__TDSO__)
enum {
	NO0 = 1,
	NO1,
	NO2
};

#define CONTROL_MULTIPLIER_Pos 	8
#define CONTROL_SCALE_Pos		   4
#define CONTROL_SWITCH_MASK ((3<<CONTROL_SCALE_Pos) | (3<<CONTROL_MULTIPLIER_Pos)) // 2 bits per switch

#define CONTROL_SCALE_PORT GPIOB
#define CONTROL_SET_SCALE(x) CONTROL_SCALE_PORT->BSRR = ((CONTROL_SWITCH_MASK << 16) | x) // If both BSx and BRx are set, BSx has priority.

#define SCALE_001 NO0
#define SCALE_01  NO1
#define SCALE_1   NO2

#define MULT_1 NO0
#define MULT_2 NO1
#define MULT_5 NO2

#define CONTROL_SWITCH_SCALE(x)       (x<<CONTROL_SCALE_Pos)
#define CONTROL_SWITCH_MULTIPLIER(x)  (x<<CONTROL_MULTIPLIER_Pos)

#define CONTROL_TGPWM_OFFSET 100
#define CONTROL_TGPWM_STEPS 200 // DSO_GRID_H
#define CONTROL_TGPWM_CALIBRATION -21
#define CONTROL_TGLEVEL_MIN (-CONTROL_TGPWM_OFFSET)
#define CONTROL_TGLEVEL_MAX (CONTROL_TGPWM_OFFSET)


#elif defined(__EMU__) || defined(__BLUEBOARD__)

#else
#error config board
#endif

#ifndef null
#define null ((void*)0)
#endif

void CONTROL_SelectScale(uint8_t scale);
void CONTROL_SetTriggerLevel(int16_t level);
void CONTROL_SetTriggerEdge(uint16_t edge);

#endif /* CONTROL_H_ */

/*
 * control.c
 *
 *  Created on: Aug 19, 2017
 *      Author: hmr
 */

#include "dso.h"
#include "common.h"
#include "control.h"
#include "tdso_util.h"

#if defined(__TDSO__)
static const uint16_t sbitmap[] = {
		/*(CONTROL_SWITCH_SCALE(SCALE_001) | CONTROL_SWITCH_MULTIPLIER(MULT_1)), // 10mV
		(CONTROL_SWITCH_SCALE(SCALE_001) | CONTROL_SWITCH_MULTIPLIER(MULT_2)), // 20mV
		(CONTROL_SWITCH_SCALE(SCALE_001) | CONTROL_SWITCH_MULTIPLIER(MULT_5)), // 50mV
		(CONTROL_SWITCH_SCALE(SCALE_01) | CONTROL_SWITCH_MULTIPLIER(MULT_1)),  // 100mV
		(CONTROL_SWITCH_SCALE(SCALE_01) | CONTROL_SWITCH_MULTIPLIER(MULT_2)),  // 200mV
		(CONTROL_SWITCH_SCALE(SCALE_01) | CONTROL_SWITCH_MULTIPLIER(MULT_5)),  // 500mV */
		(CONTROL_SWITCH_SCALE(SCALE_1) | CONTROL_SWITCH_MULTIPLIER(MULT_1)),   // 1V
		(CONTROL_SWITCH_SCALE(SCALE_1) | CONTROL_SWITCH_MULTIPLIER(MULT_2)),   // 2V
		(CONTROL_SWITCH_SCALE(SCALE_1) | CONTROL_SWITCH_MULTIPLIER(MULT_5))    // 5V
};


void CONTROL_SelectScale(uint8_t scale){
	if(scale > (sizeof(sbitmap)/sizeof(uint16_t)))
		return ;
	//CONTROL_SET_SCALE(sbitmap[scale]);
	CONTROL_SET_SCALE(0);
}

/*
 * Configure PWM duty to generate required analog voltage to trigger comparator
 *
 */
void CONTROL_SetTriggerLevel(int16_t level){
    level += CONTROL_TGPWM_CALIBRATION;
    TIM2->CCR1 = map(level, CONTROL_TGLEVEL_MIN, CONTROL_TGLEVEL_MAX, 0, CONTROL_TGPWM_STEPS);
}

/*
 * Configure interrupt edge to match trigger comparator output edge
 */
void CONTROL_SetTriggerEdge(uint16_t edge){

}

#elif defined(__EMU__)
void CONTROL_SelectScale(uint8_t scale){}
void CONTROL_SetTriggerLevel(int16_t level){}
void CONTROL_SetTriggerEdge(uint16_t edge) {}
#elif defined(__BLUEBOARD__)
void CONTROL_SelectScale(uint8_t scale){}
void CONTROL_SetTriggerLevel(int16_t level){}
void CONTROL_SetTriggerEdge(uint16_t edge) {}
#endif

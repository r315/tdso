/*
 * control.c
 *
 *  Created on: Aug 19, 2017
 *      Author: hmr
 */


#include "common.h"
#include "control.h"
#include "tdso_util.h"

#if defined(__TDSO__)

//TODO: COnfigure pins

void CONTROL_SetGain(uint8_t gain){
	if(gain < CONTROL_GAIN_STAGES)
		CONTROL_SET_GAIN(gain);
}

/*
 * Configure PWM duty to generate required analog voltage to trigger comparator
 *
 */
void CONTROL_SetTriggerLevel(int16_t level){
    TIM2->CCR1 = map(level + CONTROL_TGLEVEL_ZERO, 
				CONTROL_TGLEVEL_MIN, CONTROL_TGLEVEL_MAX, 
				CONTROL_TGLEVEL_PWM_MIN, CONTROL_TGLEVEL_PWM_MAX);
}

/*
 * Configure interrupt edge to match trigger comparator output edge
 */
void CONTROL_SetTriggerEdge(uint16_t edge){

}


void CONTROL_SetVpos(Channel *ch){
	TIM2->CCR4 = map(ch->pos + CONTROL_VPOS_ZERO, 
				CONTROL_VPOS_MIN, CONTROL_VPOS_MAX, 
				CONTROL_VPOS_PWM_MIN, CONTROL_VPOS_PWM_MAX);
}

void CONTROL_Init(Dso *dso){
	CONTROL_SetGain(dso->channels[DSO_SIGNAL0_CHANNEL].scaleidx);
	CONTROL_SetTriggerLevel(dso->trigger.pos);
	CONTROL_SetVpos(&dso->channels[DSO_SIGNAL0_CHANNEL]);
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

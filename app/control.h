/*
 * control.h
 *
 *  Created on: Aug 19, 2017
 *      Author: hmr
 */

#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <gpio.h>
#include "dso.h"

#if defined(__TDSO__)

#define CONTROL_GAIN_STAGES		4
#define CONTROL_GAIN_PORT		GPIOB
#define CONTROL_GAIN_BIT0		(1<<4)
#define CONTROL_GAIN_BIT1		(1<<5)
#define CONTROL_GAIN_MASK		(CONTROL_GAIN_BIT1 | CONTROL_GAIN_BIT0)
#define CONTROL_SET_GAIN(x)		CONTROL_GAIN_PORT->BSRR = ((CONTROL_GAIN_MASK << 16) | ((x << 4) & CONTROL_GAIN_MASK)) // If both BSx and BRx are set, BSx has priority.

#define CONTROL_TGLEVEL_ZERO	-2
#define CONTROL_TGLEVEL_MIN     -100
#define CONTROL_TGLEVEL_MAX     100
#define CONTROL_TGLEVEL_PWM_MIN	0
#define CONTROL_TGLEVEL_PWM_MAX	200

#define CONTROL_VPOS_ZERO		-10
#define CONTROL_VPOS_MIN		-100
#define CONTROL_VPOS_MAX		100
#define CONTROL_VPOS_PWM_MIN	46
#define CONTROL_VPOS_PWM_MAX	140


#elif defined(__EMU__) || defined(__BLUEBOARD__)

#else
#error config board
#endif

#ifndef null
#define null ((void*)0)
#endif

void CONTROL_SetGain(uint8_t scale);
void CONTROL_SetTriggerLevel(int16_t level);
void CONTROL_SetTriggerEdge(uint16_t edge);
void CONTROL_SetVpos(Channel *ch);
void CONTROL_Init(Dso *dso);

#endif /* CONTROL_H_ */

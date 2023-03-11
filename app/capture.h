#ifndef __capture_h__
#define __capture_h__

#include "dso.h"

#define START_MAT0_1  4
#define START_MAT0_3  5
#define START_MAT1_0  6
#define START_MAT1_1  7

/*
 * @brief initialize ADC, timer, DMA
 */
void CAP_Init(void);

/*
 * @brief Start a capture, capture must stop if buffer full or valid trigger
 */
void CAP_Start(int16_t *dst, uint16_t size);

/*
 * @brief stop current capture, deprecated??
 */
void CAP_Stop(void);

/*
 * @brief test if current capture has ended
 */
uint8_t CAP_IsEnded(void);

/*
 * @brief Set time between samples
 */
void CAP_SetSampleRate(uint32_t tb_us);

/*
 * @brief deprecated
 */
void CAP_SetTrigger(uint16_t level);

/*
 * @brief get index for trigger
 */
uint16_t CAP_GetTriggerOffset(void);

/*
 * @brief check if trigger has occurred on capture
 */
uint8_t CAP_Triggered(void);

void CAP_SetTriggerEdge(uint8_t rising);
#endif


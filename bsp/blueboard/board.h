#ifndef _board_h_
#define _board_h_

#include <blueboard.h>

#define BUTTON_LEFT2	(BUTTON_LEFT | (1<<9))  
#define BUTTON_RIGHT2	(BUTTON_RIGHT | (1<<12))

#define Board_Init BB_Init

#define LED_ON LED1_ON
#define LED_OFF LED1_OFF

#define DelayMs BB_RitDelay
#define GetTicks BB_RitTicks

static inline uint32_t ElapsedTicks(uint32_t start_ticks){ 
    uint32_t current = GetTicks(); 
    return (current > start_ticks) ? current - start_ticks : 0xFFFFFFFF - start_ticks + current;
}


#endif


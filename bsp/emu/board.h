#ifndef _board_h_
#define _board_h_

#define __EMU__

#include <libemb.h>
#include <lcdsdl.h>
#include <SDL2/SDL.h>

#define LCD_W 320
#define LCD_H 240

#define BUTTON_LEFT2	(BUTTON_LEFT | (1<<9))  
#define BUTTON_RIGHT2	(BUTTON_RIGHT | (1<<12))

#define GetTicks SDL_GetTicks
#define DelayMs(x) SDL_Delay(x)

static inline uint32_t ElapsedTicks(uint32_t start_ticks){ 
    uint32_t current = GetTicks(); 
    return (current > start_ticks) ? current - start_ticks : 0xFFFFFFFF - start_ticks + current;
}

#define LED_ON
#define LED_OFF

extern uint32_t SystemCoreClock;
#endif


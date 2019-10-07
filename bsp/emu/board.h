#ifndef _board_h_
#define _board_h_

#include <libemb.h>
#include <lcdsdl.h>
#include <SDL2/SDL.h>

#define LCD_W 240
#define LCD_H 320

#define BUTTON_LEFT2	(BUTTON_LEFT | (1<<9))  
#define BUTTON_RIGHT2	(BUTTON_RIGHT | (1<<12))

#define GetTicks SDL_GetTicks
#define DelayMs(x) SDL_Delay(x)
//#define ElapsedTicks(x) { GetTicks() - (x);}
#endif


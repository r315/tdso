#include "lcd.h"
#include "tdso_util.h"
#include "dso.h"
#include "display.h"

volatile uint32_t time;



#if defined(__TDSO__)
#include "stm32f1xx_hal.h"

void DelayMs(uint32_t ms){
	HAL_Delay(ms);
} 
uint32_t GetTicks(void){
return HAL_GetTick();
} 

#endif

int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void UTIL_StartTiming(void){
	time = GetTicks();
}

void UTIL_StopTiming(void){
	 time = GetTicks() - time;
    DISPLAY_GotoAbsolute(DSO_GRID_ORIGIN_X + 2, DSO_GRID_ORIGIN_Y + 2);
	 DISPLAY_printf("%ums",time);
}


void UTIL_wps(void){
static uint32_t ticks;
static uint8_t wps;

	if(GetTicks() < ticks){
		wps++;
	}else{
      DISPLAY_GotoAbsolute(DSO_GRID_ORIGIN_X + 2, DSO_GRID_ORIGIN_Y + 12);
		DISPLAY_printf("%uwps  ", wps);
		wps = 0;
		ticks = GetTicks() + 1000;
	}
}

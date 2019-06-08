#include "board.h"
#include "dso.h"
#include "tdso_util.h"


volatile uint32_t time;

int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max){
  return ((x - in_min) * (out_max - out_min) / (in_max - in_min)) + out_min;
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



uint32_t ElapsedTicks(uint32_t start_ticks){ 
    uint32_t current = GetTicks(); 
    return (current > start_ticks) ? current - start_ticks : 0xFFFFFFFF - start_ticks + current;
}

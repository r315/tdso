#include "board.h"
#include "dso.h"
#include "tdso_util.h"


volatile uint32_t time;

int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max){
  return ((x - in_min) * (out_max - out_min) / (in_max - in_min)) + out_min;
}

void UTIL_StartTiming(void){
	time = GetTick();
}

void UTIL_StopTiming(void){
	time = GetTick() - time;
    LIB2D_SetPos(DSO_GRID_ORIGIN_X + 2, DSO_GRID_ORIGIN_Y + 2);
	LIB2D_Print("%ums",time);
}

void UTIL_wps(void){
static uint32_t ticks;
static uint8_t wps;

	if(GetTick() < ticks){
		wps++;
	}else{
    	LIB2D_SetPos(DSO_GRID_ORIGIN_X + 2, DSO_GRID_ORIGIN_Y + 12);
		LIB2D_Print("%uwps   ", wps);
		wps = 0;
		ticks = GetTick() + 1000;
	}
}


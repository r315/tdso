/**
 * 
 * */

#if defined(__BLUEBOARD__)
#include "dso.h"
#include "capture.h"
#include "board.h"

volatile uint8_t done = 0;

uint8_t signal;
uint16_t timebase;
uint16_t nsamples;

void CAP_Sample(void *ptr, uint16_t size){
    done = 1;
}

void CAP_SetSampleRate(uint32_t sr_us){
   timebase = sr_us;
   //printf("Set time base to %u\n", timebase);
}

void CAP_Init(void){
    signal = 1;     // generate sine wave
    done = 1;
}

void CAP_Start(int16_t *dst, uint16_t size){
   done = 0;
   nsamples = size;
   CAP_Sample(dst, size);
}

void CAP_Stop(void){
   done = 1;
}

uint8_t CAP_IsEnded(void){
    return done;
}

uint16_t CAP_GetEdge(int16_t *buf, uint16_t size, int16_t level){
uint16_t index =0;
    if(level > 0){
        while(index < DSO_MAX_SAMPLES && buf[index] < -level) index++;
        while(index < DSO_MAX_SAMPLES && buf[index] > -level) index++;
    }else{
        level = -level;
        while(index < DSO_MAX_SAMPLES && buf[index] > level) index++;
        while(index < DSO_MAX_SAMPLES && buf[index] < level) index++;
    }
    return index;
}

uint16_t CAP_GetTriggerIndex(void){
return 0;
}

uint8_t CAP_Triggered(void){
return 0;
}
#endif

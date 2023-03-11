/**
 * 
 * */

#include "dso.h"
#include "capture.h"
#include "board.h"

#include <math.h>

volatile uint8_t done = 0;

#define FREQ 466
#define AMP  1000
#define PI   3.141592 
#define ADC_OFFSET 2048

uint8_t signal;
uint16_t timebase;
uint16_t nsamples;

void CAP_Sample(void *ptr, uint16_t size){
uint16_t idx = 0;
int16_t *p = (int16_t*)ptr;
double t,ts;
ts = (double)((timebase*10) / (DSO_GRID_W/DSO_GRID_H_DIVISIONS));

    for(idx = 0; idx < size; idx++){
        t = idx * ts;
        switch(signal){
                default:
                case 0:  //square wave
                    //p->channels[0].samples[idx] = ((t%(1/FREQ)) < (1/(FREQ*2))) ? 25 : -25;
                    break;

               case 1:  // vs = sin(2pift+w) * A
                    p[idx] = ADC_OFFSET + sin(2 * PI * FREQ * t) * AMP;
                    //printf("%f\n", t);
                    break;

               case 2:  // triangle
                    //p->channels[0].samples[idx] = abs((t%100) - 50) - 25;
                    break;
            }
    }   
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

uint16_t CAP_GetTriggerOffset(void){
return 0;
}

uint8_t CAP_Triggered(void){
return 0;
}


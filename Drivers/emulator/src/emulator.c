#include <string.h>
#include <SDL2/SDL.h> 
#include <time.h>
#include <button.h>
//#include <flash.h>

#if defined(DEBUG)
#define DBG printf
#else
#define DBG printf
#endif

uint32_t SystemCoreClock = 0;
uint32_t CLOCK_GetPCLK(void){ return 0;}
uint32_t CLOCK_GetCCLK(void){ return 0;}
void CLOCK_Init(unsigned int cclk){}
uint32_t CLOCK_GetTicks(void){ return SDL_GetTicks();}
void CLOCK_DelayMs(unsigned int ms){ SDL_Delay(ms);}
//---------------------------------------------------------------------------
void SPI_Init(int frequency, int bitData){ }
void SPI_BeginTransfer(int csBitId){}
void SPI_EndTransfer(int csBitId){}
void SPI_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int lenght){}
uint16_t SPI_Send(uint16_t data){ return 0xFFFF;}

//---------------------------------------------------------------------------
static int __led_state;
void LED_Init(int pinId, int state){__led_state = state;}
int LED_GetState(void){return __led_state;}
void LED_SetState(int state){ __led_state = state;}

//---------------------------------------------------------------------------
static struct tm __rtc;
static struct tm __rtca;
static uint32_t __amr;

void RTC_SetValue(struct tm *dateTime){	
	__rtc.tm_sec = dateTime->tm_sec % 60;
	__rtc.tm_min = dateTime->tm_min % 60;
	__rtc.tm_hour = dateTime->tm_hour % 24;
	__rtc.tm_mday = (dateTime->tm_mday > 1 && dateTime->tm_mday < 32)? dateTime->tm_mday : 1;
	__rtc.tm_mon = (dateTime->tm_mon > 1 && dateTime->tm_mon < 13)? dateTime->tm_mon : 1;
	__rtc.tm_wday = (dateTime->tm_wday > 1 && dateTime->tm_wday < 7)? dateTime->tm_wday : 0;
	__rtc.tm_year = (dateTime->tm_year > 0 && dateTime->tm_year < 4096)? dateTime->tm_year : 1900;
	__rtc.tm_yday = (dateTime->tm_yday > 1 && dateTime->tm_yday < 366)? dateTime->tm_yday : 1;	
}

uint32_t rtcinc(uint32_t interval, void*ptr){
      __rtc.tm_sec = (__rtc.tm_sec == 59)? 0 : __rtc.tm_sec + 1;
      if(__rtc.tm_sec == 0)
         __rtc.tm_min = (__rtc.tm_min == 59)? 0 : __rtc.tm_sec + 1;
      //printf("sec: %u\n", __rtc.tm_sec);      
      return interval;
}

void RTC_Init(struct tm *dateTime){ 
   RTC_SetValue(dateTime);
   __amr = 0;
   SDL_AddTimer(1000, rtcinc,(void*)NULL);
}

void RTC_GetValue(struct tm *dateTime){
//time_t rawtime;
 //  time ( &rawtime );
 //  memcpy((uint8_t *)dateTime, (uint8_t *)localtime(&rawtime), sizeof(struct tm));
   memcpy((uint8_t *)dateTime, (uint8_t *)&__rtc, sizeof(struct tm));
}

void RTC_GetAlarmValue(struct tm *dateTime){
   memcpy((void*)dateTime, (void*)&__rtca, sizeof(struct tm));	
}

void RTC_SetAlarmValue(struct tm *dateTime){
   memcpy((void*)&__rtca, (void*)dateTime, sizeof(struct tm));		
}

int checkAlarm(int alm){
   switch(alm & 7){
      case 0: return __rtca.tm_sec == __rtc.tm_sec;
      case 1: return __rtca.tm_min == __rtc.tm_min;
      case 2: return __rtca.tm_hour == __rtc.tm_hour;
      case 3: return __rtca.tm_mday == __rtc.tm_mday;
      case 4: return __rtca.tm_wday == __rtc.tm_wday;
      case 5: return __rtca.tm_yday == __rtc.tm_yday;
      case 6: return __rtca.tm_mon == __rtc.tm_mon;
      case 7: return __rtca.tm_year == __rtc.tm_year;
   }
   return 0;
}


uint32_t RTC_CheckAlarm(void){
uint8_t bits;

   if(!__amr)
      return 0;
      
   for(bits = 0; bits < 8; bits++){
         if(__amr & (1<<bits))
            if(!checkAlarm(bits))
               return 0;   
   }
   
   return 1;
}

void RTC_ClearAlarm(void){
}

void RTC_ActivateAlarm(uint32_t alarm){
   __amr = alarm;
}

void RTC_DeactivateAlarm(uint32_t alarm){
   __amr = alarm;
}

//---------------------------------------------------------------------------
#define BUTTON_EXIT SDLK_q
typedef struct{
    uint32_t button;
    uint32_t event;
	unsigned int htime;
    unsigned int count;
}Button;

Button __button;

int BUTTON_Filter(const Uint8 *ink){
		int keys = 0;
        if(ink[SDL_SCANCODE_A])   keys |= BUTTON_LEFT;
        if(ink[SDL_SCANCODE_D])   keys |= BUTTON_RIGHT;
        if(ink[SDL_SCANCODE_W])   keys |= BUTTON_UP;
        if(ink[SDL_SCANCODE_S])   keys |= BUTTON_DOWN;
        if(ink[SDL_SCANCODE_E])   keys |= BUTTON_A;
        if(ink[SDL_SCANCODE_Q] || ink[SDL_SCANCODE_ESCAPE]) keys = BUTTON_EXIT;
        //printf("key pressed %u\n",keys);
		return keys;
}

void BUTTON_Init(int ht){
	__button.htime = ht;
}

int BUTTON_Hit(void){
static SDL_Event event;

    if(SDL_PollEvent(&event)){       
        if(event.type == SDL_KEYDOWN && __button.event == BUTTON_EMPTY){
            __button.count = SDL_GetTicks() + __button.htime;
			__button.button = BUTTON_Filter(SDL_GetKeyboardState(NULL));
            __button.event = (__button.button != BUTTON_EXIT)? BUTTON_PRESSED : SDL_QUIT;
            //printf("Start Timeout %u\n", __button.timeout);
            return __button.event;
        }     

        if(event.type == SDL_KEYUP){
            __button.event = BUTTON_RELEASED; 
            return __button.event;
        }

        if(event.type == SDL_QUIT){
            __button.button = SDLK_q;
            __button.event = SDL_QUIT;
	         return __button.event;
		}
    }

    switch(__button.event){
        case BUTTON_PRESSED:
            __button.event = BUTTON_TIMING;
            break;

        case BUTTON_TIMING:
            if(SDL_GetTicks() > __button.count)
                __button.event = BUTTON_HOLD;               
            break;

        case BUTTON_RELEASED:
                __button.event = BUTTON_EMPTY;
                __button.button = BUTTON_EMPTY;

        default : break;            
    }   
    return __button.event;
}

void BUTTON_WaitEvent(int event){
 do{
     BUTTON_Hit();
 }while(__button.event != event);    
}

				   
int BUTTON_Read(void){
    BUTTON_WaitEvent(BUTTON_PRESSED);
    return __button.button; 
}


int BUTTON_GetEvents(void){
    return __button.event;
}

int BUTTON_GetValue(void){
    return __button.button;
}

void BUTTON_SetHoldTime(int t){
	__button.htime = t;
}

char BUTTON_Pressed(int button){
	return (__button.button & button) == 0 ? 0 : 1;
}
//---------------------------------------------------------------------------
/*
unsigned int FLASH_EraseSectors(unsigned int startSector, unsigned int endSector){
    return CMD_SUCESS;
}

unsigned int FLASH_WriteBlock( void *dstAddr, void *srcAddr, unsigned int size){
    memcpy(dstAddr, srcAddr, size);
    return CMD_SUCESS;
}

unsigned int FLASH_WriteData(void *dstAddr, void *srcAddr, unsigned int size){
    return  FLASH_WriteBlock(dstAddr, srcAddr, size);
}

unsigned int FLASH_VerifyData(void *dstAddr, void *srcAddr, unsigned int size){
      while(size--){
         if(*((uint8_t*)(dstAddr++))  != *((uint8_t*)(srcAddr++)))
            return COMPARE_ERROR;
      }
    return CMD_SUCESS;
}
//---------------------------------------------------------------------------
void TIMER0_Init(unsigned int frequency){}

unsigned int TIMER0_GetValue(void){
	return SDL_GetTicks();
}

unsigned int TIMER0_Elapse(unsigned int lastRead){
	return SDL_GetTicks()-lastRead;
}

unsigned int TIMER0_TicksToMs(unsigned int ticks){
	return  ticks;
}

void TIMER0_DelayMs(unsigned int ms){
    SDL_Delay(ms);
}

*/





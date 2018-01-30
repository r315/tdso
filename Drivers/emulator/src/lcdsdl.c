//Using SDL and standard IO 
#include <SDL2/SDL.h> 
#include <stdio.h> 
#include <lcd.h>
#include "lcdsdl.h"

#define UPDATE_TIME 30 //30ms => 33fps

#define WINDOW_W 240
#define WINDOW_H 320

#define LCD_OFFSETX 0 
#define LCD_OFFSETY 0

typedef struct _Wnd{
	SDL_Window *window;
	SDL_Surface *surface;
    char *title;
	uint32_t w;
	uint32_t h;
	uint32_t wx;			//wrap x
	uint32_t wy;			//wrap y
	uint32_t ww;			//wrap w
	uint32_t wh;			//wrap h
	uint32_t mx;			//memory current x
	uint32_t my;			//memory current y
    uint32_t auto_update;
}Lcd;

Lcd lcd;

void LCD_Scroll(uint16_t y){}
void LCD_Bkl(uint8_t state){}

uint32_t LCD_Update(uint32_t interval, void *ptr){
Lcd *lcd = (Lcd*)ptr;    
	
    SDL_UpdateWindowSurface(lcd->window);
    if(!lcd->auto_update)
      return 0;	
    return interval;
}

SDL_Window *Window_Init(Lcd *plcd){

SDL_Window *window;
SDL_Surface *surface;

    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 ) { 
        fprintf(stdout,"LCDSDL: SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return NULL; 
    } 
      
    window = SDL_CreateWindow(  (plcd->title == NULL)? "Lcd emulator" : plcd->title, 
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, 
                                    plcd->w, 
                                    plcd->h, 
                                    SDL_WINDOW_SHOWN); 
    if( window == NULL ) { 
        fprintf(stdout,"LCDSDL: Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return NULL;
    }
        
    surface = SDL_GetWindowSurface( window );
     
    fprintf(stdout,"LCDSDL: Window size: %dx%d %dbpp\n",surface->w, surface->h, surface->format->BitsPerPixel);    
    
    //if(WINDOW_W != LCD_W || WINDOW_H != LCD_H){
        //SDL_FillRect(wnd->surface, NULL, SDL_MapRGB(wnd->surface->format, 0x0, 0x0, 0 ) );
        //fprintf(stdout,"Display size: %dx%d\n",(int)LCD_W,(int)LCD_H);        
    //}else
       SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x30, 0x0A, 0x24 ) );    
    
    plcd->auto_update = 1;
    plcd->window = window;
    plcd->surface = surface;

    SDL_AddTimer(UPDATE_TIME, LCD_Update, plcd);
    return plcd->window;
}

void LCD_Init(void){
	lcd.w = WINDOW_W;
	lcd.h = WINDOW_H;
    Window_Init(&lcd);	
}

void LCD_Close(void){
    lcd.auto_update = 0;    
	SDL_DestroyWindow(lcd.window); 
	SDL_Quit(); 
}

void LCD_Rotation(uint8_t m){
    LCD_Close();
     switch (m) {
        case LCD_PORTRAIT:            
             lcd.w = WINDOW_W;
             lcd.h = WINDOW_H;            
            break;
            
        case LCD_LANDSCAPE:
            lcd.h = WINDOW_W;
            lcd.w = WINDOW_H; 
            break;
            
        case LCD_REVERSE_PORTRAIT:            
            break;
            
        case LCD_REVERSE_LANDSCAPE:            
            break;

        default:
            return;
    }
    Window_Init(&lcd);
}

void LCD_Fill(uint32_t n, uint16_t color){
	while(n--)
		LCD_Data(color);
}

void LCD_IndexedColor(uint16_t *colors, uint8_t *index, uint32_t size){
	while(size--){
        LCD_Data(colors[*index]);
		index += 1;
    }
}

void LCD_Window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    lcd.mx = x;
    lcd.my = y;   

    lcd.wx = x;
    lcd.wy = y;
    lcd.ww = x + (w-1);
    lcd.wh = y + (h-1);    
}

void LCD_Pixel(uint16_t x, uint16_t y, uint16_t color){
    LCD_Window(x,y,1,1);
    LCD_Data(color);
}
//-------------------------------------------------------------
//        RRRRRGGGGGGBBBBB 
//        BBBBBGGGGGGRRRRR
//RRRRRRRRGGGGGGGGBBBBBBBB
//-------------------------------------------------------------
void LCD_Data(uint16_t color)
{
   uint32_t *pixels = (uint32_t *)lcd.surface->pixels;    
   
	if(lcd.mx >= lcd.wx && lcd.mx <= lcd.ww && 
       lcd.my >= lcd.wy && lcd.my <= lcd.wh &&
       lcd.mx < lcd.w && lcd.my < lcd.h)
	{
    	pixels[ (lcd.my * lcd.w) + lcd.mx] = (uint32_t) ( ((color&0xf800)<<8) | ((color&0x7e0)<<5) | ((color&0x1f)<<3) );   
        if(lcd.mx == lcd.ww){
            lcd.mx = lcd.wx;
            if(lcd.my == lcd.wh) lcd.my = lcd.wy;
            else lcd.my++;
        }
        else lcd.mx++;
    }   
}


uint16_t LCD_GetWidth(void){
    return lcd.w;
}
uint16_t LCD_GetHeight(void){
    return lcd.h;
}

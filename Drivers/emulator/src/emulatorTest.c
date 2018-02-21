#if 0
#include <stdlib.h>

#include <SDL2/SDL.h>

#define WINDOW_WIDTH 600

int main(void) {
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;
    int i;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(320, 240, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 128, 0, 0, 0);
    SDL_RenderClear(renderer);
    //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    //for (i = 0; i < WINDOW_WIDTH; ++i)
    //    SDL_RenderDrawPoint(renderer, i, i);
    SDL_RenderPresent(renderer);
    while (1) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
#endif

#if 1
#include <stdio.h> 
#include <button.h>
#include <lcd.h>

#define FONT (unsigned char *)font8x16

extern const unsigned char font8x16[97][16];
static uint16_t _x, _y;
static uint16_t _fColor, _bColor;

void LCD_SetColors(uint16_t fColor, uint16_t bColor){
	_fColor = fColor;
	_bColor = bColor;
}

void LCD_PutChar(char c, int x, int y, int fColor, int bColor) {
	int i, colIndex;
	unsigned int nCols;
	unsigned int nRows;
	unsigned int nBytes;
	unsigned char pixelRow;
	unsigned char *pFont;
	unsigned char *pChar;

	pFont = (unsigned char *) font8x16;
	nCols = *pFont;
	nRows = *(pFont + 1);
	nBytes = *(pFont + 2);

	pChar = pFont + (nBytes * (c - 0x1F));
	
	_x = (x + nCols);
	_y = y;
	_fColor = fColor;
	_bColor = bColor;

	if( _x >= LCD_W){
		_x = 0;
		_y += nRows;
		if(_y >= LCD_H)
			_y = 0;
	}	
	
	LCD_Window(x,y,nCols,nRows);

	for (i = 0; i < nBytes; i++) {
		pixelRow = *pChar++;
		for (colIndex = 0; colIndex < nCols; colIndex++) {
			LCD_Data(((pixelRow & 0x80) != 0) ? fColor : bColor);
			pixelRow <<= 1;
		}
	}
}

void LCD_NewLine(void){
uint8_t h = *((uint8_t*)font8x16+1);
    _y += h;
    if(_y > (LCD_H - h))
        _y = 0;
    _x = 0;
}


void LCD_WriteChar(char ch){
    if(ch == '\n')
        LCD_NewLine();
    else
	    LCD_PutChar(ch, _x, _y, _fColor, _bColor);
}

void LCD_WriteString(char *str){
	while(*str)
		LCD_WriteChar(*str++);
}

int main(int argc, char *argv[]){
 
   LCD_Init();
#if 0  // basic init
    while(loop){ 
        BUTTON_Hit();	    
    }
	LCD_Close(); 
	return 0;

#endif
     
   LCD_Rotation(LCD_LANDSCAPE);
   LCD_SetColors(GREEN,BLACK);  
   //LCD_Clear(BLUE);  
   //LCD_FillRect(10,10,100,100,RED);
   //LCD_WriteString("hello");
   	
    while(loop){

		if( BUTTON_Read() == BUTTON_PRESSED){
	        switch(BUTTON_GetValue()){
	            case BUTTON_LEFT:
	                LCD_WriteString("LEFT ");		
	                break;
	            case BUTTON_RIGHT:
	                LCD_WriteString("RIGHT ");
	                break;
	            case BUTTON_UP:
	                LCD_WriteString(" UP ");
	                break;   
	            case BUTTON_DOWN:
	                LCD_WriteString("DOWN ");
	                break;
				case BUTTON_A:
	                LCD_WriteString("CENTER ");
	                break;
	                
	            default: printf("%X\n",BUTTON_GetValue());
	                break;
	        }      
		}
    }
	LCD_Close(); 
	return 0;
}
#endif

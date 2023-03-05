#include "dso.h"
#include <board.h>


#if defined(__EMU__)
int main(int argc, char *argv[]){

unsigned int uitime = 0;

	LIB2D_Init();

	LCD_Clear(BLACK);

	//LCD_Rotation(LCD_LANDSCAPE);

	//LIB2D_SetFont(FONT_PIXELDUST);
	
	DSO_Init();

    while(loop){    
    	if(uitime < GetTicks()){
    	    DSO_Run(OFF);    		
    		#ifdef __EMU__
    		uitime = GetTicks() + 10;
    		#else
    		uitime = GetTicks() + 5;
    		#endif
    	}
    }
    return 0;
}
#else

void TEST_Run(void);

int main(void){

	Board_Init();
    LIB2D_Init();
    LCD_Clear(LCD_BLACK);

    BUTTON_Init(200);

    BUTTON_Read();

    if(BUTTON_GetEvents() == BUTTON_PRESSED && BUTTON_GetValue() == BUTTON_CENTER){                
        LCD_Bkl(ON);        
        TEST_Run(); // Doesn't return        
    }else{
        LCD_SetOrientation(LCD_LANDSCAPE);
        //LCD_Rotation(LCD_REVERSE_LANDSCAPE);
        DSO_Init();
        while(1){
          DSO_Run(ON);
        }
    }
return 0;
}
#endif




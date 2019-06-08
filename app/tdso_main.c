#include "dso.h"
#include "board.h"


#if defined(__EMU__)
int main(int argc, char *argv[]){

unsigned int uitime = 0;

	DISPLAY_Init(ON);

	LCD_Clear(BLACK);

	LCD_Rotation(LCD_LANDSCAPE);

	//DISPLAY_SetFont(FONT_PIXELDUST);
	
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
void TEST_BlinkLed(uint8_t times);
void TEST_Run(void);

int main(void){

	Board_Init();

    LCD_Init();
    LCD_Clear(BLACK);

    BUTTON_Read();

    if(BUTTON_GetEvents() == BUTTON_PRESSED && BUTTON_GetValue() == BUTTON_CENTER){
        TEST_BlinkLed(3);
        DISPLAY_printf("Entering Test mode\n");
        LCD_Bkl(ON);
        while(1){
            TEST_Run();
        }
    }else{
        //LCD_Rotation(LCD_LANDSCAPE);
        LCD_Rotation(LCD_REVERSE_LANDSCAPE);
        DSO_Init();
        while(1){
          DSO_Run(ON);
        }
    }
return 0;
}
#endif




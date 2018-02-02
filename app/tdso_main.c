
#include <display.h>
#include <button.h>
#include "dso.h"
#include "common.h"


#if defined(__EMU__)
int main(int argc, char *argv[]){

unsigned int uitime = 0;

	DISPLAY_Init(ON);

	LCD_Clear(BLACK);

	LCD_Rotation(LCD_LANDSCAPE);

	DISPLAY_SetFont(FONT_PIXELDUST);
	
	DSO_Init();

    while(loop){    
    	if(uitime < GetTick()){
    	    DSO_Run(OFF);    		
    		#ifdef __EMU__
    		uitime = GetTick() + 10;
    		#else
    		uitime = GetTick() + 5;
    		#endif
    	}
    }
    return 0;
}
#else
void TEST_BlinkLed(uint8_t times);
void TEST_Run(void);


#define DMA_CCR_PL_Medium   (1<<12)
#define DMA_CCR_MSIZE_8     (0<<10)
#define DMA_CCR_MSIZE_16    (1<<10)
#define DMA_CCR_MSIZE_32    (2<<10)
#define DMA_CCR_PSIZE_16    (1<<8)
#define DMA_CCR_PSIZE_8     (0<<8)


void LCD_DMA_XFER(uint16_t *buf, uint32_t size, uint8_t fill){

    DMA1_Channel3->CMAR = (uint32_t)buf;
    SPI1->CR2 |= SPI_CR2_TXDMAEN;

    if(fill){
    	DMA1_Channel3->CCR &= ~DMA_CCR_MINC;
    }
    else{
    	DMA1_Channel3->CCR |= DMA_CCR_MINC;
    }

    size <<= 1;

    while(size > 0x10000){
    	DMA1_Channel3->CNDTR = 0xFFFF;
    	DMA1_Channel3->CCR |= DMA_CCR_EN;
    	 while((DMA1->ISR & DMA_ISR_TCIF3)){
    	    	DMA1->IFCR = DMA_IFCR_CGIF3;
    	    }

    	 DMA1_Channel3->CCR &= ~DMA_CCR_EN;
    	 size -= 0x10000;
    }

    DMA1_Channel3->CNDTR = size;
    DMA1_Channel3->CCR |= DMA_CCR_EN;

	 while((DMA1->ISR & DMA_ISR_TCIF3)){
			DMA1->IFCR = DMA_IFCR_CGIF3;
		}

	DMA1_Channel3->CCR &= ~DMA_CCR_EN;
    SPI1->CR2 &= ~SPI_CR2_TXDMAEN;
}



void tdo_main(void){

    LCD_Init();
    LCD_Bkl(ON);

    DMA1_Channel3->CPAR = (uint32_t)&SPI1->DR;
    DMA1_Channel3->CCR =
    		DMA_CCR_PL_Medium |   // priority
			DMA_CCR_MSIZE_16  |   // 16bit src size
			DMA_CCR_PSIZE_8   |   // 8bit dst size
			DMA_CCR_DIR       ;   // Read from Memory

    //LCD_Clear(RED);
    LCD_Clear(0x00F8);
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
}
#endif




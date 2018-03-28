
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

#if defined(__TDSO__)
#define DMA_CCR_PL_Medium   (1<<12)
#define DMA_CCR_MSIZE_8     (0<<10)
#define DMA_CCR_MSIZE_16    (1<<10)
#define DMA_CCR_MSIZE_32    (2<<10)
#define DMA_CCR_PSIZE_16    (1<<8)
#define DMA_CCR_PSIZE_8     (0<<8)

void LCD_DMA_XFER(uint32_t count, uint16_t *data, uint8_t isbuf){
	GPIOB->BRR = (1<<9);

    DMA1_Channel3->CMAR = (uint32_t)data;
    DMA1_Channel3->CCR =
       		DMA_CCR_PL_Medium |   // priority
   			DMA_CCR_MSIZE_16  |   // 16bit src size
   			DMA_CCR_PSIZE_16  |   // 16bit dst size
   			DMA_CCR_DIR       ;   // Read from Memory

    if(isbuf){
    	 DMA1_Channel3->CCR |= DMA_CCR_MINC;
    }

    // Configure Spi for 16bit DMA
   	SPI1->CR1 &= ~SPI_CR1_SPE;
    SPI1->CR2 |= SPI_CR2_TXDMAEN;
    SPI1->CR1 |= SPI_CR1_DFF | SPI_CR1_SPE;

    // DMA only supports transfers of 65536
    while(count > 0x10000){
    	DMA1_Channel3->CNDTR = 0xFFFF;
    	DMA1_Channel3->CCR |= DMA_CCR_EN;
    	 while(!(DMA1->ISR & DMA_ISR_TCIF3)){
    	    }
    	DMA1->IFCR = DMA_IFCR_CGIF3;

    	 DMA1_Channel3->CCR &= ~DMA_CCR_EN;
    	 count -= 0x10000;
    }

    // Send the remaining
    if(count){
		DMA1_Channel3->CNDTR = count;
		DMA1_Channel3->CCR |= DMA_CCR_EN;

		while((DMA1->ISR & DMA_ISR_TCIF3) == 0 );

		DMA1->IFCR = DMA_IFCR_CGIF3;
		//DMA1_Channel3->CCR &= ~DMA_CCR_EN;
		 DMA1_Channel3->CCR =
		    		DMA_CCR_PL_Medium |
					DMA_CCR_MSIZE_8  |
					DMA_CCR_PSIZE_8  |
					DMA_CCR_DIR;

		// wait for last transfer
		while((SPI1->SR & SPI_SR_TXE) == 0 );
		while((SPI1->SR & SPI_SR_BSY) != 0 );
    }

    // Restore 8bit Spi
	SPI1->CR1 &= ~(SPI_CR1_SPE | SPI_CR1_DFF);
	SPI1->CR2 = 0;
	SPI1->CR1 |= SPI_CR1_SPE;

	GPIOB->BSRR = (1<<9);
}


void LCD_Fill_DMA(uint32_t count, uint16_t color){
	LCD_DMA_XFER(count, &color, OFF);
}


void LCD_Fill_Data_DMA(uint32_t count, uint16_t *data){
	LCD_DMA_XFER(count, data, ON);
}

void SPI_Send_DMA(uint8_t data){
	SPI1->DR = data;
	while((SPI1->SR & SPI_SR_TXE) == 0 );
	while((SPI1->SR & SPI_SR_BSY) != 0 );

}
#endif /* __TDSO__ */

int main(void){


#if defined(__TDSO__)
    //TODO: Add call to main in stm lib
	SPI1->CR1 |= SPI_CR1_SPE;
	SPI_Send_DMA(0xFF);

    DMA1_Channel3->CPAR = (uint32_t)&SPI1->DR;
    DMA1_Channel3->CCR = DMA_CCR_PL_Medium |
    					 DMA_CCR_MSIZE_8  |
    					 DMA_CCR_PSIZE_8  |
    					 DMA_CCR_DIR;
#endif /* __TDSO__ */

    LCD_Init();
    LCD_Clear(BLACK);
    //GPIOB->BSRR = (1<<9);

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




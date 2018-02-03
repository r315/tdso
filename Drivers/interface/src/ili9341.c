
#include <common.h>
#include <ili9341.h>
#include <spi.h>


static uint16_t _width, _height;

void LCD_Command(uint8_t data){
    LCD_CD0;    
    SPI_Send(data);    
    LCD_CD1;
}

void LCD_Data(uint16_t data){
    LCD_CS0;
#ifdef SPI_16XFER
    SPI_Send16(data);
#else
    SPI_Send(data>>8);
    SPI_Send(data);   
#endif
    LCD_CS1;
}

void LCD_Scroll(uint16_t sc){
    LCD_CS0;
    LCD_Command(VSCRSADD);
#ifdef SPI_16XFER
    SPI_Send16(sc);
#else
    SPI_Send(sc>>8);
    SPI_Send(sc);
#endif
    LCD_CS1;
}

void LCD_Fill(uint32_t n, uint16_t color){
	if(!n) return;
    LCD_CS0;
#ifdef SPI_16XFER
    while(n--)
        SPI_Send16(color);
#elif defined(LCD_DMA)
    LCD_Fill_DMA(n, color);
#else
	while(n--){
		SPI_Send(color>>8);
		SPI_Send(color);
	}
#endif
    LCD_CS1;
}

void LCD_Fill_Data(uint32_t n, uint16_t *data){
	if(!n) return;
    LCD_CS0;
#if !defined(LCD_DMA)
    LCD_Fill_Data_DMA(n, data);
#else
	while(n--){
		SPI_Send((*data)>>8);
		SPI_Send(*(data++));
	}
#endif
    LCD_CS1;
}

void LCD_IndexedColor(uint16_t *colors, uint8_t *index, uint32_t size){
    LCD_CS0;
#ifdef SPI_16XFER
    while(size--){
        SPI_Send16(colors[*index]);
        index += 1;
    }
#else
    while(size--){
        SPI_Send(colors[*index]>>8);
        SPI_Send(colors[*index]);
        index += 1;
    }
#endif
    LCD_CS1;
}

void LCD_Pixel(uint16_t x, uint16_t y, uint16_t c){
    LCD_CS0;
#ifdef SPI_16XFER
    LCD_Command(CASET);
    SPI_Send16(x);
    SPI_Send16(x);

    LCD_Command(PASET);
    SPI_Send16(y);
    SPI_Send16(y);

    LCD_Command(RAMWR);
#elif defined(LCD_DMA)
    LCD_Window(x,y,1,1);
    LCD_Data(c);
#else
    LCD_Command(CASET);
    SPI_Send(x>>8);
    SPI_Send(x);
    SPI_Send(x>>8);
    SPI_Send(x);

    LCD_Command(PASET);
    SPI_Send(y>>8);
    SPI_Send(y);
    SPI_Send(y>>8);
    SPI_Send(y);

    LCD_Command(RAMWR);
    SPI_Send(c>>8);
    SPI_Send(c);
#endif
    LCD_CS1;
}

void LCD_Window(uint16_t x, uint16_t y, uint16_t w, uint16_t h){
    LCD_CS0;
#ifdef SPI_16XFER
    LCD_Command(CASET);
    SPI_Send16(x);
    SPI_Send16(x + w -1);
  
    LCD_Command(PASET);
    SPI_Send16(y);
    SPI_Send16(y + h - 1);

    LCD_Command(RAMWR);
#elif defined(LCD_DMA)
uint16_t data [2];
	data[0] = x;
	data[1] = x + ( w - 1 );
	LCD_Command(CASET);
	LCD_Fill_Data_DMA(sizeof(data)/sizeof(uint16_t), data);
	data[0] = y;
	data[1] = y + ( h - 1 );
	LCD_Command(PASET);
	LCD_Fill_Data_DMA(sizeof(data)/sizeof(uint16_t), data);
	LCD_Command(RAMWR);
#else
    LCD_Command(CASET);
    SPI_Send(x>>8);
    SPI_Send(x);
    x += w - 1;
    SPI_Send(x>>8);
    SPI_Send(x);
  
    LCD_Command(PASET);
    SPI_Send(y>>8);
    SPI_Send(y);
    y += h - 1;
    SPI_Send(y>>8);
    SPI_Send(y);
    LCD_Command(RAMWR);
#endif
    LCD_CS1;
}

void LCD_Init(void){
   LCD_PIN_INIT;

   LCD_CD0;
   LCD_CS1;
   LCD_BKL0;
   LCD_RST1;
   DelayMs(10);

   LCD_RST0;
   DelayMs(2);
   LCD_RST1;
   DelayMs(5);

   LCD_CS0;
   LCD_Command(SWRST);
   LCD_CS1;
   DelayMs(5);
   LCD_CS0;
   //LCD_Command(DISPOFF);

   LCD_Command(PCONB);
   SPI_Send(0x00);
   SPI_Send(0XC1);
   SPI_Send(0X30);

   LCD_Command(PSCON);
   SPI_Send(0x64);
   SPI_Send(0x03);
   SPI_Send(0X12);
   SPI_Send(0X81);

   LCD_Command(DTCONA);
   SPI_Send(0x85);
   SPI_Send(0x00);
   SPI_Send(0x78);

   LCD_Command(PCONA);
   SPI_Send(0x39);
   SPI_Send(0x2C);
   SPI_Send(0x00);
   SPI_Send(0x34);
   SPI_Send(0x02);

   LCD_Command(PRCON);
   SPI_Send(0x20);

   LCD_Command(DTCONB);
   SPI_Send(0x00);
   SPI_Send(0x00);
   /* Power Control */
   LCD_Command(PCON1);
   SPI_Send(0x23);      //VRH[5:0]

   LCD_Command(PCON2);
   SPI_Send(0x10);      //SAP[2:0];BT[3:0]
   /* VCOM Control */
   LCD_Command(VCOM1);
   SPI_Send(0x3e);      //Contrast
   SPI_Send(0x28);

   LCD_Command(VCOM2);
   SPI_Send(0x86);
   /* Memory Access Control */
   LCD_Command(MADCTL);  // Memory Access Control
   SPI_Send(0x48);       //C8  //48 68??//28 E8 ??

   LCD_Command(COLMOD);
   SPI_Send(0x55);       //16bit/pixel
   /* frame rate */
   LCD_Command(FRCONN);
   SPI_Send(0x00);
   SPI_Send(0x18);
   /* Gamma */

   /* ddram */

   /*  display  */
   LCD_Command(DFCTL);    // Display Function Control
   SPI_Send(0x08);
   SPI_Send(0x82);
   SPI_Send(0x27);
   SPI_Send(0x00);

   LCD_Command(SLPOUT);    //Exit Sleep
   DelayMs(120);
   LCD_Command(DISPON);    //Display on
   DelayMs(120);

   LCD_CS1;

   _width  = TFT_W;
   _height = TFT_H;
   // user must enable backlight after LCD_Init
}

void LCD_Rotation(uint8_t m) {

    switch (m) {
        case LCD_PORTRAIT:
            m = (MADCTL_MX | MADCTL_BGR);
            _width  = TFT_W;
            _height = TFT_H;
            break;
        case LCD_LANDSCAPE:
            m = (MADCTL_MV | MADCTL_BGR);
            _width  = TFT_H;
            _height = TFT_W;
            break;
        case LCD_REVERSE_PORTRAIT:
            m = (MADCTL_MY | MADCTL_BGR);
            _width  = TFT_W;
            _height = TFT_H;
            break;
        case LCD_REVERSE_LANDSCAPE:
            m = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
            _width  = TFT_H;
            _height = TFT_W;
            break;

        default:
         return;
    }
    LCD_CS0;
    LCD_Command(MADCTL);
    SPI_Send(m);
    LCD_CS1;
}

uint16_t LCD_GetWidth(void){
   return _width;
}

uint16_t LCD_GetHeight(void){
   return _height;
}

uint32_t LCD_GetSize(void){
   return _height*_width;
}

void LCD_Bkl(uint8_t state){
    if(state != 0) {LCD_BKL1;}
    else {LCD_BKL0;}
}


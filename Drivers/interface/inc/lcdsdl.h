#ifndef _lcdsdl_h_
#define _lcdsdl_h_

#include <stdint.h>

#ifndef LCD_W
	#define LCD_W LCD_GetWidth()
	#define LCD_H LCD_GetHeight()
#endif	

#define LCD_PORTRAIT  0
#define LCD_LANDSCAPE 1
#define LCD_REVERSE_PORTRAIT  2
#define LCD_REVERSE_LANDSCAPE 3

#define LCD_SIZE (LCD_W*LCD_H)

void LCD_Close(void);
void LCD_Data(uint16_t color);
void LCD_Fill( uint32_t n, uint16_t color);
void LCD_IndexedColor(uint16_t *colors, uint8_t *index, uint32_t size);
void LCD_Goto(uint16_t x, uint16_t y);
void LCD_Rotation(uint8_t m);

uint16_t LCD_GetWidth(void);
uint16_t LCD_GetHeight(void);
#endif

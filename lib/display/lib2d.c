#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include "lib2d.h"
#include "liblcd.h"
#include "font.h"

#define LIB2D_FLOAT_MAX_PRECISION 		8

typedef struct lib2d_s{
	void (*xputc)(char);		//for use on xstdout
	char (*xgetchar)(void);
	uint16_t forecolor;
	uint16_t backcolor;
	font_t *font;
	uint16_t cx;
	uint16_t cy;
	uint16_t sc;
	uint8_t vspace;
	uint16_t (*drawChar)(uint16_t  x, uint16_t y, uint8_t *char_data, uint16_t *buffer);
}lib2d_t;

static lib2d_t lib2d;
static uint16_t scratch[8 * 8 * 2]; // font dependent
static uint16_t scratch_offset;
//-----------------------------------------------------------
//
//-----------------------------------------------------------
//----------------------------------------------------------
//draws a character without any attributes
//TODO: optimize
//----------------------------------------------------------
static uint16_t drawCharSimple(uint16_t x, uint16_t y, uint8_t *char_data, uint16_t *buffer){
	uint8_t w,h;
	uint16_t *pbuf = buffer;
	
	for (h=0; h < lib2d.font->h; h++){ 	
		for(w=0; w < lib2d.font->w; w++){			
			*pbuf++ = (*char_data & (0x80 >> w)) ? lib2d.forecolor : lib2d.backcolor;
		}
		char_data += 1;
 	}

	LCD_WriteArea(x, y, lib2d.font->w, lib2d.font->h, buffer);
	return x + (lib2d.font->w);
}

static uint16_t drawCharInverted(uint16_t x, uint16_t y, uint8_t *char_data, uint16_t *buffer){
	uint8_t w,h;
	uint16_t *pbuf = buffer;

	for (h=0; h < lib2d.font->h; h++){ 	
		for(w=0; w < lib2d.font->h; w++){
			*pbuf++ = (*char_data & (0x80 >> w)) ? lib2d.backcolor : lib2d.forecolor;
		}
		char_data += 1;		
 	}
	return x+(lib2d.font->w);
}

static uint16_t drawCharTransparent(uint16_t x, uint16_t y, uint8_t *char_data, uint16_t *buffer)
{
	uint8_t w,h;
	for (h=0; h < lib2d.font->h; h++){
		for(w=0;w<lib2d.font->w; w++){
			if(*char_data & (0x80 >> w))
				LCD_Pixel(x + w, y + h, lib2d.forecolor);							
		}	
		char_data += 1;
	}
	return x+(lib2d.font->w);
}
static uint16_t drawCharDouble(uint16_t x, uint16_t y, uint8_t *char_data, uint16_t *buffer)
{
	uint8_t w,h;
    //LCD_Window(x,y,lib2d.font->w * 2, lib2d.font->h * 2);
	for (h=0;h<lib2d.font->h;h++){			    // altura
		for(w=0;w<lib2d.font->w;w++){			// primeira linha
			if(*char_data & (0x80 >> w))	// se pixel
			{
				//LCD_Data(lib2d.forecolor);			// desenha 2 px w de FC
				//LCD_Data(lib2d.forecolor);
			}
	        else
			{
    	        //LCD_Data(lib2d.backcolor);			// desenha 2 px w de BC
				//LCD_Data(lib2d.backcolor);
			}
		}
		for(w=0;w<lib2d.font->w;w++)			// segunda linha igual a primeira
		{
			if(*char_data & (0x80 >> w))
			{
				//LCD_Data(lib2d.forecolor);
				//LCD_Data(lib2d.forecolor);
			}
	        else
			{
    	        //LCD_Data(lib2d.backcolor);					
				//LCD_Data(lib2d.backcolor);
			}
		}
		char_data += 1;
 	}	
	return x+(lib2d.font->w * 2);
}

void LIB2D_SetColors(uint16_t fc, uint16_t bc){ lib2d.forecolor = fc; lib2d.backcolor = bc;}
void LIB2D_SetFcolor(uint16_t color){ lib2d.forecolor = color;}
void LIB2D_SetBcolor(uint16_t color){ lib2d.backcolor = color;}
uint16_t LIB2D_GetFontWidth(void){ return lib2d.font->w; }
uint16_t LIB2D_GetFontHeight(void){ return lib2d.font->h; }

void LIB2D_SetPos(uint16_t x, uint16_t y){
   	lib2d.cx = x;
	lib2d.cy = y;
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
void LIB2D_SetCursor(uint16_t x, uint16_t y){
	lib2d.cx = x * lib2d.font->w;
	lib2d.cy = y * lib2d.font->h;
}
//----------------------------------------------------------
//
//----------------------------------------------------------
uint16_t LIB2D_Char(uint16_t x, uint16_t y, uint8_t c)
{
    c -= 0x20;
	scratch_offset = (scratch_offset == 0) ? 8*8 : 0; 
	return lib2d.drawChar(x, y, (uint8_t*)lib2d.font->data + (c * lib2d.font->h), scratch + scratch_offset);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
uint16_t LIB2D_Text(uint16_t x, uint16_t y, const char *s)
{   
    while(*s){
        x = LIB2D_Char(x, y, *s++);
    }
	return x;
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_Init(void)
{
	lib2d.font = &defaultFont;
	lib2d.forecolor = LCD_WHITE;
	lib2d.backcolor = LCD_BLACK;
	lib2d.vspace = 0;
	lib2d.cx = 0;
	lib2d.cy = 0;
	lib2d.sc = 0;
	//lib2d.xputc = DISPLAY_putc;
	lib2d.drawChar = drawCharSimple;
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_SetFont(font_t *fnt)
{

#ifndef SINGLE_FONT
	uint8_t mod, h;
	lib2d.font = fnt;

	h = lib2d.font->h;

	do{
		mod = LCD_GetHeight()  - (LCD_GetHeight() / h) * h;
		h++;
	}while(mod);

	lib2d.vspace = (h - 1) - lib2d.font->h;

#endif			
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_SetAttribute(uint8_t atr){
	switch(atr){
		default:
		case FONT_NORMAL: lib2d.drawChar = drawCharSimple;break;
		case FONT_INVERTED: lib2d.drawChar = drawCharInverted;break;
		case FONT_DOUBLE: lib2d.drawChar = drawCharDouble;break;
		case FONT_TRANSPARENT: lib2d.drawChar = drawCharTransparent;break;
	}
}
//----------------------------------------------------------
// setwrap(sx,sy,w,h) tem de ser chamado anteriormente
// a ESTE drawPixel(x,y) pixel com cor Foreground color
//----------------------------------------------------------
void LIB2D_Pixel(uint16_t x0, uint16_t y0)
{
    LCD_Pixel(x0, y0, lib2d.forecolor);
}

void LIB2D_HLine(uint16_t x, uint16_t y, uint16_t width)
{
	LCD_FillRect(x, y, width, 1, lib2d.forecolor);
}

void LIB2D_VLine(uint16_t x, uint16_t y, uint16_t height)
{
	LCD_FillRect(x, y, 1, height, lib2d.forecolor);
}

void LIB2D_Line(uint16_t x1, uint16_t y1,  uint16_t x2, uint16_t y2)
{
	LCD_Line(x1, y1, x2, y2, lib2d.forecolor);
}

void LIB2D_FillRect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h)
{
	LCD_FillRect(x, y, w, h, lib2d.forecolor);
}

void LIB2D_Rect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h)
{
	LCD_FillRect(x, y, w, 1, lib2d.forecolor);	
	LCD_FillRect(x + w , y, 1, h+1, lib2d.forecolor);	
	LCD_FillRect(x, y + h , w, 1, lib2d.forecolor);	
	LCD_FillRect(x ,y ,1, h, lib2d.forecolor);   
}

void LIB2D_FillRoundRect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h)
{
 	LCD_FillRect(x + 1, y, w-2, h, lib2d.forecolor);
    h -= 2;
    LCD_FillRect(x, y+1, 1, h, lib2d.forecolor);
    LCD_FillRect(x+w-1, y+1, 1, h, lib2d.forecolor);
}


//----------------------------------------------------------
//stdout calls this function
//----------------------------------------------------------
void LIB2D_Putc(char c)
{
	if (c == '\n' || lib2d.cx == LCD_GetWidth()){
		lib2d.cx = 0;
		lib2d.cy += lib2d.font->h + lib2d.vspace;
		if(lib2d.cy >= LCD_GetHeight()){
			lib2d.cy = 0;
			if(!lib2d.sc)
				lib2d.sc = 1;
		}
		if(lib2d.sc){
			LCD_Scroll(lib2d.cy + lib2d.font->h + lib2d.vspace);
			LCD_FillRect(0, lib2d.cy, LCD_GetWidth(), lib2d.font->h + lib2d.vspace, lib2d.backcolor);		
			lib2d.cx = 0;
		}
		if(c == '\n')
			return;	
	}
	if(c == '\r'){
		lib2d.cx = 0;
		return;
	}	
	lib2d.cx = LIB2D_Char(lib2d.cx, lib2d.cy, c);
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
void LIB2D_String(const char *s){   
    while(*s)
        LIB2D_Putc(*s++);	
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
void xitoa (long val, signed char radix, signed char len){
	unsigned char c, r, sgn = 0, pad = ' ';
	unsigned char s[16], i = 0;
	unsigned int v;

	if (radix < 0) {
		radix = -radix;
		if (val < 0) {
			val = -val;
			sgn = '-';
		}
	}
	
	v = val;
	r = radix;
	
	if (len < 0) {
		len = -len;
		pad = '0';
	}
	
	if (len > 16) len = 16;
	
	do {
		c = (unsigned char)(v % r);
		if (c >= 10) c += 7;
		c += '0';
		s[i++] = c;
		v /= r;
	} while (v);
	
	if(sgn) 
		s[i++] = sgn;
	
	while (i < len)
		s[i++] = pad;
		
	do{
		LIB2D_Putc(s[--i]);
	}while (i);
}
//-----------------------------------------------------------
//https://en.wikipedia.org/wiki/Single-precision_floating-point_format
//https://wirejungle.wordpress.com/2011/08/06/displaying-floating-point-numbers
//-----------------------------------------------------------
void xftoa(double f, char places){
long int_part, frac_part;
char prec;
  
    int_part = (long)(f);  
    
	if(places > LIB2D_FLOAT_MAX_PRECISION)
		places = LIB2D_FLOAT_MAX_PRECISION;
		
	frac_part = 0;
	prec = 0;
	while ((prec++) < places){
			f *= 10;
			frac_part = (frac_part * 10) + (long)f - ((long)f / 10) * 10;  //((long)f%10);			
	}

    xitoa(int_part, -10, 0);
    LIB2D_Putc('.');   
    xitoa(abs(frac_part), 10, -places);  
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
void LIB2D_Print(const char* str, ...){
	va_list arp;
	signed char d, r, w, s, l, f;

	va_start(arp, str);

	while ((d = *str++) != 0) {
		if (d != '%') {
			LIB2D_Putc(d); 
			continue;
		}
		
		d = *str++; 
		w = r = s = l = f = 0;
		
		if(d == '.'){
            f = 1;
			d = *str++; 
		}		
		
		if (d == '0') {  // printf("%2X",3) produces  3, printf("%02X",3) produces in 03
            s = 1;
			d = *str++; 
		}
		
		while ((d >= '0')&&(d <= '9')) {
			w += (w * 10) + (d - '0');
			d = *str++;
		}		
		
		if (d == 'l') {
			l = 1;
			d = *str++;
		}
		
		if (!d) break;
		
		if (d == 's') {
			LIB2D_String(va_arg(arp, char*));
			continue;
		}
		
		if (d == 'c') {
			LIB2D_Putc(va_arg(arp, int));
			continue;
		}
		
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X' || d == 'x') r = 16;
		if (d == 'b') r = 2;

		if(d == 'p'){
			LIB2D_String("0x");
			r = 16;
		}		
		
		if(d == 'f'){
			if(!f)
				w = 6;						// dafault 6 decimal places
			xftoa(va_arg(arp, double), w);			
			continue;
		}		
		
		if (!r) break;
		
		if (s) w = -w;
		
		if (l) {
			xitoa(va_arg(arp, long), r, w);
		} else {
			if (r > 0)
				xitoa((long)va_arg(arp, unsigned int), r, w);
			else
				xitoa((long)va_arg(arp, int), r, w);
		}
	}

	va_end(arp);
}

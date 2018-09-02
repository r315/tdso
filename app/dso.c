#include <string.h>
#include <display.h>
#include <button.h>
#include <common.h>
#include <math.h>
#include "dso.h"
#include "tdso_util.h"
#include "capture.h"
#include "control.h"
#include "softpower.h"
#include "led.h"

int16_t screenwave[DSO_GRID_W +1];

static Dso dso;
static const uint16_t tbases[] =  {10, 20, 50, 100, 200, 500, TB_1MS, 2000, 5000, 10000, 20000, 50000};  //us values
static const uint16_t vscales[] = {500, VS_1V , 2000, 5000};//{10, 20, 50, 100, 200, 500, VS_1V , 2000, 5000};  //mV values

static const uint8_t tgmodes[] = {TRIGGER_AUTO, TRIGGER_NORMAL, TRIGGER_SINGLE};
static const uint8_t tgrisingedge[] = {TRIGGER_RISING_EDGE, 0x07,0x0B,0x1e,0x10,0x10,0x10,0x38,0x7c,0xfe,0x10,0x10,0x10,0xf0};
static const uint8_t tgfallingedge[] = {TRIGGER_FALLING_EDGE, 0x07,0x0B,0xf0,0x10,0x10,0x10,0xfe,0x7c,0x38,0x10,0x10,0x10,0x1e};
static const uint8_t schannels[] = {DSO_SIGNAL0_CHANNEL};

 
int16_t samples[DSO_MAX_SAMPLES];

uint8_t DSO_ShowMenu(Menu *dm);
void DSO_HideMenu(Menu *dm);
uint8_t DSO_MenuSelector(void*);
uint8_t DSO_ChannelMoveVpos(void*);
uint8_t DSO_SelectHpos(void*);
uint8_t DSO_SelectTrigger(void*);
uint8_t DSO_ChannelVscale(void*);
uint8_t DSO_SelectTbase(void*);
uint8_t DSO_TriggerLevel(void*);
uint8_t DSO_TriggerMode(void*);
uint8_t DSO_TriggerEdge(void*);
void DSO_TriggerStatus(uint8_t tgs);

Menuentry tmenu [] = {
    {"Level", null, DSO_TriggerLevel},
    {"Auto", (void*)(tgmodes), DSO_TriggerMode},
    {"Normal", (void*)(tgmodes + 1), DSO_TriggerMode},
    {"Single", (void*)(tgmodes + 2), DSO_TriggerMode},
    {"Rising", (void*)tgrisingedge, DSO_TriggerEdge},
    {"Falling", (void*)tgfallingedge, DSO_TriggerEdge},
};

Menu triggermenu = {
  .x = 208,
  .y = 5,
  .w = 85,
  .h = sizeof(tmenu)/sizeof(Menuentry) * DSO_MENU_ITEM_SPACING,
  .items = tmenu,
  .nitems = sizeof(tmenu)/sizeof(Menuentry),
  .fcolor = WHITE,
  .bcolor = RGB(8,16,8),
  .sfcolor = WHITE,
  .sbcolor = RGB(6,18,19),
  .select = 0,
  .visible = OFF
};

Menuentry fmenu [] = {
    {"T/Div",null,DSO_SelectTbase},
    {"H Pos",null,DSO_SelectHpos},
    {"Trigger",(void*)&triggermenu,DSO_MenuSelector},
    {"V/Div",(void*)(schannels+0),DSO_ChannelVscale},
    {"V Pos",(void*)(schannels+0),DSO_ChannelMoveVpos}
};

Menu mainmenu = {
  .x = 208,
  .y = 5,
  .w = 85,
  .h = sizeof(fmenu)/sizeof(Menuentry) * DSO_MENU_ITEM_SPACING,
  .items = fmenu,
  .nitems = sizeof(fmenu)/sizeof(Menuentry),
  .fcolor = WHITE,
  .bcolor = RGB(8,16,8),
  .sfcolor = WHITE,
  .sbcolor = RGB(6,18,19),
  .select = 0,
  .visible = OFF
};

/*
 * @brief move through samples
 * @param distance to move, range -52:52
 */
void DSO_MoveHpos(int8_t dist){
int16_t newpos = dso.hpos + dist;
uint8_t dx;

    if(newpos < -((DSO_HPOS_W/2)-(DSO_HPOS_BOX_W/2)) || newpos > ((DSO_HPOS_W/2)-(DSO_HPOS_BOX_W/2)))
        return;

    dx = (dist < 0 ) ? -dist : dist;

    // Redraw hposition top box
    if(newpos >= dso.hpos ){
        LCD_FillRect(DSO_HPOS_BOX_ORIGIN + dso.hpos, DSO_HPOS_ORIGIN_Y, dx ,DSO_HPOS_H, DSO_BACKGROUND);
        LCD_Line(DSO_HPOS_BOX_ORIGIN + dso.hpos+dx, DSO_HPOS_ORIGIN_Y + (DSO_HPOS_H/2), DSO_HPOS_BOX_ORIGIN + dso.hpos, DSO_HPOS_ORIGIN_Y + (DSO_HPOS_H/2), DSO_HPOS_COLOR);
    }
    else{
        LCD_FillRect(DSO_HPOS_BOX_ORIGIN + dso.hpos + DSO_HPOS_BOX_W - dx , DSO_HPOS_ORIGIN_Y, dx ,DSO_HPOS_H, DSO_BACKGROUND);
        LCD_Line(DSO_HPOS_BOX_ORIGIN + DSO_HPOS_BOX_W + dso.hpos, DSO_HPOS_ORIGIN_Y + (DSO_HPOS_H/2), DSO_HPOS_BOX_ORIGIN + DSO_HPOS_BOX_W + dso.hpos - dx, DSO_HPOS_ORIGIN_Y + (DSO_HPOS_H/2), DSO_HPOS_COLOR);
    }

    dso.hpos = newpos;
    LCD_FillRect(DSO_HPOS_BOX_ORIGIN + dso.hpos, DSO_HPOS_ORIGIN_Y, DSO_HPOS_BOX_W, DSO_HPOS_H, DSO_HPOS_BOX_COLOR);
    //printf("%u (%d)\n",(uint16_t)dso.hpos, dso.hpos);
}

/**
 * @brief grid stuff
 **/
void DSO_DrawHpos(uint8_t hp){
    LCD_Line(DSO_HPOS_ORIGIN_X-1-(DSO_HPOS_W/2), DSO_HPOS_ORIGIN_Y + (DSO_HPOS_H/2), DSO_HPOS_ORIGIN_X + (DSO_HPOS_W/2), DSO_HPOS_ORIGIN_Y + (DSO_HPOS_H/2), DSO_HPOS_COLOR);
    LCD_Line(DSO_HPOS_ORIGIN_X-1-(DSO_HPOS_W/2), DSO_HPOS_ORIGIN_Y-1, DSO_HPOS_ORIGIN_X-1-(DSO_HPOS_W/2) , DSO_HPOS_ORIGIN_Y-1 + DSO_HPOS_H, DSO_HPOS_COLOR);
    LCD_Line(DSO_HPOS_ORIGIN_X+1+(DSO_HPOS_W/2), DSO_HPOS_ORIGIN_Y-1, DSO_HPOS_ORIGIN_X+1+ (DSO_HPOS_W/2) , DSO_HPOS_ORIGIN_Y - 1 + DSO_HPOS_H, DSO_HPOS_COLOR);
    DSO_MoveHpos(0);
}


 /**
  *
  **/
void DSO_DrawGridSlice(int16_t p1, int16_t p2, uint16_t index){
uint16_t dy; 

    if(p1 > p2){
        dy = p2;
        p2 = p1;
        p1 = dy;
    }
    
    dy = p2 - p1;
    
    dy = (dy > DSO_GRID_H)? DSO_GRID_H : dy;
    
    p1 = (p1 < -(DSO_GRID_H/2)) ? -(DSO_GRID_H/2) : p1;    // clip to window
    p2 = (p2 >  (DSO_GRID_H/2)) ? (DSO_GRID_H/2) : p2;
    
    LCD_Window(index + DSO_GRID_ORIGIN_X, p1 + DSO_GRID_CENTER, 1, (dy > 0) ? dy + 1 :1);
	#if defined(__TDSO__)
    LCD_CS0;
	#endif
    
    if( ((index % (DSO_GRID_W / DSO_GRID_H_DIVISIONS)) == 0) ||
            index == ((DSO_GRID_W /2) - 1) || index == ((DSO_GRID_W /2) + 1)){  
        //vertical grid rule
        while(p1 <= p2 ){
            if(((p1 % DSO_GRID_DOT_SPACE) == 0) || 
                ( ((p1 == -1) || (p1 == 1)) && (index != ((DSO_GRID_W/2) -1)) && (index != ((DSO_GRID_W/2)) +1) )){
				#if defined(__TDSO__)
                SPI_Send(DSO_GRID_COLOR>>8);
                SPI_Send(DSO_GRID_COLOR);
				#else
				LCD_Data(DSO_GRID_COLOR);
				#endif
            }else{
				#if defined(__TDSO__)
            	SPI_Send(BLACK>>8);
            	SPI_Send(BLACK);
				#else
				LCD_Data(BLACK);
				#endif
            }
            p1++;
        }
    }else{
        if( ((index % DSO_GRID_DOT_SPACE) == 0) ){
            //horizontal grid rule
            while(p1 <= p2 ){
                if(((p1 % (DSO_GRID_H / DSO_GRID_V_DIVISIONS)) == 0) ||
                     (p1 == -1) || (p1 == 1)){
					#if defined(__TDSO__)
                	SPI_Send(DSO_GRID_COLOR>>8);
                	SPI_Send(DSO_GRID_COLOR);
					#else
					LCD_Data(DSO_GRID_COLOR);
					#endif
                }else{
					#if defined(__TDSO__)
                	SPI_Send(BLACK>>8);
                	SPI_Send(BLACK);
					#else
					LCD_Data(BLACK);
					#endif
                }
                p1++;
            }
        }else{
            // no grid rule
            while(p1 <= p2 ){
				#if defined(__TDSO__)
            	SPI_Send(BLACK>>8);
            	SPI_Send(BLACK);
				#else
				LCD_Data(BLACK);
				#endif
                p1++;
            }
        }
    }
	#if defined(__TDSO__)
    LCD_CS1;
	#endif
}

void DSO_DrawGridHline(uint16_t y){
uint16_t i;
    y += DSO_GRID_ORIGIN_Y;
    for(i=0; i < DSO_GRID_W; i+= DSO_GRID_DOT_SPACE){
        LCD_Pixel(DSO_GRID_ORIGIN_X + i, y, DSO_GRID_COLOR);
    }
}

void DSO_DrawGridVline(uint16_t x){
uint16_t i;
    x += DSO_GRID_ORIGIN_X;
    for(i=0; i < DSO_GRID_H; i+= DSO_GRID_DOT_SPACE){
        LCD_Pixel(x,DSO_GRID_ORIGIN_Y + i, DSO_GRID_COLOR);
    }
}

void DSO_DrawGrid(void){
uint16_t i;
uint8_t hspace = DSO_GRID_W / DSO_GRID_H_DIVISIONS, vspace = DSO_GRID_H / DSO_GRID_V_DIVISIONS;
    
    for(i=hspace; i < DSO_GRID_W; i+=hspace ){      //vertical dots
        DSO_DrawGridVline(i);
    }
    
    for(i=vspace; i < DSO_GRID_H; i+=vspace ){      // Horizontal dots
        DSO_DrawGridHline(i);
    }
    
    DSO_DrawGridVline((DSO_GRID_W/2) - 1);          //Y axis Highlight
    DSO_DrawGridVline((DSO_GRID_W/2) + 1);
    
    DSO_DrawGridHline((DSO_GRID_H/2) - 1);          //X axis Highlight
    DSO_DrawGridHline((DSO_GRID_H/2) + 1);
    
    LCD_Rect(DSO_GRID_ORIGIN_X - 1, DSO_GRID_ORIGIN_Y - 1, DSO_GRID_W + 1, DSO_GRID_H + 1, DSO_FRAME_COLOR);    //Outside frame
}

void DSO_ClearGrid(void){

#if 1   
    LCD_FillRect(DSO_GRID_ORIGIN_X, DSO_GRID_ORIGIN_Y, DSO_GRID_W, DSO_GRID_H, BLACK);  
    DSO_DrawGrid();
#else
    for(i = 0; i< DSO_GRID_W; i++)
        DSO_DrawGridSlice(-(DSO_GRID_H/2),(DSO_GRID_H/2),i);
#endif
    
}

/**
  * @brief Draws triangle shape cursor indicating the channel position
  **/
void DSO_ChannelCursor(Channel *ch){
uint8_t a;
    if(!ch->active)
        return;
        
    for(a = 0; a < DSO_CURSOR_W; a++){
        LCD_FillRect(
            (DSO_GRID_ORIGIN_X - 2 - DSO_CURSOR_W) + a, // cursor is drawn 2 pixel before grid
            DSO_CURSOR_ORIGIN + ch->pos + a,            // Cursor center + offset
            1,                                          // 1 pixel line
            DSO_CURSOR_H - (a * 2),                     // each vline of triangle is 2px smaller than previous
            ch->color);
    }
}

void DSO_ChannelUpdateVpos(Dso *ds, uint8_t last, uint8_t nchannels){
uint8_t i;
    for(i = last; i < last + nchannels; i++){
        DSO_ChannelCursor(&ds->channels[(i+1)%nchannels]);
    }
}

uint8_t DSO_ChannelMoveVpos(void *data){
int8_t step = 0;
int16_t newpos;
Channel *ch;

    if(BUTTON_GetEvents() == BUTTON_PRESSED || BUTTON_GetEvents() == BUTTON_HOLD){
        dso.channel = *((uint8_t*)(data));
        switch(BUTTON_GetValue()){
#if defined(__BLUEBOARD__)
            case BUTTON_UP:
                //DSO_ChannelMoveVpos(&dso.channels[0].cursor,-1);
                break;
            case BUTTON_DOWN:
                //DSO_ChannelMoveVpos(&dso.channels[0].cursor,1);
                break;

            case BUTTON_LEFT:
            case BUTTON_RIGHT:
                //DSO_ChannelMoveVpos(&dso.channels[0].cursor,-dso.channels[0].cursor.pos);
                break;
#else
            case BUTTON_LEFT:
                 step = -1;
                 break;
            case BUTTON_RIGHT:
                 step = 1;
                 break;
#endif
            case BUTTON_A:
                return ON;
            default:
                break;
        }
    

        ch = &dso.channels[dso.channel];
        newpos = ch->pos + step;

        if(newpos < -((DSO_GRID_H/2)) || newpos > ((DSO_GRID_H/2)))
            return OFF;

        LCD_FillRect((DSO_GRID_ORIGIN_X - 2 - DSO_CURSOR_W),
                DSO_CURSOR_ORIGIN + ch->pos,
                DSO_CURSOR_W,
                DSO_CURSOR_H,
                DSO_BACKGROUND);
        ch->pos = newpos;

        DSO_ChannelUpdateVpos(&dso, dso.channel, DSO_MAX_CHANNELS);
        CONTROL_SetVpos(&dso.channels[dso.channel]);
    }
    return OFF;
}

uint16_t DSO_ChannelUpdateVscale(Channel *ch){
uint16_t scale = vscales[ch->scaleidx];

    DISPLAY_SetFcolor(DSO_TRACE_COLOR);    
    LCD_FillRoundRect(
            DSO_VSCALE_DRO_X,
            DSO_VSCALE_DRO_Y,
            DISPLAY_GetFontWidth() * DSO_VSCALE_DRO_W,
            DISPLAY_GetFontHeight() + DSO_VSCALE_DRO_H,
            BLACK);
             
    DISPLAY_GotoAbsolute(
    		DSO_VSCALE_DRO_X + DISPLAY_GetFontWidth(),
            DSO_VSCALE_DRO_Y + DSO_VSCALE_DRO_H/2);

    DISPLAY_SetColors(ch->color, BLACK);

    if(scale < VS_1V){
        DISPLAY_printf("%umV",scale);
    }else{
        DISPLAY_printf("%uV", scale/VS_1V);
    }
    return scale;
}

uint8_t DSO_ChannelVscale(void *data){
uint16_t index;
    dso.channel = *((uint8_t*)data);
    index = dso.channels[dso.channel].scaleidx;
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        switch(BUTTON_GetValue()){
            case BUTTON_RIGHT:
                if( index < (sizeof(vscales)/sizeof(uint16_t))-1)
                    index++;
                break;

            case BUTTON_LEFT:
                if(index > 0)
                    index--;
                break;

            case BUTTON_A:
                return ON;

            default:
                break;
        }
        dso.channels[dso.channel].scaleidx = index;
        DSO_ChannelUpdateVscale(&dso.channels[dso.channel]);
        CONTROL_SetGain(dso.channels[dso.channel].scaleidx);

    }
    return OFF;
}

void DSO_DrawMenuItem(Menu *dm, uint8_t item, uint8_t selected){
uint16_t x,y;
    
    x = dm->x + DSO_GRID_ORIGIN_X;
    y = dm->y + DSO_GRID_ORIGIN_Y;    
    
    if( selected ){
        DISPLAY_SetFcolor(dm->sfcolor);
        DISPLAY_SetBcolor(dm->sbcolor);
        LCD_FillRect(x,         
                     y + DSO_MENU_TOP_PADDING - 1 +
                    ((DISPLAY_GetFontHeight() + DSO_MENU_TOP_PADDING) * item),
                    dm->w,
                    DISPLAY_GetFontHeight() + 2,
                    dm->sbcolor);
    }else{
        DISPLAY_SetFcolor(dm->fcolor);
        DISPLAY_SetBcolor(dm->bcolor);
        LCD_FillRect(x,         
                     y + DSO_MENU_TOP_PADDING -1 +
                    ((DISPLAY_GetFontHeight() + DSO_MENU_TOP_PADDING) * item),
                    dm->w,
                    DISPLAY_GetFontHeight() + 2,
                    dm->bcolor);
    }
    
    DISPLAY_Text(x + DSO_MENU_LEFT_PADDING,         
                 y + DSO_MENU_TOP_PADDING + 
                 ((DISPLAY_GetFontHeight() + 
                 DSO_MENU_TOP_PADDING) * item),
                 dm->items[item].name);
}

uint8_t DSO_ShowMenu(Menu *dm){
int8_t i;  

//printf("draw menu %u\n", dm->x);
//DISPLAY_SetFont(corrierFont);
    if(!dm->visible){
        //dm->h = dm->nitems * (DISPLAY_GetFontHeight() + DSO_MENU_ITEM_SPACING);
        LCD_FillRect(dm->x + DSO_GRID_ORIGIN_X,
                     dm->y + DSO_GRID_ORIGIN_Y,
                     dm->w, dm->h, dm->bcolor);

        for(i=0; i< dm->nitems; i++){           
            DSO_DrawMenuItem(dm, i, i == dm->select ? ON : OFF);
        }
        dm->visible = ON;  
    }
    
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        switch(BUTTON_GetValue()){
            case BUTTON_LEFT:
                DSO_DrawMenuItem(dm, dm->select, OFF);
                if((--dm->select) < 0)
                    dm->select = 0;         
                break;
                    
            case BUTTON_RIGHT:
                DSO_DrawMenuItem(dm, dm->select, OFF);                             
                if((++dm->select) > dm->nitems - 1)
                    dm->select = dm->nitems - 1;    
                break;

            case BUTTON_A:
                dm->visible = OFF;
                return ON;                
                
            default:
                return OFF;
        }             
        DSO_DrawMenuItem(dm, dm->select, ON);
        DISPLAY_SetBcolor(BLACK);
    }  
     
//DISPLAY_SetFont(defaultFont);  
    return OFF;
}

void DSO_HideMenu(Menu *dm){
uint16_t i;
    for(i = dm->x; i < dm->x + dm->w; i++)
        DSO_DrawGridSlice(
            -((DSO_GRID_H/2) - dm->y) , 
            -((DSO_GRID_H/2) - dm->y) + dm->h , 
            i);
    dm->visible = OFF;
}

uint16_t DSO_DrawTimeBase(Dso *ds){
uint16_t timebase = tbases[ds->timebase];

    LCD_FillRoundRect(
            DSO_TIMEBASE_DRO_X,
            DSO_TIMEBASE_DRO_Y    ,
            DISPLAY_GetFontWidth() * DSO_TIMEBASE_DRO_W,
            DISPLAY_GetFontHeight() + DSO_TIMEBASE_DRO_H,
            BLACK);
             
    DISPLAY_GotoAbsolute(
    		DSO_TIMEBASE_DRO_X + DISPLAY_GetFontWidth(),
            DSO_TIMEBASE_DRO_Y + DSO_TIMEBASE_DRO_H/2);

    DISPLAY_SetColors(DSO_DEFAULT_COLOR, BLACK);

    if(timebase < TB_1MS){
        DISPLAY_printf("%uus", timebase);
    }else{
        DISPLAY_printf("%ums", timebase/TB_1MS);
    }
    return timebase;
}
   

uint8_t DSO_SelectHpos(void *data){
    if(BUTTON_GetEvents() == BUTTON_PRESSED || BUTTON_GetEvents() == BUTTON_HOLD){
            switch(BUTTON_GetValue()){
                case BUTTON_LEFT:
                    DSO_MoveHpos(-1);
                    break;

                case BUTTON_RIGHT:
                    DSO_MoveHpos(1);
                    break;
#if defined(__BLUEBOARD__)
                case BUTTON_UP:
                case BUTTON_DOWN:
                    DSO_MoveHpos(-dso.hpos);
                    break;
#endif
                case BUTTON_A:
                    return ON;
                default:
                    break;
                }   
        }
        return OFF;
}

uint8_t DSO_SelectTbase(void *data){

    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        switch(BUTTON_GetValue()){
            case BUTTON_RIGHT:              
                if(dso.timebase < (sizeof(tbases)/sizeof(uint16_t))-1)
                    dso.timebase++;
                    //printf("Button RIGHT %u\n",BUTTON_RIGHT);                
                break;
                    
            case BUTTON_LEFT:
                if( dso.timebase > 0 )
                    dso.timebase--;
                break;

            case BUTTON_A:
                return ON;
                    
            default:
                break;;
            }
            CAP_SetSampleRate(DSO_DrawTimeBase(&dso) / DSO_SAMPLES_PER_DIVISION);
    }
    return OFF; 
}

void DSO_TriggerCursor(Trigger *tg){
int8_t i;
    for(i = DSO_CURSOR_W - 1; i >= 0 ; --i){
        LCD_FillRect(
            (DSO_GRID_ORIGIN_X + DSO_GRID_W ) + DSO_CURSOR_W - i,     // cursor is drawn one pixel after grid
            DSO_CURSOR_ORIGIN + tg->pos + i,                          // Cursor center + offset
            1,                                                        // 1 pixel line
            DSO_CURSOR_H - (i * 2),                                   // each line is bigger by 2 px
            tg->color);
    }
}

uint8_t DSO_TriggerLevel(void *data){
int8_t step = 0;
int16_t newpos;
    if(BUTTON_GetEvents() == BUTTON_PRESSED || BUTTON_GetEvents() == BUTTON_HOLD){
        switch(BUTTON_GetValue()){
#if defined(__BLUEBOARD__) || defined(__EMU__)
            case BUTTON_UP:
            	step = -1;
                break;
                    
            case BUTTON_DOWN:
            	step = 1;
                break;
                
            case BUTTON_LEFT:
            case BUTTON_RIGHT:
                step = -dso.trigger.pos ;
                break;
#else
            case BUTTON_LEFT:
            	step = -1;
                 break;

            case BUTTON_RIGHT:
                 step = 1;
                 break;
#endif
            case BUTTON_A:
                return ON;

            default:
                break;
        }


          newpos = dso.trigger.pos + step;

          if(newpos < -((DSO_GRID_H/2)) || newpos > ((DSO_GRID_H/2)))
              return OFF;

          LCD_FillRect(
                      (DSO_GRID_ORIGIN_X + DSO_GRID_W + 1), // erase needs offset of one pixel
                      DSO_CURSOR_ORIGIN + dso.trigger.pos,
                      DSO_CURSOR_W,
                      DSO_CURSOR_H,
                      DSO_BACKGROUND);

          dso.trigger.pos = newpos;
          //dso.trigger.level = DSO_TGOFFSET_LEVEL + newpos;
        DSO_TriggerCursor(&dso.trigger);
        CONTROL_SetTriggerLevel(dso.trigger.pos);
    }
    return OFF; 
}

/*
 * @brief this function changes trigger mode and change the current control to
 * time base changing, check if this feature is useful
 */
uint8_t DSO_TriggerMode(void *data){
    dso.trigger.mode = *((uint8_t*)data);
    DSO_TriggerStatus(dso.trigger.mode);
    dso.activeControl = DSO_SelectTbase;
    return OFF;
}

uint8_t DSO_TriggerEdge(void *data){
uint8_t i, iw, ih, r, *icon = (uint8_t*)(data + DSO_ICON_DATA_OFFSET);
    iw = *((uint8_t*)(data + DSO_ICON_W_OFFSET));
    ih = *((uint8_t*)(data + DSO_ICON_H_OFFSET));

    dso.trigger.edge = *((uint8_t*)(data));

    LCD_FillRoundRect(
            DSO_TGMODE_DRO_X,
            DSO_TGMODE_DRO_Y,
            DSO_TGMODE_DRO_W,
            DSO_TGMODE_DRO_H,
            BLACK);

    LCD_Window(DSO_TGEDGE_DRO_X, DSO_TGEDGE_DRO_Y, iw, ih);

    while(ih--){
        r = *icon++;
        for(i=0; i < iw; i++){
            if(r & (0x80>>i))
                LCD_Data(DSO_TG_EDGE_COLOR);
            else
                LCD_Data(BLACK);
        }
    }

    CONTROL_SetTriggerEdge(dso.trigger.edge);
    dso.activeControl = DSO_SelectTbase;
    return OFF;
}

void DSO_TriggerStatus(uint8_t tgs){
char *st;
    LCD_FillRoundRect(
            DSO_TGSWEEP_DRO_X,
            DSO_TGSWEEP_DRO_Y,
            DSO_TGSWEEP_DRO_W,
            DSO_TGSWEEP_DRO_H,
            BLACK);

    DISPLAY_SetFcolor(DSO_TGSWEEP_DRO_COLOR);

    switch(tgs & (TRIGGER_STATUS_MASK | TRIGGER_MODE_MASK)){
        case TRIGGER_STOP:
            DISPLAY_SetFcolor(RED);
            st = "STOP";
            break;
        case TRIGGER_READY:
            st = "READY";
            break;
        case TRIGGER_AUTO:
            st = "AUTO";
            break;
        case TRIGGER_NORMAL:
            st = "NORM";
            break;

        default:
            return;
    }
    DISPLAY_Text(DSO_TGSWEEP_DRO_X + 6, DSO_TGSWEEP_DRO_Y + 2, st);
}

void DSO_TriggerInfo(Trigger *tg){
    //TODO Add info on trigger level and edge
}


uint8_t DSO_MenuSelector(void *data){
Menuentry *entry;
    if(DSO_ShowMenu((Menu*)data)){
        DSO_HideMenu((Menu*)data);
        entry = &((Menu*)data)->items[((Menu*)data)->select];
        dso.activeControl = entry->function;
        dso.menudata = entry->data;
    }
    return OFF;
}

void DSO_DrawSample(Channel *ch, uint16_t sindex, uint16_t index){
uint16_t nextindex = (sindex + 1) & (DSO_MAX_SAMPLES - 1);
int16_t p1, p2;
    sindex = sindex & (DSO_MAX_SAMPLES - 1);

    p1 = map(ch->buffer[sindex] + DSO_ZERO_CAL, 0 , DSO_SAMPLE_MAX_VALUE, (DSO_GRID_H/2), -(DSO_GRID_H/2));        // map sample value to display grid
    p2 = map(ch->buffer[nextindex] + DSO_ZERO_CAL, 0 , DSO_SAMPLE_MAX_VALUE, (DSO_GRID_H/2), -(DSO_GRID_H/2));
    
    //p1 += ch->pos;      // add channel offset
    //p2 += ch->pos;

    p1 = (p1 > (DSO_GRID_H/2)) ? (DSO_GRID_H/2) : p1;     // clip point1 top if off screen
    p1 = (p1 < -(DSO_GRID_H/2)) ? -(DSO_GRID_H/2) : p1;   // clip point bottom if off screen

    screenwave[index] = p1;    

    if( p1 == p2){
        LCD_Pixel(index + DSO_GRID_ORIGIN_X, p1 + DSO_GRID_CENTER, ch->color);
    }
    else{
        p2 = (p2 > (DSO_GRID_H/2)) ? (DSO_GRID_H/2) : p2;   // clip point2 top
        p2 = (p2 < -(DSO_GRID_H/2)) ? -(DSO_GRID_H/2) : p2; // clip point2 bottom
        LCD_Line(index + DSO_GRID_ORIGIN_X, DSO_GRID_CENTER + p1, 
                 index + DSO_GRID_ORIGIN_X, DSO_GRID_CENTER + p2, 
                 ch->color);
    }
}

void DSO_DrawWaves(void){
uint16_t i, sidx;
  
    UTIL_StartTiming();
    sidx = CAP_GetTriggerIndex();

    if(sidx != OFF){
        sidx -= DSO_MAX_SAMPLES/2;
    }

    for(i = 0; i < DSO_GRID_W; i++, sidx++){
        //sidx &= ((DSO_MAX_SAMPLES/2) - 1);
        DSO_DrawGridSlice(screenwave[i], screenwave[i + 1], i);
        DSO_DrawSample(&dso.channels[0], sidx, i); 
        #ifndef DSO_SINGLE_CHANNEL
        DSO_DrawSample(&dso.channels[1], sidx, i); 
        #endif
    }

    UTIL_StopTiming();
    UTIL_wps();
}



void DSO_systemInfo(void){
	DISPLAY_printf("System Clock: %uHz\n", SystemCoreClock);
	DISPLAY_printf("Sample Memory: %u bytes\n",DSO_MAX_CHANNELS * DSO_MAX_SAMPLES);
   DISPLAY_printf("Version: %s\n",DSO_VERSION);
#ifdef LCD_USE_ID
	DISPLAY_printf("LCD ID: 0x%X\n",LCD_ReadID());
#endif
	LCD_Bkl(ON);
	DelayMs(1000);
    LCD_Bkl(OFF);
}

void DSO_Init(void){

	DSO_systemInfo();

    BUTTON_SetHoldTime(200);        
    
    LCD_Clear(DSO_BACKGROUND);

    DSO_ClearGrid();    

    dso.hpos = 0;
    DSO_DrawHpos(DSO_HPOS_W/2);

    dso.timebase = TB_1MS_IDX - 3;
    DSO_DrawTimeBase(&dso);
    
    dso.trigger.index = (DSO_GRID_W / 2);
    dso.channel = DSO_SIGNAL0_CHANNEL;   
   
    dso.trigger.color = ORANGE;
    dso.trigger.mode = TRIGGER_AUTO;
    dso.trigger.edge = TRIGGER_RISING_EDGE;    
	dso.trigger.pos = 0;
    
    DSO_TriggerCursor(&dso.trigger);

    DSO_TriggerStatus(dso.trigger.mode);
    DSO_TriggerEdge((void*)tgrisingedge);

    dso.channels[DSO_SIGNAL0_CHANNEL].pos = 0;
    dso.channels[DSO_SIGNAL0_CHANNEL].scaleidx = VS_1V_IDX;
    dso.channels[DSO_SIGNAL0_CHANNEL].color = YELLOW;
    dso.channels[DSO_SIGNAL0_CHANNEL].active = ON;
    dso.channels[DSO_SIGNAL0_CHANNEL].buffer = samples;      //buffer  
    
#ifndef DSO_SINGLE_CHANNEL      
    dso.channels[1].cursor.pos = 0;
    dso.channels[1].cursor.color = BLUE;
    dso.channels[1].cursor.active = ON;    
#endif

    DSO_ChannelUpdateVscale(&dso.channels[DSO_SIGNAL0_CHANNEL]);

    CONTROL_Init(&dso);
    
    DSO_ChannelUpdateVpos(&dso,DSO_SIGNAL0_CHANNEL, DSO_MAX_CHANNELS);

    dso.activeControl = DSO_SelectTbase;
    dso.menudata = null;

    memset(screenwave,0,512); // clear old wave storage

    LCD_Bkl(ON);

    CAP_Init();
    CAP_SetSampleRate(tbases[dso.timebase] / DSO_SAMPLES_PER_DIVISION);
}

void DSO_Run(uint8_t doloop){
uint32_t ticks;
uint32_t btn;
	do{
      ticks = GetTicks();	   
		switch(dso.trigger.mode){
			case TRIGGER_AUTO:              // display signal even if trigger not present
                if(CAP_IsEnded()){
                   DSO_DrawWaves(); 
                   if( CAP_Triggered()){
                	   LED1_ON;
                	   DelayMs(10);
                	   LED1_OFF;
                   }
                   CAP_Start(dso.channels[DSO_SIGNAL0_CHANNEL].buffer, DSO_MAX_SAMPLES );
                }
                break;

			case TRIGGER_NORMAL:            // only update wave if signal reach trigger level
			    if(CAP_IsEnded()){
			        if(CAP_Triggered()){
			            DSO_DrawWaves();
			        }
			        CAP_Start(dso.channels[DSO_SIGNAL0_CHANNEL].buffer, DSO_MAX_SAMPLES );
			    }
			    break;

			case TRIGGER_SINGLE:            //TODO fix set trigger when navigating menus
			    if(CAP_IsEnded()){
			        if(dso.state != DSO_STOP){
			            dso.state = DSO_STOP;
			            DSO_DrawWaves();
			            DSO_TriggerStatus(TRIGGER_STOP);
			        }else{
			           if(BUTTON_GetEvents() == BUTTON_PRESSED){
			               btn = BUTTON_GetValue();
			               if( btn == BUTTON_LEFT || btn == BUTTON_RIGHT){
			                   CAP_Start(dso.channels[DSO_SIGNAL0_CHANNEL].buffer, DSO_MAX_SAMPLES ); //TODO set trigger waiting
			                   DSO_TriggerStatus(TRIGGER_READY);
			                   dso.state = DSO_READY;
			               }
			           }
			        }
			    }

				break;

			default: break;
		}

		if(SOFTPOWER_CheckStandby()){
			LCD_Clear(BLACK);
			DISPLAY_SetColors(WHITE, BLACK);
		    DISPLAY_Text(20,20, "Shuting Down!");
		    DelayMs(2000);
		    SOFTPOWER_PowerOff();
		}
      
		BUTTON_Read();
		if(dso.activeControl(dso.menudata)){
			//mainmenu.select = 0;
			dso.activeControl = DSO_MenuSelector;
			dso.menudata = &mainmenu;
		}

		ticks = ElapsedTicks(ticks) / 1000;  // convert to ms
		if(ticks < 20)                       // 50Hz refresh
		    DelayMs(20 - ticks);
            
	}while(doloop);
}


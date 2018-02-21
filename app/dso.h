#ifndef _dso_h_
#define _dso_h_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DSO_VERSION "V1.0"
#define DSO_BACKGROUND			RGB(4,8,6)

#define DSO_GRID_COLOR 			RGB(8,15,8)
#define DSO_FRAME_COLOR 		RGB(16,32,16) //WHITE
#define DSO_GRID_ORIGIN_X 		12
#define DSO_GRID_ORIGIN_Y 		16
#define DSO_GRID_W 301
#define DSO_GRID_H 201
#define DSO_GRID_H_DIVISIONS 	12
#define DSO_GRID_V_DIVISIONS 	8
#define DSO_GRID_DOT_SPACE 		5
#define DSO_GRID_CENTER 		(DSO_GRID_ORIGIN_Y + (DSO_GRID_H / 2))

#define DSO_TIMEBASE_DRO_X 		257
#define DSO_TIMEBASE_DRO_Y 		222
#define DSO_TIMEBASE_DRO_W 		7
#define DSO_TIMEBASE_DRO_H 		4
#define DSO_SAMPLES_PER_DIVISIO (DSO_GRID_W/DSO_GRID_H_DIVISIONS)

#define DSO_VSCALE_DRO_X 		DSO_GRID_ORIGIN_X //40
#define DSO_VSCALE_DRO_Y    	222
#define DSO_VSCALE_DRO_W    	7
#define DSO_VSCALE_DRO_H    	4
#define DSO_ZERO_CAL        	0

#define DSO_HPOS_ORIGIN_X 		(DSO_GRID_ORIGIN_X + (DSO_GRID_W/2))
#define DSO_HPOS_ORIGIN_Y 		2
#define DSO_HPOS_W 				128 //DSO_GRID_W/2
#define DSO_HPOS_H 				9
#define DSO_HPOS_COLOR 			CYAN
#define DSO_HPOS_BOX_W 			(DSO_GRID_W/DSO_GRID_H_DIVISIONS)
#define DSO_HPOS_BOX_ORIGIN 	(DSO_HPOS_ORIGIN_X - (DSO_HPOS_BOX_W/2))
#define DSO_HPOS_BOX_COLOR 		CYAN

#define DSO_TGSWEEP_DRO_X 		DSO_GRID_ORIGIN_X
#define DSO_TGSWEEP_DRO_Y 		2
#define DSO_TGSWEEP_DRO_W 		(6 * 8)
#define DSO_TGSWEEP_DRO_H 		(4 + 8)
#define DSO_TGSWEEP_DRO_COLOR 	GREEN
#define DSO_TGMODE_DRO_X 		265
#define DSO_TGMODE_DRO_Y 		2
#define DSO_TGMODE_DRO_W 		(6 * 8)
#define DSO_TGMODE_DRO_H 		(4 + 8)
#define DSO_TGMODE_DRO_COLOR 	GREEN
#define DSO_TGEDGE_DRO_X 		(DSO_TGMODE_DRO_X + 8)
#define DSO_TGEDGE_DRO_Y  		DSO_TGMODE_DRO_Y
#define DSO_TG_EDGE_COLOR 		CYAN

#define DSO_TRACE_COLOR 		CYAN
#define DSO_DEFAULT_COLOR 		WHITE

#define DSO_MAX_CURSORS 		2
#define DSO_CURSOR_H 			9
#define DSO_CURSOR_W 			5
#define DSO_CURSOR_ORIGIN 		(DSO_GRID_ORIGIN_Y + (DSO_GRID_H/2) - (DSO_CURSOR_H/2))

#define DSO_MENU_LEFT_PADDING 	5
#define DSO_MENU_TOP_PADDING  	5
#define DSO_MENU_ITEM_SPACING 	15

#define DSO_SINGLE_CHANNEL
#ifdef DSO_SINGLE_CHANNEL
#define DSO_MAX_CHANNELS 		1
#else
#define DSO_MAX_CHANNELS 		3
#endif

#define DSO_SIGNAL0_CHANNEL 	0
#define DSO_TRIGGER_CHANNEL 	1

#define DSO_MAX_SAMPLES 		2048
#define DSO_SAMPLE_MAX_VALUE 	4096

#define ON 1
#define OFF 0

#define TB_1MS 					1000
#define TB_1MS_IDX 				6
#define VS_1V 1000
#define VS_1V_IDX 				0//6

#ifdef __LPCXpresso__
#define DSO_ADC_CHANNEL 		ADC_5
#else
#define DSO_ADC_CHANNEL 		ADC_0
#endif

#define DSO_PWM_CHANNEL 		PWM_1

#define DSO_ICON_W_OFFSET    	1
#define DSO_ICON_H_OFFSET    	2
#define DSO_ICON_DATA_OFFSET 	3

enum TriggerModes{
    TRIGGER_AUTO   = (1<<0),
    TRIGGER_NORMAL = (1<<1),
    TRIGGER_SINGLE = (1<<2),
    TRIGGER_STOP   = (1<<3),
    TRIGGER_READY  = (1<<4),
    TRIGGER_RISING_EDGE   = (1<<5),
    TRIGGER_FALLING_EDGE  = (1<<6)
};

enum DsoStates{
    DSO_RUN,
    DSO_STOP,
    DSO_READY
};

#define TRIGGER_MODE_MASK (TRIGGER_AUTO | TRIGGER_NORMAL | TRIGGER_SINGLE)
#define TRIGGER_STATUS_MASK (TRIGGER_STOP | TRIGGER_READY)
#define TRIGGER_EDGE_MASK (TRIGGER_RISING_EDGE | TRIGGER_FALLING_EDGE)


typedef struct{
	int16_t pos;        // holds position on display
	uint16_t color;     // channel color
	uint8_t active;     // active flag
    uint8_t scaleidx;   // holds the index of current voltage scale for channel
	int16_t *buffer;    // pointer for channel sample data
}Channel;

typedef struct{         //
    int16_t pos;
    uint16_t color;
    uint8_t  mode;
    uint16_t level;
    uint8_t edge;
    uint16_t index;     // sample index for trigger
}Trigger;

typedef struct{
	uint8_t state;                      // current dso state
    uint16_t timebase;                  // timebase
	int16_t hpos;                       // horizontal position
	uint8_t channel;                    // current channel affected by control
	Trigger trigger;                    // trigger related data
	Channel channels[DSO_MAX_CHANNELS]; // channels array
	void *menudata;                     // pointer to parameters of callback
	uint8_t (*activeControl)(void *);   // function callback for current control
}Dso;

typedef struct{
	char *name;
	void *data;
	uint8_t (*function)(void *);
}Menuentry;

typedef struct{
	uint16_t x;         // X Absolute position
	uint16_t y;         // Y Absolute position
	uint16_t w;         // Width of menu
	uint16_t h;	        // Height of menu
	Menuentry *items;   // pointer to items on menu
	uint8_t nitems;     // items count
	uint16_t fcolor;    // Text fore color
	uint16_t bcolor;    // Text back color
	uint16_t sfcolor;   // Selected text fore color
	uint16_t sbcolor;   // selected text back color
	int8_t  select;     // Selection holder
	uint8_t  visible;   // Visible flag
}Menu;

void DSO_Init(void);
void DSO_Run(uint8_t doloop);

#ifdef __cplusplus
}
#endif

#endif




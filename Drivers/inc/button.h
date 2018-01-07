/**
* @file		button.h
* @brief	Contains API header for reading buttons.
*     		
* @version	1.0
* @date		3 Nov. 2016
* @author	Hugo Reis
**********************************************************************/

#ifndef _button_h_
#define _button_h_

#include <stdint.h>

typedef struct{
	uint32_t cur;
	uint32_t last;
	uint32_t counter;
	uint32_t events;
    uint32_t htime;
}BUTTON_Controller;

#define BUTTON_DEFAULT_HOLD_TIME 2000   //2 seconds

enum Benvent{
    BUTTON_EMPTY = 0,
    BUTTON_PRESSED,
    BUTTON_TIMING,
    BUTTON_HOLD,
    BUTTON_RELEASED	
}Bstate;


/**
* @brief Button connections
*	P1.14, p1.15, p1.16, p1.17, p1.28
**/
#if defined(__BLUEBOARD__) || defined(__EMU__)
    #define BUTTON_UP    (1<<15) //0x10034000
	#define BUTTON_DOWN  (1<<17) //0x1001C000
	#define BUTTON_LEFT  (1<<16) //0x1002C000
	#define BUTTON_RIGHT (1<<28) //0x0003C000
	#define BUTTON_A     (1<<14) //0x10038000

#if defined(__USE_CMSIS)
	#if defined(__LPCXpresso__)
	#define BUTTON_Capture() (~(LPC_GPIO0->FIOPIN) & BUTTON_MASK)
	#define BUTTON_SetInput(x) LPC_GPIO0->FIODIR &= ~(x)
	#else
	#define BUTTON_Capture() (~(LPC_GPIO1->FIOPIN) & BUTTON_MASK)
	#define BUTTON_SetInput(x) LPC_GPIO1->FIODIR &= ~(x)
	#endif
#else
	#if defined(__LPCXpresso__)
	#define BUTTON_Capture() (~(GPIO0->FIOPIN) & BUTTON_MASK)
	#define BUTTON_SetInput(x) GPIO0->FIODIR &= ~(x)
	#else
	#define BUTTON_Capture() (~(GPIO1->FIOPIN) & BUTTON_MASK)
	#define BUTTON_SetInput(x) GPIO1->FIODIR &= ~(x)
	#endif
#endif

#define BUTTON_MASK (BUTTON_UP | BUTTON_DOWN | BUTTON_LEFT | BUTTON_RIGHT | BUTTON_A )

#elif defined(__TDSO__)
	#define BUTTON_LEFT  	(1<<15)
	#define BUTTON_RIGHT 	(1<<13)
	#define BUTTON_CENTER	(1<<14)
	#define BUTTON_A 		BUTTON_CENTER

	#define BUTTON_Capture() (~GPIO_Read(GPIOB) & BUTTON_MASK)
	#define BUTTON_MASK (BUTTON_LEFT | BUTTON_RIGHT | BUTTON_A )
#endif /* boardselect */



#if defined(__EMU__)
#define loop BUTTON_GetEvents() != 256 //SDL_QUIT
#else
#define loop 1
#endif


/**
* @brief Faz a iniciação do sistema para permitir o acesso aos botões
* @param ht : mask bitmap com os bits correspondentes aos pinos ligados nos botões
**/
void BUTTON_Init(int ht);

/**
* @brief Devolve o estado dos botões, pressionado, largado, mantido ou em contagem. 
* Não é bloqueante.
**/
int BUTTON_Read(void);

/**
* @brief Devolve o código (bitmap) do botão pressionado. É bloqueante.
*
**/
int BUTTON_Get(void);

/**
* @brief Devolve o código (bitmap) do estado do botão: 
* 		 pressionado (transição),
*        libertado (transição),
*        repetição (mantido pressionado)
**/
int BUTTON_GetEvents(void);

/**
 * @brief Devolve o codigo (bitmap) da tecla corrente
 *        EMPTY se nenhuma tecla pressionada
 **/
int BUTTON_GetValue(void); 

/**
* @brief Efectua uma espera pelo evento dado no parametro*        
**/
void BUTTON_WaitEvent(int event);

/**
* @brief configura o tempo minimo para que uma tecla 
*        seja considerada como mantida pressionada 
**/
void BUTTON_SetHoldTime(int t);

/**
* @brief verifica se o botao passado no parametro foi premido
*        
**/
char BUTTON_Pressed(int button);
#endif


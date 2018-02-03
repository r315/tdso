/**
* @file		spi.h
* @brief	Contains the spi API header.
*     		
* @version	1.0
* @date		5 Nov. 2016
* @author	Hugo Reis
**********************************************************************/

#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>

#if defined(__TDSO__)

#if defined(USE_HAL_DRIVER)
#include "stm32f1xx_hal.h"

extern SPI_HandleTypeDef hspi1;

#define SPI_Init()    // Handle By HAL_Init

#if defined(LCD_DMA)
#define SPI_Send(x) SPI_Send_DMA(x)

#else
#define SPI_Send(x)                    \
{                                      \
	uint8_t _data = x;                  \
HAL_SPI_Transmit(&hspi1, &(x), 1, 10);\
}
#endif

#else /* USE_HAL_DRIVER */

//#define SPI_16XFER // Not functional

#ifdef SPI_16XFER

void SPI_Send(uint8_t dt){
	SPI1->CR1 &= ~(SPI_CR1_DFF);
	SPI1->DR = dt;
	while((SPI1->SR & SPI_SR_BSY));
	SPI1->CR1 |= SPI_CR1_DFF;
}

void SPI_Send16(uint16_t dt){
	SPI1->DR = dt;
	while((SPI1->SR & SPI_SR_BSY));
}

#else /* SPI_16XFER */

void SPI_Send(uint8_t dt){ //TODO Fix for -O3
	SPI1->DR = dt;
	while((SPI1->SR & SPI_SR_BSY));
}

#endif /* LCD_16XFER */

#endif /* USE_HAL_DRIVER */

#else  /* __TDO__ */

#if defined(__USE_CMSIS)

#define SPI_PowerUp() SC->PCONP |= SPI0_ON
#define SSP0_PowerUp() LPC_SC->PCONP |= SSP0_ON;
#define SSP1_PowerUp() LPC_SC->PCONP |= SSP1_ON;

#define SSP0_ConfigPins()                             \
	    LPC_PINCON->PINSEL0 &= ~(SSP0_CLK_PIN_MASK);  \
	    LPC_PINCON->PINSEL1 &= ~(SSP0_PINS_MASK);     \
	    LPC_PINCON->PINSEL0 |= SSP0_CLK_PIN;          \
	    LPC_PINCON->PINSEL1 |= SSP0_PINS;

#define SSP1_ConfigPins()                         \
		LPC_PINCON->PINSEL0 &= ~(SSP1_PINS_MASK); \
		LPC_PINCON->PINSEL0 |= SSP1_PINS;

#define SPI_ConfigPins() PINCON->PINSEL0 = SPI0_PINS;

#else
#define SPI_PowerUp() SC->PCONP |= SPI0_ON
#define SSP0_PowerUp() LPC_SC->PCONP |= SSP0_ON;
#define SSP1_PowerUp() LPC_SC->PCONP |= SSP1_ON;

#define SSP0_ConfigPins()                             \
	    LPC_PINCON->PINSEL0 &= ~(SSP0_CLK_PIN_MASK);  \
	    LPC_PINCON->PINSEL1 &= ~(SSP0_PINS_MASK);     \
	    LPC_PINCON->PINSEL0 |= SSP0_CLK_PIN;          \
	    LPC_PINCON->PINSEL1 |= SSP0_PINS;

#define SSP1_ConfigPins()                         \
		LPC_PINCON->PINSEL0 &= ~(SSP1_PINS_MASK); \
		LPC_PINCON->PINSEL0 |= SSP1_PINS;

#define SPI_ConfigPins() PINCON->PINSEL0 = SPI0_PINS;

#endif

#define SPI0_ON (1<<8)
#define SSP0_ON (1<<21)
#define SSP1_ON (1<<10)

/* SSPx pins */
#define SSP0_CLK_PIN_MASK (3<<30)
#define SSP0_CLK_PIN (2<<30)
#define SSP0_PINS_MASK (0x0F<<2)
#define SSP0_PINS (0x0A<<2)
#define SSP1_PINS_MASK (0x3F<<14)
#define SSP1_PINS (0x2A<<14)

#define SSP_CPOL (1<<6)  // idle level
#define SSP_CPHA (1<<7)  // data valid
#define SSP_SSE  (1<<1)  // ssp enable
#define SSP_MS   (1<<2)  // master mode
#define SSP_SOD  (1<<3)  // slave output disable
#define SSP_TFE  (1<<0)
#define SSP_TNF  (1<<1)
#define SSP_BSY  (1<<4)

#define SSP_8BIT  8
#define SSP_16BIT 16

#define SSP_MAX_CLK 10
#define SSP_MIN_CLK 254

#define PCLK_SSP0 10
#define PCLK_SSP0_MASK (3<<PCLK_SSP0)
#define PCLK_SSP1 20
#define PCLK_SSP1_MASK (3<<PCLK_SSP1)

/* Control bits */
#define SPI0_EN_NBITS (1<<2)	//transfered bits selected by bits 11:8
#define SPI0_CPHA     (1<<3)
#define SPI0_CPOL     (1<<4)
#define SPI0_MSTR     (1<<5)	//Master mode
#define SPI0_LSBF     (1<<6)
#define SPI0_SPIE     (1<<7)

#define SPI0_MIN_DIV 8

/* status register bits */
#define SPI0_SPIF     (1<<7)

/* pincon function select */
/*Remark: LPC2104/05/06 and LPC2104/05/06/00 configured to operate as a SPI master MUST
select SSEL functionality on P0.7 and have HIGH level on this pin in order to act as a master.*/
#define SPI0_PINS     (0x55<<8)

#define SPI_MAX_CLK   8		//min pckl divider
#define SPI_MAX_FREQ  0
#define SPI_8BIT 8
#define SPI_16BIT 0


/**
* @brief Faz a iniciação do controlador, configurando os pinos, o ritmo de envio e o
*        numero de bits de dados.
*/
void SPI_Init(int frequency, int bitData);

/**
* @brief Coloca ativo o chip select do dispositivo slave
*/
void SPI_BeginTransfer(int csBitId);

/**
* @brief Coloca desativo o chip select do dispositivo slave
**/
void SPI_EndTransfer(int csBitId);

/**
* @brief Realiza uma transferencia. 
**/
void SPI_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int lenght);

/**
* @brief initiates and 8bit data transfer
*		 rev 00 only suports 8bit
**/
uint16_t SPI_Send(uint16_t data);

#endif /* __TDSO__ */
#endif /* _SPI_H_ */

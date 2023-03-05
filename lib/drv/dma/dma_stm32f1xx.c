#include <stdint.h>
#include "stm32f1xx.h"
#include "dma_stm32f1xx.h"
#include "dma.h"

typedef void (*eot_t)(void);
static eot_t ch_eot[DMA_NUM_CHANNELS];

/**
 * @brief Request dma controller/channel
 * \param ctrl    : pointer to hold controller and channel
 * \param request : Request in format
 *                   [9:8] -> DMA number
 *                   [7:4] -> Channel number
 *                   [3:0] -> Request source
 * */
void DMA_Config(dmatype_t *dma, uint32_t request){
    DMA_Channel_TypeDef *stream; 
    uint8_t ch_num;

    if((request & DMA_NUMBER_MASK) == 0){
        __HAL_RCC_DMA1_CLK_ENABLE();
        ch_num = (request & DMA_CHANNEL_MASK) >> DMA_CHANNEL_POS;
        dma->per = DMA1;
        stream = (DMA_Channel_TypeDef*)((uint32_t)DMA1_Channel1 + (ch_num * 0x14));

        uint32_t config = 0;

        switch(dma->dir){
            case DMA_DIR_P2P:
                config |=   DMA_CCR_MSIZE_CFG(dma->ssize) | DMA_CCR_PSIZE_CFG(dma->dsize);
                break;

            case DMA_DIR_P2M:
                config |=   DMA_CCR_PSIZE_CFG(dma->ssize) | 
                            DMA_CCR_MSIZE_CFG(dma->dsize) |
                            DMA_CCR_PINC;                   // Increment destination
                break;

            case DMA_DIR_M2P:
                config |=   DMA_CCR_PSIZE_CFG(dma->dsize) | 
                            DMA_CCR_MSIZE_CFG(dma->ssize) | 
                            DMA_CCR_MINC | DMA_CCR_DIR;     // Increment source
                break;

            case DMA_DIR_M2M:
                config |=   DMA_CCR_PSIZE_CFG(dma->dsize) | 
                            DMA_CCR_MSIZE_CFG(dma->ssize) | 
                            DMA_CCR_MEM2MEM | DMA_CCR_MINC | DMA_CCR_PINC;
                break;
        }

        if(dma->eot){
            config |= DMA_CCR_TCIE;  // enable terminal count
            HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn + ch_num);
        }

        stream->CCR = config;
        stream->CPAR = (uint32_t)dma->dst;
        dma->stream = stream;
        
        ch_eot[ch_num] = dma->eot;
    }
}

static inline void dma_irq_handler(uint8_t ch_num)
{
    if(ch_eot[ch_num] != NULL){
        ch_eot[ch_num]();
    }
}

void DMA1_Channel3_IRQHandler(void)
{ 
    if(DMA1->ISR & DMA_ISR_GIF3){
        dma_irq_handler(2);
        DMA1->IFCR = (DMA_IFCR_CGIF3 | DMA_IFCR_CTCIF3 | DMA_IFCR_CTEIF3);
    }
}
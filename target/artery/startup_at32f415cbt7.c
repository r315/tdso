#include <stdint.h>
#include "at32f4xx.h"
#include "core_cm4.h"


#define WEAK    __attribute__((weak))
#define NAKED   __attribute__((naked, aligned(4)))
#define ISR     __attribute__((section(".isr_vector")))


ISR void *vector_table[];
extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _stack, _estack;

WEAK int main(void){}

void Reset_Handler(void)
{
    volatile uint32_t *src, *dest;

    /* Copy initialize variables with .data section values*/
    for (src = &_sidata, dest = &_sdata; dest < &_edata; src++, dest++){
        *dest = *src;
    }

    /* Clear .bss */
    dest = &_sbss;

    while (dest < &_ebss)
        *dest++ = 0;

    main();

    while(1){}
}


typedef struct {
    uint32_t r0, r1, r2, r3, r12, lr, pc, psr;
}stackframe_t;

void Stack_Dump(stackframe_t *stack){
    //GPIOJ->MODER = (1 << 26);
    //HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET);

    asm volatile
    (
        "bkpt #01 \n"
        "b . \n"
    );
}

NAKED
void HardFault_Handler(void){
    asm volatile
    (
        " tst lr, #4                                 \n"        // Check current stack
        " ite eq                                     \n"
        " mrseq r0, msp                              \n"        // Move msp to r0 ??
        " mrsne r0, psp                              \n"        // Move psp to r0 ??
        " ldr r1, [r0, #24]                          \n"        // Get address were exception happen ?
        " ldr r2, dumpHandler_address                \n"
        " bx r2                                      \n"
        " dumpHandler_address: .word Stack_Dump     \n"
    );
}

void defaultHandler(void){
    volatile uint8_t isr_number = (SCB->ICSR & 255) - 16;
    // See position number on Table 61 from RM0008
    (void)isr_number;

    asm volatile
    (
        "bkpt #01 \n"
        "b .      \n"
    );
}

WEAK void NMI_Handler(void);
#pragma weak NMI_Handler = defaultHandler
//WEAK void HardFault_Handler(void);
//#pragma weak HardFault_Handler = defaultHandler
WEAK void MemManage_Handler(void);
#pragma weak MemManage_Handler = defaultHandler
WEAK void BusFault_Handler(void);
#pragma weak BusFault_Handler = defaultHandler
WEAK void UsageFault_Handler(void);
#pragma weak UsageFault_Handler = defaultHandler
WEAK void SVC_Handler(void);
#pragma weak SVC_Handler = defaultHandler
WEAK void DebugMon_Handler(void);
#pragma weak DebugMon_Handler = defaultHandler
WEAK void PendSV_Handler(void);
#pragma weak PendSV_Handler = defaultHandler
WEAK void SysTick_Handler(void);
#pragma weak SysTick_Handler = defaultHandler
WEAK void WWDG_IRQHandler(void);
#pragma weak WWDG_IRQHandler = defaultHandler
WEAK void PVD_IRQHandler(void);
#pragma weak PVD_IRQHandler = defaultHandler
WEAK void TAMP_STAMP_IRQHandler(void);
#pragma weak TAMP_STAMP_IRQHandler = defaultHandler
WEAK void ERTC_WKUP_IRQHandler(void);
#pragma weak ERTC_WKUP_IRQHandler = defaultHandler
WEAK void FLASH_IRQHandler(void);
#pragma weak FLASH_IRQHandler = defaultHandler
WEAK void RCC_IRQHandler(void);
#pragma weak RCC_IRQHandler = defaultHandler
WEAK void EXTI0_IRQHandler(void);
#pragma weak EXTI0_IRQHandler = defaultHandler
WEAK void EXTI1_IRQHandler(void);
#pragma weak EXTI1_IRQHandler = defaultHandler
WEAK void EXTI2_IRQHandler(void);
#pragma weak EXTI2_IRQHandler = defaultHandler
WEAK void EXTI3_IRQHandler(void);
#pragma weak EXTI3_IRQHandler = defaultHandler
WEAK void EXTI4_IRQHandler(void);
#pragma weak EXTI4_IRQHandler = defaultHandler
WEAK void DMA1_Channel1_IRQHandler(void);
#pragma weak DMA1_Channel1_IRQHandler = defaultHandler
WEAK void DMA1_Channel2_IRQHandler(void);
#pragma weak DMA1_Channel2_IRQHandler = defaultHandler
WEAK void DMA1_Channel3_IRQHandler(void);
#pragma weak DMA1_Channel3_IRQHandler = defaultHandler
WEAK void DMA1_Channel4_IRQHandler(void);
#pragma weak DMA1_Channel4_IRQHandler = defaultHandler
WEAK void DMA1_Channel5_IRQHandler(void);
#pragma weak DMA1_Channel5_IRQHandler = defaultHandler
WEAK void DMA1_Channel6_IRQHandler(void);
#pragma weak DMA1_Channel6_IRQHandler = defaultHandler
WEAK void DMA1_Channel7_IRQHandler(void);
#pragma weak DMA1_Channel7_IRQHandler = defaultHandler
WEAK void ADC1_2_IRQHandler(void);
#pragma weak ADC1_2_IRQHandler = defaultHandler
WEAK void CAN1_TX_IRQHandler(void);
#pragma weak CAN1_TX_IRQHandler = defaultHandler
WEAK void CAN1_RX0_IRQHandler (void);
#pragma weak CAN1_RX0_IRQHandler = defaultHandler
WEAK void CAN1_RX1_IRQHandler (void);
#pragma weak CAN1_RX1_IRQHandler = defaultHandler
WEAK void CAN1_SCE_IRQHandler (void);
#pragma weak CAN1_SCE_IRQHandler = defaultHandler
WEAK void EXTI9_5_IRQHandler (void);
#pragma weak EXTI9_5_IRQHandler = defaultHandler
WEAK void TMR1_BRK_TMR9_IRQHandler (void);
#pragma weak TMR1_BRK_TMR9_IRQHandler = defaultHandler
WEAK void TMR1_OV_TMR10_IRQHandler (void);
#pragma weak TMR1_OV_TMR10_IRQHandler = defaultHandler
WEAK void TMR1_TRG_COM_TMR11_IRQHandler (void);
#pragma weak TMR1_TRG_COM_TMR11_IRQHandler = defaultHandler
WEAK void TMR1_CC_IRQHandler (void);
#pragma weak TMR1_CC_IRQHandler = defaultHandler
WEAK void TMR2_GLOBAL_IRQHandler (void);
#pragma weak TMR2_GLOBAL_IRQHandler = defaultHandler
WEAK void TMR3_GLOBAL_IRQHandler (void);
#pragma weak TMR3_GLOBAL_IRQHandler = defaultHandler
WEAK void TMR4_GLOBAL_IRQHandler (void);
#pragma weak TMR4_GLOBAL_IRQHandler = defaultHandler
WEAK void I2C1_EV_IRQHandler (void);
#pragma weak I2C1_EV_IRQHandler = defaultHandler
WEAK void I2C1_ER_IRQHandler (void);
#pragma weak I2C1_ER_IRQHandler = defaultHandler
WEAK void I2C2_EV_IRQHandler (void);
#pragma weak I2C2_EV_IRQHandler = defaultHandler
WEAK void I2C2_ER_IRQHandler (void);
#pragma weak I2C2_ER_IRQHandler = defaultHandler
WEAK void SPI1_IRQHandler (void);
#pragma weak SPI1_IRQHandler = defaultHandler
WEAK void SPI2_IRQHandler (void);
#pragma weak SPI2_IRQHandler = defaultHandler
WEAK void USART1_IRQHandler (void);
#pragma weak USART1_IRQHandler = defaultHandler
WEAK void USART2_IRQHandler (void);
#pragma weak USART2_IRQHandler = defaultHandler
WEAK void USART3_IRQHandler (void);
#pragma weak USART3_IRQHandler = defaultHandler
WEAK void EXTI15_10_IRQHandler (void);
#pragma weak EXTI15_10_IRQHandler = defaultHandler
WEAK void ERTCAlarm_IRQHandler (void);
#pragma weak ERTCAlarm_IRQHandler = defaultHandler
WEAK void USBWakeUp_IRQHandler (void);
#pragma weak USBWakeUp_IRQHandler = defaultHandler
WEAK void SDIO_IRQHandler (void);
#pragma weak SDIO_IRQHandler = defaultHandler
WEAK void TMR5_GLOBAL_IRQHandler (void);
#pragma weak TMR5_GLOBAL_IRQHandler = defaultHandler
WEAK void DMA2_Channel1_IRQHandler (void);
#pragma weak DMA2_Channel1_IRQHandler = defaultHandler
WEAK void DMA2_Channel2_IRQHandler (void);
#pragma weak DMA2_Channel2_IRQHandler = defaultHandler
WEAK void DMA2_Channel3_IRQHandler (void);
#pragma weak DMA2_Channel3_IRQHandler = defaultHandler
WEAK void DMA2_Channel4_5_IRQHandler (void);
#pragma weak DMA2_Channel4_5_IRQHandler = defaultHandler
WEAK void USBOTG_IRQHandler (void);
#pragma weak USBOTG_IRQHandler = defaultHandler
WEAK void COMP1_IRQHandler (void);
#pragma weak COMP1_IRQHandler = defaultHandler
WEAK void COMP2_IRQHandler (void);
#pragma weak COMP2_IRQHandler = defaultHandler
WEAK void ACC_IRQHandler (void);
#pragma weak ACC_IRQHandler = defaultHandler
WEAK void DMA2_Channel6_7_IRQHandler (void);
#pragma weak DMA2_Channel6_7_IRQHandler = defaultHandler


ISR void *vector_table[] = {
    &_estack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
  
    /* External Interrupts */
    WWDG_IRQHandler,                     /* Window Watchdog                             */
    PVD_IRQHandler,                      /* PVD through EXTI Line detect                */
    TAMP_STAMP_IRQHandler,               /* Tamper and TimeStamps through the EXTI line */
    ERTC_WKUP_IRQHandler,                /* ERTC Wakeup through the EXTI line           */
    FLASH_IRQHandler,                    /* Flash                                       */
    RCC_IRQHandler,                      /* RCC                                         */
    EXTI0_IRQHandler,                    /* EXTI Line 0                                 */
    EXTI1_IRQHandler,                    /* EXTI Line 1                                 */
    EXTI2_IRQHandler,                    /* EXTI Line 2                                 */
    EXTI3_IRQHandler,                    /* EXTI Line 3                                 */
    EXTI4_IRQHandler,                    /* EXTI Line 4                                 */
    DMA1_Channel1_IRQHandler,            /* DMA1 Channel 1                              */
    DMA1_Channel2_IRQHandler,            /* DMA1 Channel 2                              */
    DMA1_Channel3_IRQHandler,            /* DMA1 Channel 3                              */
    DMA1_Channel4_IRQHandler,            /* DMA1 Channel 4                              */
    DMA1_Channel5_IRQHandler,            /* DMA1 Channel 5                              */
    DMA1_Channel6_IRQHandler,            /* DMA1 Channel 6                              */
    DMA1_Channel7_IRQHandler,            /* DMA1 Channel 7                              */
    ADC1_2_IRQHandler,                   /* ADC1 & ADC2                                 */
    CAN1_TX_IRQHandler,                  /* CAN1 TX                                     */
    CAN1_RX0_IRQHandler,                 /* CAN1 RX0                                    */
    CAN1_RX1_IRQHandler,                 /* CAN1 RX1                                    */
    CAN1_SCE_IRQHandler,                 /* CAN1 SCE                                    */
    EXTI9_5_IRQHandler,                  /* EXTI Line [9:5]                             */
    TMR1_BRK_TMR9_IRQHandler,            /* TMR1 Break and TMR9                         */
    TMR1_OV_TMR10_IRQHandler,            /* TMR1 Update and TMR10                       */
    TMR1_TRG_COM_TMR11_IRQHandler,       /* TMR1 Trigger and Commutation and TMR11      */
    TMR1_CC_IRQHandler,                  /* TMR1 Capture Compare                        */
    TMR2_GLOBAL_IRQHandler,              /* TMR2                                        */
    TMR3_GLOBAL_IRQHandler,              /* TMR3                                        */
    TMR4_GLOBAL_IRQHandler,              /* TMR4                                        */
    I2C1_EV_IRQHandler,                  /* I2C1 Event                                  */
    I2C1_ER_IRQHandler,                  /* I2C1 Error                                  */
    I2C2_EV_IRQHandler,                  /* I2C2 Event                                  */
    I2C2_ER_IRQHandler,                  /* I2C2 Error                                  */
    SPI1_IRQHandler,                     /* SPI1                                        */
    SPI2_IRQHandler,                     /* SPI2                                        */
    USART1_IRQHandler,                   /* USART1                                      */
    USART2_IRQHandler,                   /* USART2                                      */
    USART3_IRQHandler,                   /* USART3                                      */
    EXTI15_10_IRQHandler,                /* EXTI Line [15:10]                           */
    ERTCAlarm_IRQHandler,                /* ERTC Alarm through EXTI Line                */
    USBWakeUp_IRQHandler,                /* USB Wakeup from suspend                     */
    0,                                   /* Reserved                                    */
    0,                                   /* Reserved                                    */
    0,                                   /* Reserved                                    */
    0,                                   /* Reserved                                    */
    0,                  			  	 /* Reserved                                    */
    0,                  			  	 /* Reserved                                    */
    SDIO_IRQHandler,                     /* SDIO                                        */
    TMR5_GLOBAL_IRQHandler,              /* TMR5                                        */
    0,                  			  	 /* Reserved                                    */
    0,                                   /* Reserved                                    */
    0,                                   /* Reserved                                    */
    0,                  			  	 /* Reserved                                    */
    0,                  			  	 /* Reserved                                    */
    DMA2_Channel1_IRQHandler,            /* DMA2 Channel1                               */
    DMA2_Channel2_IRQHandler,            /* DMA2 Channel2                               */
    DMA2_Channel3_IRQHandler,            /* DMA2 Channel3                               */
    DMA2_Channel4_5_IRQHandler,          /* DMA2 Channel4 & Channel5                    */
    0,                                   /* Reserved                                    */
    0,                                   /* Reserved                                    */
    0,                                   /* Reserved                                    */
    0,                                   /* Reserved                                    */
    0,                                   /* Reserved                                    */
    0,                                   /* Reserved                                    */
    0,                                   /* Reserved                                    */
    USBOTG_IRQHandler,                   /* USBOTG                                      */
    0,                                   /* Reserved                                    */
    0,                                   /* Reserved                                    */
    COMP1_IRQHandler,                    /* COMP1                                       */
    COMP2_IRQHandler,                    /* COMP2                                       */
    ACC_IRQHandler,                      /* ACC                                         */
    0,                                   /* Reserved                                    */
    0,                                   /* Reserved                                    */
    DMA2_Channel6_7_IRQHandler,          /* DMA2 Channel6 & Channel7                    */
};

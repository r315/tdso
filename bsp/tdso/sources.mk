#########################################################
# project files
#########################################################
REPOSITORY =$(HOME)/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.0/

CSRCPATH += \
bsp/tdso \
bsp/bluepill/Src bsp/bluepill/Src \
bsp/bluepill/Src bsp/bluepill/startup \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src \

INCSPATH += \
bsp/tdso \
bsp/bluepill/Src bsp/bluepill/Inc \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Inc \
$(REPOSITORY)/Drivers/CMSIS/Include \
$(REPOSITORY)/Drivers/CMSIS/Device/ST/STM32F1xx/Include \

CSRCS += \
tdso.c \
system_stm32f1xx.c \
stm32f1xx_it.c \
stm32f1xx_hal_pcd.c \
stm32f1xx_hal_pcd_ex.c \
stm32f1xx_hal_msp.c \
stm32f1xx_hal_rcc.c \
stm32f1xx_hal_gpio.c \
stm32f1xx_hal.c \
stm32f1xx_hal_tim.c \
stm32f1xx_hal_rcc_ex.c \
stm32f1xx_hal_tim_ex.c \
stm32f1xx_hal_cortex.c \
stm32f1xx_hal_dma.c \
stm32f1xx_hal_adc.c \
stm32f1xx_hal_adc_ex.c \
stm32f1xx_hal_i2c.c \
stm32f1xx_hal_spi.c \
stm32f1xx_ll_usb.c \
capture_stm32.c \
ili9341.c\

ASRCS +=startup_stm32f103xb.s

LDSCRIPT ="bsp/bluepill/startup/STM32F103C8Tx_FLASH.ld"

DEVICE =STM32F103xB
GCSYMBOLS +=__TDSO__ $(DEVICE) USE_HAL_DRIVER __weak='__attribute__((weak))' __packet='__attribute__((__packed__))' LCD_DMA
#########################################################
#Startup files and libraries
#########################################################
GCFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Og -g3 -Wall -fmessage-length=0 -ffunction-sections
LDFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Wl,--gc-sections
#LIBS =-nostdlib -lgcc #-lc -lnosys



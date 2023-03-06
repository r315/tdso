#########################################################
# project files
#########################################################
BSP_PATH =bsp
DRIVER_PATH = lib

CSRCPATH += \
$(DRIVER_PATH)/button \
$(DRIVER_PATH)/display \
$(DRIVER_PATH)/drv/tft \
$(DRIVER_PATH)/drv/spi \
$(DRIVER_PATH)/drv/dma \
$(BSP_PATH)/tdso \
$(BSP_PATH)/bluepill/Src \
$(BSP_PATH)/bluepill/startup \
$(BSP_PATH)/bluepill/STM32F1xx_HAL_Driver/Src \

INCSPATH += \
$(DRIVER_PATH)/include \
$(DRIVER_PATH)/drv/inc \
$(BSP_PATH)/tdso \
$(BSP_PATH)/bluepill/STM32F1xx_HAL_Driver/Inc \
$(BSP_PATH)/CMSIS/Device/ST/STM32F1xx/Include \
$(BSP_PATH)/CMSIS/Include \

CSRCS += \
board_tdso.c \
system_stm32f1xx.c \
stm32f1xx_hal.c \
stm32f1xx_hal_dma.c \
stm32f1xx_hal_pcd.c \
stm32f1xx_hal_pcd_ex.c \
stm32f1xx_hal_msp.c \
stm32f1xx_hal_rcc.c \
stm32f1xx_hal_gpio.c \
stm32f1xx_hal_tim.c \
stm32f1xx_hal_rcc_ex.c \
stm32f1xx_hal_tim_ex.c \
stm32f1xx_hal_cortex.c \
stm32f1xx_hal_adc.c \
stm32f1xx_hal_adc_ex.c \
stm32f1xx_hal_spi.c \
stm32f1xx_ll_usb.c \
spi_stm32f1xx.c \
dma_stm32f1xx.c \
capture_stm32.c \
ili9341.c\

ASRCS +=startup_stm32f103xb.s

LDSCRIPT ="$(BSP_PATH)/bluepill/startup/STM32F103C8Tx_FLASH.ld"

DEVICE =STM32F103xB
GCSYMBOLS +=__TDSO__ $(DEVICE) USE_HAL_DRIVER __weak='__attribute__((weak))' __packet='__attribute__((__packed__))'
#########################################################
#Startup files and libraries
#########################################################
GCFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections
LDFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Wl,--gc-sections
#LIBS =-nostdlib -lgcc #-lc -lnosys



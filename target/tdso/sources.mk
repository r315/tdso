#########################################################
# 
#########################################################
CMSIS_PATH :=$(ROOT_PATH)/target/CMSIS_5-5.9.0
DRIVER_PATH :=$(ROOT_PATH)/sdk_emb/drv

CSRCPATH += \
$(TARGET_PATH) \
$(DRIVER_PATH)/src \
$(TARGET_PATH)/../bluepill/Src \
$(TARGET_PATH)/../bluepill/startup \
$(TARGET_PATH)/../bluepill/STM32F1xx_HAL_Driver/Src \

INCSPATH += \
$(TARGET_PATH) \
$(DRIVER_PATH)/inc \
$(CMSIS_PATH)/Device/ST/STM32F1xx/Include \
$(CMSIS_PATH)/CMSIS/Core/Include \
$(TARGET_PATH)/../bluepill/STM32F1xx_HAL_Driver/Inc \

BSP_SOURCES = \
board_tdso.c \
spi_stm32f1xx.c \
dma_stm32f1xx.c \
ili9341.c\
startup_stm32f103.c \

HAL_SOURCES = \
stm32f1xx_hal.c \
stm32f1xx_hal_dma.c \
stm32f1xx_hal_pcd.c \
stm32f1xx_hal_pcd_ex.c \
stm32f1xx_hal_msp.c \
stm32f1xx_hal_gpio.c \
stm32f1xx_hal_tim.c \
stm32f1xx_hal_rcc.c \
stm32f1xx_hal_rcc_ex.c \
stm32f1xx_hal_tim_ex.c \
stm32f1xx_hal_cortex.c \
stm32f1xx_hal_adc.c \
stm32f1xx_hal_adc_ex.c \
stm32f1xx_hal_spi.c \

CSRCS += $(HAL_SOURCES) $(BSP_SOURCES)
#ASRCS +=startup_stm32f103xb.s

LDSCRIPT ="$(TARGET_PATH)/../bluepill/startup/STM32F103C8Tx_FLASH.ld"

DEVICE =STM32F103xB
GCSYMBOLS +=BOARD_TDSO $(DEVICE) USE_HAL_DRIVER __weak='__attribute__((weak))' __packet='__attribute__((__packed__))'
#########################################################
#Startup files and libraries
#########################################################
GCFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections
LDFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Wl,--gc-sections
#LIBS =-nostdlib -lgcc #-lc -lnosys



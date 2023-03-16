#########################################################
# 
#########################################################
CMSIS_PATH :=$(ROOT_PATH)/target/CMSIS_5-5.9.0
SDK_DRIVER_PATH :=$(SDK_PATH)/drv
BOARD_PATH :=$(TARGET_PATH)/../bluepill
BOARD_DRIVER_PATH =$(BOARD_PATH)/STM32F1xx_HAL_Driver

INCSPATH += \
$(SDK_DRIVER_PATH)/inc \
$(CMSIS_PATH)/Device/ST/STM32F1xx/Include \
$(CMSIS_PATH)/CMSIS/Core/Include \
$(BOARD_DRIVER_PATH)/Inc \

BSP_SOURCES = \
$(BOARD_PATH)/board_tdso.c \
$(BOARD_PATH)/startup_stm32f103.c \
$(BOARD_PATH)/sample_engine.c \
$(SDK_DRIVER_PATH)/src/spi_stm32f1xx.c \
$(SDK_DRIVER_PATH)/src/dma_stm32f1xx.c \
$(SDK_LIB_PATH)/src/ili9341.c \

CSRCS += \
$(BOARD_DRIVER_PATH)/src/stm32f1xx_hal.c \
$(BOARD_DRIVER_PATH)/src/stm32f1xx_hal_dma.c \
$(BOARD_DRIVER_PATH)/src/stm32f1xx_hal_pcd.c \
$(BOARD_DRIVER_PATH)/src/stm32f1xx_hal_pcd_ex.c \
$(BOARD_DRIVER_PATH)/src/stm32f1xx_hal_gpio.c \
$(BOARD_DRIVER_PATH)/src/stm32f1xx_hal_tim.c \
$(BOARD_DRIVER_PATH)/src/stm32f1xx_hal_rcc.c \
$(BOARD_DRIVER_PATH)/src/stm32f1xx_hal_rcc_ex.c \
$(BOARD_DRIVER_PATH)/src/stm32f1xx_hal_tim_ex.c \
$(BOARD_DRIVER_PATH)/src/stm32f1xx_hal_cortex.c \
$(BOARD_DRIVER_PATH)/src/stm32f1xx_hal_adc.c \
$(BOARD_DRIVER_PATH)/src/stm32f1xx_hal_adc_ex.c \
$(BOARD_DRIVER_PATH)/src/stm32f1xx_hal_spi.c \

CSRCS += $(HAL_SOURCES) $(BSP_SOURCES)
#ASRCS +=startup_stm32f103xb.s

LDSCRIPT ="$(BOARD_PATH)/startup/STM32F103C8Tx_FLASH.ld"

DEVICE =STM32F103xB
GCSYMBOLS +=BOARD_TDSO $(DEVICE) USE_HAL_DRIVER __weak='__attribute__((weak))' __packet='__attribute__((__packed__))'
#########################################################
#Startup files and libraries
#########################################################
GCFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections
LDFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Wl,--gc-sections
#LIBS =-nostdlib -lgcc #-lc -lnosys



#########################################################
# project files
#########################################################
DRIVERS_PATH :=$(ROOT_PATH)/target/Drivers
CMSIS_PATH  =$(DRIVERS_PATH)/CMSIS
DRIVER_HAL_PATH =$(DRIVERS_PATH)/STM32F1xx_HAL_Driver
DRIVER_COSTUM_PATH =$(DRIVERS_PATH)/Costum
DRIVER_COMPONENT_PATH =$(DRIVERS_PATH)/Component

INCSPATH += \
$(DRIVER_COSTUM_PATH)/inc \
$(CMSIS_PATH)/Core/Include \
$(CMSIS_PATH)/Device/ST/STM32F1xx/Include \
$(DRIVER_HAL_PATH)/Inc \
$(DRIVER_COMPONENT_PATH)/inc

CSRCS += \
$(TARGET_PATH)/Src/bluepill.c \
$(TARGET_PATH)/Src/system_stm32f1xx.c \
$(TARGET_PATH)/Src/stm32f1xx_it.c \
$(DRIVER_COSTUM_PATH)/src/spi_stm32f1xx.c \
$(DRIVER_COSTUM_PATH)/src/dma_stm32f1xx.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_hal_pcd.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_hal_pcd_ex.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_hal_msp.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_hal_rcc.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_hal_gpio.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_hal.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_hal_tim.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_hal_rcc_ex.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_hal_tim_ex.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_hal_cortex.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_hal_dma.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_hal_adc.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_hal_adc_ex.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_hal_i2c.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_hal_spi.c \
$(DRIVER_HAL_PATH)/src/stm32f1xx_ll_usb.c \
$(DRIVER_COMPONENT_PATH)/src/ili9341.c \
#capture_stm32.c \
#usbd_core.c \
#usbd_ctlreq.c \
#usbd_ioreq.c \
#usbd_conf.c \
#usbd_desc.c \
#usbd_dfu.c \
#usbd_dfu_if.c \
#usb_device.c \

ASRCS +=$(TARGET_PATH)/startup/startup_stm32f103xb.s

LDSCRIPT =$(TARGET_PATH)/startup/STM32F103C8Tx_FLASH.ld

DEVICE =STM32F103xB
GCSYMBOLS +=BOARD_TDSO $(DEVICE) USE_HAL_DRIVER __weak='__attribute__((weak))' __packet='__attribute__((__packed__))'
#########################################################
#Startup files and libraries
#########################################################
GCFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections
LDFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Wl,--gc-sections
#LIBS =-nostdlib -lgcc #-lc -lnosys



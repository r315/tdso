#########################################################
# project files
#########################################################
CSRCPATH +=bsp/bluepill/Src bsp/bluepill/startup \
bsp/bluepill/STM32F1xx_HAL_Driver/Src \
bsp/bluepill/STM32_USB_Device_Library/Core/Src bsp/bluepill/STM32_USB_Device_Library/Class/DFU/Src


INCSPATH +=bsp/bluepill/Inc \
bsp/bluepill/STM32F1xx_HAL_Driver/Inc \
bsp/bluepill/STM32_USB_Device_Library/Class/DFU/Inc bsp/bluepill/STM32_USB_Device_Library/Core/Inc \
bsp/CMSIS/Device/ST/STM32F1xx/Include bsp/CMSIS/Include

CSRCS +=bluepill.c \
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
#usbd_core.c \
#usbd_ctlreq.c \
#usbd_ioreq.c \
#usbd_conf.c \
#usbd_desc.c \
#usbd_dfu.c \
#usbd_dfu_if.c \
#usb_device.c \

ASRCS +=startup_stm32f103xb.s

LDSCRIPT ="bsp/bluepill/startup/STM32F103C8Tx_FLASH.ld"

DEVICE =STM32F103xB
GCSYMBOLS +=__TDSO__ $(DEVICE) USE_HAL_DRIVER __weak='__attribute__((weak))' __packet='__attribute__((__packed__))'
#########################################################
#Startup files and libraries
#########################################################
GCFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections
LDFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Wl,--gc-sections
#LIBS =-nostdlib -lgcc #-lc -lnosys



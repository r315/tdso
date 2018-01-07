#########################################################
# project files
#########################################################
TARGET =tdso
PRJPATH =.
OBJPATH =obj

CSRCPATH =Src app Drivers/STM32F1xx_HAL_Driver/Src Drivers/button Drivers/display \
Middlewares/ST/STM32_USB_Device_Library/Core/Src Middlewares/ST/STM32_USB_Device_Library/Class/DFU/Src

INCSPATH =Inc Src app Drivers/inc Middlewares/ST/STM32_USB_Device_Library/Class/DFU/Inc \
Drivers/inc Drivers/STM32F1xx_HAL_Driver/Inc \
Drivers/CMSIS/Device/ST/STM32F1xx/Include \
Drivers/CMSIS/Include Middlewares/ST/STM32_USB_Device_Library/Core/Inc

CSRCS =main.c \
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
usbd_core.c \
usbd_ctlreq.c \
usbd_ioreq.c \
usbd_conf.c \
usbd_desc.c \
usbd_dfu.c \
usbd_dfu_if.c \
usb_device.c \
button.c \
display.c \
fonts.c \
ili9341.c \
lcd.c \
dso.c \
capture_stm32.c \
control.c \
softpower.c \
tests.c \
tdso_main.c \
tdso_util.c
GCSYMBOLS =-D__TDO__ -DSTM32F103xB -DUSE_HAL_DRIVER -D__weak='__attribute__((weak))' -D__packet='__attribute__((__packed__))'
#########################################################
# TOOLCHAIN
#########################################################
GCC_EXEC_PREFIX =arm-none-eabi
GCC = $(GCC_EXEC_PREFIX)-gcc
GPP = $(GCC_EXEC_PREFIX)-g++
AS = $(GCC_EXEC_PREFIX)-as
LD = $(GCC_EXEC_PREFIX)-ld
SIZE = $(GCC_EXEC_PREFIX)-size
OBJCOPY = $(GCC_EXEC_PREFIX)-objcopy
OBJDUMP = $(GCC_EXEC_PREFIX)-objdump
DBG = $(GCC_EXEC_PREFIX)-insight
JLINK =JLinkExe
REMOVE = rm -f
#########################################################
#Startup files and libraries
#########################################################
BSPPATH   =startup
INCSPATH +=
LIBSPATH +=lib
DEVICE  =STM32F103C8
GCFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -fmessage-length=0
#GCFLAGS =-O3 -mcpu=cortex-m3 -mthumb -Wall -g
LDFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Wl,--gc-sections
LIBS =-nostdlib #-lgcc -lc -lnosys
CSRCS   +=system_stm32f1xx.c
ASRCS   +=startup_stm32f103xb.s
LDSCRIPT ="startup/STM32F103C8Tx_FLASH.ld"
##########################################################
OBJECTS =$(addprefix $(OBJPATH)/, $(CSRCS:.c=.o)) $(addprefix $(OBJPATH)/,$(ASRCS:.s=.o))
VPATH += $(CSRCPATH) $(BSPPATH) $(LIBSPATH)

all: $(TARGET).bin stats

	
$(TARGET).axf:  $(OBJECTS)
	@echo "---- Linking ---->" $@
	$(GCC) $(LDFLAGS) -T$(LDSCRIPT) $(addprefix -L, $(LIBSPATH)) -o "$(TARGET).axf" $(OBJECTS) $(LIBS)
#arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -T"../STM32F103C8Tx_FLASH.ld" -Wl,-Map=output.map -Wl,--gc-sections -o "TDSO.elf" @"objects.list"   -lm

$(TARGET).hex: $(TARGET).axf
	@$(OBJCOPY) -O ihex -j .text -j .data $(TARGET).axf $(TARGET).hex

$(TARGET).bin: $(TARGET).axf
	@$(OBJCOPY) -O binary -j .text -j .data $(TARGET).axf $(TARGET).bin

stats: $(TARGET).axf
	@echo "---- Sections Summary ---"
	@$(SIZE) -A -x $<

aslist: $(TARGET).axf
	@$(OBJDUMP) -D $(TARGET).axf > $(TARGET).lst
	
dis: $(TARGET).axf
	@$(OBJDUMP) -S $(TARGET).axf > $(TARGET).lst


clean:
	$(REMOVE) $(OBJECTS) $(XOBJS) $(TARGET) $(TARGET).exe $(TARGET).hex $(TARGET).axf *.bin	

rebuild: clean all

debug:	$(TARGET).axf
	$(DBG) $(TARGET).axf
	
doc:
	doxygen difs.txt

$(TARGET).jlink:
	@echo "Creating Jlink configuration file"
	echo "erase\nloadbin $(TARGET).bin , 0x8000000\nr\nq" > $(TARGET).jlink
	
flash: $(TARGET).jlink #tdso.bin #$(TARGET).bin
	$(JLINK) -device $(DEVICE) -if SWD -speed auto -CommanderScript $(TARGET).jlink

$(TARGET).cfg:
	@echo "Creating opencod configuration file"
	echo "interface jlink\ntransport select swd\nsource [find target/stm32f1x.cfg]\nadapter_khz 4000" > $(TARGET).cfg

program: $(TARGET).axf $(TARGET).cfg
	openocd -f $(TARGET).cfg -c "program $(TARGET).axf verify reset exit"


$(OBJPATH)/%.o : %.c
	@echo "---- Compile" $< "---->" $@
	$(GCC) $(GCFLAGS) $(addprefix -I, $(INCSPATH)) $(GCSYMBOLS) -c $< -o $@
	
$(OBJPATH)/%.o : %.s
	@echo "---- Assemble" $< "---->" $@
	@$(AS) $(ASFLAGS) $< -o $@
########################################################
# Emulator make
########################################################
GPP = gcc

XTARGET  =$(TARGET)
XLIBPATH =.
XOBJPATH =$(OBJPATH)
XINCPATH =Inc Drivers/inc

XCSRC =tdso_main.c dso.c capture_emu.c tdso_util.c control.c softpower.c
XOBJS = $(addprefix $(XOBJPATH)/, $(XCSRC:.c=.obj))

XCFLAGS =-Wall -D__EMU__

ifeq ($(shell uname -s),Linux)
XLIBS +=-lSDL2 -lm

emulator: XLIBS +=-lemu
emulator: $(XTARGET)
	./$(XTARGET)	

emulatorx64: XLIBS +=-lemux64
emulatorx64: $(XTARGET)
	./$(XTARGET)
else

XLIBS =-lemu -lmingw32 -lSDL2main -lSDL2 -lm
XCFLAGS += -Wl,-subsystem,windows -w
XINCPATH += "C:/tools/SDL2/include" "../../includes"
XLIBPATH +="C:/tools/SDL2/lib" "C:/tools/MinGw/lib"

emulator: $(XTARGET)
	cmd /c $(XTARGET).exe
endif

$(XTARGET): $(XOBJS)
	$(GPP) $(XOBJS) $(XCFLAGS) $(addprefix -L, $(XLIBPATH)) $(XLIBS) -o $(XTARGET)

$(XOBJPATH)/%.obj: %.c
	$(GPP) $(addprefix -I, $(XINCPATH)) $(XCFLAGS) -c $< -o $@
	

.PHONY: doc
	

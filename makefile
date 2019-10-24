#########################################################
# project files
#########################################################
TARGET =$(OBJPATH)/tdso
PRJPATH =.
OBJPATH =build
#LIBEMB_PATH = $(HOME)/Dropbox/Projects/software/libemb

#Drivers/interface/src
CSRCPATH =app \
$(LIBEMB_PATH)/drv/lcd \
$(LIBEMB_PATH)/button \
$(LIBEMB_PATH)/drv/tft \
$(LIBEMB_PATH)/display \
$(LIBEMB_PATH)/drv/clock \

INCSPATH =app \
$(LIBEMB_PATH)/include

CSRCS = button.c display.c font.c lcd.c dso.c control.c softpower.c tdso_main.c tdso_util.c tests.c

CFLAGS += -D__TDSO__

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
REMOVE = rm -fR
##########################################################

OBJECTS =$(addprefix $(OBJPATH)/, $(CSRCS:.c=.o)) $(addprefix $(OBJPATH)/,$(ASRCS:.s=.o))
VPATH += $(CSRCPATH)

-include bsp/bluepill/sources.mk

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
	@echo "\n---- Sections Summary ---"
	@$(SIZE) -A -x $<
	@$(SIZE) -t  $<

aslist: $(TARGET).axf
	@$(OBJDUMP) -D $(TARGET).axf > $(TARGET).lst
	
dis: $(TARGET).axf
	@$(OBJDUMP) -S $(TARGET).axf > $(TARGET).lst


clean:
#$(REMOVE) $(OBJECTS) $(XOBJS) $(TARGET) $(TARGET).exe $(TARGET).hex $(TARGET).axf *.bin libemu.a
	$(REMOVE) $(OBJPATH)
	
$(OBJPATH):
	mkdir $@	
	
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


$(OBJPATH)/%.o : %.c | $(OBJPATH)
	@echo "---- Compile" $< "---->" $@
	@$(GCC) $(GCFLAGS) $(addprefix -I, $(INCSPATH)) $(addprefix -D, $(GCSYMBOLS))  -c $< -o $@
	
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
XINCPATH =$(LIBEMB_PATH)/include app bsp/emu

XLIBEMU =libemu.a

XCSRC =tdso_main.c dso.c capture_emu.c tdso_util.c control.c softpower.c
XOBJS = $(addprefix $(XOBJPATH)/, $(XCSRC:.c=.obj))

XCFLAGS =-Wall

ifeq ($(shell uname -s),Linux)
XLIBS +=$(shell sdl2-config --libs) -lemu -lm 

$(XLIBEMU):
	$(MAKE) -C $(LIBEMB_PATH)/lcdemulator APP_PATH=$(CURDIR)/bsp/emu lib 

emulator: $(XTARGET)
	./$(XTARGET)	

else

XLIBS =-lemu -lmingw32 -lSDL2main -lSDL2 -lm
XCFLAGS += -Wl,-subsystem,windows -w
XINCPATH += "C:/tools/SDL2/include" "../../includes"
XLIBPATH +="C:/tools/SDL2/lib" "C:/tools/MinGw/lib"

emulator: $(XTARGET)
	cmd /c $(XTARGET).exe
endif

$(XTARGET): $(XOBJS) $(XLIBEMU)
	$(GPP) $(XOBJS) $(XCFLAGS) $(addprefix -L, $(XLIBPATH)) $(XLIBS) -o $(XTARGET)

$(XOBJPATH)/%.obj: %.c | $(OBJPATH)
	$(GPP) $(addprefix -I, $(XINCPATH)) $(XCFLAGS) -c $< -o $@
	

.PHONY: doc
	

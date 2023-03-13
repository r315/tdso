#########################################################
# project files
#########################################################
TARGET =tdso
TARGET_PATH =$(CURDIR)
BUILD_DIR :=build
ROOT_PATH :=$(CURDIR)/../..
LIB_PATH :=$(ROOT_PATH)/sdk_emb/lib
APP_PATH :=$(ROOT_PATH)/app

CSRCPATH =\
$(LIB_PATH)/src \
$(APP_PATH) \

INCSPATH =\
$(LIB_PATH)/inc \
$(APP_PATH) \

CSRCS =\
libbutton.c \
lib2d.c \
liblcd.c \
font.c \
dso.c \
control.c \
power.c \
tdso.c \
tdso_util.c \
tests.c \
sample_engine.c \

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
VERBOSE =@
##########################################################

OBJECTS =$(addprefix $(BUILD_DIR)/, $(CSRCS:.c=.o)) $(addprefix $(BUILD_DIR)/,$(ASRCS:.s=.o))
VPATH += $(CSRCPATH)

-include $(TARGET_PATH)/sources.mk

all: $(TARGET).bin stats

	
$(TARGET).axf:  $(OBJECTS)
	@echo "---- Linking ---->" $@
	$(VERBOSE)$(GCC) $(LDFLAGS) -T$(LDSCRIPT) $(addprefix -L, $(LIBSPATH)) -o "$(TARGET).axf" $(OBJECTS) $(LIBS)
#arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -T"../STM32F103C8Tx_FLASH.ld" -Wl,-Map=output.map -Wl,--gc-sections -o "TDSO.elf" @"objects.list"   -lm

$(TARGET).hex: $(TARGET).axf
	$(VERBOSE)$(OBJCOPY) -O ihex -j .text -j .data $(TARGET).axf $(TARGET).hex

$(TARGET).bin: $(TARGET).axf
	$(VERBOSE)$(OBJCOPY) -O binary -j .text -j .data $(TARGET).axf $(TARGET).bin

stats: $(TARGET).axf
	@echo "---- Sections Summary ---"
	$(VERBOSE)$(SIZE) -A -x $<
	$(VERBOSE)$(SIZE) -t  $<

aslist: $(TARGET).axf
	$(VERBOSE)$(OBJDUMP) -D $(TARGET).axf > $(TARGET).lst
	
dis: $(TARGET).axf
	$(VERBOSE)$(OBJDUMP) -S $(TARGET).axf > $(TARGET).lst


clean:
#$(REMOVE) $(OBJECTS) $(XOBJS) $(TARGET) $(TARGET).exe $(TARGET).hex $(TARGET).axf *.bin libemu.a
	$(VERBOSE)$(REMOVE) $(BUILD_DIR)
	
$(BUILD_DIR):
	mkdir -p $@	
	
rebuild: clean all

debug:	$(TARGET).axf
	$(VERBOSE)$(DBG) $(TARGET).axf
	
doc:
	doxygen difs.txt

$(TARGET).jlink:
	@echo "Creating Jlink configuration file"
	echo "erase\nloadbin $(TARGET).bin , 0x8000000\nr\nq" > $(TARGET).jlink
	
flash: $(TARGET).jlink #tdso.bin #$(TARGET).bin
	$(JLINK) -device $(DEVICE) -if SWD -speed auto -CommanderScript $(TARGET).jlink

$(TARGET).cfg:
	@echo "Creating opencod configuration file"
	echo "interface jlink" > $(TARGET).cfg
	echo "transport select swd" >> $(TARGET).cfg
	echo "source [find target/stm32f1x.cfg]" >> $(TARGET).cfg
	echo "adapter_khz 4000" >> $(TARGET).cfg

program: $(TARGET).axf $(TARGET).cfg
	openocd -f $(TARGET).cfg -c "program $(TARGET).axf verify reset exit"


$(BUILD_DIR)/%.o : %.c | $(BUILD_DIR)
	@echo "---- Compile" $< "---->" $@
	$(VERBOSE)$(GCC) $(GCFLAGS) $(addprefix -I, $(INCSPATH)) $(addprefix -D, $(GCSYMBOLS))  -c $< -o $@
	
$(BUILD_DIR)/%.o : %.s
	@echo "---- Assemble" $< "---->" $@
	$(VERBOSE)$(AS) $(ASFLAGS) $< -o $@
	

########################################################
# Emulator make
########################################################
GPP = gcc

XTARGET  =$(TARGET)
XLIBPATH =.
XBUILD_DIR =$(BUILD_DIR)
XINCPATH =$(LIBEMB_PATH)/include app bsp/emu

XLIBEMU =libemu.a

XCSRC =tdso_main.c dso.c capture_emu.c tdso_util.c control.c softpower.c
XOBJS = $(addprefix $(XBUILD_DIR)/, $(XCSRC:.c=.obj))

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

$(XBUILD_DIR)/%.obj: %.c | $(BUILD_DIR)
	$(GPP) $(addprefix -I, $(XINCPATH)) $(XCFLAGS) -c $< -o $@
	

.PHONY: doc
	

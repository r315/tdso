#########################################################
# project files
#########################################################
TARGET =tdso
EXEC =$(BUILD_DIR)/$(TARGET)
BUILD_DIR :=build
TARGET_PATH =$(CURDIR)
APP_PATH :=$(ROOT_PATH)/app
LIB_PATH :=$(ROOT_PATH)/lib

INCSPATH =\
$(LIB_PATH)/inc \
$(APP_PATH) \
$(TARGET_PATH)/Inc \

CSRCS =\
$(SDK_LIB_PATH)/src/libbutton.c \
$(SDK_LIB_PATH)/src/lib2d.c \
$(SDK_LIB_PATH)/src/liblcd.c \
$(SDK_LIB_PATH)/src/font.c \
$(APP_PATH)/dso.c \
$(APP_PATH)/tdso.c \
$(APP_PATH)/control.c \
$(APP_PATH)/power.c \
$(APP_PATH)/tdso_test.c \
$(APP_PATH)/tdso_util.c \

include $(TARGET_PATH)/sources.mk
#########################################################
# TOOLCHAIN
#########################################################
GCC_EXEC_PREFIX =arm-none-eabi-
GCC = $(GCC_EXEC_PREFIX)gcc
GPP = $(GCC_EXEC_PREFIX)g++
AS = $(GCC_EXEC_PREFIX)as
LD = $(GCC_EXEC_PREFIX)ld
SIZE = $(GCC_EXEC_PREFIX)size
OBJCOPY = $(GCC_EXEC_PREFIX)objcopy
OBJDUMP = $(GCC_EXEC_PREFIX)objdump
DBG = $(GCC_EXEC_PREFIX)insight
JLINK =JLinkExe
REMOVE = rm -fR
ifndef V
VERBOSE =@
else
VERBOSE =
endif
##########################################################

OBJECTS =$(addprefix $(BUILD_DIR)/, $(notdir $(CSRCS:.c=.o))) $(addprefix $(BUILD_DIR)/, $(notdir $(ASRCS:.s=.o)))
VPATH  +=$(sort $(dir $(CSRCS))) $(sort $(dir $(ASRCS)))

all: $(EXEC).elf size

$(EXEC).elf:  $(OBJECTS)
	@echo "[LD]  $@"
	$(VERBOSE)$(GCC) $(LDFLAGS) -T$(LDSCRIPT) -o "$@" $^ $(LIBS)

$(EXEC).hex: $(EXEC).elf
	$(VERBOSE)$(OBJCOPY) -O ihex -j .text -j .data $< $@

$(EXEC).bin: $(EXEC).elf
	$(VERBOSE)$(OBJCOPY) -O binary -j .text -j .data $< $@

size: $(EXEC).elf
	@echo "---- Sections Summary ---"
	$(VERBOSE)$(SIZE) -A -x $<
	$(VERBOSE)$(SIZE) -t  $<

aslist: $(EXEC).elf
	$(VERBOSE)$(OBJDUMP) -D $< > $(TARGET).lst
	
dis: $(EXEC).elf
	$(VERBOSE)$(OBJDUMP) -S $< > $(TARGET).lst

clean:
	$(VERBOSE)$(REMOVE) $(BUILD_DIR)
	
$(BUILD_DIR):
	$(VERBOSE)mkdir -p $@	
	
rebuild: clean all

debug:	$(EXEC).elf
	$(VERBOSE)$(DBG) $<
	
doc:
	doxygen difs.txt

$(EXEC).jlink:
	@echo "Creating Jlink configuration file"
	echo "erase\nloadbin $(TARGET).bin , 0x8000000\nr\nq" > $(TARGET).jlink
	
flash: $(TARGET).jlink #tdso.bin #$(TARGET).bin
	$(JLINK) -device $(DEVICE) -if SWD -speed auto -CommanderScript $(TARGET).jlink

$(EXEC).cfg:
	@echo "Creating opencod configuration file"
	echo "interface jlink" > $(TARGET).cfg
	echo "transport select swd" >> $(TARGET).cfg
	echo "source [find target/stm32f1x.cfg]" >> $(TARGET).cfg
	echo "adapter_khz 4000" >> $(TARGET).cfg

program: $(EXEC).elf $(TARGET).cfg
	openocd -f $(TARGET).cfg -c "program $(EXEC).elf verify reset exit"


$(BUILD_DIR)/%.o : %.c $(BUILD_DIR)
	@echo "[CC]  " $<
	$(VERBOSE)$(GCC) $(GCFLAGS) $(addprefix -I, $(INCSPATH)) $(addprefix -D, $(GCSYMBOLS))  -c $< -o $@
	
$(BUILD_DIR)/%.o : %.s $(BUILD_DIR)
	@echo "[AS]  " $<
	$(VERBOSE)$(AS) $(ASFLAGS) $< -o $@

######################################
# target
######################################
TARGET =tdso_artery

######################################
# building variables
######################################
ifdef RELEASE
DEBUG =0
OPT   =-O2
else
DEBUG =1
OPT   =-O0 -g -gdwarf-2
endif

#######################################
# paths
#######################################
BUILD_DIR :=build
ROOT_PATH :=$(CURDIR)
TARGET_PATH =$(CURDIR)
DRIVERS_PATH :=$(TARGET_PATH)/../Drivers
DRIVERS_HAL_PATH =$(DRIVERS_PATH)/StdPeriph_Driver
DRIVERS_CUSTOM_PATH =$(DRIVERS_PATH)/Costum
DRIVERS_COMPONENT_PATH =$(DRIVERS_PATH)/Component
CMSIS_PATH :=$(DRIVERS_PATH)/CMSIS
APP_PATH :=$(ROOT_PATH)/app
LIB_PATH :=$(ROOT_PATH)/lib

C_INCLUDES =  \
$(APP_PATH) \
$(TARGET_PATH) \
$(DRIVERS_HAL_PATH)/inc \
$(DRIVERS_CUSTOM_PATH)/inc \
$(DRIVERS_COMPONENT_PATH)/inc \
$(LIB_PATH)/inc \
$(CMSIS_PATH)/Device/ARTERY/AT32F4xx/inc \
$(CMSIS_PATH)/Core/Include \

AS_INCLUDES =\

######################################
# sources
######################################

BOARD_SOURCES = \
$(TARGET_PATH)/startup_at32f415cbt7.c\
$(TARGET_PATH)/board_artery.c \
$(DRIVERS_HAL_PATH)/src/at32f4xx_rcc.c \
$(DRIVERS_CUSTOM_PATH)/src/spi_at32f4xx.c \
$(DRIVERS_CUSTOM_PATH)/src/dma_at32f4xx.c \
$(DRIVERS_CUSTOM_PATH)/src/gpio_at32f4xx.c \
$(DRIVERS_COMPONENT_PATH)/src/ili9341.c \
$(TARGET_PATH)/sample_engine.c \

C_SOURCES = \
$(BOARD_SOURCES) \
$(LIB_PATH)/src/liblcd.c \
$(LIB_PATH)/src/lib2d.c \
$(LIB_PATH)/src/libbutton.c \
$(LIB_PATH)/src/font.c \
$(APP_PATH)/control.c \
$(APP_PATH)/tdso.c \
$(APP_PATH)/dso.c \
$(APP_PATH)/power.c \
$(APP_PATH)/tdso_util.c \
$(APP_PATH)/tdso_test.c \


# ASM sources
#ASM_SOURCES =startup_at32f415cbt7.s

LDSCRIPT :=$(TARGET_PATH)/AT32F415xB_FLASH.ld

OCD_CONFIG =$(TARGET_PATH)/at32f415.cfg

#######################################
# binaries
#######################################
BINPATH = 
PREFIX = arm-none-eabi-
CC = $(BINPATH)$(PREFIX)gcc
CPP = $(BINPATH)$(PREFIX)g++
AS = $(BINPATH)$(PREFIX)gcc -x assembler-with-cpp
CP = $(BINPATH)$(PREFIX)objcopy
AR = $(BINPATH)$(PREFIX)ar
SZ = $(BINPATH)$(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

ifeq ($(GCC_COLORS), )
export GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'
#unexport GCC_COLORS
endif
#######################################
# CFLAGS
#######################################
# cpu
CPU =-mcpu=cortex-m4 -mthumb

# fpu
#at43f415 does not have FPU
#FPU =-mfloat-abi=hard -mfpu=fpv4-sp-d16 

# float-abi
FLOAT-ABI = #-u_printf_float

# mcu
MCU = $(CPU) $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS +=\
AT32F415CBT7 \
USE_STDPERIPH_DRIVER

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(addprefix -D, $(C_DEFS)) $(addprefix -I, $(C_INCLUDES)) $(OPT) -Wall -fdata-sections -ffunction-sections -std=gnu11
CPPFLAGS = $(MCU) $(C_DEFS) $(addprefix -I, $(C_INCLUDES)) $(OPT) -Wall -fdata-sections -ffunction-sections

# Generate dependency information
#CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst))

ifndef V
VERBOSE =@
else
VERBOSE =
endif
#######################################
# linker
#######################################

# libraries
ifdef SEMIHOSTING
SPECS =--specs=rdimon.specs
LDLIBS =-nostartfiles -lc -lrdimon
else
SPECS =--specs=nano.specs
#-nostdlib -lstd++ -lnosys -lm 
LDLIBS =
endif

LDLIBS_PATH =
LDFLAGS = $(MCU) $(SPECS) -T$(LDSCRIPT) $(LDLIBS_PATH) $(LDLIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

#######################################
# Rules
#######################################
# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf size
#$(BUILD_DIR)/$(TARGET).bin 

program: $(BUILD_DIR)/$(TARGET).elf
	openocd -f $(OCD_CONFIG) -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
#vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.obj)))
#vpath %.cpp $(sort $(dir $(CPP_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
#vpath %.s $(sort $(dir $(ASM_SOURCES)))

VPATH +=$(sort $(dir $(C_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo "CC  " $<
	$(VERBOSE)$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.obj: %.cpp Makefile | $(BUILD_DIR)
	@echo "CP  " $<
	$(VERBOSE)$(CPP) -c $(CPPFLAGS)  -fno-exceptions -fno-unwind-tables -fno-rtti $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo "AS " $<
	$(VERBOSE)$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) #Makefile
	@echo "--- Linking ---"
	@echo $@
	$(VERBOSE)$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(VERBOSE)$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(VERBOSE)$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir -p $@		

size: $(BUILD_DIR)/$(TARGET).elf
	@echo "--- Size ---"
	$(VERBOSE)$(SZ) -A -x $<
	$(VERBOSE)$(SZ) -B $<

#######################################
# clean up
#######################################
clean:
	$(VERBOSE)-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
#-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***

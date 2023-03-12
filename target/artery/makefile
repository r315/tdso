######################################
# target
######################################
TARGET =blink

######################################
# building variables
######################################
ifeq ($(RELEASE), 1)
DEBUG = 1
OPT = -Os
else
DEBUG = 1
OPT = -Og
endif

#######################################
# paths
#######################################
ARTERY_SDK_ROOT =$(CURDIR)
DRIVERS_PATH  =$(ARTERY_SDK_ROOT)/Drivers
APP_SRC_PATH :=$(CURDIR)


SOURCES_PATH =  \
$(APP_SRC_PATH) \
$(DRIVERS_PATH)/StdPeriph_Driver/src \
$(ARTERY_SDK_ROOT)/Startup \


# firmware library path
PERIFLIB_PATH = 

# Build path
BUILD_DIR :=build

######################################
# Include
######################################

# C includes
C_INCLUDES =  \
$(APP_SRC_PATH) \
$(DRIVERS_PATH)/StdPeriph_Driver/inc \
$(DRIVERS_PATH)/CMSIS/Device/ARTERY/AT32F4xx/Include \
$(DRIVERS_PATH)/CMSIS/Core/Include \
# AS includes
AS_INCLUDES =\

######################################
# source
######################################

# C sources
C_SOURCES =  \
blink.c \
system_at32f4xx.c \
at32f4xx_rcc.c \
at32f4xx_gpio.c \
misc.c \

# ASM sources
ASM_SOURCES =  \
$(APP_SRC_PATH)/startup_at32f415cbt7.s

#######################################
# Misc files
#######################################

# Linker script
LDSCRIPT :=$(APP_SRC_PATH)/AT32F415xB_FLASH.ld

# Openocd configuration
OCD_CONFIG =$(APP_SRC_PATH)/at32f415.cfg

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
CPU = -mcpu=cortex-m4

# fpu
# NONE

# float-abi
FLOAT-ABI = #-u_printf_float

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS +=\
-DAT32F415CBT7 \
-DUSE_STDPERIPH_DRIVER

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(addprefix -I, $(C_INCLUDES)) $(OPT) -Wall -fdata-sections -ffunction-sections -std=gnu11
CPPFLAGS = $(MCU) $(C_DEFS) $(addprefix -I, $(C_INCLUDES)) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
DEBUGFLAGS =-g -gdwarf-2
endif

CFLAGS +=$(DEBUGFLAGS)
CPPFLAGS +=$(DEBUGFLAGS)

# Generate dependency information
#CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst))

#######################################
# LDFLAGS
#######################################
# link script


SPECS =-specs=nano.specs
# libraries
#LIBS =-nostartfiles -nostdlib
LIBS =-lstdc++#-lnosys -lm
LIBDIR = 
LDFLAGS = $(MCU) $(SPECS) -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

#######################################
# Rules
#######################################
# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin size


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
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
#vpath %.s $(sort $(dir $(ASM_SOURCES)))

VPATH +=$(SOURCES_PATH)

$(BUILD_DIR)/%.o: %.c $(BUILD_DIR)
	@echo "CC  " $<
	@$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.obj: %.cpp $(BUILD_DIR)
	@echo "CP  " $<
	@$(CPP) -c $(CPPFLAGS)  -fno-exceptions -fno-unwind-tables -fno-rtti $< -o $@

$(BUILD_DIR)/%.o: %.s $(BUILD_DIR)
	@echo "AS " $<
	@$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) #Makefile
	@echo "--- Linking ---"
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir -p $@		

size: $(BUILD_DIR)/$(TARGET).elf
	@echo "--- Size ---"
	$(SZ) -A -x $<
	$(SZ) -B $<

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
#-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***

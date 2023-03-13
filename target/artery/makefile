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
BUILD_DIR :=build
SDK_PATH :=$(CURDIR)/../../sdk_emb
SDK_DRIVERS_PATH :=$(SDK_PATH)/drv
SDK_LIB_PATH :=$(SDK_PATH)/lib
CMSIS_PATH :=$(CURDIR)/../../target/CMSIS_5-5.9.0

SOURCES_PATH =  \
$(APP_SRC_PATH) \
$(DRIVERS_PATH)/StdPeriph_Driver/src \
$(ARTERY_SDK_ROOT)/Startup \
$(SDK_DRIVERS_PATH)/src \

C_INCLUDES =  \
$(APP_SRC_PATH) \
$(SDK_DRIVERS_PATH)/inc \
$(SDK_LIB_PATH)/inc \
$(DRIVERS_PATH)/StdPeriph_Driver/inc \
$(CMSIS_PATH)/Device/ARTERY/AT32F4xx/Include \
$(CMSIS_PATH)/CMSIS/Core/Include \

AS_INCLUDES =\

######################################
# source
######################################

# C sources
C_SOURCES =  \
startup_at32f415cbt7.c\
board_artery.c \
at32f4xx_rcc.c \
spi_at32f4xx.c \
dma_at32f4xx.c \
gpio_at32f4xx.c \
ili9341.c \


# ASM sources
#ASM_SOURCES =startup_at32f415cbt7.s

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

VERBOSE =@
ifeq ($(GCC_COLORS), )
export GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'
#unexport GCC_COLORS
endif
#######################################
# CFLAGS
#######################################
# cpu
CPU =-mcpu=cortex-m4

# fpu
# FPU =

# float-abi
FLOAT-ABI = #-u_printf_float

# mcu
MCU = $(CPU) $(FPU) $(FLOAT-ABI) -mthumb

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
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
#vpath %.s $(sort $(dir $(ASM_SOURCES)))

VPATH +=$(SOURCES_PATH)

$(BUILD_DIR)/%.o: %.c $(BUILD_DIR)
	@echo "CC  " $<
	$(VERBOSE)$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.obj: %.cpp $(BUILD_DIR)
	@echo "CP  " $<
	$(VERBOSE)$(CPP) -c $(CPPFLAGS)  -fno-exceptions -fno-unwind-tables -fno-rtti $< -o $@

$(BUILD_DIR)/%.o: %.s $(BUILD_DIR)
	@echo "AS " $<
	$(VERBOSE)$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) #Makefile
	@echo "--- Linking ---"
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

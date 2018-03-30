#########################################################
# Board specific files
#########################################################
CSRCPATH +=bsp/blueboard bsp/blueboard/startup
INCSPATH +=bsp/blueboard
LDSCRIPT ="bsp/blueboard/startup/linker.ld"

CSRCS +=blueboard.c ili9328.c startup.c #mystdlib.c

GCSYMBOLS +=__BLUEBOARD__
DEVICE  =LPC1768
#########################################################
#Startup files and libraries
#########################################################
GCFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Og -g3 -Wall -fmessage-length=0 -ffunction-sections
LDFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Wl,--gc-sections
#LIBS =-nostdlib -lgcc #-lc -lnosys



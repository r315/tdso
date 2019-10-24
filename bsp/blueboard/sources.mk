#########################################################
# Board specific files
#########################################################
CSRCPATH +=$(LIBEMB_PATH)/bsp/blueboard $(LIBEMB_PATH)/bsp/blueboard/startup
INCSPATH +=$(LIBEMB_PATH)/bsp/blueboard bsp/blueboard
LDSCRIPT ="$(LIBEMB_PATH)/bsp/blueboard/lpc1768.ld"

CSRCS +=blueboard.c ili9328.c startup_lpc1768.c capture_bb.c #mystdlib.c

GCSYMBOLS +=__BLUEBOARD__
DEVICE  =LPC1768
#########################################################
#Startup files and libraries
#########################################################
GCFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Og -g3 -Wall -fmessage-length=0 -ffunction-sections
LDFLAGS =-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Wl,--gc-sections
#LIBS =-nostdlib -lgcc #-lc -lnosys



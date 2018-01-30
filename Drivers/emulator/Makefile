CC = gcc
#####################################################
TARGET =EmulatorTest
INCPATH =../interface/inc inc
LIBPATH =.
SRCPATH =../interface/src src
OBJS =emulatorTest.o font8x16.o lcdsdl.o emulator.o #lcd.c
#####################################################
LIBTARGET =emu
LIBTARGETPATH ="."
LIBOBJS =lcdsdl.o emulator.o lcd.o fonts.o display.o
#####################################################
VPATH +=$(SRCPATH)

ifeq ($(shell uname -s),Linux)
CFLAGS =-Wall -D__EMU__
LDFLAGS =-lSDL2 -lSDL2main
else
CFLAGS =-Wall -D__EMU__
INCPATH +="C:/tools/SDL2/include"
LIBPATH +="C:/tools/SDL2/lib" "C:/tools/MinGw/lib"
LDFLAGS = -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2
endif
#####################################################
all : lib

test: $(TARGET)
	./$(TARGET)	

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(addprefix -L, $(LIBPATH)) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe $(LIBTARGETPATH)/lib$(LIBTARGET).a

.c.o:
#%.o:%.c
	$(CC) $(addprefix -I, $(INCPATH)) $(CFLAGS) -c $< -o ./$@

lib: $(LIBOBJS)
	ar rcs $(LIBTARGETPATH)/lib$(LIBTARGET).a $(LIBOBJS)
	rm -f *.o
#####################################################
	

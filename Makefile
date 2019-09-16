CFLAGS = -std=c99 -Wall -pedantic -ggdb
BIN = gui

ifeq ($(OS),Windows_NT)
LIBS = -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lm -lGLU32 -lGLEW32
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LIBS = -lSDL2 -framework OpenGL -lm -lGLEW
	else
		LIBS = -lSDL2 -lGL -lm -lGLU -lGLEW
	endif
endif

all: $(BIN)
	
clean:
	rm -f $(BIN)


gui: main.c chp8.c defines.h chp8_gui.c emu.c
	$(CC) main.c chp8.c chp8_gui.c $(CFLAGS) -o $@ $(LIBS)

.PHONY: all clean

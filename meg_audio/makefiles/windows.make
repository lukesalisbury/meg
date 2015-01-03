ifeq ($(SUPPORTPATH), )
	SUPPORTPATH = D:/mingw/supportlibs
endif

PLATFORM = __GNUWIN32__
PLATFORM_LIBS = -L"$(SUPPORTPATH)/lib" -lglib-2.0.dll -m32
PLATFORM_FLAGS = -I"$(SUPPORTPATH)/include" -I"$(SUPPORTPATH)/include/glib-2.0" -I"$(SUPPORTPATH)/lib/glib-2.0/include" -D_CRT_SECURE_NO_WARNINGS -m32

USE_SDL2=true

ifeq ($(USE_SDL2), true)
	PLATFORM_LIBS += -lSDL2 -lSDL2_mixer 
	PLATFORM_FLAGS += -I"$(SUPPORTPATH)/include/SDL2" 
else
	PLATFORM_LIBS += -lSDL -lSDL_mixer 
	PLATFORM_FLAGS += -I"$(SUPPORTPATH)/include/SDL"
endif

BIN = libmegaudio.dll
CC = gcc

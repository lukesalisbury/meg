ifeq ($(SUPPORTPATH), )
	SUPPORTPATH = D:/mingw/supportlibs
endif

PLATFORM = __GNUWIN32__
PLATFORM_LIBS =  -L"$(SUPPORTPATH)/lib" -lglib-2.0.dll
PLATFORM_FLAGS = -I"$(SUPPORTPATH)/include" -I"$(SUPPORTPATH)/include/glib-2.0" -I"$(SUPPORTPATH)/lib/glib-2.0/include" -D_CRT_SECURE_NO_WARNINGS

USE_SDL2=true

ifeq ($(USE_SDL2), true)
	PLATFORM_LIBS += -L"C:/dev/sdl2libs/lib" -lSDL2 -lSDL2_mixer 
	PLATFORM_FLAGS += -I"C:/dev/sdl2libs/include/SDL2" 
else
	PLATFORM_LIBS += -lSDL -lSDL_mixer 
	PLATFORM_FLAGS += -I"$(SUPPORTPATH)/include/SDL"
endif

BIN = libmegaudio.dll
CC = gcc

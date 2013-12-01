ifeq ($(SUPPORTPATH), )
	SUPPORTPATH = D:/mingw/supportlibs
endif

PLATFORM = __GNUWIN32__
PLATFORM_LIBS =  -L"$(SUPPORTPATH)/lib" -lSDL -lSDL_mixer -lglib-2.0.dll
PLATFORM_FLAGS = -I"$(SUPPORTPATH)/include" -I"$(SUPPORTPATH)/include/glib-2.0" -I"$(SUPPORTPATH)/lib/glib-2.0/include" -D_CRT_SECURE_NO_WARNINGS


BIN = libmegaudio.dll
CC = gcc

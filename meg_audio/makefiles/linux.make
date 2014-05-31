
PLATFORM = LINUX

PLATFORM_LIBS =
PLATFORM_FLAGS =
GTKPACKAGES = error

ifeq (${shell pkg-config gtk+-2.0 --exists && echo 1}, 1)
	GTKPACKAGES = gtk+-2.0 
endif

ifeq (${shell pkg-config gtk+-3.0 --exists && echo 1}, 1)
	GTKPACKAGES = gtk+-3.0 
endif

ifeq (${shell pkg-config sdl2 --exists && echo 1}, 1)
	PLATFORM_FLAGS += `sdl2-config --cflags`
	PLATFORM_LIBS += `sdl2-config --libs` -lSDL2_mixer
else
	PLATFORM_FLAGS += `sdl-config --cflags`
	PLATFORM_LIBS += `sdl-config --libs` -lSDL_mixer
endif



PLATFORM_FLAGS += `pkg-config $(GTKPACKAGES) --cflags` -fPIC
PLATFORM_LIBS += `pkg-config $(GTKPACKAGES) --libs`

RES =
CC = gcc

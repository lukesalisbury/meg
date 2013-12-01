
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

PLATFORM_FLAGS += `pkg-config $(GTKPACKAGES) --cflags` `sdl-config --cflags` -fPIC
PLATFORM_LIBS += `pkg-config $(GTKPACKAGES) --libs` -lSDL -lSDL_mixer


RES =
CC = gcc

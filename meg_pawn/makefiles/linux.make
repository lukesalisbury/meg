PLATFORM = LINUX
PLATFORM_LIBS = -lz -pthread -lm
PLATFORM_FLAGS = -fPIC -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H

GTKVERSION = error

ifeq (${shell pkg-config gtk+-2.0 --exists && echo 1}, 1)
	GTKVERSION = gtk+-2.0
	GTKPACKAGES = gtk+-2.0 gthread-2.0 gmodule-2.0
endif
ifeq (${shell pkg-config gtk+-3.0 --exists && echo 1}, 1)
	GTKVERSION = gtk+-3.0
	GTKPACKAGES = gtk+-3.0 gthread-2.0 gmodule-2.0
endif

PLATFORM_FLAGS += `pkg-config $(GTKPACKAGES) --cflags`
PLATFORM_LIBS += `pkg-config $(GTKPACKAGES) --libs`


ifeq ($(PLATFORMBITS), 64)
	PLATFORM_FLAGS += -DHAVE_I64
endif


RES = 
CC = gcc
BINEXT = 
SOEXT = so
SOPREFIX = lib


ifeq ($(SUPPORTPATH), )
	SUPPORTPATH = /Users/user/gtk/inst
endif

GTK_LIBS = `$(SUPPORTPATH)/bin/pkg-config gtk+-3.0 gthread-2.0 --libs`
GTK_FLAGS = `$(SUPPORTPATH)/bin/pkg-config gtk+-3.0 gthread-2.0 --cflags`


PLATFORM = apple
PLATFORMBITS = 32
PLATFORM_LIBS = -arch i386 $(GTK_LIBS) -lz -pthread -dynamic -dylib -undefined dynamic_lookup
PLATFORM_FLAGS = -arch i386 -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5  $(GTK_FLAGS) 



RES = 
CC = gcc-4.2
BINEXT = 
SOEXT = so

#used for builderheader
MINI_PLATFORM_LIBS = -arch i386 $(GTK_LIBS)
MINI_PLATFORM_FLAGS = $(GTK_FLAGS)

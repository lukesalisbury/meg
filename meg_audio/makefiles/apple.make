ifeq ($(SUPPORTPATH), )
	SUPPORTPATH = /Users/user/gtk/inst
endif

GTK_LIBS = `$(SUPPORTPATH)/bin/pkg-config gtk+-3.0 gthread-2.0 --libs`
GTK_FLAGS = `$(SUPPORTPATH)/bin/pkg-config gtk+-3.0 gthread-2.0 --cflags`

PLATFORM = apple
PLATFORMBITS = 32
PLATFORM_LIBS = -arch i386 $(GTK_LIBS) -F../resources/Frameworks/ -framework SDL_mixer -framework SDL -dylib -undefined dynamic_lookup
PLATFORM_FLAGS = -arch i386 -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5 $(GTK_FLAGS) -dynamic
PLATFORM_FLAGS += -I"../resources/Frameworks/Headers/"

CC = gcc-4.2


ifeq ($(SUPPORTPATH), )
	SUPPORTPATH = /Users/user/gtk/inst
endif

GTK_LIBS = `$(SUPPORTPATH)/bin/pkg-config gtk+-3.0 gtk-mac-integration gmodule-2.0  gthread-2.0 --libs`
GTK_FLAGS = `$(SUPPORTPATH)/bin/pkg-config gtk+-3.0 gtk-mac-integration gmodule-2.0  gthread-2.0 --cflags`

PLATFORM = apple
PLATFORMBITS = 32
PLATFORM_LIBS = -arch i386  $(GTK_LIBS) -lz -framework Carbon -framework IOKit

PLATFORM_FLAGS = -arch i386 -DHAVE_STDINT_H -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5 $(GTK_FLAGS) -fvisibility=default -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DHAVE_ALLOCA_H=0 -I/Library/Frameworks/IOKit.framework/Headers


ifeq ( ${shell $(SUPPORTPATH)/bin/pkg-config libsoup-2.4 --exists && echo 1}, 1)
	PLATFORM_FLAGS +=  -DUSE_SOUP `$(SUPPORTPATH)/bin/pkg-config libsoup-2.4 --cflags`
	PLATFORM_LIBS += `$(SUPPORTPATH)/bin/pkg-config libsoup-2.4 --libs`
endif

ifeq (${shell $(SUPPORTPATH)/bin/pkg-config gtksourceview-3.0 --exists && echo 1}, 1)
	PLATFORM_LIBS += `$(SUPPORTPATH)/bin/pkg-config gtksourceview-3.0 --libs`
	PLATFORM_FLAGS += -DUSEGTKSOURCEVIEW `$(SUPPORTPATH)/bin/pkg-config gtksourceview-3.0 --cflags`
endif


#used for builderheader
MINI_PLATFORM_LIBS = -arch i386 $(GTK_LIBS) 
MINI_PLATFORM_FLAGS = -arch i386 $(GTK_FLAGS)

CC = gcc-4.2
BIN = meg

#$(FINALOUTPUT) :
#	$(CC) -bundle -o $(FINALOUTPUT) $(OBJ)

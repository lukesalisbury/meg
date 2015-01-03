ifeq ($(SUPPORTPATH), )
$(error Please set SUPPORTPATH variable before running make.   )
endif

ifeq ($(GTKVERSION), 3)
$(warning GTK3 for windows not recommend. )
	USEGTKSOURCEVIEW=FALSE
	USESOUP=FALSE
	USECURL=TRUE
	OBJDIR=objects-gtk3
else
	USEGTKSOURCEVIEW=TRUE
	USESOUP=FALSE
	USECURL=TRUE
endif

PLATFORM = __GNUWIN32__
PLATFORM_LIBS = -Wl,--enable-auto-import -L"$(SUPPORTPATH)/lib" -limm32 -lshell32 -lole32 -luuid  -lintl
PLATFORM_LIBS += -lcairo.dll -lpangowin32-1.0.dll -latk-1.0.dll -lgdk_pixbuf-2.0.dll  -lglib-2.0.dll -lgmodule-2.0.dll
PLATFORM_LIBS += -lgobject-2.0.dll -lgthread-2.0.dll -lpango-1.0.dll -lgio-2.0.dll

PLATFORM_FLAGS = -mms-bitfields  -I"$(SUPPORTPATH)/include" -I"$(SUPPORTPATH)/include/atk-1.0" -I"$(SUPPORTPATH)/include/cairo" -I"$(SUPPORTPATH)/include/gdk-pixbuf-2.0" -I"$(SUPPORTPATH)/include/glib-2.0"  -I"$(SUPPORTPATH)/include/pango-1.0" -I"$(SUPPORTPATH)/lib/glib-2.0/include"

ifeq ($(GTKVERSION), 3)
	PLATFORM_LIBS += -lgdk-3.dll -lgtk-3.dll
	PLATFORM_FLAGS += -I"$(SUPPORTPATH)/include/gtk-3.0" -I"$(SUPPORTPATH)/lib/gtk-3.0/include" -DGTK_DISABLE_SINGLE_INCLUDES -DGSEAL_ENABLE
else
	PLATFORM_LIBS += -lgdk-win32-2.0.dll -lgtk-win32-2.0.dll
	PLATFORM_FLAGS += -I"$(SUPPORTPATH)/include/gtk-2.0" -I"$(SUPPORTPATH)/lib/gtk-2.0/include" -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE
endif

ifeq ($(USEGTKSOURCEVIEW), TRUE)
	PLATFORM_LIBS += -lgtksourceview-2.0.dll
	PLATFORM_FLAGS += -DUSEGTKSOURCEVIEW -I"$(SUPPORTPATH)/include/gtksourceview-2.0"
endif
ifeq ($(USESOUP), TRUE)
	PLATFORM_FLAGS += -DUSE_SOUP -I"$(SUPPORTPATH)/include/libsoup-2.4"
	PLATFORM_LIBS += -L"$(SUPPORTPATH)/lib" -lsoup-2.4.dll
endif

ifeq ($(USECURL), TRUE)
	PLATFORM_FLAGS += -DUSE_CURL
	PLATFORM_LIBS +=  -lcurldll -lssl.dll -lcrypto.dll
endif


RES = $(OBJDIR)/meg.res
RES_SOURCE = res/meg.rc
RES_OUTPUT = pe-i386
ifeq ($(PLATFORMBITS), 64)
	RES_OUTPUT = pe-x86-64
endif

ifeq ($(GTKVERSION), 3)
	BIN = meg-gtk3.exe
else
	BIN = meg.exe
endif

CC = gcc

ifeq ($(BUILDDEBUG), TRUE)
	PLATFORM_LIBS += -mconsole
else
	PLATFORM_LIBS += -mwindows
endif


$(RES): $(RES_SOURCE)
	windres -i $(RES_SOURCE) --input-format=rc -o $(RES) -O coff  -F $(RES_OUTPUT)


#used for builderheader
MINI_PLATFORM_LIBS = -L"$(SUPPORTPATH)/lib" -lwinmm -limm32 -lshell32 -lole32 -luuid  -lintl -lglib-2.0.dll -lgmodule-2.0.dll -lgobject-2.0.dll -lgthread-2.0.dll -mconsole -s
MINI_PLATFORM_FLAGS = -I"$(SUPPORTPATH)/include" -I"$(SUPPORTPATH)/include/glib-2.0" -I"$(SUPPORTPATH)/lib/glib-2.0/include" -D_CRT_SECURE_NO_WARNINGS
ifeq ($(PLATFORMBITS), 64)
	MINI_PLATFORM_LIBS += -m64
	MINI_PLATFORM_FLAGS += -m64
else
	MINI_PLATFORM_FLAGS += -m32
	MINI_PLATFORM_LIBS += -m32
endif

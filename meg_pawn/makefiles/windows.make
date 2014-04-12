ifeq ($(SUPPORTPATH), )
	SUPPORTPATH = F:/supportlibs
endif


PLATFORM = __GNUWIN32__
PLATFORM_LIBS = -mwindows -L"$(SUPPORTPATH)/lib" -lwinmm -limm32 -lshell32 -lole32 -luuid  -lintl -lglib-2.0.dll -lgmodule-2.0.dll -lgobject-2.0.dll -lgthread-2.0.dll
PLATFORM_FLAGS = -I"$(SUPPORTPATH)/include" -I"$(SUPPORTPATH)/include/atk-1.0" -I"$(SUPPORTPATH)/include/cairo"  -I"$(SUPPORTPATH)/include/gdk-pixbuf-2.0" -I"$(SUPPORTPATH)/include/glib-2.0"  -I"$(SUPPORTPATH)/include/gtk-2.0"  -I"$(SUPPORTPATH)/include/pango-1.0"  -I"$(SUPPORTPATH)/lib/gtk-2.0/include"  -I"$(SUPPORTPATH)/lib/glib-2.0/include" -I"$(SUPPORTPATH)/include/libsoup-2.4" -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DHAVE_ALLOCA_H=0 -D_CRT_SECURE_NO_WARNINGS

BINEXT = .exe
SOEXT = dll
SOPREFIX = lib

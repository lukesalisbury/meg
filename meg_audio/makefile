# megaudio make file
# Build Options
# BUILDOS - windows, linux, apple
# OPTIMIZE - FULL, SOME, NONE
# BUILDDEBUG - TRUE, FALSE
# CC (Compiler)
# INSTALLDIR (Path to install files to, when running make install)
# BIN (Binary output)

#Default Settings
include setting.mk

BIN = libmegaudio.so

#Read platform and custom settings
include makefiles/$(BUILDOS).make

#Compiler 
COMPILER_LIBS = $(OPTIMIZER) $(DEBUG) $(PLATFORM_LIBS)  -shared
COMPILER_FLAGS = $(OPTIMIZER) $(DEBUG) $(PLATFORM_FLAGS) -D$(PLATFORM)
COMPILER_FLAGSPP = -Wno-write-strings -fno-access-control -fno-exceptions -fno-rtti $(COMPILER_FLAGS)

OBJ = objects/main.o $(PLATFORM_OBJECTS)

include ../build.mk


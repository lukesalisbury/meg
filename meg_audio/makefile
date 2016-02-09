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

ifeq ($(BUILDDEBUG), TRUE)
	OBJDIR =object-debug
endif


BIN = libmegaudio.so

#Read platform and custom settings
include makefiles/$(BUILDOS).make

#Compiler 
COMPILER_LIBS = $(OPTIMIZER) $(DEBUG) $(PLATFORM_LIBS)  -shared
COMPILER_FLAGS = $(OPTIMIZER) $(DEBUG) $(PLATFORM_FLAGS) -D$(PLATFORM)
COMPILER_FLAGSPP = -Wno-write-strings -fno-access-control -fno-exceptions -fno-rtti $(COMPILER_FLAGS)

OBJ = $(OBJDIR)/main.o $(PLATFORM_OBJECTS)

all: all-before $(BIN) $(FINALOUTPUT)
	@echo --------------------------------

all-before:
	@echo --------------------------------
	@echo Building $(BIN) $(PROGRAM_VERSION)
	@echo Debug Build? $(BUILDDEBUG)

$(BIN): $(OBJ)
	@echo --------------------------------
	@$(CPP) $(OBJ) -o $(BUILDDIR)/$(BIN) $(COMPILER_LIBS)

install: $(BIN)
	@echo Installing $< to $(INSTALLDIR)
	@-$(MKDIR) $(dir $@)
	@cp $(BUILDDIR)/$(BIN)  $(INSTALLDIR)/bin/

clean: 
	$(RM) $(OBJ) $(BUILDDIR)/$(BIN)

objects/%.o : src/%.cpp
	@-$(MKDIR) $(dir $@)
	@$(CPP) $(COMPILER_FLAGSPP) -o $@ -c $<

$(OBJDIR)/%.o : src/%.c
	@-$(MKDIR) $(dir $@)
	@$(CC) -c $(COMPILER_FLAGS) -o $@ $<


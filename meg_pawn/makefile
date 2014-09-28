# pawn comiler make file
# Build Options
# BUILDOS - windows, linux, apple
# OPTIMIZE - FULL, SOME, NONE
# BUILDDEBUG - TRUE, FALSE
# PLATFORMBITS 64, 32
# CC (Compiler)
# BIN (Binary output)


#Default Settings
include setting.mk

include makefiles/$(BUILDOS).make
ifneq ($(CUSTOMSETTINGS), )
	include custom/$(CUSTOMSETTINGS).make
endif

ifeq ($(PAWN), 4)
	SO32 =
	SO64 =
	OBJ32 =
	OBJ64 =
	MAINOBJ = $(OBJDIR)/libpawncc.o $(OBJDIR)/pawn4/keeloq.o $(OBJDIR)/pawn4/lstring.o $(OBJDIR)/pawn4/sc1.o $(OBJDIR)/pawn4/sc2.o $(OBJDIR)/pawn4/sc3.o $(OBJDIR)/pawn4/sc4.o $(OBJDIR)/pawn4/sc5.o $(OBJDIR)/pawn4/sc6.o $(OBJDIR)/pawn4/sc7.o $(OBJDIR)/pawn4/scexpand.o $(OBJDIR)/pawn4/sci18n.o $(OBJDIR)/pawn4/sclist.o $(OBJDIR)/pawn4/scstate.o $(OBJDIR)/pawn4/scvars.o $(OBJDIR)/pawn4/memfile.o $(OBJDIR)/pawn4/scmemfil.o
	MAINOBJ += $(OBJDIR)/mokoipawnc.o
else
	PAWN=3
	ifeq ($(SO32), )
		SO32 = $(SOPREFIX)mokoipawn32.$(SOEXT)
	endif

	ifeq ($(SO64), )
		SO64 = $(SOPREFIX)mokoipawn64.$(SOEXT)
	endif

	OBJ32 = $(OBJDIR)/32/libpawncc.o $(OBJDIR)/32/pawn3/lstring.o $(OBJDIR)/32/pawn3/sc1.o $(OBJDIR)/32/pawn3/sc2.o $(OBJDIR)/32/pawn3/sc3.o $(OBJDIR)/32/pawn3/sc4.o $(OBJDIR)/32/pawn3/sc5.o $(OBJDIR)/32/pawn3/sc6.o $(OBJDIR)/32/pawn3/sc7.o $(OBJDIR)/32/pawn3/scexpand.o $(OBJDIR)/32/pawn3/sci18n.o $(OBJDIR)/32/pawn3/sclist.o $(OBJDIR)/32/pawn3/scstate.o $(OBJDIR)/32/pawn3/scvars.o $(OBJDIR)/32/pawn3/memfile.o $(OBJDIR)/32/pawn3/scmemfil.o
	OBJ64 = $(OBJDIR)/64/libpawncc.o $(OBJDIR)/64/pawn3/lstring.o $(OBJDIR)/64/pawn3/sc1.o $(OBJDIR)/64/pawn3/sc2.o $(OBJDIR)/64/pawn3/sc3.o $(OBJDIR)/64/pawn3/sc4.o $(OBJDIR)/64/pawn3/sc5.o $(OBJDIR)/64/pawn3/sc6.o $(OBJDIR)/64/pawn3/sc7.o $(OBJDIR)/64/pawn3/scexpand.o $(OBJDIR)/64/pawn3/sci18n.o $(OBJDIR)/64/pawn3/sclist.o $(OBJDIR)/64/pawn3/scstate.o $(OBJDIR)/64/pawn3/scvars.o $(OBJDIR)/64/pawn3/memfile.o $(OBJDIR)/64/pawn3/scmemfil.o
	MAINOBJ = $(OBJDIR)/mokoipawnc.o
endif

ifeq ($(BIN), )
	BIN = pawn_compiler$(PAWN)$(BINEXT)
endif

COMPILER_LIBS = $(OPTIMIZER) $(DEBUG) -DHAVE_STDINT_H  $(PLATFORM_LIBS)
COMPILER_LIBS += $(LDFLAGS)

COMPILER_FLAGS = $(OPTIMIZER) $(DEBUG) -I"./" $(PLATFORM_FLAGS) -DNO_MAIN=TRUE -D$(PLATFORM) -DPAWN_LIGHT -DPAWN_NO_CODEPAGE
ifeq ($(PAWN), 4)
	COMPILER_FLAGS += -DPAWN=4
endif

COMPILER_FLAGS += $(CFLAGS)
COMPILER_PAWN_CELL = -DPAWN_CELL_SIZE=64

ifeq ($(PLATFORMBITS), 64)
	COMPILER_FLAGS +=  -m64
	COMPILER_LIBS +=  -m64
	COMPILER_PAWN_CELL = -DPAWN_CELL_SIZE=64
endif

ifeq ($(PLATFORMBITS), 32)
	COMPILER_FLAGS +=  -m32
	COMPILER_LIBS +=  -m32
endif


INSTALL_FILES = $(SO32) $(SO64) $(BIN)

PHONY: all-before all
	@echo --------------------------------

all: all-before $(SO32) $(SO64) $(BIN)

all-before:
	@echo --------------------------------
	@echo Building pawn compiler
	@echo Build Platform: $(BUILDDIR)
#	@echo Build Platform: $(BUILDPLATFORM)
#	@echo Target Platform: $(BUILDOS)/$(PLATFORMBITS)
#	@echo Debug Build? $(BUILDDEBUG)
#	@echo DLL/SO: $(SO64) $(SO32)
#	@echo --------------------------------


clean:
	@echo Clean up pawn compiler
	@${RM} $(OBJ32) $(OBJ64) $(SO32) $(SO64) $(MAINOBJ) $(BIN)

install:
ifeq ($(PAWN), 3)
	@echo Installing $(SO64) to $(INSTALLDIR)
	@cp $(BUILDDIR)/$(SO64) $(INSTALLDIR)/bin/
	@echo Installing $(SO32) to $(INSTALLDIR)
	@cp $(BUILDDIR)/$(SO32) $(INSTALLDIR)/bin/
endif
	@echo Installing $(BIN) to $(INSTALLDIR)
	@cp $(BUILDDIR)/$(BIN) $(INSTALLDIR)/bin/


$(OBJDIR)/32/%.o : src/%.c
	@$(MKDIR) $(dir $@)
	@$(CC) -c $(COMPILER_FLAGS) -DPAWN_CELL_SIZE=32 -o $@ $<

$(OBJDIR)/64/%.o : src/%.c
	@$(MKDIR) $(dir $@)
	@$(CC) -c $(COMPILER_FLAGS) -DPAWN_CELL_SIZE=64 -o $@ $<

$(OBJDIR)/%.o : src/%.c
	@$(MKDIR) $(dir $@)
#	@echo $(COMPILER_FLAGS) $(COMPILER_PAWN_CELL)
	@$(CC) -c $(COMPILER_FLAGS) $(COMPILER_PAWN_CELL) -o $@ $<

$(SO64): $(OBJ64)
	@echo Building $(SO64) $(MESSAGE)
	@$(CC) $(OBJ64) -shared -o $(BUILDDIR)/$(SO64) $(COMPILER_LIBS)

$(SO32): $(OBJ32)
	@echo Building $(SO32) $(MESSAGE)
	@$(CC) $(OBJ32) -shared -o $(BUILDDIR)/$(SO32) $(COMPILER_LIBS)

$(BIN): $(MAINOBJ)
	@echo Building $(BIN) $(MESSAGE)
	@$(CC) $(MAINOBJ) -o $(BUILDDIR)/$(BIN) $(COMPILER_LIBS)


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

#Read platform and custom settings
include makefiles/$(BUILDOS).make
ifneq ($(CUSTOMSETTINGS), )
	include custom/$(CUSTOMSETTINGS).make
endif

MAINOBJ = $(OBJDIR)/libpawncc.o $(OBJDIR)/pawn4/keeloq.o $(OBJDIR)/pawn4/lstring.o $(OBJDIR)/pawn4/sc1.o $(OBJDIR)/pawn4/sc2.o $(OBJDIR)/pawn4/sc3.o
MAINOBJ += $(OBJDIR)/pawn4/sc4.o $(OBJDIR)/pawn4/sc5.o $(OBJDIR)/pawn4/sc6.o $(OBJDIR)/pawn4/sc7.o $(OBJDIR)/pawn4/scexpand.o
MAINOBJ += $(OBJDIR)/pawn4/sci18n.o $(OBJDIR)/pawn4/sclist.o $(OBJDIR)/pawn4/scstate.o $(OBJDIR)/pawn4/scvars.o $(OBJDIR)/pawn4/memfile.o $(OBJDIR)/pawn4/scmemfil.o
MAINOBJ += $(OBJDIR)/mokoipawnc.o

BIN=$(BUILDDIR)/pawn_compiler$(PAWN)$(BINEXT)

COMPILER_LIBS = $(OPTIMIZER) $(DEBUG) -DHAVE_STDINT_H  $(PLATFORM_LIBS)
COMPILER_LIBS += $(LDFLAGS)

COMPILER_FLAGS = $(OPTIMIZER) $(DEBUG) -I"./" $(PLATFORM_FLAGS) -DNO_MAIN=TRUE -D$(PLATFORM) -DPAWN_LIGHT -DPAWN_NO_CODEPAGE -DPAWN=4

COMPILER_FLAGS += $(CFLAGS)
COMPILER_PAWN_CELL = -DPAWN_CELL_SIZE=64

ifeq ($(PLATFORMBITS), 64)
	COMPILER_FLAGS +=  -m64
	COMPILER_LIBS +=  -m64
endif

ifeq ($(PLATFORMBITS), 32)
	COMPILER_FLAGS +=  -m32
	COMPILER_LIBS +=  -m32
endif

RESOURCES_FILES = $(wildcard ../share/mokoi-1.0/include/*.inc)
RESOURCES = $(addprefix $(INSTALLDIR)/, $(RESOURCES_FILES) )

PHONY: all-before all

all: all-before $(BIN)
	@echo --------------------------------

all-before:
	@echo --------------------------------
	@echo Building $(BIN)
	@echo Build Platform: $(BUILDPLATFORM)
	@echo Target Platform: $(BUILDOS)/$(PLATFORMBITS)

clean:
	@echo Clean up pawn compiler
	@${RM} $(MAINOBJ) $(BIN)

$(INSTALLDIR)/../share/%: ../share/%
	@$(MKDIR) $(dir $@)
	cp $< $@
	@echo $@

install-bin: $(BIN)
	cp $(BIN) $(INSTALLDIR)/bin/

install: install-bin
	@echo Installing to $(INSTALLDIR)
	@$(MKDIR) $(INSTALLDIR)/share/mokoi-1.0/include/
	@cp ../share/mokoi-1.0/include/*.* $(INSTALLDIR)/share/mokoi-1.0/include/


$(OBJDIR)/%.o : src/%.c
	@$(MKDIR) $(dir $@)
	@$(CC) -c $(COMPILER_FLAGS) $(COMPILER_PAWN_CELL) -o $@ $<

$(BIN): $(MAINOBJ)
	@echo Building $(BIN) $(MESSAGE)
	@$(CC) $(MAINOBJ) -o $(BIN) $(COMPILER_LIBS)


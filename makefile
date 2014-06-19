# Mokoi Editor (Gtk) make file
# Build Options
# BUILDOS - windows, linux, apple
# OPTIMIZE - FULL, SOME, NONE
# BUILDDEBUG - TRUE, FALSE
# PLATFORMBITS 64, 32
# CC (Compiler)
# BIN (Binary output)
# USEGTKSOURCEVIEW
# USESOUP


#Default Settings
include ./setting.mk

BIN = meg
BUILDHEADER_GUI_TITLE = 'Meg'
BUILDHEADER_GUI_DESCRIPT = 'Mokoi Game Editor'
BUILDHEADER_GUI_ICON = 'mokoi-game-editor'

#Read platform and custom settings
include makefiles/$(BUILDOS).make
ifneq ($(CUSTOMSETTINGS), )
	include custom/$(CUSTOMSETTINGS).make
endif

#Compiler
COMPILER_LIBS += $(OPTIMIZER) $(DEBUG) -L"lib" $(PLATFORM_LIBS) -lz
COMPILER_FLAGS += $(OPTIMIZER) $(DEBUG) -I"include" -I"src" -I"src/physfs" $(PLATFORM_FLAGS) -D$(PLATFORM)
COMPILER_FLAGS += -DPHYSFS_NO_CDROM_SUPPORT=1 -DPHYSFS_SUPPORTS_MOKOIRESOURCE=1 -DPHYSFS_SUPPORTS_ZIP=1 -DPAWN_VERSION=$(PAWN)

COMPILER_FLAGS += $(CFLAGS)
COMPILER_LIBS += $(LDFLAGS)

ifeq ($(PLATFORMBITS), 64)
	COMPILER_FLAGS += -m64
	COMPILER_LIBS +=  -m64
endif
ifeq ($(PLATFORMBITS), 32)
	COMPILER_FLAGS +=  -m32
	COMPILER_LIBS +=  -m32
endif



# Objects
OBJ := $(OBJDIR)/ma_dialog.o $(OBJDIR)/ma_directories.o $(OBJDIR)/ma_editor.o $(OBJDIR)/ma_events.o $(OBJDIR)/ma_help.o
OBJ += $(OBJDIR)/ma_initwin.o $(OBJDIR)/ma_language.o $(OBJDIR)/ma_language_events.o $(OBJDIR)/ma_loader.o $(OBJDIR)/ma_log.o
OBJ += $(OBJDIR)/ma_main.o $(OBJDIR)/ma_map.o $(OBJDIR)/ma_map_edit.o $(OBJDIR)/ma_map_events.o $(OBJDIR)/ma_map_section.o
OBJ += $(OBJDIR)/ma_map_object.o $(OBJDIR)/ma_map_edit_events.o $(OBJDIR)/ma_map_section_events.o $(OBJDIR)/ma_help_parser.o
OBJ += $(OBJDIR)/ma_misc.o $(OBJDIR)/ma_project.o $(OBJDIR)/ma_project_events.o $(OBJDIR)/ma_spritesheets.o
OBJ += $(OBJDIR)/ma_spritesheets_events.o $(OBJDIR)/ma_git.o $(OBJDIR)/ma_textedit.o $(OBJDIR)/ma_textedit_events.o
OBJ += $(OBJDIR)/ma_web.o $(OBJDIR)/main.o $(OBJDIR)/ma_audio.o $(OBJDIR)/ma_entitylist.o $(OBJDIR)/gtk_compat.o
OBJ += $(OBJDIR)/ma_question.o $(OBJDIR)/ma_question_functions.o $(OBJDIR)/ma_map_settings.o
OBJ += $(OBJDIR)/ma_map_settings_events.o $(OBJDIR)/ma_section.o $(OBJDIR)/ma_section_events.o $(OBJDIR)/ma_types.o

OBJ += $(OBJDIR)/widgets/widget_map.o $(OBJDIR)/widgets/map_info.o $(OBJDIR)/widgets/display_object.o

OBJ += $(OBJDIR)/data/updates_handler.o $(OBJDIR)/data/text_editor.o $(OBJDIR)/data/sprite_sheets.o $(OBJDIR)/data/sprite_grouping.o
OBJ += $(OBJDIR)/data/sprite_dialog.o $(OBJDIR)/data/sprite_collision.o $(OBJDIR)/data/sheets.o $(OBJDIR)/data/settings.o
OBJ += $(OBJDIR)/data/setting_functions.o $(OBJDIR)/data/sections.o $(OBJDIR)/data/section_functions.o $(OBJDIR)/data/entity_options_parser.o
OBJ += $(OBJDIR)/data/resources_functions.o $(OBJDIR)/data/project.o $(OBJDIR)/data/plugin_info.o $(OBJDIR)/data/path_functions.o
OBJ += $(OBJDIR)/data/patch_functions.o $(OBJDIR)/data/package_functions.o $(OBJDIR)/data/object_advance.o
OBJ += $(OBJDIR)/data/maps.o $(OBJDIR)/data/map_parser.o $(OBJDIR)/data/map_functions.o $(OBJDIR)/data/managed_entity.o
OBJ += $(OBJDIR)/data/logger_functions.o $(OBJDIR)/data/loader_functions.o  $(OBJDIR)/data/language_functions.o $(OBJDIR)/data/language.o
OBJ += $(OBJDIR)/data/import_functions.o $(OBJDIR)/data/image_handling.o $(OBJDIR)/data/game_compiler.o $(OBJDIR)/data/functionlist_parser.o
OBJ += $(OBJDIR)/data/entity_parser.o $(OBJDIR)/data/entity_list_menu.o $(OBJDIR)/data/entity_functions.o $(OBJDIR)/data/entity_editor.o
OBJ += $(OBJDIR)/data/map_objects.o $(OBJDIR)/data/compression_functions.o $(OBJDIR)/data/mokoi_package.o $(OBJDIR)/data/virtual_sprite.o
OBJ += $(OBJDIR)/data/sheets_parser.o $(OBJDIR)/data/entity_compile.o $(OBJDIR)/data/audio_playback.o $(OBJDIR)/data/audio_functions.o
OBJ += $(OBJDIR)/data/audio_chooser.o $(OBJDIR)/data/animation_dialog.o $(OBJDIR)/data/animation_events.o $(OBJDIR)/data/animation_functions.o
OBJ += $(OBJDIR)/data/progress_dialog.o $(OBJDIR)/data/entity_options.o $(OBJDIR)/data/export_functions.o $(OBJDIR)/data/stored_file_info.o

OBJ += $(OBJDIR)/data/data_types.o $(OBJDIR)/data/virtual_sprite_dialog.o

OBJ += $(OBJDIR)/distribution/distro_android.o $(OBJDIR)/distribution/distro_linux64.o $(OBJDIR)/distribution/distro_osx.o $(OBJDIR)/distribution/distro_raspberrypi.o $(OBJDIR)/distribution/distro_windows.o

OBJ += $(OBJDIR)/ma_physfs.o $(OBJDIR)/physfs/physfs.o $(OBJDIR)/physfs/physfs_byteorder.o $(OBJDIR)/physfs/physfs_unicode.o $(OBJDIR)/physfs/archivers/dir.o $(OBJDIR)/physfs/archivers/mokoiresource.o $(OBJDIR)/physfs/archivers/zip.o $(OBJDIR)/physfs/platform/macosx.o $(OBJDIR)/physfs/platform/posix.o $(OBJDIR)/physfs/platform/unix.o $(OBJDIR)/physfs/platform/windows.o

OBJ += $(RES)


#Build

PHONY: all-before all
	@echo --------------------------------

all: all-before buildheader $(BIN) $(FINALOUTPUT)
	@echo --------------------------------

all-before:
	@echo --------------------------------
	@echo Building Mokoi Editor
#	@echo Build Platform: $(BUILDPLATFORM)
#	@echo Target Platform: $(BUILDOS)/$(PLATFORMBITS)
#	@echo Debug Build? $(BUILDDEBUG)
#	@echo Build Flags? $(COMPILER_FLAGS)
#	@echo --------------------------------

include/ui/%.gui.h: res/ui/%.gui
	@echo Converting GUI $<
	@-$(MKDIR) include/ui/
	@$(OBJDIR)/buildheader.exe $< $@ $(BUILDHEADER_GUI_TITLE) $(BUILDHEADER_GUI_DESCRIPT) $(BUILDHEADER_GUI_ICON)

buildheader.exe:
#	@echo Building builderheader
	@-$(MKDIR) $(OBJDIR)
	@$(CC) $(MINI_PLATFORM_FLAGS) -o $(OBJDIR)/buildheader.o -c buildheader.c
	@$(CC) $(OBJDIR)/buildheader.o -o $(OBJDIR)/buildheader.exe  $(MINI_PLATFORM_LIBS)

buildheader: buildheader.exe $(patsubst res/ui/%,include/ui/%.h,$(wildcard res/ui/*.gui))
	@echo buildheader done.

clean:
	@echo Clean up Mokoi Editor
	${RM} $(OBJ) $(BIN) $(OBJDIR)/buildheader.exe $(wildcard include/ui/*.gui.h)
	@$(MAKE) -C meg_audio clean BUILDDIR=$(CURDIR)/$(BUILDDIR)
	@$(MAKE) -C meg_pawn clean BUILDDIR=$(CURDIR)/$(BUILDDIR)

$(OBJDIR)/%.o : src/%.c
	@echo Compiling $@ $(MESSAGE)
	@-$(MKDIR) $(dir $@)
	@$(CC) -c $(COMPILER_FLAGS) -o $@ $<

$(BIN): $(OBJ)
	@echo Building $(BIN) $(MESSAGE)
	@-$(MKDIR) $(BUILDDIR)
	@$(CC) $(OBJ) -o $(BUILDDIR)/$(BIN) $(COMPILER_LIBS)
ifneq ($(SKIPMODULES), TRUE)
	@$(MAKE) -C meg_audio all BUILDDIR=$(CURDIR)/$(BUILDDIR)
	@$(MAKE) -C meg_pawn all BUILDDIR=$(CURDIR)/$(BUILDDIR)
endif

install: $(BIN)
	@echo Installing $< to $(INSTALLDIR)
	@-$(MKDIR) $(INSTALLDIR)
	@-$(MKDIR) $(INSTALLDIR)/bin/
	@cp $(BUILDDIR)/$(BIN) $(INSTALLDIR)/bin/
	@cp -r ./share/ $(INSTALLDIR)/
	@$(MAKE) -C meg_audio install BUILDDIR=$(CURDIR)/$(BUILDDIR)
	@$(MAKE) -C meg_pawn install BUILDDIR=$(CURDIR)/$(BUILDDIR)

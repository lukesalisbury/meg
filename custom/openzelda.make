BIN = quest_designer$(BINEXT)

ifeq ($(BUILDOS),windows)
	RES_SOURCE = custom/openzelda.rc
endif


PLATFORM_FLAGS += -DPROGRAM_TITLE="\"Quest Designer\""
PLATFORM_FLAGS += -DPROGRAM_DESCRIPTION="\"Open Zelda Quest Editor\""
PLATFORM_FLAGS += -DPROGRAM_ICON="\"openzelda-quest-designer\""
PLATFORM_FLAGS += -DPROGRAM_DOCUMENTS="\"openzelda-quests\""
PLATFORM_FLAGS += -DMEGCONFIGDIRECTORY="\"quest-designer\""
PLATFORM_FLAGS += -DMEGSHAREDIRECTORY="\"openzelda-2.0\""
PLATFORM_FLAGS += -DROOT_FILENAME="\"edit.zelda\""
PLATFORM_FLAGS += -DROOT_FILENAME_EXT="\"zelda\""
PLATFORM_FLAGS += -DROOT_MIMETYPE="\"application/x-openzelda-quest-source\""
PLATFORM_FLAGS += -DICON_MIMETYPE="\"application-x-openzelda-quest-source\""
PLATFORM_FLAGS += -DGAME_EXTENSION="\"quest\""
PLATFORM_FLAGS += -DENGINE_FILENAME="\"openzelda$(BINEXT)\""
PLATFORM_FLAGS += -DPACKAGE_URL="\"http://openzelda.net/userpackages/\""

PLATFORM_FLAGS += -DQ2A_URL="\"http://openzelda.net/qa/xml-rpc\""

PLATFORM_FLAGS += -DCUSTOMSETTINGS

PLATFORM_FLAGS += -DBUG_URL="\"http://openzelda.net/\#!Bugs\""
PLATFORM_FLAGS += -DFORUM_URL="\"http://openzelda.net/\#!Message\""
PLATFORM_FLAGS += -DMANUAL_URL="\"http://users.openzelda.net/guide\""
PLATFORM_FLAGS += -DFORCE_PACKAGE="\"OpenZelda-2_0-Alpha1.package\""
PLATFORM_FLAGS += -DFORCE_PACKAGE_URL="\"http://openzelda.net/get/?OpenZelda-2_0-Alpha1.package\""
PLATFORM_FLAGS += -DPROGRAMSHELPDIRECTORY="\"QuestDesigner\""


PLATFORM_FLAGS += -DPROGRAM_TITLE="\"Quest Designer\""
PLATFORM_FLAGS += -DPROGRAM_DESCRIPTION="\"Open Zelda Quest Editor\""

BUILDHEADER_GUI_TITLE = 'Quest Designer'
BUILDHEADER_GUI_DESCRIPT = 'Open Zelda Quest Editor'
BUILDHEADER_GUI_ICON = 'openzelda-quest-designer'

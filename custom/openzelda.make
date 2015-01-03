BIN = quest_designer$(BINEXT)
CUSTOMPATH = ../custom/questdesigner$

ifeq ($(BUILDOS),windows)
	RES_SOURCE = $(CUSTOMPATH)/openzelda.rc
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
PLATFORM_FLAGS += -DPACKAGE_URL="\"http://openzelda.net/userpackages/1.9.0\""

PLATFORM_FLAGS += -DPROGRAMSHELPDIRECTORY="\"QuestDesigner\""

#PLATFORM_FLAGS += -DQ2A_URL="\"http://openzelda.net/qa/xml-rpc\""

PLATFORM_FLAGS += -DCUSTOMSETTINGS
PLATFORM_FLAGS += -I$(CUSTOMPATH)


PLATFORM_FLAGS += -DBUG_URL="\"http://openzelda.net/\#!Bugs\""
PLATFORM_FLAGS += -DFORUM_URL="\"http://openzelda.net/\#!Message\""
PLATFORM_FLAGS += -DMANUAL_URL="\"http://openzelda.github.io/openzelda-docs/\""
PLATFORM_FLAGS += -DFORCE_PACKAGE="\"OpenZelda-2_0-Beta1.package\""
PLATFORM_FLAGS += -DFORCE_PACKAGE_URL="\"http://openzelda.net/get/OpenZelda-2_0-beta1.package\""
PLATFORM_FLAGS += -DFORCE_PACKAGE_CHECKSUM_URL="\"http://openzelda.net/package/2_0-beta1.txt\""
PLATFORM_FLAGS += -DPROGRAM_UPDATE_URL="\"http://openzelda.net/version/1_9_0.txt\""

PLATFORM_FLAGS += -DPROGRAM_TITLE="\"Quest Designer\""
PLATFORM_FLAGS += -DPROGRAM_DESCRIPTION="\"Open Zelda Quest Editor\""

BUILDHEADER_GUI_TITLE = 'Quest Designer'
BUILDHEADER_GUI_DESCRIPT = 'Open Zelda Quest Editor'
BUILDHEADER_GUI_ICON = 'openzelda-quest-designer'

/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Global Header */
#include "global.h"

/* Required Headers */
#include "ma_map.h"
#include "ma_map_edit.h"
#include "ma_map_section.h"

/* GtkWidget */
GtkWidget * widget_map_notebook = NULL;
GtkWidget * widget_object_menu = NULL;

/* Global Variables */
GtkListStore * mapFileStore = NULL;
GtkWidget * mapNotebook = NULL;
MapSelection mapDefaultSelection = {FALSE, FALSE, 0.0, 10.0, 20.0, 30.0, 0.0, 0.0, 0, 0, 0, 0, NULL};

/* External Functions */
gint Alchera_Map_AnimationWatch(GtkWidget * view);
void Meg_Help_Load( const gchar * file, GtkWidget * textview );

/* UI */
#include "ui/map_page.gui.h"
const gchar * alchera_map_ui_main = GUIMAP_PAGE


/********************************
* Alchera_Map_Init
* Initialises Map + Overview Page
*/
void MegWidget_Map_Create()
{
	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(alchera_map_ui_main, __func__, __LINE__);
	g_return_if_fail( ui );

	/* Widget */
	GtkWidget * widget, * treeview_map, * help_text;

	mapNotebook = GET_WIDGET( ui, "map_notebook" );
	widget = GET_WIDGET( ui, "map_widget" );
	help_text = GET_WIDGET( ui, "help_text" );
	treeview_map = GET_WIDGET( ui, "treeview_map" );

	mapFileStore = GET_LISTSTORE( ui, "store_map" );

	g_object_ref(widget);

	/* Tab Setting */
	g_object_set_data( G_OBJECT(widget), "meg-help-page", g_strdup(PROGRAMSHELPDIRECTORY"/MapPage.xml") ); //Set the help page
	Meg_Main_AddSection(widget, "Maps", PAGE_ICON_MAP);

	/* Signals */
	g_signal_connect( G_OBJECT(treeview_map), "row-activated", G_CALLBACK(Meg_MapEdit_OpenFile), NULL);
	g_signal_connect( G_OBJECT(treeview_map), "popup-menu", G_CALLBACK(Meg_MapEdit_ListMenu), NULL);

	SET_OBJECT_SIGNAL(ui, "actionAddMap", "activate", G_CALLBACK(Meg_MapEdit_AddFile), treeview_map );
	SET_OBJECT_SIGNAL(ui, "actionRemoveMap", "activate", G_CALLBACK(Meg_MapEdit_RemoveFile), treeview_map );

	/* */
	Meg_Help_Load(PROGRAMSHELPDIRECTORY"/MapPage.xml", help_text);

}


/********************************
* MegWidget_Map_Refresh
*
*/
void MegWidget_Map_Refresh()
{
	AL_Map_Files(mapFileStore);

}

/********************************
* MegWidget_Map_Refresh
*
*/
void MegWidget_Map_Init()
{
	MegWidget_Map_Refresh();
}

/********************************
* MegWidget_Map_Refresh
*
*/
void MegWidget_Map_Close()
{
	gtk_list_store_clear( mapFileStore );
}

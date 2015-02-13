/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/


/* Standard Headers */
#include "global.h"
#include "alchera_functions.h"

/* Required Headers */
#include "data/entity_functions.h"
#include "data/entity_editor.h"
#include "ma_entitylist.h"

/* Local Type */

/* External Functions */
void MegWidget_EntityList_Refresh( GtkWidget * widget, gpointer user_data);

/* Global Variables */
extern GError * mokoiError;
extern gboolean mokoiCompileError;

/* Local Variables */
GtkWidget * mokoiEntityMenu = NULL;
GtkWidget * mokoiEntityMainLabel = NULL;
GtkWidget * mokoiEntityTreeview = NULL;

/* UI */
const gchar * mokoiUI_EntityMenu = GUI_ENTITY_MENU;
const gchar * mokoiUI_Entity = GUI_PAGE_ENTITY;

/********************************
* MegWidget_EntityList_Create
*
*/
void MegWidget_EntityList_Create()
{
	GtkWidget * widget;

	/* UI */
	GError * error = NULL;
	GtkBuilder * ui = gtk_builder_new();
	if ( !gtk_builder_add_from_string(ui, mokoiUI_Entity, -1, &error) )
	{
		Meg_Error_Print( __func__, __LINE__, "UI creation error '%s'.", error->message );
		return;
	}
	if ( !gtk_builder_add_from_string( ui, mokoiUI_EntityMenu, -1, &error ) )
	{
		Meg_Error_Print( __func__, __LINE__, "UI creation error: %s", error->message );
		return;
	}

	/* Widgets */
	widget = GET_WIDGET( ui, "meg_entitylist_widget" );
	mokoiEntityTreeview = GET_WIDGET( ui, "meg_entitylist_treeview" );
	mokoiEntityMenu = GET_WIDGET( ui, "object_menu");
	mokoiEntityMainLabel = GET_WIDGET( ui, "main_entity_label");

	/* Signals */
	g_signal_connect( widget, "realize", G_CALLBACK(MegWidget_EntityList_Refresh), NULL );

	SET_OBJECT_SIGNAL( ui, "menu_open", "activate", G_CALLBACK(EntityListMenu_OpenAction), (gpointer) mokoiEntityTreeview);
	SET_OBJECT_SIGNAL( ui, "menu_compile", "activate", G_CALLBACK(EntityListMenu_CompileAction), (gpointer) mokoiEntityTreeview);
	SET_OBJECT_SIGNAL( ui, "menu_settings", "activate", G_CALLBACK(EntityListMenu_SettingsAction), (gpointer) mokoiEntityTreeview);
	SET_OBJECT_SIGNAL( ui, "menu_remove", "activate", G_CALLBACK(EntityListMenu_RemoveAction), (gpointer) mokoiEntityTreeview);

	SET_OBJECT_SIGNAL( ui, "button_new", "clicked", G_CALLBACK(Meg_EntityList_NewDialog), (gpointer) mokoiEntityTreeview);
	SET_OBJECT_SIGNAL( ui, "button_recompile", "clicked", G_CALLBACK(Meg_EntityList_RebuildAll), (gpointer) mokoiEntityTreeview);

	SET_OBJECT_SIGNAL( ui, "button_main_open", "clicked", G_CALLBACK(EntityEditor_NewMain), NULL);

	g_signal_connect( mokoiEntityTreeview, "button_press_event", G_CALLBACK(Meg_EntityList_MenuClick), mokoiEntityMenu );
	g_signal_connect( mokoiEntityTreeview, "row_activated", G_CALLBACK(Meg_EntityList_Selection), NULL );
	g_signal_connect( mokoiEntityTreeview, "popup_menu", G_CALLBACK(Meg_EntityList_Menu), mokoiEntityMenu );

	/* Help support */
	GtkWidget * help_text = GET_WIDGET( ui, "meg_entitylist_help" );
	Meg_Help_Load( PROGRAMSHELPDIRECTORY"/Entities.xml", help_text );
	g_object_set_data( G_OBJECT(widget), "meg-help-page", g_strdup(PROGRAMSHELPDIRECTORY"/Entities.xml") ); //Set the help page

	Meg_Main_AddSection( widget, "Scripts", PAGE_ICON_SCRIPTS );

}

/********************************
* MegWidget_EntityList_Refresh
*
*/
void MegWidget_EntityList_Refresh( GtkWidget * widget, gpointer user_data)
{
	EntityList_UpdatePage( gtk_tree_view_get_model(GTK_TREE_VIEW(mokoiEntityTreeview)) );
	gtk_tree_view_expand_all( GTK_TREE_VIEW(mokoiEntityTreeview) );
}


/********************************
* MegWidget_EntityList_Init
*
*/
void MegWidget_EntityList_Init()
{
	/* MegWidget_EntityList_Refresh(NULL, NULL); */
}

/********************************
* MegWidget_EntityList_Close
*
*/
void MegWidget_EntityList_Close()
{
	GtkTreeModel * model = gtk_tree_view_get_model( GTK_TREE_VIEW(mokoiEntityTreeview) );
	gtk_tree_store_clear( GTK_TREE_STORE(model) );
}

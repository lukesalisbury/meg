/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Global Header */
#include "global.h"
#include <gdk/gdk.h>
#if (GTK_MAJOR_VERSION == 2)
#include "gdkkeysyms-backward.h"
#endif

/* Required Headers */
#include "ma_section.h"
#include "ma_map_edit.h"
#include "ma_map_section.h"

/* Global Variables */
extern GtkListStore * sectionFileStore;

/* Local Variables */


/* Functions */



/********************************
* Meg_MapSection_AddFile
* Event: Add Map Section
* Result:
*/
void Meg_MapSection_AddFile( GtkAction * action, GtkWidget * treeview )
{
	GtkWidget * dialog, * frame, * vbox, * entry;

	dialog = gtk_dialog_new_with_buttons("New Section", Meg_Main_GetWindow(), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

	frame = gtk_frame_new( "Section Name" );
	entry = gtk_entry_new();

	vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

	gtk_frame_set_shadow_type( GTK_FRAME(frame), GTK_SHADOW_NONE );
	gtk_box_pack_start( GTK_BOX(vbox), frame, TRUE, TRUE, 0 );
	gtk_container_add( GTK_CONTAINER(frame), entry );

	gtk_widget_show_all(vbox);

	gint result = gtk_dialog_run( GTK_DIALOG(dialog) );
	const gchar * newfilename = gtk_entry_get_text( GTK_ENTRY(entry) );
	if ( !g_utf8_strlen(newfilename, -1) )
	{
		result = GTK_RESPONSE_REJECT;
	}

	if ( result == GTK_RESPONSE_ACCEPT )
	{
		AL_MapSections_Add( g_strdup(newfilename) );
		GtkTreeModel * store = gtk_tree_view_get_model( GTK_TREE_VIEW(treeview) );
		AL_MapSections_Files( GTK_LIST_STORE(store) );
	}
	gtk_widget_destroy( dialog );
}

/********************************
* Meg_MapSection_RemoveFile
* Event: Map Listing Remove Button
* Result: Removes the current selected map.
*/
void Meg_MapSection_RemoveFile( GtkAction * action, GtkWidget * treeview )
{
	GtkTreeIter iter;
	GtkTreeModel * model;
	GtkTreeSelection * selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(treeview) );

	if ( gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gchar * filename = NULL;
		gtk_tree_model_get( model, &iter, 0, &filename, -1 );
		if ( filename )
		{
			GtkWidget * question = gtk_message_dialog_new( Meg_Misc_ParentWindow( treeview ), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Do you want to remove %s", filename);
			gint answer = gtk_dialog_run( GTK_DIALOG(question) );
			gtk_widget_destroy( question );
			if ( answer == GTK_RESPONSE_YES )
			{
				AL_MapSections_Remove(filename);
				GtkTreeModel * store = gtk_tree_view_get_model( GTK_TREE_VIEW(treeview) );
				AL_MapSections_Files( GTK_LIST_STORE(store) );
			}
		}
		g_free( filename );
	}
}

/********************************
* Meg_MapSection_OpenFile
* Event: Double Clicking on the Section List
* Result: Open the current selected Section.
*/
void Meg_MapSection_OpenFile(GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn *column, gpointer user_data  )
{
	GtkWidget * window = NULL;
	GtkTreeModel * model;
	GtkTreeIter iter;

	model = gtk_tree_view_get_model( tree_view );
	if ( gtk_tree_model_get_iter( model, &iter, path ) )
	{
		gchar * filename;
		gtk_tree_model_get( model, &iter, 0, &filename, 3, &window, -1 );
		if ( window )
		{
			/*gtk_window_present( GTK_WINDOW(window) );*/
			gint page = gtk_notebook_page_num( GTK_NOTEBOOK(sectionNotebook), window);
			if ( page != -1 )
			{
				gtk_notebook_set_current_page( GTK_NOTEBOOK(sectionNotebook), page );
			}
		}
		else
		{
			window = Meg_MapSection_Open( filename );
			gtk_list_store_set( GTK_LIST_STORE(model), &iter, 1, "<i>opened</i>", 3, window, -1);
			g_object_set_data( G_OBJECT(window), "alchera-list-path", gtk_tree_model_get_string_from_iter( model, &iter) );
		}
		g_free( filename );
	}
}


/********************************
* Meg_MapSection_ListMenu
* Event:
* Result:
*/
void Meg_MapSection_ListMenu( GtkTreeView * treeview, gpointer user_data )
{
	GtkTreeIter iter;
	GtkTreeModel * model;
	GtkTreeSelection * selection = gtk_tree_view_get_selection( treeview );

	if ( gtk_tree_selection_get_selected(selection, &model, &iter) )
	{
		GtkWidget * menu = gtk_menu_new();
		gtk_menu_popup( GTK_MENU(menu), NULL, NULL, NULL, NULL, 1, gtk_get_current_event_time());
		/*
		if ( AL_Map_Files_popup )
			AL_Map_Files_popup( &iter, 0 );
		*/
	}
}

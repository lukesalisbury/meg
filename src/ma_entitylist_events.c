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

#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>
#include <sys/stat.h>
#include <string.h>

/* Local Type */

/* External Functions */
gboolean EntitySettings_Editor_Open( const gchar * entity_name );


/* Global Variables */

/* Local Variables */


/* UI */


/**
 * @brief Meg_EntityList_Properties
 * @param file_path
 * @return
 */
gboolean Meg_EntityList_Settings( const gchar * file_path )
{
	gboolean successful;
	gchar * file_name = NULL;
	gchar * entity_name = NULL;

	file_name = g_path_get_basename( file_path );
	entity_name = STRIP_FILE_EXTENSION( file_name, 4 );

	successful = EntitySettings_Editor_Open( file_path );

	g_free( file_name );
	g_free( entity_name );

	return successful;
}

/**
 * @brief Meg_EntityList_NewDialog
 * @param button
 * @param tree
 */
void Meg_EntityList_NewDialog( GtkButton * button, GtkTreeView * tree )
{
	if ( Entity_Add() )
	{
		GtkTreeModel * model = gtk_tree_view_get_model( tree );
		EntityList_UpdatePage( model );
	}
}

/**
 * @brief Right click on Entity List
 * @param treeview
 * @param entityMenu
 */
void Meg_EntityList_Menu(GtkWidget * treeview, GtkWidget * entityMenu)
{
	gtk_widget_show_all( entityMenu );
	gtk_menu_popup( GTK_MENU(entityMenu), NULL, treeview, NULL, NULL, 3, gtk_get_current_event_time() );
}

/**
 * @brief Event
 * @param treeview
 * @param event
 * @param data
 * @return
 */
gboolean Meg_EntityList_MenuClick( GtkWidget * treeview, GdkEventButton *event, gpointer data)
{
	GtkTreeSelection * selection = NULL;
	if ( event->type == GDK_BUTTON_PRESS && event->button == GDK_BUTTON_SECONDARY )
	{
		selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(treeview) );
		if ( gtk_tree_selection_count_selected_rows(selection) <= 1 )
		{
			GtkTreePath *path;
			if ( gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), (gint)event->x, (gint)event->y, &path, NULL, NULL, NULL) )
			{
				gtk_tree_selection_unselect_all(selection);
				gtk_tree_selection_select_path(selection, path);
				gtk_tree_path_free(path);
			}
		}
		Meg_EntityList_Menu( treeview, data );
		return TRUE;
	}

	return FALSE;
}

/**
 * @brief Meg_EntityList_RebuildAll
 * @param button
 * @param list
 */
void Meg_EntityList_RebuildAll( GtkButton * button, GtkTreeStore * list )
{
	Entity_RebuildAll();
}

/**
 * @brief Meg_EntityList_Selection
 * @param tree_view
 * @param path
 * @param column
 * @param user_data
 */
void Meg_EntityList_Selection( GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data )
{
	GtkWidget * window = NULL;
	GtkTreeIter iter;
	GtkTreeModel * model;
	GtkTreeSelection * selection = gtk_tree_view_get_selection( tree_view );
	gchar * file = NULL;
	gchar * file_path = NULL;

	if ( gtk_tree_selection_get_selected( selection, &model, &iter) )
	{
		gtk_tree_model_get( model, &iter, 0, &file, 4, &window, -1 );
		if ( !file )
		{
			return;
		}
		if ( file[0] == '<' )
		{
			return;
		}
		if ( window )
		{
			gtk_window_present( GTK_WINDOW(window) );
		}
		else
		{
			file_path = g_strconcat( "/scripts/", file, NULL );
			window = EntityEditor_New( file_path );

			gtk_tree_store_set( GTK_TREE_STORE(model), &iter, 5, "<i>opened</i>", 4, window, -1);
			g_object_set_data( G_OBJECT(window), "alchera-list-path", gtk_tree_model_get_string_from_iter( model, &iter) );
		}
		g_free( file_path );
	}
}



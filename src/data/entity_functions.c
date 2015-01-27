/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Standard Headers */
#include "loader_global.h"

/* Required Headers */
#include "loader_functions.h"
#include "entity_functions.h"
#include "game_compiler.h"
#include "logger_functions.h"
#include "entity_options_parser.h"

#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>
#include <sys/stat.h>
#include <string.h>

/* Local Type */

/* External Functions */
gboolean EntityOptionEditor_Open( gchar * entity_name );
gboolean ManagedEntity_New(gchar * filename, gchar * image_text);

time_t physfs_file_get_date( gchar * file );
guint physfs_file_get_size( gchar * file );

/* Global Variables */
extern gboolean mokoiCompileError;
extern GtkWidget * mokoiEntityMenu;
extern GtkWidget * mokoiEntityMainLabel;

/* Local Variables */
gchar * mokoiScriptLoaded = NULL;
gchar * mokoiScriptBlankEntity = "/* Init function is the call before anything else */\npublic Init(...)\n{\n\t\n}\n\n/* Close function when it is deleted' */\npublic Close()\n{\n\t\n}\n\nmain()\n{\n\t\n}\n";
GtkWidget * mokoiEntityEditor = NULL;

/* UI */

const gchar * mokoiUI_EntityAdd = GUI_ENTITY_ADD;


/********************************
* Entity_New
* Create new entity
@ filename:
@ script:
*/
gboolean Entity_New( gchar * filename, gchar * script )
{
	/*
	gchar * runtime_file = g_strconcat( filename, ".options", NULL );
	Meg_file_set_contents( runtime_file, "<entity/>", -1, NULL );
	g_free( runtime_file );
	*/

	return Meg_file_set_contents( filename, script, -1, NULL);
}

/********************************
* Entity_Add (Exported Function)
*
* Alchera: Entity Editor
*/
gboolean Entity_Add()
{
	GtkWidget * dialog, * entry_filename, * radio_managed, * file_base_entity;
	gchar * file_name = NULL, * entity_base = NULL;
	const gchar * entity_name;
	gboolean return_value = FALSE;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(mokoiUI_EntityAdd, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );

	/* Widget */
	dialog = GET_WIDGET( ui, "dialog");
	entry_filename = GET_WIDGET( ui, "entry_filename");
	file_base_entity = GET_WIDGET( ui, "file_base_entity");
	radio_managed = GET_WIDGET( ui, "radio_managed");


	/* Setting */
	GtkFileFilter * entity_filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern( entity_filter, "*.mps" );
	gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(file_base_entity), entity_filter);

	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ) );
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Main_GetWindow() );

	/* Run Dialog */
	if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
	{
		entity_name = gtk_entry_get_text( GTK_ENTRY(entry_filename) );
		entity_base = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(file_base_entity) );
		if ( entity_name )
		{
			if ( g_utf8_strlen( entity_name, -1 ) )
			{
				file_name = g_strdup_printf( "/scripts/%s.mps", entity_name );
				if ( !Meg_file_test( file_name, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR) ) )
				{
					if ( entity_base )
					{
						Meg_Log_Print( LOG_NONE, "Copying Entity '%s' to '%s'", entity_base, file_name );
						return_value = Meg_FileCopy( entity_base, file_name );
					}
					else if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(radio_managed) ) )
					{
						Meg_Log_Print( LOG_NONE, "Creating New Entity '%s'", file_name );
						return_value = ManagedEntity_New( file_name, NULL );
					}
					else
					{
						Meg_Log_Print( LOG_NONE, "Creating New Entity '%s'", file_name );
						return_value = Entity_New( file_name, mokoiScriptBlankEntity );
					}
				}
				g_free( file_name );
			}
		}

		if ( !return_value )
		{
			GtkWidget * message = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Error Creating Entity" );
			gtk_dialog_run( GTK_DIALOG(message) );
			gtk_widget_destroy( message );
		}
	}

	gtk_widget_destroy( dialog );

	return return_value;
}


/********************************
* Entity_Remove
*
@ file: file to remove.
*/
gboolean Entity_Remove( gchar * filename )
{
	GtkWidget * message = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "Scripts can't be removed at this time." );
	gtk_dialog_run( GTK_DIALOG(message) );
	gtk_widget_destroy( message );
	return FALSE;
}

/********************************
* Entity_Properties
*
@ filename:
*/
gboolean Entity_Properties( const gchar * file_path )
{
	gboolean successful;
	gchar * file_name = NULL;
	gchar * entity_name = NULL;

	file_name = g_path_get_basename( file_path );
	entity_name = STRIP_FILE_EXTENSION( file_name, 4 );

	successful = EntityOptionEditor_Open( entity_name );


	g_free( file_name );
	g_free( entity_name );

	return successful;
}


/********************************
* EntityCombo_Properties_Show
*
@ button:
@ combo:
*/
gboolean EntityCombo_Properties_Show( GtkButton * button, GtkComboBox * combo )
{
	g_return_val_if_fail( button != NULL, FALSE );
	g_return_val_if_fail( combo != NULL, FALSE );

	gchar * filename = Meg_ComboText_GetText(combo);
	gchar * file_path = NULL;


	if ( filename )
	{
		file_path = g_strdup_printf( "/scripts/%s", filename );
		return Entity_Properties( file_path );
	}
	return FALSE;
}

/********************************
* EntityCombo_Properties_Open
*
@ button:
@ event:
@ combo:
*/
gboolean EntityCombo_Properties_Open( GtkWidget * button, GdkEventButton * event, GtkComboBox * combo )
{
	if ( event->type == GDK_BUTTON_PRESS && event->button == 1 )
	{
		EntityCombo_Properties_Show( GTK_BUTTON(button), combo);
	}
	return FALSE;

}

/********************************
* Entity_NewDialog
*
@ file: file to remove.
*/
void Entity_NewDialog( GtkButton * button, GtkTreeView * tree )
{
	if ( Entity_Add() )
	{
		GtkTreeModel * model = gtk_tree_view_get_model( tree );
		EntityList_UpdatePage( model );
	}
}


/********************************
* Entity_UpdateMain
* Updates Label for Main Entity
*/
void EntityList_UpdateMain()
{
	time_t source = physfs_file_get_date( "/scripts/main.mps" );
	time_t script = physfs_file_get_date( "/c/scripts/main.amx" );
	gboolean latest = ( script >= source );

	gchar * markup = g_markup_printf_escaped( "<span color=\"%s\">%s</span>", (latest ? "green" : "red"), (latest ? "compiled" : "outdated") );
	gtk_label_set_markup( GTK_LABEL(mokoiEntityMainLabel), markup );
	g_free( markup );

}


/********************************
* EntityList_Menu
* Event: Right click on Entity List
*
*/
void EntityList_Menu(GtkWidget * treeview, gpointer data)
{
	gtk_widget_show_all( mokoiEntityMenu );
	gtk_menu_popup( GTK_MENU(mokoiEntityMenu), NULL, treeview, NULL, NULL, 3, gtk_get_current_event_time() );
}


/********************************
* Event:
* Result:
*/
gboolean EntityList_MenuClick( GtkWidget *treeview, GdkEventButton *event, gpointer data)
{
	GtkTreeSelection *selection;
	if ( event->type == GDK_BUTTON_PRESS && event->button == 3 )
	{
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
		if ( gtk_tree_selection_count_selected_rows(selection) <= 1 )
		{
			GtkTreePath *path;
			if ( gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), (gint) event->x, (gint) event->y, &path, NULL, NULL, NULL) )
			{
				gtk_tree_selection_unselect_all(selection);
				gtk_tree_selection_select_path(selection, path);
				gtk_tree_path_free(path);
			}
		}
		EntityList_Menu(treeview, NULL);
		return TRUE;
	}

	return FALSE;
}


/********************************
* EntityList_Selection
*
@
@
@
*/
void EntityList_Selection( GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data )
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
		if (!file)
		{
			return;
		}
		if (file[0] == '<')
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

/**
 * @brief EntityList_GetIter
 * @param file
 * @param list [ 0=>file, 1=>time, 2=>desc, 3=>compiled time, 4=>window, 5=?wintext ]
 * @param parent
 * @return
 */
GtkTreeIter  EntityList_GetIter( const gchar * file, GtkTreeStore * list, GtkTreeIter * parent )
{
	GtkTreeIter child, search;
	gchar * sfile;
	GtkTreeModel * model = GTK_TREE_MODEL(list);

	if ( gtk_tree_model_iter_children( model, &search, parent ) )
	{
		do
		{
			gtk_tree_model_get( model, &search, 0, &sfile, -1);
			if ( !g_strcmp0(sfile, file) )
			{
				return search;
			}
		} while ( gtk_tree_model_iter_next( model, &search ) );
	}

	gtk_tree_store_append( list, &child, parent );
	gtk_tree_store_set( list, &child, 4, NULL, 5, NULL, -1 );

	return child;
}


/********************************
* EntityList_ScanDirectory
*
@ path:
@ ext:
@ list: [ 0=>file, 1=>time, 2=>desc, 3=>compiled time, 4=>window, 5=?wintext ]
*/
void EntityList_ScanDirectory( gchar * path, gchar * ext, gboolean routines, GtkTreeStore * list, GtkTreeIter * parent )
{
	GtkTreeIter children;
	gchar * full_path = g_strconcat( "/scripts/", path, NULL );

	/* Scan Directory */
	char ** directory_listing = PHYSFS_enumerateFiles(full_path);
	char ** current_file;

	for (current_file = directory_listing; *current_file != NULL; current_file++)
	{
		if ( g_str_has_suffix( *current_file, ext ) )
		{
			gchar * name = g_strndup( *current_file, strlen(*current_file) - 3 );
			gchar * file = NULL;
			if ( path )
				file = g_strconcat( path, *current_file, NULL );
			else
				file = g_strdup(*current_file);

			if ( routines )
			{
				children = EntityList_GetIter( file, list, parent );
				gtk_tree_store_set( list, &children, 0, g_strdup( file ), 1, 0, 2, "Routine Source", 3, 0, -1 );
			}
			else
			{
				gchar * sfile = g_strconcat( full_path, name, "mps", NULL );
				gchar * cfile = g_strconcat( "c/", full_path, name, "amx", NULL );

				time_t source = physfs_file_get_date( sfile );
				time_t script = physfs_file_get_date( cfile );

				children = EntityList_GetIter( file, list, parent );
				gtk_tree_store_set( list, &children, 0, g_strdup( file ), 1, 0, 2, (source > script ? "Need compiling" : ""), 3, script, -1 );

				g_free( sfile );
				g_free( cfile );
			}
			g_free( name );
			g_free( file );
		}
	}
	PHYSFS_freeList(directory_listing);
	g_free( full_path );
}

/********************************
* EntityList_UpdatePage
*
@ list: [ 0=>file, 1=>time, 2=>desc, 3=>compiled time, 4=>window, 5=?wintext ]
*/
void EntityList_UpdatePage( GtkTreeModel * list )
{
	if ( !Project_ValidDir() || !list )
		return;

	gint c = 0;
	GtkTreeIter parent;
	if ( gtk_tree_model_get_iter_first( list, &parent ) )
	{
		gchar * sfile;
		do
		{
			gtk_tree_model_get( list, &parent, 0, &sfile, -1);
			switch (c) {
				case 0:
					EntityList_ScanDirectory( NULL, ".mps", FALSE, GTK_TREE_STORE(list), &parent );
					break;
				case 1:
					EntityList_ScanDirectory( "routines/", ".inc", TRUE, GTK_TREE_STORE(list), &parent );
					break;
				case 2:
					EntityList_ScanDirectory( "maps/", ".mps", FALSE, GTK_TREE_STORE(list), &parent );
					break;
				case 3:
					EntityList_ScanDirectory( "maps/routines/", ".inc", TRUE, GTK_TREE_STORE(list), &parent );
					break;
			}
			c++;
		} while ( gtk_tree_model_iter_next( list, &parent ) );
	}
	else
	{
		gtk_tree_store_append( GTK_TREE_STORE(list), &parent, NULL );
		gtk_tree_store_set( GTK_TREE_STORE(list), &parent, 0, "<b>Entities</b>", 1, 0, 2, NULL, 3, 0, 4, NULL, 5, NULL, -1 );
		EntityList_ScanDirectory( NULL, ".mps", FALSE, GTK_TREE_STORE(list), &parent );

		gtk_tree_store_append( GTK_TREE_STORE(list), &parent, NULL );
		gtk_tree_store_set( GTK_TREE_STORE(list), &parent, 0, "<b>Entities Routines</b>", 1, 0, 2, NULL, 3, 0, 4, NULL, 5, NULL, -1 );
		EntityList_ScanDirectory( "routines/", ".inc", TRUE, GTK_TREE_STORE(list), &parent );

		gtk_tree_store_append( GTK_TREE_STORE(list), &parent, NULL );
		gtk_tree_store_set( GTK_TREE_STORE(list), &parent, 0, "<b>Entities Routines</b>", 1, 0, 2, NULL, 3, 0, 4, NULL, 5, NULL, -1 );
		EntityList_ScanDirectory( "maps/", ".mps", FALSE, GTK_TREE_STORE(list), &parent );

		gtk_tree_store_append( GTK_TREE_STORE(list), &parent, NULL );
		gtk_tree_store_set( GTK_TREE_STORE(list), &parent, 0, "<b>Maps Routines</b>", 1, 0, 2, NULL, 3, 0, 4, NULL, 5, NULL, -1 );
		EntityList_ScanDirectory( "maps/routines/", ".inc", TRUE, GTK_TREE_STORE(list), &parent );
	}
}


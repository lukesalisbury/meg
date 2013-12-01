/****************************
Copyright © 2007-2013 Luke Salisbury
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
#include <glib.h>
#include <glib/gstdio.h>

/* External Functions */


/* Global Variables */
extern gboolean mokoiCompileError;

/* Local Variables */

/********************************
* EntityListMenu_GetFilePath
*
*/
gchar * EntityListMenu_GetFilePath( GtkTreeView * tree_view )
{
	gchar * file = NULL;
	gchar * file_path = NULL;
	GtkTreeIter iter;
	GtkTreeModel * model = gtk_tree_view_get_model( GTK_TREE_VIEW(tree_view) );
	GtkTreeSelection * selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(tree_view) );

	if ( gtk_tree_selection_get_selected( selection, &model, &iter) )
	{
		gtk_tree_model_get( model, &iter, 0, &file, -1 );
		file_path = g_strdup_printf( "/scripts/%s", file );
	}
	return file_path;
}



/********************************
* EntityMenu_Open
*
*/
void EntityListMenu_OpenAction(GtkMenuItem * menuitem , GtkTreeView * tree_view)
{
	gchar * file = EntityListMenu_GetFilePath( tree_view );
	if ( file )
	{
		EntityEditor_New( file );
		g_free( file );
	}
}

/********************************
* EntityMenu_Compile
*
*/
void EntityListMenu_CompileAction(GtkMenuItem * menuitem , GtkTreeView * tree_view)
{
	GtkWidget * dialog, * scroll, * log;
	gchar * file = EntityListMenu_GetFilePath( tree_view );

	if ( file )
	{
		dialog = gtk_dialog_new_with_buttons( "Compiling file", Meg_Main_GetWindow(), GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL );
		log = gtk_text_view_new();

		scroll = gtk_scrolled_window_new( NULL, NULL );
		gtk_scrolled_window_add_with_viewport( GTK_SCROLLED_WINDOW(scroll), log );/* FIX: GTK3 */

		gtk_container_add( GTK_CONTAINER(gtk_dialog_get_content_area( GTK_DIALOG(dialog) )), scroll );
		gtk_widget_set_size_request(scroll, 300, 200);

		g_signal_connect( dialog, "close", (GCallback)gtk_widget_destroy, NULL );
		g_signal_connect( dialog, "response", (GCallback)gtk_widget_destroy, NULL );

		if ( !EntityCompiler_FileWithRountines( file, log, NULL ) )
		{
			gtk_widget_show_all( dialog );
			Meg_Log_Print( LOG_WARNING, "Entity '%s' compile failed, see dialog for details\n", file );
		}
		else
		{
			gtk_widget_destroy( dialog );
			Meg_Log_Print( LOG_NONE, "Entity '%s' compiled\n", file );
		}
		GtkTreeModel * model = gtk_tree_view_get_model( tree_view );
		EntityList_UpdatePage( GTK_TREE_STORE(model) );
		gtk_tree_view_expand_all( tree_view );
		g_free( file );
	}
}

/********************************
* EntityMenu_Settings
*
*/
void EntityListMenu_SettingsAction(GtkMenuItem * menuitem , GtkTreeView * tree_view)
{
	gchar * file = EntityListMenu_GetFilePath( tree_view );
	if ( file )
	{
		Entity_Properties( file );
		g_free( file );
	}
}

/********************************
* EntityMenu_Remove
*
*/
void EntityListMenu_RemoveAction(GtkMenuItem * menuitem , GtkTreeView * tree_view)
{
	gchar * newfile;
	gchar * oldfile = EntityListMenu_GetFilePath( tree_view );
	if ( oldfile )
	{
		newfile = g_strconcat( oldfile, "-remove", NULL );
		g_rename( oldfile, newfile );

		GtkTreeModel * model = gtk_tree_view_get_model( tree_view );
		EntityList_UpdatePage( GTK_TREE_STORE(model) );
		gtk_tree_view_expand_all( tree_view );
		g_free( newfile );
		g_free( oldfile );
	}
}



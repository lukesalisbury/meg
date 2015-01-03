/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Standard Headers */
#include <zlib.h>
#include "loader_global.h"
#include "loader_functions.h"
#include "package.h"

#ifndef PACKAGE_URL
	#define PACKAGE_URL "http://mokoi.info/packages/xml/"
#endif


/* Functions local */
void element_start_updates( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, gpointer data, GError **error);
void element_end_updates( GMarkupParseContext *context, const gchar *element_name, gpointer data, GError **error);
void element_text_updates( GMarkupParseContext *context, const gchar *text, gsize text_len, gpointer data, GError **error);

/* Global Variables */
extern GError * mokoiError;

/* Local Variables */
static GMarkupParser updates_parser = { element_start_updates, element_end_updates, element_text_updates, NULL, NULL};

/* Local Type */


/* UI */



/* Functions */


/********************************
* Import_AddListing
*
@
*/

void Import_AddListing( GtkListStore * store, const gchar * package_name, const gchar * package_path, const gchar * package_author, const gchar * package_version,
						const gchar * package_description, guint8 package_type, gboolean is_local )
{
	GtkTreeIter iter;
	gtk_list_store_append( store, &iter );
	gtk_list_store_set( store, &iter,
						1, g_strdup(package_name),
						2, g_strdup(package_path),
						4, g_strdup(package_version),
						7, NULL,
						8, package_type,
						9, g_markup_printf_escaped( "<b>URL:%s</b>\n<b>Author:%s</b>\n%s\n", package_path, package_author, package_description ),
						-1 );


	if ( is_local )
	{
		gtk_list_store_set( store, &iter,
							0, 1, // Active
							3, TRUE, // Local
							5, "Install", // Button
							6, "gtk-harddisk", // Button Image
							-1 );
	}
	else
	{
		gtk_list_store_set( store, &iter,
							0, 1, // Active
							3, FALSE, // Local
							5, "Download", // Button
							6, "gtk-go-down", // Button Image
							-1 );
	}

}


/********************************
* ImportDialog_Download
*
@ url:
@ target_file_name
-
*/
GAsyncQueue * ImportDialog_Download( GtkWidget * parent, const gchar * url, const gchar * target_file_name, gint (*return_text)( gchar *,  gpointer ), gpointer data )
{
	GAsyncQueue * queue = NULL;

	if ( target_file_name )
	{
		queue = Meg_WebQueue_Retrieve( parent, url, NULL, NULL, target_file_name, return_text, data );
	}
	else if ( return_text )
	{
		queue = Meg_WebQueue_RetrieveText( parent, url, NULL, NULL, return_text, data );
	}

	return queue;
}

/********************************
* ImportDialog_UpdatePackageListing
*
@
-
*/
gint ImportDialog_UpdatePackageListing( gchar * filename, gpointer data )
{
	if ( filename )
	{
		TreeItem * tree_item = (TreeItem*)data;
		if ( g_file_test( filename, G_FILE_TEST_EXISTS) )
		{
			gtk_list_store_set( GTK_LIST_STORE(tree_item->model), &tree_item->iter, 0, TRUE, 2, filename, 3, 1, 5, "Install", 6, "gtk-harddisk", -1 );
		}
		else
		{
			gtk_list_store_set( GTK_LIST_STORE(tree_item->model), &tree_item->iter, 0, TRUE, 2, filename, 3, 1, 5, "Error", 6, GTK_STOCK_STOP, -1 );
		}
	}
	if ( data )
	{
		g_free(data);
	}
	return 0;
}

/********************************
* Import_HandleXML
*
@
-
*/
gint Import_HandleXML( gchar * content, gpointer data )
{
	if ( content )
	{
		GMarkupParseContext * ctx = NULL;
		ctx = g_markup_parse_context_new( &updates_parser,( GMarkupParseFlags)0, data, NULL );
		if ( !g_markup_parse_context_parse( ctx, content, -1, &mokoiError ) )
		{
			Meg_Log_Print(LOG_ERROR, "Package listing XML parse error: %s", mokoiError->message );
			g_clear_error( &mokoiError );
		}
		else
		{
			if ( !g_markup_parse_context_end_parse( ctx, &mokoiError) )
			{
				Meg_Log_Print(LOG_ERROR, "Package listing parse error: %s", mokoiError->message );
				g_clear_error( &mokoiError );
			}
			else
			{
				g_markup_parse_context_free (ctx);
			}
		}
		return 1;
	}
	return 0;
}


/********************************
* ImportDialog_RequestUpdates
*
@
@
@
@
*/
void ImportDialog_RequestUpdates( GtkWidget * widget,  GtkListStore * store )
{
	GAsyncQueue * queue = ImportDialog_Download( gtk_widget_get_toplevel(widget), PACKAGE_URL, NULL, &Import_HandleXML, store );

}

/********************************
* ImportDialog_RequestPackage
*
@
@
@
@
*/
void ImportDialog_RequestPackage( GtkWidget * widget,  gchar * package_title, gchar * package_version,  gchar * url,  TreeItem * tree_item )
{
	GtkWidget * question_dialog = NULL;
	//GAsyncQueue * queue = NULL;
	gchar * file_name = NULL;
	gchar * package_name = g_strdup(package_title);
	g_strdelimit( package_name, "_-|> <.\\/\"'!@#$%^&*(){}[]", '_' );

	file_name = g_strdup_printf("%s%c%s-%s.package", Meg_Directory_Data("packages"), G_DIR_SEPARATOR, package_name, package_version);

	question_dialog = gtk_message_dialog_new( Meg_Misc_ParentWindow(widget), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Are you sure you want to download '%s'?", file_name );
	if ( gtk_dialog_run( GTK_DIALOG(question_dialog) ) == GTK_RESPONSE_YES )
	{
		ImportDialog_Download( gtk_widget_get_toplevel(widget), url, file_name, &ImportDialog_UpdatePackageListing, (gpointer)tree_item );
	}
	gtk_widget_destroy( question_dialog );


}


/********************************
* ImportDialog_InstallSelected
*
@
@
@
@
*/
void ImportDialog_InstallSelected( GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data )
{
	GtkTreeSelection * select;
	GtkTreeModel * model;
	GtkTreeIter iter;
	gboolean installable = FALSE, local = FALSE;
	gchar * package_title = NULL, * package_version = NULL, * file_path = NULL;
	GtkWidget * progress_widget = NULL;

	select = gtk_tree_view_get_selection(tree_view);
	progress_widget = (GtkWidget *)user_data;

	if ( gtk_tree_selection_get_selected( select, &model, &iter) )
	{
		gtk_tree_model_get( model, &iter, 0, &installable, 1, &package_title, 2, &file_path, 3, &local, 4, &package_version, -1 );
		if ( installable )
		{
			if ( local )
			{
				Package_Import( file_path, progress_widget, model, iter, FALSE );
				gtk_list_store_set( GTK_LIST_STORE(model), &iter, 0, FALSE, 3, 0, 5, "Installed", 6, "gtk-yes", -1 );
			}
			else
			{
				TreeItem * tree_item = g_new0(TreeItem, 1);
				tree_item->model = model;
				tree_item->iter = iter;
				ImportDialog_RequestPackage( GTK_WIDGET(tree_view),  package_title, package_version,  file_path,  tree_item );

			}
		}

	}

	g_free( package_title );
	g_free( package_version );
	g_free( file_path );
}





/********************************
* ImportDialog_ScanLocal
*
@button:
@store:
Active (gboolean), Name (char*), Path (char*), Local (gboolean), Version (char*), button text (char*), button image (char*), type_image (GdkPixbuf*), type (guchar), Description (char*)

*/
void ImportDialog_ScanLocal( GtkWidget * button, GtkListStore * store )
{
	gtk_list_store_clear(store); // Clear Old List

	GDir * current_directory;
	const gchar * current_file;
	gchar * packages_dir;

	packages_dir = Meg_Directory_Data("packages");
	current_directory = g_dir_open( packages_dir, 0, NULL );

	if ( current_directory )
	{
		current_file = g_dir_read_name(current_directory);
		while ( current_file != NULL )
		{
			if ( g_str_has_suffix(current_file, ".package") )
			{
				guint8 package_type;
				gchar * package_path = NULL, * package_name = NULL, * package_version = NULL, * package_description = NULL;

				if ( Package_Infomation( current_file, &package_name, &package_path, &package_version, &package_description, &package_type ) )
				{
					Import_AddListing( store, package_name ? package_name : current_file, package_path, "", package_version, package_description, package_type, TRUE);
				}

				g_free(package_path);
				g_free(package_name);
				g_free(package_version);
				g_free(package_description);

			}

			current_file = g_dir_read_name( current_directory );
		}
	}
	else
	{
		g_mkdir_with_parents( packages_dir, 0755 );
	}

	g_dir_close( current_directory );
	g_free( packages_dir );

}





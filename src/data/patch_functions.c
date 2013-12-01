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
#include "logger_functions.h"
#include "game_compiler.h"
#include <glib.h>
#include <glib/gstdio.h>
#include "compression_functions.h"
#include "progress_dialog.h"

gpointer Patch_CreationThread( ProgressDialogWidgets * wids );

guint32 file_read_long( FILE * fd );
guint16 file_read_short( FILE * fd );
guint32 file_get_size( gchar * file );
void file_write_long( FILE * fd, gulong value );

/* Global Variables */
extern gchar * mokoiBasePath;


ProgressDialogWidgets mokoiPatchCompiler;
gchar * mokoiPatchDirectories[] = {
	"dialog",
	"lang",
	"maps",
	"music",
	"masks",
	"scripts",
	"scripts/maps",
	"soundfx",
	"sprites",
	"preload",
	"sections",
	"",
	NULL
};

/* UI */


/* Functions */

/********************************
* Patch_HasChanged
*
@ list:
@ file_name:
@ size:
*/
gboolean Patch_HasChanged( GSList * list,  gchar * file_name, guint size )
{
	GSList * list_scan = list;
	while ( list_scan )
	{
		if ( file_name && ((StoredFileInfo *)list_scan->data)->name )
		{
			if ( !g_ascii_strcasecmp(((StoredFileInfo *)list_scan->data)->name, file_name) )
			{
				return (((StoredFileInfo *)list_scan->data)->length != size);
			}
		}
		list_scan = g_slist_next( list_scan );
	}
	return FALSE;
}

/********************************
* Patch_ScanCurrent
*
@ store:
@ game_files:
*/
void Patch_ScanCurrent( GtkListStore * store, GSList * game_files )
{
	GtkTreeIter children;
	gchar * directory_path, * file_path, * package_file;
	const gchar * current;
	GDir * directory;
	guint array_count = 0;


	gtk_list_store_clear( store );


	if ( g_strv_length( mokoiPatchDirectories ) )
	{
		while ( mokoiPatchDirectories[array_count] != NULL )
		{
			directory_path = g_build_path( G_DIR_SEPARATOR_S, mokoiBasePath, mokoiPatchDirectories[array_count], NULL );
			directory = g_dir_open( directory_path, 0, NULL);
			current = g_dir_read_name( directory );
			while ( current != NULL )
			{
				if ( current[0] != '.' )
				{
					package_file = g_build_path( "/", mokoiPatchDirectories[array_count], current, NULL );
					file_path = g_build_filename( directory_path, current, NULL );
					if ( g_file_test( file_path, G_FILE_TEST_IS_REGULAR ) )
					{
						gtk_list_store_append( store, &children );
						gtk_list_store_set( store, &children, 0, Patch_HasChanged( game_files, package_file, file_get_size(file_path) ), 1, package_file, -1 );
					}
					g_free( package_file );
					g_free( file_path );
				}
				current = g_dir_read_name( directory );
			}
			g_dir_close( directory );
			array_count++;
		}
	}

}

/********************************
* Patch_CreationForeach
*
*/
gboolean Patch_AddFile( GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, GSList ** list )
{
	gboolean selected;
	gchar * file_name;
	gtk_tree_model_get( model, iter, 0, &selected, 1, &file_name, -1 );

	if ( selected )
	{
		if ( g_strcmp0( ROOT_FILENAME, file_name ) == 0 )
		{
			Compression_PackFile( list, "patch.setting", file_name, FALSE);
		}
		else
		{
			Compression_PackFile( list, file_name, file_name, FALSE );
		}
	}
	g_free( file_name );

	return FALSE;
}


/********************************
* Patch_CreationThread
*
*/
gpointer Patch_CreationThread( ProgressDialogWidgets * info )
{
	Logger_SetQueue( info->queue ); // redirects a Logger_* to pass messages

	GSList * file_list = NULL;

	/* Title and ID */
	guint32 id = 0;
	id = AL_SettingNumber( "project.id" );

	/* Get Selected File */
	gtk_tree_model_foreach( GTK_TREE_MODEL(info->list), (GtkTreeModelForeachFunc)Patch_AddFile, &file_list );

	/* Save package file */
	MokoiPackage_SavePatch( info->filename, file_list, id, 0 );
	Logger_FormattedLog(NULL, LOG_BOLD, "Patch saved to %s.\n", info->filename);

	Logger_SetQueue( NULL );  // resets logger
	g_async_queue_push( info->queue, g_thread_self() ); // closes thread

	return NULL;

}




/********************************
* Patch_Create
*
*/
void Patch_Create( gchar * file, GtkListStore * store )
{
	GTimeVal time;
	gchar * file_name = NULL;


	ProgressDialog_Clear( &mokoiPatchCompiler );


	/* Generate File Name */
	g_get_current_time( &time );
	file_name = g_strdup_printf( "%s-%ld.gamepatch", mokoiBasePath, time.tv_sec );

	/* Create Thread */
	if ( ProgressDialog_Create( &mokoiPatchCompiler, "Compiling Patch", file_name ) )
	{
		g_object_ref(store);
		mokoiPatchCompiler.list = store;
		ProgressDialog_Activate( &mokoiPatchCompiler, (GThreadFunc)Patch_CreationThread, (GSourceFunc)ProgressDialog_Watch );
	}

}


/********************************
* Patch_FileChanged
*
*/
void Patch_FileChanged( GtkFileChooserButton * widget, gpointer user_data)
{
	gchar * file_path = NULL;
	GSList * game_files = NULL;
	FILE * fd;
	GtkListStore * store = (GtkListStore*)user_data;

	/* Scan Game File */
	file_path = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(widget) );
	fd = g_fopen( file_path, "rb" );
	if ( fd )
		MokoiPackage_OpenGame( fd, &game_files );
	fclose(fd);

	/* */
	Patch_ScanCurrent( store, game_files );

	/* Clear Game Files */
	g_slist_free_full(game_files, (GDestroyNotify)StoredFileInfo_Free);
}


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
#include "loader_functions.h"
#include "setting_functions.h"

/* Required Headers */
#include <string.h>
#include <zlib.h>
#include <sys/stat.h>
#include <glib.h>
#include <glib/gstdio.h>
#include "mokoi_package.h"
#include "compression_functions.h"
#include "sheets_functions.h"
#include "progress_dialog.h"
#include "logger_functions.h"


void Package_ImportConfig(gboolean remove_file);


/* Global Variables */
extern gchar * mokoiBasePath;
extern GError * mokoiError;
extern GKeyFile * mokoiConfigTable;
extern gboolean mokoiUsingPackage;

/* Local Variables */
ProgressDialogWidgets mokoiPackageImport;
typedef struct {
	GAsyncQueue * queue;
	gchar * url;
	gchar * filename;
	GSList * files; /*<StoredFileInfo>*/
	GtkProgressBar * progress;
	GtkTreeModel * model;
	GtkTreeIter iter;
} PackageWatch;


/* UI */
#include "ui/package_question.gui.h"
#include "ui/package_files.gui.h"
const gchar * mokoiUI_PackageFiles = GUIPACKAGE_FILES
const gchar * package_question_ui = GUIPACKAGE_QUESTION

/********************************
* Package_ExtractFile
*
@ in_file:
@ out_dir:
*/
gboolean Package_ExtractFile( FILE * fd, StoredFileInfo * fi )
{
	if ( fd == NULL )
	{
		Meg_Error_Print( __func__, __LINE__, "Error opening resource file");
		return FALSE;
	}
	if ( fi == NULL )
	{
		return FALSE;
	}

	if ( fi->from_zip )
	{
		CompressionZip_Extract( fd, fi, fi->name );
	}
	else
	{
		MokoiPackage_Extract( fd, fi, fi->name);
	}
	return FALSE;
}



/********************************
* Package_Infomation
*
@ in_file:
@ out_dir:
*/
gboolean Package_Infomation( const gchar * file, gchar ** package_name, gchar ** package_path, gchar ** package_version,
						gchar ** package_description, guint8 *package_type )
{
	gboolean valid = FALSE;
	FILE * fd = NULL;
	gchar * file_path = NULL;
	GSList * files = NULL;
	file_path = Meg_Directory_DataFile( "packages", file );

	if ( g_file_test(file_path, G_FILE_TEST_IS_REGULAR) )
	{
		fd = fopen( file_path, "rb" );

		if ( fd )
		{
			valid = MokoiPackage_OpenResource( fd, &files, package_name, NULL, package_type );

			if ( package_description != NULL && files )
			{
				*package_description = MokoiPackage_ExtractContent( fd, StoredFileInfo_Find( files, "package/DESCRIPTION" ) );
			}

			if ( package_version != NULL && files )
			{
				*package_version = MokoiPackage_ExtractContent( fd, StoredFileInfo_Find( files, "package/VERSION" ) );
			}

			if ( package_path != NULL )
				*package_path = g_strdup(file_path);

		}

		g_slist_free_full( files, (GDestroyNotify)StoredFileInfo_Free );
		fclose( fd );
	}
	g_free( file_path );
	return valid;
}

/********************************
* Package_FileList
*
@ data:
@ size:
*/
GSList * Package_FileList( const gchar * file )
{
	GSList * list = NULL;
	FILE * fd = fopen( file, "rb" );

	if ( fd != NULL )
	{
		MokoiPackage_OpenResource( fd, &list, NULL, NULL, NULL );
	}
	fclose( fd );

	return list;
}


/********************************
* Package_FileSelectionDialog_Update
*
@ data:
@ user_data:
*/
void Package_FileSelectionDialog_Update(gpointer data, gpointer user_data)
{
	StoredFileInfo *file_info = (StoredFileInfo*)data;
	GtkListStore * list = (GtkListStore*)user_data;

	GtkTreeIter iter;

	gtk_list_store_append( list, &iter );
	gtk_list_store_set( list, &iter, 0, TRUE, 1, g_strdup(file_info->name),  2, data, -1 );

}

/********************************
* Package_FileSelectionDialog
*
@ file:
@ widget:
*/
GSList * Package_FileSelectionDialog( const gchar * file, GtkWidget * widget  )
{
	GtkListStore * store_files;
	GtkWidget * dialog;
	GSList * file_list = NULL;
	GSList * file_import_list = NULL;

	file_list = Package_FileList( file );

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(mokoiUI_PackageFiles, __func__, __LINE__);
	g_return_val_if_fail( ui, NULL );

	/* Widgets */
	dialog = GET_WIDGET( ui, "messagedialog1");
	store_files = GET_LISTSTORE( ui, "store_files" ); /* char*, boolean , StoredFileInfo */

	/* signals */
	g_signal_connect( gtk_builder_get_object( ui, "cellrenderertoggle1" ), "toggled", G_CALLBACK(CellRender_Event_FixToggled), NULL );

	/* Update Dialog */
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Misc_ParentWindow(widget) );
	gtk_message_dialog_format_secondary_text( GTK_MESSAGE_DIALOG(dialog), "Package: %s", file );
	g_slist_foreach( file_list, (GFunc) Package_FileSelectionDialog_Update, store_files );


	/* Display Dialog */
	if ( gtk_dialog_run(GTK_DIALOG(dialog)) == -1 )
	{
		/* Scan List */
		GtkTreeIter iter;
		gboolean valid = gtk_tree_model_get_iter_first( GTK_TREE_MODEL(store_files), &iter);
		while ( valid )
		{
			StoredFileInfo * file_info;
			gboolean install_file;

			gtk_tree_model_get( GTK_TREE_MODEL(store_files), &iter, 2, &file_info, 0, &install_file, -1 );
			if ( install_file )
			{
				file_import_list = g_slist_append( file_import_list, file_info );
			}

			valid = gtk_tree_model_iter_next( GTK_TREE_MODEL(store_files), &iter );
		}
	}

	gtk_widget_destroy( dialog );


	return file_import_list;
}


/********************************
* Package_ImportConfig
*
@ data:
@ size:
*/
void Package_ImportConfig( gboolean remove_file )
{
	gchar * config_text = NULL;
	gsize config_length = 0;
	if ( Meg_file_get_contents( "/package/package.config", &config_text, &config_length, &mokoiError ) )
	{
		GKeyFile * package_config = NULL;


		gsize lc = 0;
		gsize length = 0;
		gchar ** keys;

		Meg_Error_Check( mokoiError, FALSE, __func__ );

		/* Config file found */
		package_config = g_key_file_new();
		g_key_file_load_from_data( package_config, config_text, config_length, G_KEY_FILE_KEEP_TRANSLATIONS, &mokoiError );


		keys = g_key_file_get_keys(package_config, "package", &length, NULL);
		if ( length )
		{
			while(keys[lc] != NULL)
			{
				GtkWidget * dialog = gtk_message_dialog_new( Meg_Main_GetWindow(), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Import Setting '%s'?", keys[lc] );
				gint responed = gtk_dialog_run( GTK_DIALOG(dialog) );
				if ( responed == GTK_RESPONSE_ACCEPT || responed == GTK_RESPONSE_YES )
				{
					gchar * value = g_key_file_get_string( package_config, "package", keys[lc], NULL );
					g_key_file_set_string( mokoiConfigTable, "Mokoi", keys[lc], value );
				}
				gtk_widget_destroy( dialog );

				lc++;
			}
		}
		g_strfreev(keys);

		g_key_file_free( package_config );

		if ( remove_file )
			PHYSFS_delete( "/package/package.config" );

	}

	g_free(config_text);

	Meg_Error_Check( mokoiError, FALSE, __func__ );


}




/********************************
* Package_Import
*
@ file:
@ progress_widget:
@ model:
@ iter:
@ initial:
*/
void Package_ImportFiles( const gchar * filename, GSList * files )
{
	FILE * fd;
	GSList * item = NULL;
	GTimeVal current_time;
	PHYSFS_File * log_file;
	StoredFileInfo * file_info;
	gchar * moved_file;

	/* Open package */
	fd = g_fopen( filename, "rb" );

	/* Update log */
	log_file = PHYSFS_openAppend("packages.log");
	g_get_current_time(&current_time);

	PHYSFS_writeString(log_file, "Starting install on %s\r\n", g_time_val_to_iso8601(&current_time) );

	item = files;
	while ( item )
	{
		file_info = (StoredFileInfo*)item->data;
		if ( file_info )
		{
			if ( !Meg_file_test( file_info->name, G_FILE_TEST_EXISTS) )
			{
				PHYSFS_writeString(log_file, "Installed: %s\r\n", file_info->name );
				Package_ExtractFile( fd, file_info );
			}
			else
			{
				moved_file = g_strdup_printf("%s.%ld.bak", file_info->name, current_time.tv_sec );
				PHYSFS_rename(file_info->name, moved_file);
				PHYSFS_writeString(log_file, "Overwriting: %s - moved to %s\r\n", file_info->name, moved_file );
				Package_ExtractFile( fd, file_info );

			}
		}


		item = g_slist_next( item );
	}



	PHYSFS_close(log_file);
	fclose(fd);
}


/********************************
* Package_ImportThread
*
*/
gpointer Package_ImportThread( gpointer data )
{
	PackageWatch * package = (PackageWatch *)data;

	g_object_ref(package->progress);
	g_object_ref(package->model);

	if ( package->files )
	{
		Package_ImportFiles( package->filename, package->files );
	}


	g_object_unref(package->progress);
	g_object_unref(package->model);

	g_async_queue_push( package->queue, package );

	return NULL;
}

/********************************
* Package_ImportThread_ProgressDialog
* Same as above but uses ProgressDialog instead.
*/
gpointer Package_ImportThread_ProgressDialog( ProgressDialogWidgets *wids )
{
	Logger_SetQueue( wids->queue ); // redirects a Logger_* to pass messages

	if ( wids->file_list )
	{
		Package_ImportFiles( wids->filename, wids->file_list );
	}

	Logger_SetQueue( NULL );  // resets logger
	g_async_queue_push( wids->queue, g_thread_self() ); // closes thread
	return NULL;
}

/********************************
* Package_ImportWatch
*
*/
gboolean Package_ImportWatch( PackageWatch * package )
{
	gpointer q = g_async_queue_try_pop( package->queue );
	if ( q == NULL )
	{
		gtk_progress_bar_pulse( package->progress );
		return TRUE;
	}

	if ( q == package )
	{
		gtk_list_store_set( GTK_LIST_STORE(package->model), &package->iter, 0, FALSE, 2, package->filename, 3, 1, 5, "Installed", 6, "gtk-harddisk", -1 );

		if ( Meg_file_test( "/package/package.config", (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) )
		{
			Package_ImportConfig( TRUE );
		}

		g_async_queue_unref( package->queue );


		g_free(package->url);
		g_free(package->filename);
		g_free(package);


		/* Reload Spritesheet */
		char ** files = PHYSFS_enumerateFiles("/sprites/");
		char ** current;

		for (current = files; *current != NULL; current++)
		{
			if ( g_str_has_suffix( *current, ".png" ) )
			{
				Sheet_Parse( *current );
			}
		}
		PHYSFS_freeList(files);

		/* Save and refresh Setting Widget */
		Setting_Save();
		AL_SettingsRefresh();

	}

	return FALSE;
}


/********************************
* Package_ImportDrop
*
@ file:
*/
gboolean Package_ImportDrop(const gchar * file )
{


	ProgressDialog_Clear( &mokoiPackageImport );
	if ( ProgressDialog_Create( &mokoiPackageImport, "Importing Files", file ) )
	{
		mokoiPackageImport.file_list = Package_FileSelectionDialog(file, Meg_Main_GetWidget());
		ProgressDialog_Activate( &mokoiPackageImport, (GThreadFunc)Package_ImportThread_ProgressDialog, (GSourceFunc)ProgressDialog_Watch );
		return TRUE;
	}
	return FALSE;
}

/********************************
* Package_ChangeMain
*
@ package:
*/
gboolean Package_ChangeMain( const gchar * new_package_name )
{
	gchar * package_location = NULL;
	gchar * package_name = AL_SettingString("package.main");


	if ( !g_strcmp0(new_package_name, package_name) )
	{
		return TRUE;
	}

	if ( package_name )
	{
		package_location = Meg_Directory_DataFile( "packages", package_name );
		if ( !PHYSFS_removeFromSearchPath(package_location) )
		{
			g_warning( "Package Unload Error %s", PHYSFS_getLastError() );
		}
		else
		{
			mokoiUsingPackage = FALSE;
		}
		g_free(package_location);
	}

	if ( new_package_name )
	{
		package_location = Meg_Directory_DataFile( "packages", new_package_name );
		if ( !PHYSFS_mount(package_location, NULL, 1) )
		{
			g_warning( "Package Load Error %s", PHYSFS_getLastError() );
		}
		else
		{
			mokoiUsingPackage = TRUE;
			Package_ImportConfig( FALSE );
		}


		g_free(package_location);
	}
	return TRUE;
}

/********************************
* Package_ImportInital
*
@ file:
@ root_dir:
*/
gboolean Package_ImportInital(const gchar * file, const gchar * root_dir )
{
	/* Install Base System */
	gboolean files_imported = FALSE;
	GSList * package_files;
	gchar * package_path;
	GtkDialog * dialog;

	GtkBuilder * ui = Meg_Builder_Create(package_question_ui, __func__, __LINE__);



	Meg_Log_Print( LOG_NONE, "Installing Base System: '%s'", file);

	package_path = Meg_Directory_DataFile( "packages", file );

	/* Run Dialog */
	dialog = GTK_DIALOG( gtk_builder_get_object( ui, "messagedialog1" ) );
	if ( gtk_dialog_run(dialog) == 1 )
	{
		/* Set Up physfs */
		PHYSFS_mount(root_dir, NULL, 0);
		PHYSFS_setWriteDir(root_dir);

		package_files = Package_FileList( file );
		Package_ImportFiles( file, package_files );
		files_imported = TRUE;


		/* Remove physfs  */
		PHYSFS_setWriteDir(NULL);
		PHYSFS_removeFromSearchPath(root_dir);

	}
	gtk_widget_destroy( GTK_WIDGET(dialog) );




	g_free( package_path );
	return files_imported;
}

/********************************
* Package_Import
*
@ file:
@ progress_widget:
@ model:
@ iter:
@ initial:
*/
void Package_Import(const gchar * file, GtkWidget * progress_widget, GtkTreeModel * model, GtkTreeIter iter, gboolean initial )
{
	PackageWatch * package = g_new0(PackageWatch, 1);

	gtk_list_store_set( GTK_LIST_STORE(model), &iter, 0, FALSE, 3, 0, 5, "Extracting", 6, "gtk-yes", -1 );

	if ( !initial )
	{
		package->files = Package_FileSelectionDialog(file, progress_widget);
	}
	else
	{
		package->files = Package_FileList( file );
	}

	package->queue = g_async_queue_new();
	package->progress = GTK_PROGRESS_BAR(progress_widget);
	package->model = model;
	package->iter = iter;
	package->filename = g_strdup(file);

	if ( g_thread_create( (GThreadFunc)Package_ImportThread, (gpointer)package, FALSE, NULL ) )
	{
		g_timeout_add( 20, (GSourceFunc)Package_ImportWatch, (gpointer)package );
	}
}




/********************************
* Package_ExportList
* Starts creating a content package
@
@
@
@
@
@
- return a list of files or null if file already exists
*/
GSList * Package_ExportInitial( const gchar * package_path, const gchar * title, const gchar * version, const gchar * description, const gchar * authors, const gchar * terms, const gchar * config )
{
	GSList * files = NULL;

	Compression_PackText( &files, "package/PACKAGE", title );
	Compression_PackText( &files, "package/VERSION", version );
	Compression_PackText( &files, "package/AUTHORS", authors );
	Compression_PackText( &files, "package/TERMSOFUSE", terms );
	Compression_PackText( &files, "package/DESCRIPTION", description );
	if ( g_utf8_strlen(config, -1) )
		Compression_PackText( &files, "package/package.config", config );

	return files;
}

/********************************
* Package_ExportFolders
* Creates Content Package.
*/
gboolean Package_ExportFolders(const gchar * package_path, const gchar * title, const gchar * version, const gchar * description, const gchar * authors, const gchar * terms, const gchar * config, GSList * folders)
{
	gboolean valid = FALSE;
	GSList * package_files = NULL;
	GSList * current_folder = NULL;
	gchar * folder = NULL;

	package_files = Package_ExportInitial( package_path, title, version, description, authors, terms, config );

	if ( package_files != NULL )
	{
		Compression_PackFile( &package_files, "map.options", "map.options", FALSE );
		Compression_PackFile( &package_files, ROOT_FILENAME, ROOT_FILENAME, FALSE );

		Compression_PackFolder( &package_files, "doc", NULL, FALSE  );

		if ( folders != NULL )
		{
			current_folder = folders;

			while ( current_folder )
			{
				folder = (gchar*)current_folder->data;
				Compression_PackFolder( &package_files, folder, NULL, FALSE );
				current_folder = g_slist_next(current_folder);
			}
		}


		/* Add Required Files */
		if ( Meg_file_test("/package/REQUIREDFILES", (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR) ) )
		{
			gchar * require_files_content = NULL;
			gchar ** require_files = NULL;
			guint n = 0;
			if ( Meg_file_get_contents("/package/REQUIREDFILES", &require_files_content, NULL, NULL) )
			{
				require_files = g_strsplit( require_files_content,"\n", -1 );

				while ( require_files[n] )
				{
					Compression_PackFile( &package_files, require_files[n], require_files[n], FALSE );
					n++;
				}
			}
		}


		/* Save package file */
		MokoiPackage_SaveResource( package_path, package_files, title, authors, 1);
		Meg_Log_Print(LOG_BOLD, "Package saved to %s.\n", package_path);

		valid = TRUE;
	}

	GtkWidget * message_dialog = NULL;
	if ( valid )
	{
		message_dialog = gtk_message_dialog_new( Meg_Main_GetWindow(), GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
												 GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "Package saved to %s.", package_path );
	}
	else
	{
		message_dialog = gtk_message_dialog_new( Meg_Main_GetWindow(), GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
												 GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Error creating package" );
	}
	gtk_dialog_run( GTK_DIALOG(message_dialog) );
	gtk_widget_destroy( message_dialog );


	g_slist_free_full(package_files, (GDestroyNotify)StoredFileInfo_Free);

	return valid;

}

/********************************
* Package_ExportList
* Creates Content Packages.
*/
gboolean Package_ExportList(const gchar * package_path, const gchar * title, const gchar * version, const gchar * description, const gchar * authors, const gchar * terms,
const gchar * config, GSList * files )
{
	g_return_val_if_fail( files, FALSE );

	gboolean valid = FALSE;
	GSList * package_files = NULL;
	GSList * current_file = NULL;
	gchar * file = NULL;
	gchar * interal_file = NULL;

	gsize len = 1;

	package_files = Package_ExportInitial( package_path, title, version, description, authors, terms, config );

	if ( package_files != NULL )
	{
		len = strlen(mokoiBasePath)+1;
		current_file = files;

		while ( current_file )
		{
			file = (gchar*)current_file->data;

			Meg_Log_Print(LOG_BOLD, "Adding %s as %s.\n", file, file+len);
			if ( g_str_has_prefix( file, mokoiBasePath )  )
			{
				interal_file = g_strdup_printf("%s", file+len); /* Strip off the file path */

				/* Replace \ with / */
				gsize c = 0;
				while ( interal_file[c] )
				{
					if ( interal_file[c] == '\\' )
						interal_file[c] = '/';
					c++;
				}
				Compression_PackFile( &package_files, interal_file, interal_file, FALSE);
			}
			current_file = g_slist_next( current_file );
		}

		/* Save package file */
		MokoiPackage_SaveResource( package_path, package_files, title, authors, 2);
		Meg_Log_Print(LOG_BOLD, "Package saved to %s.\n", package_path);


		valid = TRUE;
	}


	GtkWidget * message_dialog = NULL;
	if ( valid )
	{
		message_dialog = gtk_message_dialog_new( Meg_Main_GetWindow(), GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
												 GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "Package saved to %s.", package_path );
	}
	else
	{
		message_dialog = gtk_message_dialog_new( Meg_Main_GetWindow(), GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
												 GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Error creating package" );
	}
	gtk_dialog_run( GTK_DIALOG(message_dialog) );
	gtk_widget_destroy( message_dialog );


	g_slist_free_full(package_files, (GDestroyNotify)StoredFileInfo_Free);

	return valid;
}




/********************************
* Package_DownloadThread
*
*/
gpointer Package_DownloadThread( gpointer data )
{
	PackageWatch * download = (PackageWatch *)data;

	g_object_ref(download->progress);
	g_object_ref(download->model);

	GString * url = g_string_new(download->url);
	Meg_Web_RetrieveFile( url, NULL, NULL, download->filename, 1);
	g_string_free(url, FALSE);

	g_object_unref(download->progress);
	g_object_unref(download->model);

	g_async_queue_push( download->queue, download );

	return NULL;
}

/********************************
* Package_DownloadWatch
*
*/
gboolean Package_DownloadWatch( PackageWatch * download )
{
	gpointer q = g_async_queue_try_pop( download->queue );


	if ( q == NULL )
	{
		gtk_progress_bar_pulse( download->progress );
		return TRUE;
	}


	if ( q == download )
	{
		gtk_progress_bar_set_fraction( download->progress, 0 );
		if ( g_file_test( download->filename, G_FILE_TEST_EXISTS) )
		{
			gtk_list_store_set( GTK_LIST_STORE(download->model), &download->iter, 0, TRUE, 2, download->filename, 3, 1, 5, "Install", 6, "gtk-harddisk", -1 );
		}
		else
		{
			gtk_list_store_set( GTK_LIST_STORE(download->model), &download->iter, 0, TRUE, 2, download->filename, 3, 1, 5, "Error", 6, GTK_STOCK_STOP, -1 );
		}
		g_async_queue_unref( download->queue );

		g_free(download->url);
		g_free(download->filename);
		g_free(download);
	}

	return FALSE;
}

/********************************
* Package_Download
*
@ in_file:
@ out_dir:
*/
void Package_Download( const gchar * url, const gchar * package_title, const gchar * package_version, GtkWidget * progress_widget, GtkTreeModel * model, GtkTreeIter iter, gboolean initial )
{
	// Function causes a crash, so i'll disable it for now
	/*
	gchar * package_name = NULL;
	PackageWatch * package = g_new0(PackageWatch, 1);

	package_name = g_strdup(package_title);
	g_strdelimit( package_name, "_-|> <.\\/\"'!@#$%^&*(){}[]", '_' );

	package->queue = g_async_queue_new();
	package->filename = g_strdup_printf("packages%c%s-%s.package", G_DIR_SEPARATOR, package_name, package_version);
	package->url = g_strdup(url);
	package->progress = GTK_PROGRESS_BAR(progress_widget);
	package->model = model;
	package->iter = iter;

	if ( g_thread_create( (GThreadFunc)Package_DownloadThread, (gpointer)package, FALSE, NULL ) )
	{
		g_timeout_add( 20, (GSourceFunc)Package_DownloadWatch, (gpointer)package );
	}
	g_free( package_name );
	*/

}

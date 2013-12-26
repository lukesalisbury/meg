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
#include "audio_functions.h"
#include "entity_functions.h"
#include "physfs/physfs.h"

/* Required Headers */
#include <glib/gstdio.h>
#include <string.h>
#include <stdio.h>

#include "audio_functions.h"
#include "sheets_functions.h"
#include "package.h"

#ifdef CUSTOMSETTINGS
#include "../custom/default_banner.h"
#else
#include "../res/default_banner.h"
#endif



/* External Functions */
void Patch_CreatePage();
gboolean Meg_Dialog_Import();
void Import_Package( gchar * url, gchar * name, gchar * root );


/* Local Type */

/* Global Variables */

/* Local Variables */
gchar * mokoiBasePath = NULL;
GError * mokoiError = NULL;
GKeyFile * mokoiConfigTable = NULL;
GtkWidget * mokoiLogWindow = NULL;
GtkWidget * mokoiLogTextView = NULL;

gboolean mokoiSpritesheetsFinished = FALSE;
gboolean mokoiUsingPackage = FALSE;

gchar * mokoiGameDirectories[] = {
	"dialog",
	"doc",
	"lang",
	"maps",
	"music",
	"masks",
	"scripts",
	"scripts/maps",
	"scripts/routines",
	"sections",
	"soundfx",
	"sprites",
	"sprites/virtual",
	"collisions",
	"preload",
	"paths",
	NULL
};

gchar * mokoiGameRequiredFiles[] = {
	"lang/00.txt", "MSG_HELLO",
	"dialog/00.txt", "MSG_HELLO",
	"lang/en.txt", "Hello",
	"dialog/en.txt", "Hello",
	"preload/audio.txt", "",
	"preload/entities.txt", "",
	"preload/spritesheet.txt", "",
	"scripts/main.mps", "main()\n{\n}",
	"maps/empty.xml", "<map xmlns=\"http://mokoi.info/projects/mokoi\"><settings><dimensions width=\"1\" height=\"1\" /><color red=\"64\" blue=\"124\" green=\"98\" mode=\"0\"/></settings></map>",
	"scripts/maps/empty.mps", "#tryinclude <map_default>\nmain()\n{\n\t/**/\n}\n",
	NULL
};

gchar * mokoiDefaultConfig[] =
{
	"player.number", "1",
	"debug.able", "true",
	"save.allowed", "true",
	"map.width", "640",
	"map.height", "480",
	"screen.width", "640",
	"screen.height", "480",
	"display.width", "640",
	"display.height", "480",
	"project.title", "Hello World",
	"project.author", "Me, myself and I",
	"project.creation", "42",
	"project.id", "0",
	"language.default", "en",
	"display.mode", "Native",
	"display.colour", "0",
	"display.fullscreen", "0",
	"string.layer0", "Bottom Layer",
	"string.layer1", "layer 1",
	"string.layer2", "layer 2",
	"string.layer3", "layer 3",
	"string.layer4", "layer 4",
	"string.layer5", "layer 5",
	"string.layer6", "GUI Layer (Static)",
	NULL
};


gchar * mokoiInitalConfig = "[Mokoi]\n" \
		"project.title=%s\n"\
		"project.author=%s\n"\
		"project.creation=%lu\n"\
		"project.id=%ld\n";

/* UI */


/********************************
* AL_CreateDirectories
*
@
*/
gboolean AL_CreateDirectories( gchar * root_dir )
{
	/* Project has loaded correct, but lets make sure it has all the right directorys created */
	guint array_count = 0;
	gchar * temp_path = NULL;
	if ( g_strv_length( mokoiGameDirectories ) )
	{
		while ( mokoiGameDirectories[array_count] != NULL )
		{
			temp_path = g_build_path( G_DIR_SEPARATOR_S, root_dir, mokoiGameDirectories[array_count], NULL );
			g_mkdir_with_parents( temp_path, 0755 );
			g_free( temp_path );
			array_count++;
		}
	}
	return TRUE;
}


/********************************
* AL_CreateRequiredFiles
*
@
*/
gboolean AL_CreateRequiredFiles( gchar * root_dir )
{
	/* Create Required Files */
	guint array_count = 0;
	gchar * temp_path = NULL;
	if ( g_strv_length( mokoiGameRequiredFiles ) )
	{
		while ( mokoiGameRequiredFiles[array_count] != NULL)
		{
			temp_path = g_build_filename(root_dir, mokoiGameRequiredFiles[array_count++], NULL);
			g_file_set_contents( temp_path, mokoiGameRequiredFiles[array_count], -1, &mokoiError );
			if ( Meg_Error_Check( mokoiError, FALSE, (char*)__func__) )
			{
				Meg_Log_Print( LOG_NONE, "Error creating '%s', this may cause a issue down the track.", temp_path );
			}
			else
			{
				if ( g_str_has_suffix(temp_path, ".mps") )
				{
					EntityCompiler_FileWithRountines( temp_path, NULL, NULL );
				}
			}
			g_free( temp_path );
			array_count++;
		}
	}
	return TRUE;
}

/********************************
* AL_CreateProject
* Creates new project
@ path: path to new project set by this functions
@ title: Name of Project
@ base_project: Name of existing base project, or NULL for none;
* Returns filename or NULL;
*/
gchar * AL_CreateProject( const gchar * title )
{
	gchar * path = NULL;
	gchar * root_dir, * temp_path, * directory_name;
	GString * init_config;
	GTimeVal timestamp;
	guint array_count = 0;

	directory_name = Project_CleanTitle((gchar *)title); //Trims Project Title
	root_dir = g_build_path( G_DIR_SEPARATOR_S, Meg_Directory_Document(), directory_name, NULL );/* Create Nice name that can be used as the directory */


	g_get_current_time( &timestamp );

	if ( !g_file_test(root_dir, G_FILE_TEST_IS_DIR) )
	{
		Meg_Log_Print(LOG_NONE, "Creating Project in '%s'.", root_dir );
		g_mkdir_with_parents(root_dir, 0755);

		AL_CreateDirectories( root_dir );
		AL_CreateRequiredFiles( root_dir );

		/* Create Default game.mokoi file */
		init_config = g_string_new("");
		g_string_append_printf(init_config, mokoiInitalConfig, directory_name, g_get_real_name(), timestamp.tv_sec, g_random_int() );


		#ifdef FORCE_PACKAGE
		g_string_append_printf(init_config, "package.main=%s\n", FORCE_PACKAGE );
		#endif

		/* Write Config */
		temp_path = g_build_filename( root_dir, ROOT_FILENAME, NULL );
		path = g_strdup( temp_path );// Write Project path to 'path'
		g_file_set_contents( temp_path, init_config->str, -1, &mokoiError );
		Meg_Error_Check( mokoiError, FALSE, (char*)__func__ );
		g_string_free( init_config, TRUE);
		g_free( temp_path );

		gchar * banner_path = g_build_filename( root_dir, "sprites", "__banner.png", NULL);
		g_file_set_contents( banner_path, (const gchar *) default_banner, (gsize)default_banner_size, NULL );
		g_free( banner_path );

	}
	else
	{
		Meg_Error_Print( __func__, __LINE__, "Can't create a new project in '%s'", root_dir);
	}

	g_free( root_dir );

	return path;
}

void printDir(void *data, const char *origdir, const char *fname)
{
	gchar * path = g_strdup_printf("%s/%s", origdir, fname);
	g_print("* [%s] %s [%s]\n", origdir, fname, PHYSFS_getRealDir(path) );

	if ( PHYSFS_isDirectory( path ) )
	{
		Meg_enumerateFilesCallback(path, printDir, NULL);
	}

}

/********************************
* AL_LoadProject
* Check is a project is valid, and loadeds i
@ path: path to project
*/
GKeyFile * AL_LoadConfig( const gchar * file_path )
{
	GKeyFile * project_config = NULL;
	guint array_count = 0;
	gchar * config_text = NULL;
	gchar * key;
	gchar * value;
	gsize config_length = 0;

	if ( g_file_get_contents( file_path, &config_text, &config_length, &mokoiError ) )
	{
		Meg_Error_Check( mokoiError, FALSE, __func__ );
		if ( g_str_has_prefix(config_text, "[Mokoi]") )
		{
			/* Config file found */
			project_config = g_key_file_new();
			g_key_file_load_from_data( project_config, config_text, config_length, G_KEY_FILE_KEEP_TRANSLATIONS, &mokoiError );

			/* Make sure default config */
			if ( g_strv_length( mokoiDefaultConfig ) )
			{
				while ( mokoiDefaultConfig[array_count] != NULL )
				{
					key = mokoiDefaultConfig[array_count];
					value = mokoiDefaultConfig[++array_count];
					if ( !g_key_file_has_key(project_config, "Mokoi", key, NULL) )
					{
						g_key_file_set_string(project_config, "Mokoi", key, value);
					}
					array_count++;
				}
			}
		}
	}
	Meg_Error_Check( mokoiError, FALSE, __func__ );

	g_free( config_text );

	return project_config;
}


/********************************
* AL_CopyProject
* Check is a project is valid, and loaded
@ path: path to project
@ path: path to project
*/
gchar * AL_CopyProject( const gchar * source )
{
	GtkWidget * message, * file_dialog;
	gchar * destination = NULL;
	gchar * new_directory = NULL;


	message = gtk_message_dialog_new( Meg_Main_GetWindow(), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
									  GTK_BUTTONS_CLOSE, "Directory '%s' is not writable.\nPlease select a destination for copy of project.",
									  source );
	gtk_dialog_run( GTK_DIALOG(message) );
	gtk_widget_destroy( message );


	file_dialog = gtk_file_chooser_dialog_new("Select Folder", Meg_Main_GetWindow(),
											  GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
											  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
											  GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
											  NULL);

	if ( gtk_dialog_run( GTK_DIALOG(file_dialog) ) == GTK_RESPONSE_ACCEPT )
	{
		destination = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(file_dialog) );

		if ( Meg_DirectoryCopy(source, destination) )
		{
			new_directory = g_build_path( G_DIR_SEPARATOR_S, destination, ROOT_FILENAME, NULL );
		}
	}
	gtk_widget_destroy(file_dialog);

	g_free(destination);

	return new_directory;

}



/********************************
* AL_LoadProject
* Check is a project is valid, and loaded
@ path: path to project
*/
gchar * AL_LoadProject(const gchar *path )
{
	if ( path == NULL )
		return NULL;

	gchar * config_file_path = NULL;



	if ( g_file_test( path, G_FILE_TEST_IS_DIR ) )
	{
		config_file_path = g_build_path( G_DIR_SEPARATOR_S, path, ROOT_FILENAME, NULL );
	}
	else if ( g_str_has_suffix(path, ROOT_FILENAME) )
	{
		config_file_path = g_strdup( path );
	}


	/* Project is read only, make a copy of the project */
	if ( g_access(config_file_path, W_OK) )
	{
		gchar * directory_name = g_path_get_dirname(config_file_path);

		g_free( config_file_path );// clear the read only path.

		config_file_path = AL_CopyProject( directory_name );

		g_free(directory_name);
	}

	if ( config_file_path == NULL )
		return NULL;

	mokoiBasePath = g_path_get_dirname( config_file_path );
	mokoiConfigTable = AL_LoadConfig( config_file_path );

	if ( mokoiConfigTable )
	{
		AL_CreateDirectories( mokoiBasePath );

		/* Set Up physfs */
		if ( !PHYSFS_mount(mokoiBasePath, NULL, 0) )
		{
			g_warning( "PHYSFS_mount Error %s", PHYSFS_getLastError() );
		}
		if ( !PHYSFS_setWriteDir(mokoiBasePath) )
		{
			g_print("PHYSFS_setWriteDir %s", PHYSFS_getLastError() );
		}

		gchar * package_name = AL_Setting_GetString("package.main");
		if ( package_name )
		{
			gchar * package_location = Meg_Directory_DataFile( "packages", package_name );
			g_print( "Package Load %s\n", package_location );
			if ( !PHYSFS_mount(package_location, NULL, 1) )
			{
				g_warning( "Package Load Error %s", PHYSFS_getLastError() );
			}
			else
			{
				mokoiUsingPackage = TRUE;
			}
			g_free(package_location);
		}

		Project_LoadSpritesheet();

	}

	g_free( config_file_path );


	return mokoiBasePath;
}
/********************************
* AL_PrecheckFiles
*
*/
GAsyncQueue * AL_PrecheckFiles()
{
	GAsyncQueue * queue = NULL;

	#ifdef FORCE_PACKAGE
	gchar * package_location = Meg_Directory_DataFile( "packages", FORCE_PACKAGE );
	g_print( "Package Load %s\n", package_location );



	/* Check if package exist */
	if ( !g_file_test(package_location, G_FILE_TEST_IS_DIR | G_FILE_TEST_IS_REGULAR) )
	{
		queue = Meg_Web_RetrieveQueue(FORCE_PACKAGE_URL, NULL, NULL, package_location, 0 );
		//queue = Meg_Web_RetrieveQueue("https://www.assembla.com/spaces/OpenZelda/documents/ds_ng2QzCr4P_cacwqEsg8/download/ds_ng2QzCr4P_cacwqEsg8", NULL, NULL, package_location, 0 );
	}
	else
	{
		/* Check if package is up to date */
		gchar * content = NULL;
		gchar ** lines_contents = NULL;
		GString * address = g_string_new("http://openzelda.net/package/2_0alpha1.txt");
		gulong crc = 0;
		gulong latest_crc = 0;
		gchar * message = NULL;
		gchar * latest_url = NULL;

		if ( !g_file_test(package_location, G_FILE_TEST_IS_DIR ) )
			content = Meg_Web_RetrieveText( address, NULL, NULL );


		if ( content )
		{
			lines_contents = g_strsplit( content, "\n", 4 );

			if ( g_strv_length(lines_contents) > 2 )
			{

				gchar * latest_crc_string = strpbrk( lines_contents[1], ":" )+1;
				latest_url = strpbrk( lines_contents[2], ":" )+1;
				message = strpbrk( lines_contents[0], ":" )+1;

				if ( latest_crc_string )
				{
					latest_crc = strtoul(latest_crc_string,NULL, 10);
				}
			}

		}




		if ( latest_crc )
		{

			crc = Meg_FileCRC( package_location );
			if ( crc ) // A Directory won't return a value, so we know it's a file.
			{
				if ( crc != latest_crc )
				{
					if ( latest_url != NULL && message != NULL )
					{
						if ( g_str_has_prefix(latest_url, "http") )
						{
							queue = Meg_Web_RetrieveQueue(FORCE_PACKAGE_URL, NULL, NULL, package_location, 0 );
						}
						else
						{
							GtkWidget * dialog;
							dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "%s", message );
							gtk_dialog_add_buttons( GTK_DIALOG(dialog), GTK_STOCK_OK, 1, NULL );
							gtk_dialog_run( GTK_DIALOG(dialog) );
							gtk_widget_destroy( dialog );
						}
					}

				}





			}
		}
		g_strfreev( lines_contents );
		g_free(content);



	}





	g_free(package_location);
	#endif
	return queue;
}



/********************************
* AL_ProjectPackages
* GtkTreeStore: Title (G_TYPE_STRING), base_id (G_TYPE_STRING), tooltip (G_TYPE_STRING), icon (g_TYPE_POINTER)
@tree_view:
*/
void AL_ProjectPackages( GtkTreeView * tree_view )
{
	GtkTreeSelection * selection;
	GtkTreeStore * store;
	GtkTreeIter iter;

	GdkPixbuf * default_pixbuf = gtk_icon_theme_load_icon( gtk_icon_theme_get_default(), ICON_MIMETYPE, 32, 0, NULL);
	gchar * packages_dir = Meg_Directory_Data("packages");


	store = GTK_TREE_STORE(gtk_tree_view_get_model( GTK_TREE_VIEW(tree_view) ));
	selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(tree_view) );

	#ifdef FORCE_PACKAGE
	gtk_tree_store_append( store, &iter, NULL);
	gtk_tree_store_set( store, &iter, 0, "Standard Content", 1, FORCE_PACKAGE, 2, "Use the default content.", 3, default_pixbuf, -1 );
	gtk_tree_selection_select_iter( selection, &iter );

	#else
	const gchar * current_file;
	GDir * current_directory;

	current_directory = g_dir_open( packages_dir, 0, &mokoiError );
	if ( !Meg_Error_Check(mokoiError, FALSE, __func__) )
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
					if ( package_type == 0 )
					{
						gtk_tree_store_append( store, &iter, NULL);
						gtk_tree_store_set( store, &iter, 0, package_name, 1, current_file, 2, package_description, 3, default_pixbuf, -1 );
					}
				}

				g_free(package_path);
				g_free(package_name);
				g_free(package_version);
				g_free(package_description);


			}
			current_file = g_dir_read_name( current_directory );
		}
	}



	#endif

	g_free( packages_dir );
}





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
#include <glib/gstdio.h>


/* Required Headers */
#include "ma_initwin.h"
#include "ma_web.h"

#if defined(__GNUWIN32__) && GTK_MAJOR_VERSION < 3
#include <windows.h>
#include <Shellapi.h>
void meg_windows_url_handler( GtkLinkButton *button, const gchar *link_, gpointer user_data)
{
	ShellExecute(NULL, "open", link_, NULL, NULL, 1);
}
#endif

/* GtkWidget */
GtkWidget * alchera_init_window = NULL;

/* Global Variables */
GKeyFile * meg_pref_storage = NULL;

/* External Functions */
gboolean Meg_Main_Init();
void Entity_RebuildAll(  );
void Funclist_Free( );

/* UI */


/* Functions */

/********************************
* Meg_Load_Wait
*
*/
gboolean Meg_Load_Wait( GAsyncQueue * queue )
{
	gpointer q = g_async_queue_try_pop( queue );
	if ( q == queue )
	{
		g_async_queue_unref( queue );
		Meg_Main_Init();
		return FALSE;
	}
	return TRUE;
}


/********************
* checkDirectoryValues
* debug function to check values
*/
void checkDirectoryValues()
{
	g_print("MEGSHAREDIRECTORY: %s\n", MEGSHAREDIRECTORY);
	g_print("ROOT_FILENAME: %s\n", ROOT_FILENAME);
	g_print("ROOT_FILENAME_EXT: %s\n", ROOT_FILENAME_EXT);
	g_print("ROOT_MIMETYPE: %s\n", ROOT_MIMETYPE);
	g_print("GAME_EXTENSION: %s\n", GAME_EXTENSION);
	g_print("ENGINE_FILENAME: %s\n", ENGINE_FILENAME);
	g_print("PACKAGE_URL: %s\n", PACKAGE_URL);

	g_print("Meg_Directory: %s\n", Meg_Directory());
	g_print("Meg_Directory_ProgramShare: %s\n", Meg_Directory_ProgramShare( "icons" ) );
	g_print("Meg_Directory_GlobalShare: %s\n", Meg_Directory_GlobalShare( "icons" ) );
	g_print("Meg_Directory_Share: %s\n", Meg_Directory_Share( "examples" ));
	g_print("Meg_Directory_Data: %s\n", Meg_Directory_Data(NULL));
	g_print("Meg_Directory_Document: %s\n", Meg_Directory_Document());
	g_print("Meg_Directory_Config: %s\n", Meg_Directory_Config( ));
}


void Meg_Preference_Load( )
{
	gchar * pref_file = NULL;
	gchar * config_directory = NULL;

	config_directory = Meg_Directory_Config();

	/* Load Meg Preferences */
	meg_pref_storage = g_key_file_new();
	pref_file = g_build_filename( config_directory, "settings.ini", NULL);
	if ( !g_key_file_load_from_file( meg_pref_storage, pref_file, 0, NULL) )
	{
		/*
		g_key_file_set_string(meg_pref_storage, "path","texteditor", NULL);
		g_key_file_set_string(meg_pref_storage, "path","imageeditor",NULL);
		g_key_file_set_string(meg_pref_storage, "path","audioeditor", NULL);
		*/
		g_key_file_set_integer(meg_pref_storage, "numeric","gridvalue",8);
		g_key_file_set_integer(meg_pref_storage, "numeric","alignvalue",8);
	}

	g_free(pref_file);
	g_free(config_directory);
}

void Meg_Icon_Update()
{
	gchar * global_icons = NULL;
	gchar * local_icons = NULL;

	global_icons = Meg_Directory_ProgramShare("icons");
	local_icons = Meg_Directory_Share("icons");

	gtk_icon_theme_append_search_path( gtk_icon_theme_get_default(), local_icons );
	gtk_icon_theme_append_search_path( gtk_icon_theme_get_default(), global_icons );

	g_free(global_icons);
	g_free(local_icons);
}

#if defined(__GNUWIN32__) && GTK_MAJOR_VERSION < 3
GtkLinkButtonUriFunc gtk_link_button_set_uri_hook (GtkLinkButtonUriFunc func, gpointer data, GDestroyNotify destroy);
#endif

/********************
* main
* program entry point :)
*/
gint main (gint argc, char *argv[])
{
	GtkSettings * settings;

	/* Set up GTK */
	g_set_application_name( PROGRAM_TITLE );
	gtk_init( &argc, &argv );

	#if GLIB_CHECK_VERSION(2,32,0)
	#else
	g_thread_init( NULL );
	#endif

	#if defined(__GNUWIN32__) && GTK_MAJOR_VERSION < 3
	// Windows Bug fix for link buttons
	gtk_link_button_set_uri_hook( meg_windows_url_handler, NULL, NULL);
	#endif

	/* Get Base Directory, and set up user settings */
	PHYSFS_init( argv[0] );
	Meg_Directory_Set( argv[0] );

	/* Load settings */
	settings = gtk_settings_get_default();
	gtk_settings_set_long_property( settings, "gtk-button-images", TRUE, "main" );
	gtk_settings_set_long_property( settings, "gtk-menu-images", TRUE, "main" );

	Meg_Preference_Load();

	/* Update Icon theme Path */
	Meg_Icon_Update();

	checkDirectoryValues();

	/* Handle arguments */
	gchar * project_path = NULL;
	gint8 project_action = 0;

	if ( argc > 1 )
	{
		while ( argc > 1 )
		{
			--argc;
			if (argv[argc][0] != '-')
			{
				project_path = argv[argc];
			}
			else if ( g_ascii_strcasecmp("--rebuild", argv[argc]) )
			{
				project_action = 2;
			}
			else if ( g_ascii_strcasecmp("--compile", argv[argc]) )
			{
				project_action = 1;
			}
		}
	}

	if ( project_path )
	{
		switch ( project_action )
		{
			case 1:
			{
				g_print("Rebuild Entities\n");
				AL_LoadProject( project_path );
				Entity_RebuildAll( );
				goto program_exit;
				break;
			}
			case 2:
			{
				g_print("Compile Game\n");
				AL_LoadProject( project_path );
				MegProject_Compile();
				goto program_exit;
				break;
			}
			default:
			{
				Meg_Loaders_Init( project_path );
			}
		}
	}

	/* We need to watch a time out queue so not to lock up GUI if we need to download something in precheck */
	GAsyncQueue * queue = AL_PrecheckFiles();

	if ( queue )
	{
		g_timeout_add( 20, (GSourceFunc)Meg_Load_Wait, (gpointer)queue );
	}
	else
	{
		Meg_Main_Init();
	}

	gtk_main();

program_exit:

	Funclist_Free( );
	if ( PHYSFS_isInit() )
	{
		PHYSFS_deinit();
	}

	return 0;
}

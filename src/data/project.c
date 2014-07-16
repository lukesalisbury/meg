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
#include <zlib.h>

/* Required Headers */
#include "game_compiler.h"
#include "loader_functions.h"
#include "logger_functions.h"
#include "maps.h"
#include "package.h"
#include "import_functions.h"
#include "sheets_functions.h"

/* External Functions */
gboolean Setting_Save();

/* Local Type */

/* Global Variables */
extern gchar * mokoiBasePath;
extern GError * mokoiError;
extern GKeyFile * mokoiConfigTable;
extern gchar * mokoiGameDirectories[];

/* Local Variables */
ProgressDialogWidgets mokoiGameComplier;

/* UI */
#include "ui/project_publish.gui.h"
const gchar * mokoiUI_Publish = GUIPROJECT_PUBLISH;


/********************************
* AL_CloseProject
*
@
*/
gboolean AL_CloseProject()
{
	/* Remove packages */
	char ** i = PHYSFS_getSearchPath();
	for ( ; *i != NULL; i++)
	{
		if ( PHYSFS_removeFromSearchPath(*i) == 0 )
		{
			g_print("Failed to remove path: %s\nError: %s\n", *i, PHYSFS_getLastError() );
		}
	}
	PHYSFS_removeFromSearchPath(mokoiBasePath);

	Sheet_Unload();

	g_key_file_free(mokoiConfigTable);

	g_free(mokoiBasePath);
	mokoiBasePath = NULL;

	return TRUE;
}



/********************************
* MegProject_Save
*
*/
gboolean MegProject_Save()
{
	Setting_Save();
	return TRUE;
}

/********************************
* MegProject_Play
*
*/
gboolean MegProject_Play()
{
	if ( !Map_SetStartingPoint() )
	{
		return FALSE;
	}

	gchar * output = NULL, * errors = NULL;
	gchar * working_directory = NULL;
	gint err_code = 0;
	gchar * argv[3];
	gchar ** play_environment_settings = NULL;

	#if GLIB_CHECK_VERSION(2,32,0)
	play_environment_settings = g_get_environ();
	play_environment_settings = g_environ_setenv( play_environment_settings, "SDL_STDIO_REDIRECT", "0", TRUE );
	#else
	//Wrong way, but not much I can do.
	g_setenv( "SDL_STDIO_REDIRECT", "0", TRUE );
	#endif

	#ifdef G_OS_WIN32
		gchar * batch_file = g_build_filename( Meg_Directory(), ENGINE_FILENAME".bat", NULL);

		if ( g_file_test(batch_file, G_FILE_TEST_EXISTS) )
		{
			argv[0] = g_build_filename( Meg_Directory(), ENGINE_FILENAME".bat", NULL);
		}
		else
		{
			argv[0] = g_build_filename( Meg_Directory(), ENGINE_FILENAME".exe", NULL);
		}
		argv[1] = g_strconcat( "'", g_strescape( mokoiBasePath, "" ), "\\\\", "'", NULL);

		g_free(batch_file);
	#else
		argv[0] = g_build_filename( Meg_Directory(), ENGINE_FILENAME, NULL);
		argv[1] = g_strconcat( "'", mokoiBasePath, G_DIR_SEPARATOR_S, "'", NULL);
	#endif
	argv[2] = NULL;

	#ifdef apple
	working_directory = Meg_Directory();
	#endif
	Meg_Log_Print( LOG_NONE, "Running %s %s", argv[0], argv[1]);
    g_spawn_sync( working_directory, argv, play_environment_settings, G_SPAWN_SEARCH_PATH, NULL, NULL, &output, &errors, &err_code, &mokoiError);


	if ( mokoiError )
	{
		Meg_Error_Print( __func__, __LINE__, "Can't spawn "ENGINE_FILENAME": %s",  mokoiError->message);
		g_clear_error(&mokoiError);
		mokoiError = NULL;
	}

	if ( g_utf8_strlen(errors, 4) )
	{
		Meg_Log_Print( LOG_NONE, "-------------------");
		Meg_Log_Print( LOG_BOLD, "Errors:\n%s", errors);
		Meg_Log_Print( LOG_NONE, "-------------------");
	}
	if ( g_utf8_strlen(output, 4) )
	{
		Meg_Log_Print( LOG_NONE, "-------------------");
		Meg_Log_Print( LOG_NONE, "output:\n%s", output);
		Meg_Log_Print( LOG_NONE, "-------------------");
	}

	g_free(argv[0]);
	g_free(argv[1]);
	g_free(working_directory);
	#if GLIB_CHECK_VERSION(2,32,0)
	g_strfreev(play_environment_settings);
	#else
	//Wrong way, but not not much I can do.
	g_unsetenv( "SDL_STDIO_REDIRECT" );

	#endif


	return TRUE;
}

/********************************
* MegProject_Title
*
*
*/
gchar * MegProject_Title()
{
	gchar * title = AL_Setting_GetString( "project.title" );
	if ( title )
		return title;
	else
		return "Untitled";
}

/********************************
* AL_ResourcePath
*
*
*/
gchar * AL_ResourcePath( gchar * file, gchar * directory )
{
	/* TODO: Check if file is in a package */
	gchar * p = g_build_filename( AL_ProjectPath( ), directory, file, NULL);

	return p;
}


/********************************
* Project_Publish
*
*
*/
void Project_Publish( GtkButton *button, gpointer user_data)
{
	GtkWidget * dialog;

	/* UI */
	GError * error = NULL;
	GtkBuilder * ui = gtk_builder_new();
	if ( !gtk_builder_add_from_string(ui, mokoiUI_Publish, -1, &error) )
	{
		Meg_Error_Print( __func__, __LINE__, "UI creation error '%s'.", error->message );
		return;
	}
	gtk_builder_connect_signals( ui, NULL );

	dialog = GET_WIDGET( ui, "publish_dialog" );

	gtk_widget_show_all( dialog );

}

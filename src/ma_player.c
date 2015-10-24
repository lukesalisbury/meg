/****************************
Copyright © 2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Global Header */
#include "global.h"
#include <glib.h>
#include "ma_player.h"
/* Required Headers */
#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>  // for GDK_WINDOW_XID
#endif
#ifdef GDK_WINDOWING_WIN32
#include <gdk/gdkwin32.h>  // for GDK_WINDOW_HWND
#endif



/* GtkWidget */


/* Global Variables */
extern gchar * project_file_path;
extern PlayerInfo player_info;
GtkWidget * area_player;

/* External Functions */


/* UI */


/* Function */

/********************************
* Alchera_Map_Init
* Initialises Map + Overview Page
*/
void MegWidget_Player_Create()
{

}


/********************************
* MegWidget_Map_Refresh
*
*/
void MegWidget_Player_Init()
{

}

/********************************
* MegWidget_Map_Refresh
*
*/
void MegWidget_Player_Close()
{

}


/* Events */

/********************************
* MegWidget_Map_Refresh
*
*/
void MegWidget_Player_Play( GtkWidget *button, PlayerInfo * player_info )
{
	#ifdef GDK_WINDOWING_WIN32
	gdk_window_ensure_native( gtk_widget_get_window(player_info->widget) );
	gtk_widget_set_double_buffered(player_info->widget, FALSE);

	player_info->winId = gdk_win32_drawable_get_handle(gtk_widget_get_window(player_info->widget));


	if ( player_info->state == NOTRUNNING )
	{
		gchar * working_directory = NULL;
		gchar * argv[4];
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
			argv[1] = g_strconcat( "'", g_strescape( project_file_path, "" ), "\\\\", "'", NULL);

			g_free(batch_file);
		#else
			argv[0] = g_build_filename( Meg_Directory(), ENGINE_FILENAME, NULL);
			argv[1] = g_strconcat( "'", project_file_path, G_DIR_SEPARATOR_S, "'", NULL);
		#endif
		argv[2] = g_strdup_printf("--windowid=%d", (gint)player_info->winId);
		argv[3] = NULL;

		#ifdef apple
		working_directory = Meg_Directory();
		#endif

		Meg_Log_Print( LOG_NONE, "Running %s %s %s", argv[0], argv[1], argv[2]);
		//g_spawn_sync( working_directory, argv, play_environment_settings, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL, NULL, NULL);
		g_spawn_async( working_directory, argv, play_environment_settings, G_SPAWN_SEARCH_PATH, NULL, NULL, &player_info->pid, NULL);

		g_free(argv[0]);
		g_free(argv[1]);
		g_free(argv[2]);
		g_free(working_directory);

		#if GLIB_CHECK_VERSION(2,32,0)
		g_strfreev(play_environment_settings);
		#else
		//Wrong way, but not not much I can do.
		g_unsetenv( "SDL_STDIO_REDIRECT" );
		#endif

		player_info->state = RUNNING;
	}
	else
	{
		g_spawn_close_pid(player_info->pid);
		player_info->state = NOTRUNNING;
	}
#endif


}
void event_handler_func(GdkEvent * event, gpointer user_data);
void player_event_handler_func(GdkEvent * event, gpointer user_data)
{
	g_print(":.");
	//gtk_main_do_event(event);
}

void MegWidget_Player_Pause(GtkWidget *button, PlayerInfo *player_info)
{
	gtk_widget_grab_focus( player_info->widget );
	gtk_widget_grab_default( player_info->widget );
	/*
	gdk_event_handler_set(player_event_handler_func, NULL, NULL);

	#ifdef G_OS_WIN32
	SendMessage( player_info->winId, WM_KEYDOWN, VK_F4, 0x014F0000 );
	SendMessage( player_info->winId, WM_KEYUP, VK_F4, 0x014F0001 );
	PostMessage( player_info->winId, WM_KEYDOWN, VK_RIGHT, 0x014F0000 );
	PostMessage( player_info->winId, WM_KEYUP, VK_RIGHT, 0x014F0001 );

	PostMessage( player_info->winId, WM_KEYDOWN, VK_F4, 0x014F0000 );
	PostMessage( player_info->winId, WM_KEYUP, VK_F4, 0x014F0001 );
	#endif

	gdk_event_handler_set(event_handler_func, NULL, NULL);
	*/
}

void MegWidget_Player_Refresh( GtkWidget *button, PlayerInfo *player_info )
{
#ifdef GDK_WINDOWING_WIN32

	GdkEvent * event = gdk_event_new(GDK_CLIENT_EVENT);

	event->client.data_format = 32;
	event->client.data.s[0] = 42;
	gdk_event_send_client_message(event, player_info->winId);
#endif
}

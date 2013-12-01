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

/* Required Headers */

/* External Functions */

/* Local Type */
int (*sdl_playback_play)( gchar * file, void (*music_finished)() );
void (*sdl_playback_quit)();
void (*sdl_playback_init)();

/* Global Variables */


/* Local Variables */
GModule * sdlplayback = NULL;
GtkTreeModel * audioActiveModel = NULL;
GtkTreeIter audioActiveIter;

/* UI */



/********************************
* Audio_Payback_Register
*
*/
void Audio_Payback_Register()
{

	gchar * module_path = g_module_build_path(NULL, "megaudio");

	sdlplayback = g_module_open(module_path, G_MODULE_BIND_LAZY);
	if ( sdlplayback )
	{
		g_module_symbol( sdlplayback, "sdl_playback_play",(gpointer *)&sdl_playback_play);
		g_module_symbol( sdlplayback, "sdl_playback_quit",(gpointer *)&sdl_playback_quit);
		g_module_symbol( sdlplayback, "sdl_playback_init",(gpointer *)&sdl_playback_init);

		if ( sdl_playback_init )
			sdl_playback_init();
	}
	else
		g_warning( "GModule: %s\n", g_module_error() );
}

/********************************
* Audio_Payback_Unregister
*
*/
void Audio_Payback_Unregister()
{
	if ( sdl_playback_quit )
		sdl_playback_quit();
	if ( !g_module_close(sdlplayback) )
		g_warning( "GModule: %s\n", g_module_error() );
}


/********************************
* Audio_Music_Finished
*
*/
void Audio_Music_Finished()
{
	if ( audioActiveModel )
		gtk_tree_store_set( GTK_TREE_STORE(audioActiveModel), &audioActiveIter, 4, GTK_STOCK_MEDIA_PLAY, -1 );
}


/********************************
* Audio_Music_Play
*
@ model:
@ iter:
*/
gboolean Audio_Music_Play( gchar * file, GtkTreeModel * model, GtkTreeIter iter )
{
	gboolean results = FALSE;
	if ( sdl_playback_play )
	{
		/* Check if External Editor can be used */
		gchar * full_path = Meg_file_get_path(file);

		if ( full_path )
		{
			Audio_Music_Finished();// If audio file already playing, change icon to media stop
			if ( sdl_playback_play(full_path, Audio_Music_Finished) )
			{
				gtk_tree_store_set( GTK_TREE_STORE(model), &iter, 4, GTK_STOCK_MEDIA_STOP, -1 );
				audioActiveModel = model;
				audioActiveIter = iter;
				results = TRUE;
			}
			g_free( full_path );
		}
		else
		{

		}

	}
	else
	{
		GtkWidget * dialog = gtk_message_dialog_new( Meg_Main_GetWindow(),GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Music playback support not installed");
		gtk_dialog_run( GTK_DIALOG(dialog) );
		gtk_widget_destroy( dialog );
	}
	return results;
}

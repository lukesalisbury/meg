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
#include "ma_initwin.h"
#include "ma_events.h"

/* Global Variables */
extern GtkWidget * alchera_init_window;
GtkWidget * alchera_current_parent_window = NULL;

/* External Functions */
gboolean Meg_Main_Init();

/* UI */
#include "ui/init.gui.h"
const gchar * alchera_init_ui = GUIINIT

/********************************
* Meg_Event_OpenByButton
*
*/
void Meg_Event_OpenByButton( GtkButton * button, gchar * path )
{
	if ( Meg_Loaders_Init( path ) )
	{
		gtk_widget_destroy( alchera_init_window );
	}

}

/********************************
* Meg_Event_OpenRecent
*
*/
void Meg_Event_OpenRecent( GtkRecentChooser * chooser, gpointer user_data )
{
	gchar * uri;
	gchar * filename;

	uri = gtk_recent_chooser_get_current_uri( chooser );
	filename = g_filename_from_uri( uri, NULL, NULL );
	if ( Meg_Loaders_Init( filename ) )
	{
		gtk_widget_destroy(alchera_init_window);
	}
	g_free( filename );
	g_free( uri );

}

/********************************
* Meg_Project_Open
*
*/
void Meg_Project_Open()
{
	/* Create initial dialog windows */
	GError * error = NULL;
	GtkWidget * chooser_recent = NULL, * box_example, * button_open_game, * button_new_game;
	GtkRecentFilter * filter_recent = gtk_recent_filter_new();

	GtkBuilder * ui = gtk_builder_new();
	if ( !gtk_builder_add_from_string(ui, alchera_init_ui, -1, &error) )
	{
		Meg_Error_Print( __func__, __LINE__, "UI creation error: %s", alchera_init_ui );
		g_clear_error( &error );
		return;
	}

	alchera_init_window = GET_WIDGET( ui, "alchera-init-window" );
	chooser_recent = GET_WIDGET( ui, "alchera-recent-chooser" );
	box_example = GET_WIDGET( ui, "alchera-examples-box" );
	button_open_game = GET_WIDGET( ui, "alchera-game-open" );
	button_new_game = GET_WIDGET( ui, "alchera-game-new" );

	/* Add Signal to Widget */
	g_signal_connect( chooser_recent, "item-activated", G_CALLBACK(Meg_Event_OpenRecent), NULL);
	g_signal_connect( button_new_game, "clicked", G_CALLBACK(Meg_Event_NewProjectDialog), NULL);
	g_signal_connect( button_open_game, "clicked", G_CALLBACK(Meg_Event_OpenProjectDialog), NULL);

	/* Setup recent project window */
	gtk_recent_filter_add_pattern( filter_recent, ROOT_FILENAME);
	gtk_recent_filter_add_mime_type( filter_recent, ROOT_MIMETYPE );
	gtk_recent_filter_add_mime_type( filter_recent, "application/x-ext-"ROOT_FILENAME_EXT );

	gtk_recent_chooser_set_filter( GTK_RECENT_CHOOSER(chooser_recent), filter_recent );

	/* Examples */
	gchar * demo_dir = Meg_Directory_Share( "examples" );
	GDir * demo_directory = g_dir_open( demo_dir, 0, &error );

	if ( error )
	{
		g_clear_error( &error );
	}
	else if ( demo_directory )
	{
		const gchar * current_file = g_dir_read_name( demo_directory );
		while ( current_file != NULL )
		{
			gchar * current_dir = g_build_path( G_DIR_SEPARATOR_S, demo_dir, current_file, NULL);
			if ( g_file_test(current_dir, G_FILE_TEST_IS_DIR) )
			{
				gchar * demo_dir = g_build_filename( current_dir, ROOT_FILENAME, NULL);
				if ( g_file_test(demo_dir, G_FILE_TEST_EXISTS) )
				{
					GtkWidget * button_example = gtk_button_new_with_label(current_file);
					g_signal_connect( button_example, "clicked", G_CALLBACK(Meg_Event_OpenByButton), g_strdup(demo_dir) );
					gtk_box_pack_start( GTK_BOX(box_example), button_example, FALSE, TRUE, 2 );
				}
				g_free(demo_dir);
			}
			g_free(current_dir);
			current_file = g_dir_read_name( demo_directory );
		}
		g_dir_close( demo_directory );
	}



	gtk_widget_show_all(Meg_Main_GetWidget());

	gtk_window_set_transient_for( GTK_WINDOW(alchera_init_window), Meg_Main_GetWindow() );
	gtk_widget_show_all(alchera_init_window);
	gtk_dialog_run( GTK_DIALOG(alchera_init_window) );

	gtk_widget_destroy(alchera_init_window);

}


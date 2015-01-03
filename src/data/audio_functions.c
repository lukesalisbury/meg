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
#include "loader_functions.h"

/* Required Headers */
#include <glib/gstdio.h>
#include "audio_functions.h"
#include "audio_playback.h"

/* External Functions */

/* Local Type */

/* Global Variables */
extern gchar * mokoiBasePath;

/* Local Variables */

/* UI */

const gchar * mokoiUI_AudioCustomWidget = GUI_AUDIO_CUSTOM_WIDGET;


/********************************
* Audio_Listing_Update
*
@ widget:
*/
void Audio_Listing_Enumerate(void * data, const char *origdir, const char *fname)
{
	GtkTreeIter children;
	gchar * path = g_strdup_printf("%s/%s", origdir, fname);
	GtkTreeStore * list = (GtkTreeStore *)data;
	if ( !PHYSFS_isDirectory( path ) )
	{
		gtk_tree_store_append( list, &children, NULL);
		/* meg_audio_store: displayname, dir, preload, filename, image */

		gtk_tree_store_set( list, &children, 0, g_strdup( fname ), 1, g_strdup( origdir ), 2, 1, 3, g_strdup( path ), 4, GTK_STOCK_MEDIA_PLAY, -1 );
	}
	g_free(path);
}

/********************************
* Audio_Listing_Update
*
@ widget:
*/
void Audio_Listing_Update( GtkTreeView * widget )
{
	if ( !Project_ValidDir() || !widget )
		return;

	GtkTreeStore * list = GTK_TREE_STORE(gtk_tree_view_get_model(widget));

	/* Clear list before adding items */
	gtk_tree_store_clear( list );

	Meg_enumerateFilesCallback( "/music/", Audio_Listing_Enumerate, list );
	Meg_enumerateFilesCallback( "/soundfx/", Audio_Listing_Enumerate, list );

}

/********************************
* Audio_Listing_Refresh
*
@ treeview:
@ button:
*/
void Audio_Listing_Refresh(  GtkButton * button, GtkTreeView * treeview )
{
	Audio_Listing_Update( treeview );
}


/********************************
* Audio_Page_Save
* Create Audio page.
*/
void Meg_Audio_Save( gboolean force )
{

}


/********************************
* Audio_File_Selected
* Plays currently selected Audio File.
@ tree_view:
@ path:
@ column:
@ user_data:

*/
void Audio_File_Selected( GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data )
{
	GtkTreeIter iter;
	GtkTreeModel * model;
	GtkTreeSelection * select;
	gchar * file;

	select = gtk_tree_view_get_selection( tree_view );
	if ( gtk_tree_selection_get_selected( select, &model, &iter ) )
	{
		gtk_tree_model_get( model, &iter, 3, &file, -1 );
		Audio_Music_Play( file, model, iter );
	}
}


/********************************
* Audio_File_Add
* Opens File Chooser.
@ button:
@ treeview:
*/
void Audio_File_Add( GtkButton * button, GtkTreeView * treeview )
{
	GtkWidget * dialog = gtk_file_chooser_dialog_new( "Select Audio File", Meg_Main_GetWindow(), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL );
	GtkWidget * widget, * check = NULL;


	/* Add Custom Widget to the filer chooser dialog, allows user to preview file before adding it to projects */
	GError * error = NULL;
	GtkBuilder * ui = gtk_builder_new();
	if ( gtk_builder_add_from_string(ui, mokoiUI_AudioCustomWidget, -1, &error) )
	{
		widget = GET_WIDGET( ui, "box1" );
		check = GET_WIDGET( ui, "music" );
		/**/
		gtk_widget_show_all( widget );
		gtk_file_chooser_set_extra_widget( GTK_FILE_CHOOSER(dialog), widget );

		g_signal_connect( gtk_builder_get_object( ui, "play" ), "clicked", G_CALLBACK(Audio_Chooser_Play), dialog );
	}
	else
	{
		Meg_Error_Print( __func__, __LINE__, "UI creation error '%s'.", error->message );
		return;
	}

	Audio_Chooser_AddFilter( dialog, "Mod Music", "*.it;*.s3m;*.xm");
	Audio_Chooser_AddFilter( dialog, "Ogg Audio", "*.ogg;*.oga");
	Audio_Chooser_AddFilter( dialog, "Wave", "*.wav" );
	Audio_Chooser_AddFilter( dialog, "All Files", "*.*" );

	if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
	{
		gchar * dest, * file;
		gboolean music = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check) );

		file = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER (dialog) );
		dest = g_build_filename( mokoiBasePath, (music ? "music" : "soundfx"), g_path_get_basename(file), NULL );

		if ( Meg_FileCopy( file, dest ) )
		{
			Audio_Listing_Refresh( NULL, treeview );
		}
		else
		{
			Meg_Log_Print( LOG_ERROR, "Meg_FileCopy('%s', '%s') Error", file, dest );
		}
		g_free( dest );
		g_free( file );
	}
	gtk_widget_destroy( dialog );
}

/********************************
* Audio_File_Add
* Opens File Chooser.
@ button:
@ treeview:
*/
gboolean Audio_File_Import( gchar * source_file )
{
	gchar * dest_file = NULL;
	gchar * file_name = NULL;
	gboolean is_music = TRUE;
	gboolean success = TRUE;

	file_name = g_path_get_basename( source_file );

	if ( g_str_has_suffix(file_name, ".wav") )
	{
		is_music = FALSE;
	}
	else if ( g_str_has_suffix(file_name, ".ogg") || g_str_has_suffix(file_name, ".oga") )
	{
		is_music = FALSE;
	}

	dest_file = g_strdup_printf( "/%s/%s", (is_music ? "music" : "soundfx"), file_name );

	success = Meg_FileImport( source_file, dest_file );


	g_free(dest_file);
	g_free(file_name);


	return success;
}

/********************************
* Audio_File_Remove
* Removes currently selected
@ button:
@ treeview:
*/
void Audio_File_Remove( GtkButton * button, GtkTreeView * treeview )
{
	GtkTreeIter iter;
	GtkTreeModel * model;
	GtkTreeSelection * select;
	gchar * oldfile, * newfile;

	select = gtk_tree_view_get_selection(treeview);
	if ( gtk_tree_selection_get_selected(select, &model, &iter) )
	{
		gtk_tree_model_get( model, &iter, 3, &oldfile, -1);
		newfile = g_strconcat( oldfile, "-remove", NULL );
		g_rename( oldfile, newfile );

		Audio_Listing_Refresh( NULL, treeview );
		g_free( newfile );
		g_free( oldfile );
	}

}




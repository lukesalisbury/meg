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

/* Required Headers */
#include <glib/gstdio.h>
#include "logger_functions.h"
#include "game_compiler.h"
#include "loader_functions.h"
#include "compression_functions.h"
#include "progress_dialog.h"
#include "maps.h"

void Entity_RebuildDirectory( ProgressDialogWidgets * wids, const gchar * directory  );

/* Global Variables */
GtkWidget * mokoiCompileProgressBar;

/* Local Variables */
ProgressDialogWidgets mokoiGameComplier;
gboolean mokoiCompileStripEntities = FALSE;
gboolean mokoiCompileStripColours = FALSE;
gboolean mokoiCompileLocalFiles = FALSE;
/*
gboolean mokoiCompileFinished = FALSE;
gboolean mokoiCompileError = FALSE;
*/

/* UI */
#include "ui/compilecreate_dialog.gui.h"
const gchar * mokoiUI_CompileCreate = GUICOMPILECREATE_DIALOG

/********************************
* GameCompiler_Thread
*
*/
gpointer GameCompiler_Thread( ProgressDialogWidgets *wids )
{
	Logger_SetQueue( wids->queue ); // redirects a Logger_* to pass messages

	GSList * file_list = NULL;

	/* Title and ID */
	gchar * name = (gchar *)g_strnfill(256, 0);
	guint32 id = 0;

	/* PNG */
	guint32 logo_len = 0;
	gchar * logo_content;

	/* Title and ID */
	g_snprintf( (gchar *)name, 255, "%s", AL_Title() );
	id = AL_SettingNumber( "project.id" );

	/* Check for Logo */
	if ( Meg_file_test("/sprites/__icon.png", (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR) ) )
	{
		if ( Meg_file_get_contents("/sprites/__icon.png", &logo_content, (gsize*)&logo_len, NULL) )
		{
			Logger_FormattedLog( NULL, LOG_FINE, "Including Game Icon\n");
		}
	}


	/* Start Packing Files */
	Compression_PackFile( &file_list, "game.mokoi", ROOT_FILENAME, mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"preload", ".txt", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"lang", ".txt", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"dialog", ".txt", mokoiCompileLocalFiles );


	Entity_RebuildDirectory(wids, "/scripts/maps/");
	Compression_PackFolder( &file_list,"c/scripts", ".amx", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"c/scripts/maps", ".amx", mokoiCompileLocalFiles );
	if ( !mokoiCompileStripEntities )
	{
		Compression_PackFolder( &file_list,"c/scripts", ".amx64", mokoiCompileLocalFiles );
		Compression_PackFolder( &file_list,"c/scripts/maps", ".amx64", mokoiCompileLocalFiles );
	}

	Compression_PackFolder( &file_list,"scripts", ".managed", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"scripts", ".js", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"scripts", ".sq", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"scripts", ".lua", mokoiCompileLocalFiles );

	Compression_PackFolder( &file_list,"scripts/maps", ".js", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"scripts/maps", ".sq", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"scripts/maps", ".managed", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"scripts/maps", ".lua", mokoiCompileLocalFiles );

	Compression_PackFolder( &file_list,"dialog", ".ogg", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"dialog", ".opus", mokoiCompileLocalFiles );

	Compression_PackFolder( &file_list,"maps", ".xml", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"maps", ".entities", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"sprites", ".png.xml", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"sprites", ".png", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"sections", ".txt", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"masks", ".xpm", mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"soundfx", NULL, mokoiCompileLocalFiles );
	Compression_PackFolder( &file_list,"music", NULL, mokoiCompileLocalFiles );

	/* Save package file */
	MokoiPackage_SaveGame( wids->filename, file_list, name, id, logo_content, logo_len);
	Logger_FormattedLog(NULL, LOG_BOLD, "Game saved to %s.\n", wids->filename);

	Logger_SetQueue( NULL );  // resets logger
	g_async_queue_push( wids->queue, g_thread_self() ); // closes thread
	return NULL;
}


/********************************
* GameCompiler_Run
*
@ file_name:
@ strip_entities:
@ strip_colours:
-
*/
gboolean GameCompiler_Run( const gchar * file_name, gboolean strip_entities, gboolean strip_colours, gboolean local_files  )
{
	ProgressDialog_Clear( &mokoiGameComplier );
	if ( ProgressDialog_Create( &mokoiGameComplier, "Compiling Game", (gchar *)file_name ) )
	{
		mokoiCompileStripEntities = strip_entities;
		mokoiCompileStripColours = strip_colours;
		mokoiCompileLocalFiles = local_files;
		ProgressDialog_Activate( &mokoiGameComplier, (GThreadFunc)GameCompiler_Thread, (GSourceFunc)ProgressDialog_Watch );
		return TRUE;
	}
	return FALSE;
}

/********************************
* Game_Compile_FileChooser
*
*/
gboolean GameCompiler_FileChooser( GtkWidget * button, GdkEventButton * event, GtkWidget * label )
{
	if ( event->type == GDK_BUTTON_PRESS && event->button == 1 )
	{
		GtkFileFilter * filter = gtk_file_filter_new();
		const gchar * file_name = gtk_label_get_text( GTK_LABEL(label) );
		GtkWidget * dialog = gtk_file_chooser_dialog_new ("Save File",
									  Meg_Misc_ParentWindow(button),
									  GTK_FILE_CHOOSER_ACTION_SAVE,
									  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
									  GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
									  NULL);



		gtk_file_filter_add_pattern( filter, "*."GAME_EXTENSION );
		gtk_file_filter_set_name( filter, "Game file" );

		gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(dialog), filter );

		gtk_file_chooser_set_current_folder_uri( GTK_FILE_CHOOSER(dialog), Meg_Directory_Document() );
		gtk_file_chooser_set_do_overwrite_confirmation( GTK_FILE_CHOOSER(dialog), TRUE);
		gtk_file_chooser_set_filename( GTK_FILE_CHOOSER(dialog), file_name );
		gtk_file_chooser_set_current_name( GTK_FILE_CHOOSER(dialog), g_path_get_basename(file_name) );

		gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Misc_ParentWindow(button) );

		if (gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT)
		{
			gchar * new_file_name = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) );
			gtk_label_set_text( GTK_LABEL(label), new_file_name );
			g_free( new_file_name );
		}
		gtk_widget_destroy( dialog );



	}
	return FALSE;

}


/********************************
* AL_Compile
* Compiles project for distro
*/
gboolean AL_Compile()
{
	if ( !Map_SetStartingPoint() )
	{
		return FALSE;
	}

	gchar * title, * file_name, * file_path, * package_name ;
	GtkWidget * dialog, * button_chooser, * label_file, * check_rgba5551, * check_strip64, * tree_distrib, * check_localcontent;
	GObject * check_distrib;
	GtkListStore * store_distrib;

	title = Project_CleanTitle( NULL );
	file_name = g_strdup_printf( "%s.%s", title, GAME_EXTENSION );
	file_path = g_build_filename( Meg_Directory_Document(), file_name, NULL );
	package_name = AL_SettingString("package.main");

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(mokoiUI_CompileCreate, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );

	/* Widget */
	dialog = GET_WIDGET( ui, "dialog");
	label_file = GET_WIDGET( ui, "label_file");
	button_chooser = GET_WIDGET( ui, "button_chooser");
	check_rgba5551 = GET_WIDGET( ui, "check_rgba5551");
	check_strip64 = GET_WIDGET( ui, "check_strip64");
	check_localcontent = GET_WIDGET( ui, "check_localcontent");
	tree_distrib = GET_WIDGET( ui, "tree_distrib");
	check_distrib = gtk_builder_get_object( ui, "check_distrib" );
	store_distrib = GET_LISTSTORE( ui, "store_distrib");

	/* Signal */
	SET_OBJECT_SIGNAL( ui, "celltoggle_selected", "toggled", G_CALLBACK(CellRender_Event_FixToggled), store_distrib );
	g_signal_connect( check_distrib, "toggled", G_CALLBACK(ToggleButton_Event_WidgetSensitive), tree_distrib );
	g_signal_connect( button_chooser, "button-press-event", G_CALLBACK(GameCompiler_FileChooser), label_file );

	ToggleButton_Event_WidgetSensitive( GTK_TOGGLE_BUTTON(check_distrib), tree_distrib );

	/* Setting */
	gtk_label_set_text( GTK_LABEL(label_file), file_path );



	if ( package_name )
	{
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(check_localcontent), TRUE );
		gtk_widget_set_sensitive(check_localcontent, TRUE);
	}
	else
	{
		gtk_widget_set_sensitive(check_localcontent, FALSE);
	}


	/* Display Dialog and get results */
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Main_GetWindow() );
	gint response = gtk_dialog_run( GTK_DIALOG(dialog) );

	const gchar * output_file_name = gtk_label_get_text( GTK_LABEL(label_file) );
	gboolean strip_entities = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_rgba5551) );
	gboolean strip_colours = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_strip64) );
	gboolean local_files_only = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_localcontent) );

	gtk_widget_destroy( dialog );


	if ( response == 1 )
	{
		GameCompiler_Run( output_file_name, strip_entities, strip_colours, local_files_only );
	}


	g_free(file_path);
	g_free(file_name);
	g_free(title);
	g_free( package_name );
	return TRUE;
}









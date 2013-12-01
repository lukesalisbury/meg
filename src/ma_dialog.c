/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Global Header */
#include "global.h"

/* Required Headers */
#include "data/import_functions.h"


/* GtkWidget */
extern GtkWidget * alchera_init_window;
extern GKeyFile * meg_pref_storage;

/* Global Variables */

/* External Functions */
void Meg_Preference_ClearFile( GtkButton * button, GtkFileChooser * widget );
void Patch_FileChanged( GtkFileChooserButton * widget, gpointer user_data);
void Patch_Create( gchar * file, GtkListStore * store );

/* UI */
#include "ui/project_new.gui.h"
#include "ui/pref_dialog.gui.h"
#include "ui/import_dialog.gui.h"
#include "ui/patch_page.gui.h"

const gchar * Ui_ImportDialog = GUIIMPORT_DIALOG
const gchar * Ui_ProjectNew = GUIPROJECT_NEW
const gchar * Ui_PrefDialog = GUIPREF_DIALOG
const gchar * Ui_PatchPage = GUIPATCH_PAGE

#ifdef CUSTOMSETTINGS
#include "ui/about_window_custom.gui.h"
const gchar * Ui_AboutWindow = GUIABOUT_WINDOW_CUSTOM
#else
#include "ui/about_window.gui.h"
const gchar * Ui_AboutWindow = GUIABOUT_WINDOW
#endif

/* Functions */

/********************************
* Meg_Dialog_About

*/
gboolean Meg_Dialog_About( )
{
	GtkWidget * dialog = NULL;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(Ui_AboutWindow, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );

	/* Widget */
	dialog = GET_WIDGET( ui, "alchera_about_dialog" );

	/* Signal */
	g_signal_connect( dialog, "close", G_CALLBACK(gtk_widget_destroy), NULL );
	g_signal_connect( dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL );

	/* */
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Main_GetWindow() );
	gtk_dialog_run( GTK_DIALOG(dialog) );
	gtk_widget_destroy( dialog );
	return TRUE;
}

/********************************
* Meg_Dialog_NewProject
*
* Return true if project is created and loaded.
*/
gboolean Meg_Dialog_NewProject( )
{
	gint response = 0;
	const gchar * title = NULL;
	gboolean success = FALSE;
	GtkWidget * dialog, * entry_title;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(Ui_ProjectNew, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );

	/* Widget */
	dialog = GET_WIDGET( ui, "dialog" );
	entry_title = GET_WIDGET( ui, "entry_title" );


	/* Signal */

	/* Get List of content packages*/
	//AL_ProjectPackages( GTK_TREE_VIEW(tree_package) );


	/* Run dialog */
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Main_GetWindow() );

	while(TRUE) /* Continue showing the dialog, until valid title or cancelled */
	{
		response = gtk_dialog_run( GTK_DIALOG(dialog) );
		title = gtk_entry_get_text( GTK_ENTRY(entry_title) );
		if ( !response || response == GTK_RESPONSE_DELETE_EVENT || response == GTK_RESPONSE_REJECT )
			break;
		if ( g_utf8_strlen(title, -1) )
		{
			break;
		}
		else
			Meg_Error_Print( __func__, __LINE__, "Must give the project a name");
	}

	if ( response == GTK_RESPONSE_ACCEPT )
	{
#ifdef FORCE_PACKAGE
		gchar * base = g_strdup(FORCE_PACKAGE);
#else
		gchar * base = NULL;
#endif
		/*
		GtkTreeIter iter;
		GtkTreeModel * model;
		GtkTreeSelection * selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(tree_package) );
		if ( gtk_tree_selection_get_selected( selection, &model, &iter ) )
		{
			gtk_tree_model_get( model, &iter, 1, &base, -1 );
		}
		*/

		if ( Alchera_Loaders_CreateNew( title, base ) )
		{
			success = TRUE;
		}
		else
		{
			Meg_Error_Print( __func__, __LINE__, "Project '%s' can't be created.", title );
		}
		g_free( base );
	}
	gtk_widget_destroy( dialog );

	return success;
}


/********************************
* Meg_Dialog_OpenProject
* Event:
* Result:
*/
gboolean Meg_Dialog_OpenProject( )
{
	GtkFileFilter * open_project_filter = gtk_file_filter_new();
	gboolean success = FALSE;
	GtkWidget * file_chooser = gtk_file_chooser_dialog_new( "Open Project",
		GTK_WINDOW(alchera_init_window),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
		NULL);

	gtk_window_set_transient_for( GTK_WINDOW(file_chooser), Meg_Main_GetWindow() );

	gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER(file_chooser), Meg_Directory_Document() );
	gtk_file_chooser_add_shortcut_folder( GTK_FILE_CHOOSER(file_chooser), Meg_Directory_Share("examples"), NULL );
	gtk_file_chooser_add_shortcut_folder( GTK_FILE_CHOOSER(file_chooser), Meg_Directory_Document(), NULL );

	/* File Filter */
	gtk_file_filter_add_pattern(open_project_filter, ROOT_FILENAME);
	gtk_file_filter_add_mime_type(open_project_filter, ROOT_MIMETYPE);
	gtk_file_chooser_set_filter( GTK_FILE_CHOOSER(file_chooser), open_project_filter );

	if ( gtk_dialog_run( GTK_DIALOG(file_chooser) ) == GTK_RESPONSE_ACCEPT )
	{
		gchar * filename;
		filename = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(file_chooser) );
		if ( Alchera_Loaders_Init(filename) )
		{
			success = TRUE;
		}
		g_free( filename );
	}
	gtk_widget_destroy(file_chooser);

	return success;
}

/********************************
* Meg_Dialog_Preference
* Event:
* Result:
*/
gboolean Meg_Dialog_Preference()
{
	GtkWidget * dialog = NULL;
	GtkWidget * spin_gridvalue, * file_texteditor, * file_imageeditor, * button_textclear, * button_imageclear;

	gchar * current_image_editor_path = g_key_file_get_string(meg_pref_storage, "path", "imageeditor", NULL);
	gchar * current_text_editor_path = g_key_file_get_string(meg_pref_storage, "path", "texteditor", NULL);



	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(Ui_PrefDialog, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );

	/* Widget */
	dialog = GET_WIDGET( ui, "dialog" );
	file_texteditor = GET_WIDGET( ui, "file_texteditor" );
	file_imageeditor = GET_WIDGET( ui, "file_imageeditor" );
	button_textclear = GET_WIDGET( ui, "button_textclear" );
	button_imageclear = GET_WIDGET( ui, "button_imageclear" );
	spin_gridvalue = GET_WIDGET( ui, "spin_gridvalue" );


	/* Signal */
	g_signal_connect( button_textclear, "clicked", G_CALLBACK(Meg_Preference_ClearFile), file_texteditor );
	g_signal_connect( button_imageclear, "clicked", G_CALLBACK(Meg_Preference_ClearFile), file_imageeditor );


	/* Setting */

	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_gridvalue), g_key_file_get_double(meg_pref_storage, "numeric", "gridvalue", NULL) );

	if ( current_text_editor_path )
		gtk_file_chooser_set_filename( GTK_FILE_CHOOSER(file_texteditor), current_text_editor_path  );
	if ( current_image_editor_path )
		gtk_file_chooser_set_filename( GTK_FILE_CHOOSER(file_imageeditor), current_image_editor_path  );


	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Main_GetWindow() );

	if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == -3 )
	{
		gchar * image_editor_path = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(file_imageeditor) );
		gchar * text_editor_path = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(file_texteditor) );

		g_key_file_set_double(meg_pref_storage, "numeric", "gridvalue", gtk_spin_button_get_value( GTK_SPIN_BUTTON(spin_gridvalue) ) );


		if ( text_editor_path )
			g_key_file_set_string(meg_pref_storage, "path", "texteditor", text_editor_path );
		else
			g_key_file_remove_key(meg_pref_storage, "path", "texteditor", NULL);

		if ( image_editor_path )
			g_key_file_set_string(meg_pref_storage, "path", "imageeditor", image_editor_path );
		else
			g_key_file_remove_key(meg_pref_storage, "path", "imageeditor", NULL);

		/* Save to File */
		gchar * pref_file = g_build_filename( Meg_Directory_Config(), "mokoi-editor.ini", NULL);
		gchar * string = g_key_file_to_data( meg_pref_storage, NULL, NULL );
		g_file_set_contents( pref_file, string, -1, NULL );
		g_free( pref_file );
		g_free( string );

		g_free(image_editor_path);
		g_free(text_editor_path);

	}
	gtk_widget_destroy(dialog);
	return TRUE;
}


/********************************
* Meg_Dialog_Import
*
*
*/
gboolean Meg_Dialog_Import()
{
	GtkListStore * store_package;
	GtkWidget * dialog, * button_update, * button_close, * tree_package, * progress_action;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(Ui_ImportDialog, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );

	/* Widget */
	dialog = GET_WIDGET( ui, "dialog" );
	store_package = GET_LISTSTORE( ui, "store_package" );
	button_update = GET_WIDGET( ui, "button_update" );
	button_close = GET_WIDGET( ui, "button_close" );
	tree_package = GET_WIDGET( ui, "tree_package" );
	progress_action = GET_WIDGET( ui, "progress_action" );

	/* Signal */
	g_signal_connect_swapped( button_close, "clicked", G_CALLBACK(gtk_widget_destroy), dialog );
	g_signal_connect( button_update, "clicked", G_CALLBACK(Import_UpdateList), store_package );
	g_signal_connect( tree_package, "row-activated", G_CALLBACK(Import_InstallButton), progress_action);
	g_signal_connect( dialog, "delete-event", G_CALLBACK(gtk_widget_destroy), dialog );

	/* Fill Treeview */
	Import_UpdateList( NULL, store_package );

	/* Show Dialog */
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Main_GetWindow() );
	gtk_widget_show_all( dialog );

	return TRUE;
}


/********************************
* Meg_Dialog_CreatePatch
*
*
*/
gboolean Meg_Dialog_CreatePatch()
{
	GtkListStore * store_files;
	GtkWidget * dialog, * file_previousgame;
	GtkFileFilter * filter;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(Ui_PatchPage, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );

	/* Widget */
	dialog = GET_WIDGET( ui, "dialog" );
	store_files = GET_LISTSTORE( ui, "store_files" );
	file_previousgame = GET_WIDGET( ui, "file_previousgame" );

	/* Signal */
	g_signal_connect( gtk_builder_get_object( ui, "cellrenderertoggle1" ), "toggled", G_CALLBACK(CellRender_Event_FixToggled), store_files );
	g_signal_connect( file_previousgame, "file-set", G_CALLBACK(Patch_FileChanged), store_files );

	/* Set File Filter */
	filter = gtk_file_filter_new();

	gtk_file_filter_add_pattern( filter, "*."GAME_EXTENSION );
	gtk_file_filter_set_name( filter, "Game file" );
	gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(file_previousgame), filter );
	gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER(file_previousgame), Meg_Directory_Document() );
	gtk_file_chooser_add_shortcut_folder( GTK_FILE_CHOOSER(file_previousgame), Meg_Directory_Document(), NULL );

	/* Show Dialog */
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Main_GetWindow() );
	if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_OK )
	{
		Patch_Create( NULL, store_files );
	}
	gtk_widget_destroy(dialog);
	return TRUE;
}

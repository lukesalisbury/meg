/****************************
Copyright © 2013 Luke Salisbury
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

/* Internal Functions */

/* Local Type */

/* Global Variables */
extern gchar * mokoiBasePath;
extern GKeyFile * mokoiConfigTable;
extern gchar * mokoiGameDirectories[];

/* UI */
#include "ui/export_dialog.gui.h"
const gchar * mokoiUI_Export = GUIEXPORT_DIALOG


/* Functions */

/********************************
* ExportDialog_SetTextViewContent
* Read content of 'file' into
* 'text_view'
*
*/
void ExportDialog_SetTextViewContent( const gchar * file, const GtkWidget * text_view, const gchar * default_content  )
{
	gchar * content = NULL;
	GtkTextBuffer * buffer = NULL;

	buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(text_view) );

	if ( Meg_file_get_contents(file, &content, NULL, NULL) )
	{
		gtk_text_buffer_set_text(buffer, content, -1);
	}
	else
	{
		gtk_text_buffer_set_text(buffer, default_content, -1);
	}

	g_free(content);
}

/********************************
* ExportDialog_SetTextEntryContent
* Read content of 'file' into
* 'text_entry'
*
*/
void ExportDialog_SetTextEntryContent( const gchar * file, const GtkWidget * text_entry, const gchar * default_content )
{
	gchar * content = NULL;

	if ( Meg_file_get_contents(file, &content, NULL, NULL) )
	{
		gtk_entry_set_text( GTK_ENTRY(text_entry), content );
	}
	else
	{
		gtk_entry_set_text( GTK_ENTRY(text_entry), default_content );
	}

	g_free(content);
}

/********************************
* ExportDialog_FileList_Add
* Event:
*/
void ExportDialog_FileList_Add( GtkWidget * menuitem,  GtkTreeView * treeview )
{
	GtkTreeModel * model = gtk_tree_view_get_model(treeview);
	glong len = g_utf8_strlen(mokoiBasePath, -1) + 1;

	/* Dialog Setup */
	GtkWidget * dialog = gtk_file_chooser_dialog_new ("Select File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_select_multiple( GTK_FILE_CHOOSER(dialog), TRUE );
	gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER(dialog), mokoiBasePath );

	if ( gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT )
	{
		GSList * file_list = gtk_file_chooser_get_uris( GTK_FILE_CHOOSER(dialog) );
		GSList * current = file_list;
		while( current )
		{
			gchar * current_file = g_filename_from_uri( current->data, NULL, NULL);

			if ( g_str_has_prefix(current_file, mokoiBasePath ) )
			{
				gchar * dir = g_path_get_dirname( current_file );


				GtkTreeIter iter, parent;
				GtkTreePath * tree_path = (GtkTreePath *)g_object_get_data( G_OBJECT(model), dir + len );

				if ( gtk_tree_model_get_iter( model, &parent, tree_path ) )
				{
					gtk_tree_store_append( GTK_TREE_STORE(model), &iter, &parent);
					gtk_tree_store_set( GTK_TREE_STORE(model), &iter, 1, g_strdup( current_file ), 0, g_path_get_basename( current_file ), 2, 0,  -1 );
				}
				g_free(dir);
			}
			g_free(current_file);
			current = g_slist_next( current );
		}
		g_slist_free(file_list);
	}
	gtk_widget_destroy(dialog);
}


/********************************
* ExportDialog_FileList_Remove
* Event:
*/
void ExportDialog_FileList_Remove( GtkWidget * menuitem,  GtkTreeView * treeview )
{

	GtkTreeSelection * selection = gtk_tree_view_get_selection( treeview );
	GtkTreeIter iter;
	GtkTreeModel * model;

	if ( gtk_tree_selection_get_selected( selection, &model, &iter) )
	{
		 gtk_tree_store_remove(GTK_TREE_STORE(model), &iter);
	}


}

/********************************
* ExportDialog_FileList_Display
* Event:
*/
void ExportDialog_FileList_Display( GtkWidget *treeview, GdkEventButton * event, gpointer data )
{
	GtkWidget * menu = gtk_menu_new();
	GtkWidget * item_add, * item_remove;

	item_add = gtk_image_menu_item_new_from_stock(GTK_STOCK_ADD, NULL);
	g_signal_connect(item_add, "activate", G_CALLBACK(ExportDialog_FileList_Add), treeview);
	gtk_menu_shell_append( GTK_MENU_SHELL(menu), item_add );

	item_remove = gtk_image_menu_item_new_from_stock(GTK_STOCK_REMOVE, NULL);
	g_signal_connect(item_remove, "activate", G_CALLBACK(ExportDialog_FileList_Remove), treeview);
	gtk_menu_shell_append( GTK_MENU_SHELL(menu), item_remove );


	gtk_widget_show_all(menu);

	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, (event != NULL ? event->button : 0), gdk_event_get_time((GdkEvent*)event) );
}

/********************************
* ExportDialog_FileList_RightClick
* Event:
*/
gboolean ExportDialog_FileList_RightClick( GtkWidget *treeview, GdkEventButton *event, gpointer data )
{
	if ( event->type == GDK_BUTTON_PRESS && event->button == 3 )
	{
		ExportDialog_FileList_Display( treeview, event, data );
		return TRUE;
	}
	return FALSE;
}

/********************************
* ExportDialog_FileList_Button
* Event:
*/
gboolean ExportDialog_FileList_Button( GtkWidget *treeview, gpointer data )
{
	ExportDialog_FileList_Display( treeview, NULL, data );
	return TRUE;
}

/********************************
* ExportDialog_FileList_ForEach
*
*/
gboolean ExportDialog_FileList_ForEach( GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, GSList ** list )
{
	gboolean dir;
	gchar * file;
	gtk_tree_model_get( model, iter, 1, &file, 2, &dir, -1 );

	g_print("%s %d\n", file, dir);

	if ( !dir )
	{
		(*list) = g_slist_append( (*list), g_strdup(file) );
	}

	g_free( file );
	return FALSE;
}

/********************************
* ExportDialog_ConfigList_ForEach
*
*/
gboolean ExportDialog_ConfigList_ForEach( GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, GString * string )
{
	gboolean include;
	gchar * key;
	gtk_tree_model_get( model, iter, 1, &key, 0, &include, -1 );

	if ( include )
	{
		gchar * key_value = g_key_file_get_value( mokoiConfigTable, "Mokoi", key, NULL );
		g_string_append_printf( string, "%s = %s\n", key, key_value );
	}

	return FALSE;
}


/********************************
* Game_Compile_FileChooser
*
*/
gboolean ExportDialog_FileChooser( GtkWidget * button, GdkEventButton * event, GtkWidget * label )
{
	if ( event->type == GDK_BUTTON_PRESS && event->button == 1 )
	{
		GtkFileFilter * filter = gtk_file_filter_new();
		const gchar * file_name = gtk_label_get_text( GTK_LABEL(label) );
		GtkWidget * dialog = gtk_file_chooser_dialog_new ("Save Package",
									  Meg_Misc_ParentWindow(button),
									  GTK_FILE_CHOOSER_ACTION_SAVE,
									  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
									  GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
									  NULL);



		gtk_file_filter_add_pattern( filter, "*.package" );
		gtk_file_filter_set_name( filter, "Package" );

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
* Meg_Dialog_Export
* Export Projects
*
*/
gboolean Meg_Dialog_Export()
{
	gint response = 0;
	gboolean inc_lang, inc_music, inc_entities, inc_soundfx, inc_sprite;
	const gchar * content_title, * content_authors, * content_version;
	gchar * content_terms, * content_descript;
	GString * content_config = g_string_new("");
	GSList * files = NULL;
	GSList * folders = NULL;

	gchar * package_name;
	GtkWidget * dialog, * check_customfiles,
			* entry_title, * entry_version, * entry_authors, * text_terms, * text_description,
			* check_lang, * check_music, * check_entities, * check_sounds, * check_sprite,
			* button_chooser, * label_file;
	GtkListStore * store_config = NULL;
	GObject * tree_filelisting = NULL;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(mokoiUI_Export, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );


	/* Widgets */
	dialog = GET_WIDGET( ui, "dialog");

	entry_title = GET_WIDGET( ui, "entry_title" );
	entry_version = GET_WIDGET( ui, "entry_version" );
	entry_authors = GET_WIDGET( ui, "entry_authors" );
	text_terms = GET_WIDGET( ui, "text_terms" );
	text_description = GET_WIDGET( ui, "text_description" );

	check_lang = GET_WIDGET( ui, "check_lang" );
	check_music = GET_WIDGET( ui, "check_music" );
	check_entities = GET_WIDGET( ui, "check_entities" );
	check_sounds = GET_WIDGET( ui, "check_sounds" );
	check_sprite = GET_WIDGET( ui, "check_sprite" );

	store_config = GET_LISTSTORE(ui, "store_config" );
	check_customfiles = GET_WIDGET( ui, "check_customfiles");
	tree_filelisting = gtk_builder_get_object( ui, "tree_filelisting" );


	label_file = GET_WIDGET( ui, "label_file");
	button_chooser = GET_WIDGET( ui, "button_chooser");

	/* signals */
	g_signal_connect( check_customfiles, "toggled", G_CALLBACK(ToggleButton_Event_WidgetSensitive), tree_filelisting );
	g_signal_connect( tree_filelisting, "popup-menu", G_CALLBACK(ExportDialog_FileList_Button), NULL );
	g_signal_connect( tree_filelisting, "button-press-event", G_CALLBACK(ExportDialog_FileList_RightClick), NULL );
	g_signal_connect( button_chooser, "button-press-event", G_CALLBACK(ExportDialog_FileChooser), label_file );

	SET_OBJECT_SIGNAL( ui, "togglecell_file", "toggled", G_CALLBACK(CellRender_Event_FixToggled), store_config );
	ToggleButton_Event_WidgetSensitive( GTK_TOGGLE_BUTTON(check_customfiles), GTK_WIDGET(tree_filelisting) );

	/* Get Date to use for default version */
	const gchar * author_name = g_get_real_name();
	gchar * date_string = g_new0(gchar, 9);
	GDate * date = g_date_new();

	g_date_set_time_t( date, time(NULL) );
	g_date_strftime( date_string, 9, "%Y%m%d", date );


	/* Read source files */
	ExportDialog_SetTextViewContent( "package/DESCRIPTION", text_description, "" );
	ExportDialog_SetTextViewContent( "package/TERMSOFUSE", text_terms, "" );
	ExportDialog_SetTextEntryContent( "package/VERSION", entry_version, date_string );
	ExportDialog_SetTextEntryContent( "package/PACKAGE", entry_title, "Untitled" );
	ExportDialog_SetTextEntryContent( "package/AUTHORS", entry_authors, author_name );


	/* Set File name */
	content_title = gtk_entry_get_text( GTK_ENTRY(entry_title) );
	content_version = gtk_entry_get_text( GTK_ENTRY(entry_version) );

	package_name = g_strconcat( content_title, "-", content_version, ".package", NULL);
	g_strdelimit( package_name, "_-|> \t\n<\\/?*", (gchar)'_'); /* Strip invalid chars */
	g_strstrip( package_name );

	gchar * file_path = g_build_filename( Meg_Directory_Document(), package_name, NULL );
	gtk_label_set_text( GTK_LABEL(label_file), file_path );
	g_free(file_path);

	/* Free Date Stuff */
	g_free(date_string);
	g_date_free(date);

	/* Setup File Treeview */
	guint c = 0;
	GtkTreeModel * model = gtk_tree_view_get_model( GTK_TREE_VIEW(tree_filelisting) );
	while ( mokoiGameDirectories[c] != NULL )
	{
		GtkTreeIter iter;
		gtk_tree_store_append( GTK_TREE_STORE(model), &iter, NULL);
		gtk_tree_store_set( GTK_TREE_STORE(model), &iter, 0, mokoiGameDirectories[c], 1, NULL, 2, 1,  -1 );
		g_object_set_data( G_OBJECT(model), mokoiGameDirectories[c], gtk_tree_model_get_path( model, &iter ) );
		c++;
	}


	/* Setup Config treeview */
	guint i = 0;
	GtkTreeIter iter2;
	gchar ** config_list = g_key_file_get_keys( mokoiConfigTable, "Mokoi", NULL, NULL );
	while( config_list[i] )
	{
		gtk_list_store_append( store_config, &iter2 );
		gtk_list_store_set( store_config, &iter2, 0, FALSE, 1, g_strdup(config_list[i]), -1 );
		i++;
	}

	/* Run Dialog */
	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ) );
	while( TRUE )
	{
		response = gtk_dialog_run( GTK_DIALOG(dialog) );
		content_title = gtk_entry_get_text( GTK_ENTRY(entry_title) );
		if ( !response || response == GTK_RESPONSE_DELETE_EVENT || response == GTK_RESPONSE_REJECT )
			break;
		if ( g_utf8_strlen(content_title, -1) )
		{
			break;
		}
		else
			Meg_Error_Print( __func__, __LINE__, "Must enter a title.");
	}



	if ( response == GTK_RESPONSE_ACCEPT )
	{
		const gchar * output_file_name = gtk_label_get_text( GTK_LABEL(label_file) );
		content_title = gtk_entry_get_text( GTK_ENTRY(entry_title) );
		content_authors = gtk_entry_get_text( GTK_ENTRY(entry_authors) );
		content_version = gtk_entry_get_text( GTK_ENTRY(entry_version) );
		content_terms = TextView_GetText( text_terms );
		content_descript = TextView_GetText( text_description );

		inc_lang = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_lang) );
		inc_music = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_music) );
		inc_entities = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_entities) );
		inc_soundfx = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_sounds) );
		inc_sprite = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_sprite) );


		/* Config */
		gtk_tree_model_foreach( GTK_TREE_MODEL(store_config), (GtkTreeModelForeachFunc)ExportDialog_ConfigList_ForEach, content_config );
		if ( content_config->len )
		{
			content_config = g_string_prepend( content_config, "[package]\n");
		}


		if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_customfiles) ) )
		{
			gtk_tree_model_foreach( model, (GtkTreeModelForeachFunc)ExportDialog_FileList_ForEach, &files );
			Package_ExportList(output_file_name, content_title, content_version, content_descript, content_authors, content_terms, content_config->str, files);
		}
		else
		{
			if ( inc_lang ) { folders = g_slist_prepend(folders, "lang"); }
			if ( inc_lang ) { folders = g_slist_prepend(folders, "dialog"); }
			if ( inc_music ) { folders = g_slist_prepend(folders, "music"); }
			if ( inc_entities )
			{
				folders = g_slist_prepend(folders, "scripts");
				folders = g_slist_prepend(folders, "scripts/routines");
				folders = g_slist_prepend(folders, "scripts/map/routines");
				folders = g_slist_prepend(folders, "c/scripts");
			}
			if ( inc_soundfx ) { folders = g_slist_prepend(folders, "soundfx"); }
			if ( inc_sprite ) { folders = g_slist_prepend(folders, "sprites"); }
			if ( inc_sprite ) { folders = g_slist_prepend(folders, "masks"); }

			Package_ExportFolders(output_file_name, content_title, content_version, content_descript, content_authors, content_terms, content_config->str, folders);
		}

		g_free(content_terms);
		g_free(content_descript);
	}
	g_string_free(content_config, TRUE);

	gtk_widget_destroy( dialog );
	return TRUE;
}

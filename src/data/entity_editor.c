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
#include "loader_functions.h"
#include "entity_functions.h"
#include "game_compiler.h"
#include "logger_functions.h"
#if (GTK_MAJOR_VERSION == 2)
#include "gdkkeysyms-backward.h"
#endif

/* External Functions */
gboolean EntityEditor_InsertDialog( GtkWidget * widget, GtkWidget * view );
GtkWidget *  ManagedEntity_Edit(gchar * filename);

/* Global Variables */
extern GError * mokoiError;
extern GtkWidget * mokoiCompileLog;
extern GtkWidget * mokoiEntityTreeview;
extern GSList * mokoiFunctionDatabase;
extern GHashTable * mokoiFunctionFiles;

/* Local Variables */

/* UI */
#include "ui/entity_editor_function.gui.h"
#include "ui/entity_editor.gui.h"
#include "ui/error_dialog.gui.h"
const gchar * mokoiUI_EntityFunction = GUIENTITY_EDITOR_FUNCTION
const gchar * mokoiUI_EntityEditor = GUIENTITY_EDITOR
const gchar * mokoiUI_ErrorDialog = GUIERROR_DIALOG



/* Events */
/********************************
* EntityEditor_Close
*
@ widget:
@ event:
@ window:
*/
gboolean EntityEditor_Close( GtkWidget * widget, GdkEvent * event, GtkWidget * textview )
{
	gint result = 0;
	gboolean modified = FALSE;
	GtkWidget * dialog = NULL;
	GtkWidget * window = gtk_widget_get_toplevel( widget );
	GtkTextBuffer * buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(textview) );
	const gchar * file = gtk_window_get_title( GTK_WINDOW(window) );

	modified = gtk_text_buffer_get_modified(buffer);

	dialog = gtk_message_dialog_new( GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "Do you wish to close '%s'", file );

	/* File has been changed */
	if ( modified )
	{
		gtk_message_dialog_format_secondary_text( GTK_MESSAGE_DIALOG(dialog), "%s has been changed since last save.", file );
		gtk_dialog_add_buttons( GTK_DIALOG(dialog), "Close without Saving" , 1, GTK_STOCK_SAVE, 2, GTK_STOCK_CANCEL, 0, NULL );
	}
	else
	{
		gtk_dialog_add_buttons( GTK_DIALOG(dialog), GTK_STOCK_YES, 1, GTK_STOCK_CANCEL, 0, NULL );
	}



	result = gtk_dialog_run( GTK_DIALOG(dialog) );
	gtk_widget_destroy( dialog );

	if ( result == 0 )
	{
		return TRUE;
	}
	else
	{
		gchar * tree_path = (gchar *)g_object_get_data( G_OBJECT(window), "alchera-list-path");
		GtkTreeModel * list = gtk_tree_view_get_model( GTK_TREE_VIEW(mokoiEntityTreeview) );
		GtkTreeIter iter;
		if ( gtk_tree_model_get_iter_from_string( list, &iter, tree_path ) )
		{
			gtk_tree_store_set( GTK_TREE_STORE(list), &iter, 5, "", 4, NULL, -1);
		}
	}
	return FALSE;

}

/********************************
* EntityEditor_Close2
* Calls Close on Editor when pressing the close button
@ button:
@ textview:
*/
gboolean EntityEditor_Close2( GtkButton * button, GtkWidget * textview )
{
	GtkWidget * window = gtk_widget_get_toplevel( textview );
	if ( !EntityEditor_Close(window, NULL, textview) )
	{
		gtk_widget_destroy( window );
	}

	return FALSE;

}
/********************************
* EntityEditor_Modified
* called when the text buffer has changed.
@ buffer:
@ label:
*/
void EntityEditor_Modified( GtkTextBuffer * buffer, GtkLabel * label )
{
	gtk_label_set_markup( GTK_LABEL(label), ( gtk_text_buffer_get_modified(buffer) ? "<b>File as changed</b>" : "") );
}


/********************************
* EntityEditor_Help
*
@ widget:
@ view:
*/
gboolean EntityEditor_Help( GtkWidget * widget, gpointer view )
{
	gchar * search_text = NULL, * file_path;
	GtkTextIter start, end;
	GtkTextBuffer * buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(view) );

	gtk_text_buffer_get_selection_bounds( buffer, &start, &end );
	search_text = gtk_text_buffer_get_text( buffer, &start, &end, FALSE );

	file_path = g_strdup_printf("ScriptAPI/Pawn/%s.xml", search_text);

	Meg_Help_Open( file_path );

	g_free( file_path );
	return FALSE;
}

/********************************
* EntityEditor_ShowMenuClick
*
@ textview:
@ arg1:
@ data:
*/
void EntityEditor_ShowMenuClick( GtkTextView *textview, GtkMenu *arg1, gpointer data)
{
	GtkWidget * item3;
	GtkTextBuffer * buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(textview) );
	GtkWidget * itemsep = gtk_separator_menu_item_new();
	gtk_widget_show(itemsep);
	gtk_menu_shell_prepend( GTK_MENU_SHELL(arg1), itemsep );
	if ( gtk_text_buffer_get_has_selection( buffer ) )
	{
		item3 = gtk_menu_item_new_with_label( "Search Help" );
		g_signal_connect( G_OBJECT(item3), "activate", G_CALLBACK(EntityEditor_Help), (gpointer) textview );
	}
	else
	{
		item3 = gtk_menu_item_new_with_label( "Insert Function" );
		g_signal_connect( G_OBJECT(item3), "activate", G_CALLBACK(EntityEditor_InsertDialog), (gpointer) textview );
	}
	gtk_menu_shell_prepend( GTK_MENU_SHELL(arg1), item3 );
	gtk_widget_show(item3);
}

/********************************
* Editor_SwitchFunction
*
@ widget:
@ name:
*/
void EntityEditor_InsertFunctions( GtkComboBox * widget )
{
	Meg_ComboText_Clear(widget);
	GHashTableIter iter;
	gpointer key, value;

	g_hash_table_iter_init( &iter, mokoiFunctionFiles );
	while (g_hash_table_iter_next( &iter, &key, &value) )
	{
		Meg_ComboText_AppendText( widget, (gchar*)key);
	}

}

/********************************
* Editor_SwitchFunction
*
@ widget: GtkComboBox
@ store:

	  <!-- column-name name -->      <column type="gchararray"/>
	  <!-- column-name value -->      <column type="gchararray"/>
	  <!-- column-name gboolean -->      <column type="gboolean"/>
	  <!-- column-name gboolean1 -->      <column type="gboolean"/>
	  <!-- column-name gboolean2 -->      <column type="gboolean"/>
	  <!-- column-name type -->      <column type="gchar"/>
	  <!-- column-name tooltip -->      <column type="gchararray"/>

*/
void EntityEditor_SwitchFunction( GtkComboBox * widget, GtkListStore * argument_store )
{
	GtkTreeIter iter;
	EditorDatabaseListing * function;
	GSList * list = NULL;
	gchar * text = NULL;
	GtkLabel * label = NULL;

	if ( !widget )
		g_warning( "No combobox found");

	if ( gtk_combo_box_get_active_iter( widget, &iter ) )
		gtk_tree_model_get( gtk_combo_box_get_model(widget), &iter, 1, &text, 2, &function, -1 );
	else
		g_warning( "No combobox found");

	if ( !function || !argument_store )
	{
		return;
	}

	/* Update Summary Label */
	label = g_object_get_data( G_OBJECT(widget), "formlabel" );
	gtk_label_set_text( label, text );

	/* Update Arguments */
	gtk_list_store_clear( argument_store );
	list = (GSList*)function->user_data;
	while ( list != NULL )
	{

		EditorDatabaseListing * listing = (EditorDatabaseListing *)list->data;
		gtk_list_store_append( argument_store, &iter );
		gtk_list_store_set( argument_store, &iter, 0, g_strdup(listing->name), 1, (listing->user_data ? g_strdup(listing->user_data) : ""), 2, FALSE, 3, FALSE, 4, TRUE, 5, (listing->arguments ? listing->arguments[0] : 'n'), 6, g_strdup(listing->info), -1 );
		list = g_slist_next( list );
	}

}

/********************************
* EntityEditor_SwitchFiles
*
@ widget:
@ api_store: label, info, EntityDatabaseItems
*/
void EntityEditor_SwitchFiles( GtkComboBox * widget, GtkListStore * api_store )
{
	GtkTreeIter iter;
	GSList * list = NULL;
	gchar * file = Meg_ComboText_GetText(widget);

	list = (GSList*) g_hash_table_lookup(mokoiFunctionFiles, file );

	if ( !list || !api_store )
	{
		return;
	}

	gtk_list_store_clear( api_store );
	while ( list != NULL )
	{
		EditorDatabaseListing * listing = list->data;

		gtk_list_store_append( api_store, &iter );
		gtk_list_store_set( api_store, &iter, 0, g_strdup(listing->name), 1, (listing->info ? g_strdup(listing->info) : ""), 2, listing, -1 );
		list = g_slist_next( list );
	}
}

/********************************
* EntityEditor_EditStore
* Updates the list store with the new value
@ renderer:
@ path:
@ new_text:
@ user_data:GtkListStore
*/

void EntityEditor_EditStore( GtkCellRendererText * renderer,  gchar * path, gchar * new_text, gpointer user_data )
{
	GtkListStore * argument_store = (GtkListStore *)user_data;
	GtkTreeIter iter;

	if ( gtk_tree_model_get_iter_from_string( GTK_TREE_MODEL(argument_store), &iter, path ) )
	{
		gtk_list_store_set( argument_store, &iter, 1, g_strdup(new_text), -1 );
	}
}


/********************************
* EntityEditor_ShowHelp
* Event: key-release-event
* Result:
*/
gboolean EntityEditor_ShowHelp( GtkWidget * widget, GdkEventKey *event, gpointer user_data )
{
	if ( event->keyval == 0xFFbe )
	{
		GtkTextBuffer * buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(widget) );
		if ( gtk_text_buffer_get_has_selection( buffer ) )
		{
			EntityEditor_Help( widget, widget );
		}
	}
	return FALSE;
}

/* Functions */

/********************************
* EntityEditor_Save
*
@ button: file to remove.
@ view: TextView widget
*/
void EntityEditor_Save( GtkButton * button_save, GtkWidget * textview_content )
{
	GtkTextIter start, end;
	GtkWidget * dialog, * log;
	GtkTextBuffer * buffer;
	gchar * content, * file;

	GtkWidget * parent = gtk_widget_get_toplevel( textview_content );

	file = (gchar *)g_object_get_data( G_OBJECT(textview_content), "filename" );
	buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(textview_content) );
	Meg_Editor_ClearLineInfo( textview_content );

	if ( file )
	{
		gtk_text_buffer_get_bounds( buffer, &start, &end );
		content = gtk_text_buffer_get_text( buffer, &start, &end, FALSE );
		content = g_strdelimit( content, "\r", ' '); // Screw \r

		Meg_file_set_contents( file, content, -1, &mokoiError );

		gtk_text_buffer_set_modified( buffer, FALSE );

		if ( mokoiError )
		{
			Meg_Error_Print( __func__, __LINE__, "Can't open entity: %s (%s)", file, mokoiError->message );
			g_clear_error( &mokoiError );
		}
		else if ( g_str_has_suffix(file, ".mps") )
		{
			/* Setup Error Widget */
			/* > UI */
			GError * error = NULL;
			GtkBuilder * ui = gtk_builder_new();

			if ( !gtk_builder_add_from_string(ui, mokoiUI_ErrorDialog, -1, &error) )
			{
				Meg_Error_Print( __func__, __LINE__, "UI creation error '%s'.", error->message );
				return;
			}
			/* < UI */

			dialog = GET_WIDGET( ui, "window1");
			log = GET_WIDGET( ui, "textview1");
			gtk_window_set_modal( GTK_WINDOW(dialog), FALSE );
			gtk_window_set_transient_for( GTK_WINDOW(dialog), GTK_WINDOW(parent) );

			g_signal_connect( gtk_builder_get_object( ui, "button_close" ), "clicked", G_CALLBACK(Meg_Dialog_ButtonClose), NULL);


			if ( !EntityCompiler_FileWithRountines( file, log, textview_content ) )
			{
				/* Show Error */
				gtk_widget_show_all( dialog );
				Meg_Log_Print( LOG_WARNING, "Entity '%s' compile failed, see dialog for details", file );
			}
			else
			{
				/* Discard Dialog */
				gtk_widget_destroy( dialog );
				Meg_Log_Print( LOG_NONE, "Entity '%s' compiled", file );
			}
		}
	}
}



/********************************
* EntityEditor_Insert
*
@ widget:
@ view:
*/
gboolean EntityEditor_InsertDialog( GtkWidget * widget, GtkWidget * view )
{
	GtkWidget * dialog, * combobox, * label, * file_combobox;
	GtkListStore * argument_store, * file_store, * api_store;
	GtkTreeIter iter;
	GtkTextBuffer * buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(view) );

	/* > UI */
	GError * error = NULL;
	GtkBuilder * ui = gtk_builder_new();
	if ( !gtk_builder_add_from_string(ui, mokoiUI_EntityFunction, -1, &error) )
	{
		Meg_Error_Print( __func__, __LINE__, "UI creation error '%s'.", error->message );
		return FALSE;
	}

	/* < UI */

	dialog = GET_WIDGET( ui, "dialog1");
	label = GET_WIDGET( ui, "label2" );
	combobox = GET_WIDGET( ui, "combo_api" );
	file_combobox = GET_WIDGET( ui, "combo_file" );

	argument_store = GET_LISTSTORE( ui, "argument_store" );
	file_store = GET_LISTSTORE( ui, "file_store" );
	api_store = GET_LISTSTORE( ui, "api_store" );

	/* Signals */
	SET_OBJECT_SIGNAL( ui, "spritelist-renderer2", "edited", G_CALLBACK(EntityEditor_EditStore), argument_store );
	SET_OBJECT_SIGNAL( ui, "combo_file", "changed", G_CALLBACK(EntityEditor_SwitchFiles), api_store );
	g_signal_connect( combobox, "changed", G_CALLBACK(EntityEditor_SwitchFunction), argument_store );

	g_object_set_data( G_OBJECT(combobox), "formlabel", label );


	EntityEditor_InsertFunctions( GTK_COMBO_BOX(file_combobox) );


	gtk_window_set_transient_for( GTK_WINDOW(dialog), GTK_WINDOW(gtk_widget_get_toplevel(view)) );
	gint result = gtk_dialog_run( GTK_DIALOG(dialog) );

	if ( result == -10 )
	{
		/* Create String */
		gchar * string = NULL;
		gchar * temp = NULL;
		gchar * value;
		gchar type = 'a';
		gtk_tree_model_get_iter_first( GTK_TREE_MODEL(argument_store), &iter );
		do
		{
			gtk_tree_model_get( GTK_TREE_MODEL(argument_store), &iter, 1, &value, 5, &type, -1 );
			if ( value )
			{
				if (temp)
				{
					string = g_strjoin(",", temp, value, NULL);
					g_free(temp);
				}
				else
					string = g_strdup(value);
				temp = string;
			}
			else
			{
				if (temp)
				{
					string = g_strjoin(",", temp, ( type == 'a' ? "\"\"" : "0"), NULL);
					g_free(temp);
				}
				else
					string = g_strdup(( type == 'a' ? "\"\"" : "0"));

				temp = string;
			}

		} while ( gtk_tree_model_iter_next( GTK_TREE_MODEL(argument_store), &iter) );

		/* Insert String */
		gtk_text_buffer_insert_at_cursor( buffer, Meg_ComboText_GetText( GTK_COMBO_BOX(combobox) ), -1);
		gtk_text_buffer_insert_at_cursor( buffer,"(", -1);
		if ( string )
		{
			gtk_text_buffer_insert_at_cursor( buffer, string, -1);
			g_free(string);
		}
		gtk_text_buffer_insert_at_cursor( buffer,");\n", -1);

	}
	gtk_widget_destroy( dialog );
	return FALSE;
}


/********************************
* EntityEditor_New
*
@ file: file to open
*/
GtkWidget * EntityEditor_New( gchar * file )
{
	gboolean use_internal = TRUE;

	if ( !PHYSFS_exists(file) )
	{
		g_warning("File doesn't exists: '%s'", (file));
		return NULL;
	}


	/* Open Managed Entity Editor Instead */
	GtkWidget * managed_widget = NULL;
	gchar * managed_file = g_strconcat( file, ".managed", NULL );

	if ( Meg_file_test( managed_file, G_FILE_TEST_IS_REGULAR ) )
	{
		managed_widget = ManagedEntity_Edit( file );
	}
	g_free( managed_file );

	if ( managed_widget )
		return managed_widget;



	/* Check if External Editor can be used */
	gchar * full_path = Meg_file_get_path(file);
	if ( full_path )
	{
		use_internal = !Meg_RunProgram( "TEXT_EDITOR", full_path);
	}
	g_free( full_path );


	if ( use_internal )
	{

		/* Use inbuild editor */
		GtkWidget * widget, * text_view, * label, * viewport, * button_save, * button_close, * button_label;
		GtkTextBuffer * buffer;
		gchar * file_content;

		/* UI */
		GError * error = NULL;
		GtkBuilder * ui = gtk_builder_new();
		if ( !gtk_builder_add_from_string(ui, mokoiUI_EntityEditor, -1, &error) )
		{
			Meg_Error_Print( __func__, __LINE__, "UI creation error '%s'.", error->message );
			return NULL;
		}


		widget = GET_WIDGET( ui, "window1" );
		label = GET_WIDGET( ui, "alchera-label" );
		viewport = GET_WIDGET( ui, "scrolledwindow1" );
		button_save = GET_WIDGET( ui, "button-save" );
		button_close = GET_WIDGET( ui, "button-close" );
		button_label = GET_WIDGET( ui, "label1" );

		text_view = Meg_Editor_New( "pawn" );

		g_object_set_data_full( G_OBJECT(text_view), "filename", g_strdup(file), (GDestroyNotify)g_free );
		g_signal_connect( button_save, "clicked", G_CALLBACK(EntityEditor_Save), text_view );
		g_signal_connect( button_close, "clicked", G_CALLBACK(EntityEditor_Close2), text_view );
		g_signal_connect( text_view, "populate-popup", G_CALLBACK(EntityEditor_ShowMenuClick), NULL );
		g_signal_connect( text_view, "key-release-event", G_CALLBACK(EntityEditor_ShowHelp), NULL);
		g_signal_connect( widget, "delete-event", G_CALLBACK(EntityEditor_Close), text_view );


		/* Add Key watch */
		GtkAccelGroup * accel_group = gtk_accel_group_new();
		gtk_window_add_accel_group( GTK_WINDOW(widget), accel_group);
		gtk_widget_add_accelerator( button_save, "activate", accel_group, GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);


		gchar * markup = g_markup_printf_escaped( "<big><b>%s</b></big>\n%s", "Entity Editor", file );
		gtk_label_set_markup( GTK_LABEL(label), markup );
		g_free( markup );

		gchar * title = g_path_get_basename(file);
		gtk_window_set_title( GTK_WINDOW(widget), title );
		g_free( title );

		if ( PHYSFS_exists( file ) )
		{
			Meg_file_get_contents( file, &file_content, NULL, &mokoiError);
			if ( mokoiError )
			{
				Meg_Error_Print( __func__, __LINE__, "Can't open entity: %s (%s)", file, mokoiError->message);
				g_clear_error(&mokoiError);
				file_content = "";
			}
		}
		else
			file_content = "";

		/* Obtaining the buffer associated with the widget. */
		buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(text_view) );
		gtk_text_buffer_set_text( buffer, file_content, -1 );

		g_signal_connect( G_OBJECT(buffer), "modified-changed", G_CALLBACK(EntityEditor_Modified), button_label );
		gtk_text_buffer_set_modified( buffer, FALSE );

		gtk_container_add( GTK_CONTAINER(viewport), text_view);
		gtk_window_set_default_size( GTK_WINDOW(widget), 600, 440 );
		gtk_widget_show_all( widget );
		return widget;
	}
	return NULL;
}

/********************************
* EntityEditor_NewMain
*
*/
void EntityEditor_NewMain( GtkButton * button, gpointer data )
{
	EntityEditor_New( "/scripts/main.mps" );
}

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
#include "managed_entity.h"
#include "entity_functions.h"
#include "entity_options_parser.h"
#include "entity_options.h"

/* Required Headers */
#include <glib/gstdio.h>
#include <glib/gprintf.h>

/* Global Variables */
extern GError * mokoiError;
extern GList * mokoiEntityStateList;

/* Global Function */

/* Local Functions */
void EntityOptionParser_StartHandler( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values,  gpointer data, GError **error);


/* Local Variables */

static GMarkupParser mokoiRuntimeParser = {EntityOptionParser_StartHandler, NULL, NULL, NULL, NULL};


const gchar * mokoiUI_EntityOption = GUI_ENTITY_OPTION_EDITOR;


/********************************
* EntityOptionParser_StartHandler
* Part of the XML parser
*/
void EntityOptionParser_StartHandler( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, gpointer data, GError **error)
{
	if ( !g_ascii_strcasecmp(element_name, "option") )
	{
		gchar * name = NULL;
		EntityOptionStruct * options = EntityOption_New( NULL, NULL );

		for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
		{
			if ( !g_ascii_strcasecmp(*attribute_names, "name") )
				name = g_strdup(*attribute_values);
			else if ( !g_ascii_strcasecmp(*attribute_names, "value") )
				options->value = g_strdup(*attribute_values);
			else if ( !g_ascii_strcasecmp(*attribute_names, "type") )
				options->type = g_strdup(*attribute_values);
			else if ( !g_ascii_strcasecmp(*attribute_names, "removable") )
				options->removable = TRUE;
		}
		if ( name )
		{
			options->internal_type = EntityOption_Type(options->type);
			g_hash_table_insert(( GHashTable *)data, name, options);
		}
	}
}


/********************************
* EntityOptionParser_SaveString
*
*/
void EntityOptionParser_SaveString(gchar * key, EntityOptionStruct * option, GString * file_content )
{
	if ( option )
	{
		g_string_append_printf( file_content, "\t<option name=\"%s\" value=\"%s\" type=\"%s\" />\n", key, (option->value != NULL ? option->value : ""), (option->type != NULL ? option->type : "") );
	}

}


/********************************
* EntityOptionParser_Save
*
*/
gboolean EntityOptionParser_Save(gchar * entity_option_file, GHashTable * options)
{
	gboolean successful = FALSE;
	GString * file_content = g_string_new("");

	if ( g_hash_table_size(options) )
	{
		g_string_append( file_content, "<entity>\n");
		g_hash_table_foreach( options, (GHFunc)EntityOptionParser_SaveString, (gpointer)file_content );
		g_string_append( file_content, "</entity>");

		successful = Meg_file_set_contents( entity_option_file, file_content->str, -1, &mokoiError);
		if ( mokoiError )
		{
			Meg_Error_Print( __func__, __LINE__, "Entity options could not be saved. Reason: %s", mokoiError->message );
			g_clear_error( &mokoiError );
		}
	}
	else
	{
		if ( Meg_file_test( entity_option_file, G_FILE_TEST_IS_REGULAR ) )
		{
			PHYSFS_delete( entity_option_file );
		}
	}

	g_string_free( file_content, FALSE );

	return successful;
}


/********************************
* EntityOptionParser_Load
*
*/
GHashTable * EntityOptionParser_Load(gchar * file)
{
	GError * error  = NULL;
	gchar * file_content = NULL;
	GMarkupParseContext * ctx = NULL;
	GHashTable * table = g_hash_table_new_full( g_str_hash, g_str_equal, (GDestroyNotify)g_free, (GDestroyNotify)EntityOption_Delete );

	if ( Meg_file_get_contents( file, &file_content, NULL, NULL ) )
	{
		ctx = g_markup_parse_context_new(&mokoiRuntimeParser, (GMarkupParseFlags)G_MARKUP_TREAT_CDATA_AS_TEXT, (gpointer)table, NULL);

		if ( !g_markup_parse_context_parse(ctx, file_content, -1, &error) )
		{
			Meg_Error_Print( __func__, __LINE__, "Parsing setting error. Reason: %s", error->message );
			g_clear_error(&error);
		}
		else
		{
			g_markup_parse_context_end_parse(ctx, &error);
		}
		g_markup_parse_context_free(ctx);
	}
	return table;
}

/********************************
* EntityOptionEditor_Menu_Add
* Event:
*/
void EntityOptionEditor_Menu_Add( GtkWidget * menuitem, GtkWidget *treeview )
{
	GtkWidget * dialog, * label, * combo, * vbox;

	/* Create the widgets */
	dialog = gtk_dialog_new_with_buttons( "Add New Option", Meg_Misc_ParentWindow(treeview), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL );
	label = gtk_entry_new();
	combo = gtk_combo_box_new();
	vbox = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 1 );

	Meg_ComboText_Setup( combo, TRUE );

	/**/
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "" );
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "boolean" );
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "music" );
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "soundfx" );
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "entity" );
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "section" );
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "sectionmap" );
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "map" );
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "mapentity" );


	/* Add the label, and show everything we've added to the dialog.*/
	gtk_container_add( GTK_CONTAINER(vbox), label );
	gtk_container_add( GTK_CONTAINER(vbox), combo );
	gtk_container_add( GTK_CONTAINER(gtk_dialog_get_content_area( GTK_DIALOG(dialog) )), vbox );
	gtk_widget_show_all( vbox );


	gint result = gtk_dialog_run( GTK_DIALOG(dialog) );
	if ( result == GTK_RESPONSE_ACCEPT )
	{
		const gchar * title = gtk_entry_get_text( GTK_ENTRY(label) );
		const gchar * type = Meg_ComboText_GetText( GTK_COMBO_BOX(combo) );


		GtkTreeIter iter;
		GtkTreeModel * model = gtk_tree_view_get_model( GTK_TREE_VIEW(treeview) );
		GHashTable * settings = g_object_get_data( G_OBJECT(model), "runtime-hashtable" );

		EntityOption_InsertNew( settings, title, "", type );


		gtk_list_store_append( GTK_LIST_STORE(model), &iter );
		gtk_list_store_set( GTK_LIST_STORE(model), &iter, 0, g_strdup(title), 1, "", 2, g_strdup(type), -1 );

	}
	gtk_widget_destroy(dialog);
}

/********************************
* EntityOptionEditor_Menu_Display
* Event:
*/
void EntityOptionEditor_Menu_Display( GtkWidget *treeview, GdkEventButton *event, GHashTable * settings )
{
	GtkWidget * menu, * menuitem;
	menu = gtk_menu_new();
	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_ADD, NULL);
	g_signal_connect(menuitem, "activate", (GCallback) EntityOptionEditor_Menu_Add, treeview);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
	gtk_widget_show_all(menu);

	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, (event != NULL ? event->button : 0), gdk_event_get_time((GdkEvent*)event) );
}

/********************************
* EntityOptionEditor_Menu_RightClick
* Event:
*/
gboolean EntityOptionEditor_Menu_RightClick( GtkWidget *treeview, GdkEventButton *event, GHashTable * settings )
{
	if ( event->type == GDK_BUTTON_PRESS && event->button == 3 )
	{
		EntityOptionEditor_Menu_Display( treeview, event, settings );
		return TRUE;
	}
	return FALSE;
}

/********************************
* EntityOptionEditor_Menu_Button
* Event:
*/
gboolean EntityOptionEditor_Menu_Button( GtkWidget *treeview, GHashTable * settings )
{
	EntityOptionEditor_Menu_Display( treeview, NULL, settings );
	return TRUE;
}

/********************************
* EntityOptionEditor_EditStoreType
* Event:
*/
void EntityOptionEditor_EditStoreType( GtkCellRendererText * cellrenderertext, gchar * path_string, gchar * new_text, GtkTreeView * treeview  )
{
	gchar * key = NULL;
	GtkTreeIter iter;
	GtkTreeModel * model = gtk_tree_view_get_model( treeview );
	GHashTable * settings = g_object_get_data( G_OBJECT(model), "runtime-hashtable" );

	gtk_tree_model_get_iter_from_string( model, &iter, path_string );
	gtk_tree_model_get( model, &iter, 0, &key, -1 );

	if ( key )
	{
		EntityOptionStruct * option = EntityOption_Lookup(settings, key);
		if ( option )
		{
			REPLACE_STRING( option->type, g_strdup(new_text) )
		}
		gtk_list_store_set( GTK_LIST_STORE(model), &iter, 2, g_strdup(new_text), -1 );
	}

}

/********************************
* EntityOptionEditor_EditStoreValue
* Event:
*/
void EntityOptionEditor_EditStoreValue( GtkCellRendererText * cellrenderertext, gchar * path_string, gchar * new_text, GtkTreeView * treeview  )
{
	gchar * key = NULL;
	GtkTreeIter iter;
	GtkTreeModel * model = gtk_tree_view_get_model( treeview );
	gtk_tree_model_get_iter_from_string( model, &iter, path_string );
	gtk_tree_model_get( model, &iter, 0, &key, -1 );

	GHashTable * settings = g_object_get_data( G_OBJECT(model), "runtime-hashtable" );
	if ( key )
	{
		EntityOption_Update( settings, key, new_text, NULL );
		gtk_list_store_set( GTK_LIST_STORE(model), &iter, 1, g_strdup(new_text), -1 );
	}

}

/********************************
* EntityOptionEditor_CreateStore
*
*/
void EntityOptionEditor_CreateStore( gchar * key, EntityOptionStruct * value, GtkListStore * liststore )
{
	GtkTreeIter iter;
	gtk_list_store_append( liststore, &iter );
	gtk_list_store_set( liststore, &iter, 0, key, 1, value->value, 2, value->type, -1 );
}

/********************************
* EntityOptionEditor_Refresh
* Event:
*/
void EntityOptionEditor_Refresh( GtkWidget *treeview, GHashTable * settings )
{
	GtkTreeModel * model = gtk_tree_view_get_model( GTK_TREE_VIEW(treeview) );
	gtk_list_store_clear( GTK_LIST_STORE(model) );
	g_hash_table_foreach(settings, (GHFunc)EntityOptionEditor_CreateStore, GTK_LIST_STORE(model) );
	return ;
}

/********************************
* EntityOptionEditor_Open
*
*/
gboolean EntityOptionEditor_Open( gchar * entity_name )
{
	gint response = 0;
	GtkWidget * dialog, * label, * tree_items;
	GHashTable * options;
	GtkListStore * store_items = NULL;
	gchar * option_path = NULL;

	/* Get Options File */
	option_path = g_strdup_printf("/scripts/%s.options", entity_name );

	options = EntityOptionParser_Load( option_path );
	if ( !options )
	{
		Meg_Error_Print( __func__, __LINE__, "EntityOptionParser_Edit");
		g_free(option_path);
		return FALSE;
	}

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create( mokoiUI_EntityOption, __func__, __LINE__ );
	g_return_val_if_fail( ui, FALSE );

	/* widget */
	dialog = GET_WIDGET( ui, "dialog" );
	label = GET_WIDGET( ui, "alchera-label" );
	tree_items = GET_WIDGET( ui, "tree_items" );
	store_items = GET_LISTSTORE( ui, "store_items" );

	/* Settings */
	g_object_set_data( G_OBJECT(store_items), "runtime-hashtable", options);

	/* Events */
	g_signal_connect( tree_items, "realize", G_CALLBACK(EntityOptionEditor_Refresh), options );
	g_signal_connect( tree_items, "popup-menu", G_CALLBACK(EntityOptionEditor_Menu_Button), options );
	g_signal_connect( tree_items, "button-press-event", G_CALLBACK(EntityOptionEditor_Menu_RightClick), options );

	SET_OBJECT_SIGNAL( ui, "cell_value", "edited", G_CALLBACK(EntityOptionEditor_EditStoreValue), tree_items );
	SET_OBJECT_SIGNAL( ui, "cell_type", "edited", G_CALLBACK(EntityOptionEditor_EditStoreType), tree_items );

	/* Label */
	Meg_Misc_SetLabel(label, entity_name, "Options", '\n' );

	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ) );
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Main_GetWindow() );

	/*  Display Dialog */
	response = gtk_dialog_run( GTK_DIALOG(dialog) );
	if ( response == GTK_RESPONSE_APPLY )
	{
		EntityOptionParser_Save(option_path, options);
	}
	gtk_widget_destroy( dialog);


	g_free(option_path);

	return (response == GTK_RESPONSE_ACCEPT ? TRUE : FALSE);
}



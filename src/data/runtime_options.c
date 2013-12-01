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
#include "entity_functions.h"
#include "section_functions.h"
#include "runtime_parser.h"
#include "runtime_options.h"

/* Global Variables */
extern gchar * mokoiBasePath;
extern GError * mokoiError;


/* UI */

/********************************
* RuntimeSetting_Type
*
*/
guint RuntimeSetting_Type( const gchar * type )
{
	if (type == NULL)
	{
		return RUNTIMEOPTION_NONE;
	}
	else if ( !g_ascii_strcasecmp(type, "boolean") )
	{
		return RUNTIMEOPTION_BOOLEAN;
	}
	else if ( !g_ascii_strcasecmp(type, "music") )
	{
		return RUNTIMEOPTION_MUSIC;
	}
	else if ( !g_ascii_strcasecmp(type, "soundfx") )
	{
		return RUNTIMEOPTION_SOUNDFX;
	}
	else if ( !g_ascii_strcasecmp(type, "hidden") )
	{
		return RUNTIMEOPTION_HIDDEN;
	}
	else if ( g_str_has_prefix(type, "entity") )
	{
		return RUNTIMEOPTION_ENTITY;
	}
	else if ( !g_ascii_strcasecmp(type, "section") )
	{
		return RUNTIMEOPTION_SECTION;
	}
	else if ( !g_ascii_strcasecmp(type, "map") )
	{
		return RUNTIMEOPTION_MAP;
	}
	else if ( !g_ascii_strcasecmp(type, "mapentity") )
	{
		return RUNTIMEOPTION_MAPENTITY;
	}
	return 0;
}


/********************************
* RuntimeSetting_New
*
*/
RuntimeSettingsStruct * RuntimeSetting_New( const gchar * value, const gchar * type )
{
	RuntimeSettingsStruct * options = g_new0(RuntimeSettingsStruct, 1);

	options->value = g_strdup(value);
	options->type = g_strdup(type);
	options->removable = FALSE;
	options->deleted = FALSE;
	options->widget = NULL;
	options->internal_type = RuntimeSetting_Type(type);

	return options;
}

/********************************
* RuntimeSetting_Copy
*
*/
RuntimeSettingsStruct * RuntimeSetting_Copy( RuntimeSettingsStruct * value )
{
	if ( value == NULL )
		return NULL;
	RuntimeSettingsStruct * options = g_new0(RuntimeSettingsStruct, 1);

	if ( value->value != NULL )
		options->value = g_strdup(value->value);
	else
		options->value = NULL;
	if ( value->type != NULL )
		options->type = g_strdup(value->type);
	else
		options->type = NULL;

	options->removable = FALSE;
	options->deleted = FALSE;
	options->internal_type = RuntimeSetting_Type(options->type);
	options->widget = NULL;
	return options;
}

/********************************
* RuntimeSetting_Update
*
*/
void RuntimeSetting_Update( GHashTable * settings, gchar * key, gchar * value )
{
	RuntimeSettingsStruct * option = (RuntimeSettingsStruct*)g_hash_table_lookup(settings, key);

	if ( option == NULL)
	{
		option = RuntimeSetting_New( g_strdup(value), NULL );
		g_hash_table_replace( settings, key, option );
	}
	else
	{
		REPLACE_STRING( option->value, g_strdup(value) );
	}

	if ( !g_ascii_strcasecmp( key, "id" ) || !g_ascii_strcasecmp(key, "global") || !g_ascii_strcasecmp( key, "entity" ) )
	{
		REPLACE_STRING( option->type, g_strdup("hidden") );
	}
}

/********************************
* RuntimeSetting_UpdateBoolean
*
*/
void RuntimeSetting_UpdateBoolean( GHashTable * settings, gchar * key, gboolean value )
{
	RuntimeSettingsStruct * option = (RuntimeSettingsStruct*)g_hash_table_lookup(settings, key);

	if ( option == NULL)
	{
		option = RuntimeSetting_New( g_strdup( (value ? "true" : "false") ), NULL );
		g_hash_table_replace( settings, key, option );
	}
	else
	{
		if ( option->value )
			g_free( option->value );
		option->value = NULL;
		option->value = g_strdup( (value ? "true" : "false") );
	}
}
/********************************
* RuntimeSetting_UpdateValue
*
*/
void RuntimeSetting_UpdateValue( GHashTable * settings, gchar * key, gint value )
{
	RuntimeSettingsStruct * option = (RuntimeSettingsStruct*)g_hash_table_lookup(settings, key);

	if ( option == NULL)
	{
		option = RuntimeSetting_New( g_strdup_printf("%d", value), NULL );
		g_hash_table_replace( settings, key, option );
	}
	else
	{
		if ( option->value )
			g_free( option->value );
		option->value = NULL;
		option->value = g_strdup_printf("%d", value);
	}
}

/********************************
* RuntimeSetting_GetValue
*
*/
gint RuntimeSetting_GetValue( GHashTable * settings, gchar * key )
{
	gint value = 0;
	RuntimeSettingsStruct * option = (RuntimeSettingsStruct*)g_hash_table_lookup(settings, key);

	if ( option != NULL && option->value != NULL )
	{
		value = (gint)g_ascii_strtoll(option->value, NULL, 10);
	}
	return value;
}

/********************************
* RuntimeSetting_Delete
*
*/
void RuntimeSetting_Delete( RuntimeSettingsStruct * data )
{
	if ( data == NULL )
		return;

	if ( data->value )
		g_free( data->value );
	if ( data->type )
		g_free( data->type );

	data->value = NULL;
	data->type = NULL;
}

/********************************
* RuntimeParser_Append
*
*/
void RuntimeSetting_Append( gchar * key, RuntimeSettingsStruct * value, GHashTable * table )
{
	/*
	if ( !g_hash_table_lookup_extended( table, key, NULL, NULL) )
	{
		g_hash_table_insert( table, g_strdup(key), RuntimeSetting_Copy(value) );
	}
	*/


	RuntimeSettingsStruct * setting = (RuntimeSettingsStruct *)g_hash_table_lookup( table, key );
	if ( setting == NULL )
	{
		g_hash_table_insert( table, g_strdup(key), RuntimeSetting_Copy(value) );
	}
	else
	{
		setting->type = g_strdup(value->type);
		setting->internal_type = RuntimeSetting_Type(value->type);
	}
}



/********************************
* RuntimeSetting_SectionChanged
*
@
@
*/
void RuntimeSetting_SectionChanged( GtkComboBox * widget, GtkWidget * list )
{
	MokoiSectionFile * section = NULL;
	GtkTreeModel * model = NULL;
	GtkWidget * map_widget = (GtkWidget *)g_object_get_data( G_OBJECT(list), "options-map-widget" );
	gchar * text = Meg_ComboText_GetText( GTK_COMBO_BOX(widget) );

	if ( !map_widget || !text )
	{
		return;
	}

	model = gtk_combo_box_get_model( GTK_COMBO_BOX(map_widget) );
	gtk_list_store_clear( GTK_LIST_STORE(model) );
	section =  Section_Get( text );
	if (section)
	{
		GSList * list = section->maps;
		while ( list )
		{
			MokoiSectionMap * w = (MokoiSectionMap *)list->data;
			gchar * name = g_strdup_printf("%d: %s", w->position.x + (64*w->position.y), w->name );

			Meg_ComboText_AppendText( GTK_COMBO_BOX(map_widget), name );
			list = g_slist_next(list);
		}
	}
}

/********************************
* RuntimeSetting_MapChanged
*
@
@
*/
void RuntimeSetting_MapChanged( GtkComboBox * widget, GtkWidget * list )
{
	GtkTreeModel * model = NULL;
	GtkWidget * map_widget = (GtkWidget *)g_object_get_data( G_OBJECT(list), "options-mapentities-widget" );

	gchar * map_text = NULL;
	gchar * text = Meg_ComboText_GetText( GTK_COMBO_BOX(widget) );
	if (  !map_widget || !text )
	{
		return;
	}
	gchar ** text_array = g_strsplit(text," ", 2);
	if ( g_strv_length(text_array) == 2 )
	{
		map_text = text_array[1];
	}

	model = gtk_combo_box_get_model( GTK_COMBO_BOX(map_widget) );
	gtk_list_store_clear( GTK_LIST_STORE(model) );

	gchar * file_content = NULL;
	gchar * list_file = g_strconcat( mokoiBasePath, "/maps/", map_text, ".xml.entities", NULL );

	g_file_get_contents(list_file, &file_content, NULL, &mokoiError);
	if ( mokoiError )
	{
		g_clear_error( &mokoiError );
	}
	else
	{
		guint lc = 0;
		gchar ** lines = g_strsplit_set(file_content, "\n", -1);
		if ( g_strv_length(lines) )
		{
			while(lines[lc] != NULL)
			{
				gchar * tab = g_strstr_len( lines[lc], -1, "\t");
				if ( tab )
				{
					tab[1] = '\0';
					Meg_ComboText_AppendText( GTK_COMBO_BOX(map_widget), g_strdup(g_strstrip(lines[lc])) );
				}
				lc++;
			}
		}
	}
	g_strfreev(text_array);
}

/********************************
* RuntimeSetting_BooleanCheck
*
@
@
*/
gboolean RuntimeSetting_BooleanCheck( GHashTable * settings, gchar * value )
{
	RuntimeSettingsStruct * hash_value = (RuntimeSettingsStruct*)g_hash_table_lookup(settings, value);
	if ( hash_value )
	{
		if ( hash_value->value )
		{
			if ( !g_ascii_strcasecmp( hash_value->value, "true" ) )
			{
				return 1;
			}
		}
	}
	return 0;
}

/********************************
* RuntimeSetting_AddOption
*
@
@
*/
void RuntimeSetting_AddOption( GtkButton * button, GtkWidget * table )
{
	GtkWidget * dialog, * label;

	/* Create the widgets */
	dialog = gtk_dialog_new_with_buttons( "New Option", NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL );
	label = gtk_entry_new();

	/* Add the label, and show everything we've added to the dialog.*/
	gtk_container_add( GTK_CONTAINER (gtk_dialog_get_content_area( GTK_DIALOG(dialog) )), label );
	gtk_widget_show( label );


	if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
	{
		const gchar * title = gtk_entry_get_text( GTK_ENTRY(label) );

		GHashTable * settings = g_object_get_data( G_OBJECT(table), "runtime-hashtable" );

		RuntimeSettingsStruct * option = RuntimeSetting_New("", "");

		g_hash_table_insert( settings, g_strdup(title), option );
		RuntimeSetting_CreateWidget( (gchar*)title, option, table );
		RuntimeSetting_AttachWidget( (gchar*)title, option, table );

		gtk_widget_show_all( table );
	}

	gtk_widget_destroy(dialog);

}


void RuntimeSetting_EditPopup(GtkMenuItem *menuitem, gpointer user_data )
{
	RuntimeSettingsStruct * option = (RuntimeSettingsStruct*)user_data;
	GtkWidget *dialog, * table, *content_area;

	/* Create the widgets */
	dialog = gtk_dialog_new_with_buttons( "Edit Settings",
										  Meg_Main_GetWindow(),
										  GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL,
										  GTK_STOCK_OK,
										  GTK_RESPONSE_ACCEPT,
										  NULL);
	content_area = gtk_dialog_get_content_area( GTK_DIALOG(dialog) );
	table = gtk_table_new(1,3,FALSE); /* GTK3 FIXES */

	const gchar * menu_text = gtk_menu_item_get_label(menuitem);


	gtk_container_add( GTK_CONTAINER(content_area), table );


	RuntimeSetting_CreateWidget( menu_text, option, table );
	RuntimeSetting_AttachWidget( menu_text, option, table );

	gtk_widget_show_all( table );
	if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
	{
		RuntimeSetting_SaveWidget_Foreach( NULL, option, NULL );
	}

	gtk_widget_destroy( dialog );

}


/********************************
* RuntimeSetting_MenuItem
* Event:
*/
void RuntimeSetting_MenuItem( const gchar * name, RuntimeSettingsStruct * options, GtkWidget * list )
{

	g_return_if_fail( options );

	GtkWidget * item = gtk_menu_item_new_with_label( name );

	gtk_menu_shell_append( GTK_MENU_SHELL(list), GTK_WIDGET(item) );
	g_signal_connect( G_OBJECT(item), "activate", G_CALLBACK(RuntimeSetting_EditPopup), options );
}

/********************************
* RuntimeSetting_Changed_Combo
* Event:
*/
void RuntimeSetting_Changed_Combo( GtkComboBox * widget, gpointer data )
{
	g_return_if_fail( data );

	RuntimeSettingsStruct * options = (RuntimeSettingsStruct *)data;

	RuntimeSetting_SaveWidget_Foreach( NULL, options, NULL );
}

/********************************
* RuntimeSetting_Changed_Toggle
* Event:
*/
void RuntimeSetting_Changed_Toggle( GtkToggleButton * widget, gpointer data )
{
	g_return_if_fail( data );

	RuntimeSettingsStruct * options = (RuntimeSettingsStruct *)data;

	RuntimeSetting_SaveWidget_Foreach( NULL, options, NULL );
}

/********************************
* RuntimeSetting_Changed_Entry
* Event:
*/
void RuntimeSetting_Changed_Entry( GtkEntry * widget, gpointer data )
{
	g_return_if_fail( data );

	RuntimeSettingsStruct * options = (RuntimeSettingsStruct *)data;

	RuntimeSetting_SaveWidget_Foreach( NULL, options, NULL );

}

/********************************
* RuntimeSetting_CreateWidget
* Event:
*/
void RuntimeSetting_CreateWidget( const gchar * name, RuntimeSettingsStruct * options, GtkWidget * list )
{
	g_return_if_fail( options );

	GtkWidget * value_widget;
	switch ( options->internal_type )
	{
		case RUNTIMEOPTION_BOOLEAN:
			value_widget = options->widget = gtk_check_button_new_with_label("Enable");
			if ( options->value )
				gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(value_widget), (!g_ascii_strcasecmp(options->value, "true") ? 1 : 0) );
			break;
		case RUNTIMEOPTION_MUSIC:
			value_widget = options->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( options->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), options->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget),0);
			}
			Meg_ComboFile_Scan( value_widget, "music", NULL, TRUE, 0 );
			break;
		case RUNTIMEOPTION_SOUNDFX:
			value_widget = options->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( options->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), options->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget),0);
			}
			Meg_ComboFile_Scan( value_widget, "soundfx", NULL, TRUE, 0 );
			break;
		case RUNTIMEOPTION_ENTITY:
			value_widget = options->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( options->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), options->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}
			gchar * prefix = NULL;
			if ( options->type )
			{
				prefix = g_strrstr( options->type, "|");
				if ( prefix )
					prefix++;
			}
			Meg_ComboFile_Scan( value_widget, "scripts", prefix, TRUE, 0 );
			break;
		case RUNTIMEOPTION_HIDDEN:
			return;
			break;
		case RUNTIMEOPTION_SECTION:
			value_widget = options->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( options->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), g_strdup_printf("%s.txt",options->value) );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}
			Meg_ComboFile_Scan( value_widget, "sections", ".txt", TRUE, 0 );
			g_object_set_data( G_OBJECT(list), "options-section-widget", value_widget );
			break;
		case RUNTIMEOPTION_MAP:
			value_widget = options->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( options->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), options->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}

			g_object_set_data( G_OBJECT(list), "options-map-widget", value_widget );

			break;
		case RUNTIMEOPTION_MAPENTITY:
			value_widget = options->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( options->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), options->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}

			g_object_set_data( G_OBJECT(list), "options-mapentities-widget", value_widget );
			break;
		default:
			value_widget = options->widget = gtk_entry_new();
			if ( options->value )
				gtk_entry_set_text( GTK_ENTRY(value_widget), options->value );
			break;
	}
}

void RuntimeSetting_CreateWidgetWithSignal( const gchar * name, RuntimeSettingsStruct * options, GtkWidget * list )
{
	g_return_if_fail( options );

	GtkWidget * value_widget;
	switch ( options->internal_type )
	{
		case RUNTIMEOPTION_BOOLEAN:
			value_widget = options->widget = gtk_check_button_new_with_label("Enable");
			if ( options->value )
				gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(value_widget), (!g_ascii_strcasecmp(options->value, "true") ? 1 : 0) );
			g_signal_connect( G_OBJECT(value_widget), "toggled", G_CALLBACK(RuntimeSetting_Changed_Toggle), options );
			break;
		case RUNTIMEOPTION_MUSIC:
			value_widget = options->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( options->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), options->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget),0);
			}
			Meg_ComboFile_Scan( value_widget, "music", NULL, TRUE, 0 );
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(RuntimeSetting_Changed_Combo), options );
			break;
		case RUNTIMEOPTION_SOUNDFX:
			value_widget = options->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( options->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), options->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget),0);
			}
			Meg_ComboFile_Scan( value_widget, "soundfx", NULL, TRUE, 0 );
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(RuntimeSetting_Changed_Combo), options );
			break;
		case RUNTIMEOPTION_ENTITY:
			value_widget = options->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( options->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), options->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}
			gchar * prefix = NULL;
			if ( options->type )
			{
				prefix = g_strrstr( options->type, "|");
				if ( prefix )
					prefix++;
			}
			Meg_ComboFile_Scan( value_widget, "scripts", prefix, TRUE, 0 );
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(RuntimeSetting_Changed_Combo), options );
			break;
		case RUNTIMEOPTION_HIDDEN:
			return;
			break;
		case RUNTIMEOPTION_SECTION:
			value_widget = options->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( options->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), g_strdup_printf("%s.txt",options->value) );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}
			Meg_ComboFile_Scan( value_widget, "sections", ".txt", TRUE, 0 );
			g_object_set_data( G_OBJECT(list), "options-section-widget", value_widget );
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(RuntimeSetting_Changed_Combo), options );
			break;
		case RUNTIMEOPTION_MAP:
			value_widget = options->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( options->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), options->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}

			g_object_set_data( G_OBJECT(list), "options-map-widget", value_widget );
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(RuntimeSetting_Changed_Combo), options );

			break;
		case RUNTIMEOPTION_MAPENTITY:
			value_widget = options->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( options->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), options->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}

			g_object_set_data( G_OBJECT(list), "options-mapentities-widget", value_widget );
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(RuntimeSetting_Changed_Combo), options );
			break;
		default:
			value_widget = options->widget = gtk_entry_new();
			if ( options->value )
				gtk_entry_set_text( GTK_ENTRY(value_widget), options->value );
			g_signal_connect( G_OBJECT(value_widget), "activate", G_CALLBACK(RuntimeSetting_Changed_Entry), options );
			break;
	}
}


/********************************
* RuntimeSetting_AttachWidget
*
*@ name:
*@ options:
*@ list
*/
void RuntimeSetting_AttachWidget( const gchar * name, RuntimeSettingsStruct * options, GtkWidget * list )
{
	g_return_if_fail( options );

	guint yvalue = GPOINTER_TO_UINT(g_object_get_data( G_OBJECT(list), "table-y" ));
	GtkWidget * label = NULL, * delete_button = NULL;

	switch ( options->internal_type )
	{
		case RUNTIMEOPTION_HIDDEN:
			return;
			break;
		case RUNTIMEOPTION_SECTION: // section widget
			label = gtk_label_new( name );
			g_signal_connect( G_OBJECT(options->widget), "changed", G_CALLBACK(RuntimeSetting_SectionChanged), (gpointer) list );
			break;
		case RUNTIMEOPTION_MAP: // map
			label = gtk_label_new( name );
			g_signal_connect( G_OBJECT(options->widget), "changed", G_CALLBACK(RuntimeSetting_MapChanged), (gpointer) list );
			break;
		case RUNTIMEOPTION_MAPENTITY: // map's entities
			label = gtk_label_new( name );
			break;
		default:
			label = gtk_label_new( name );
			break;
	}

	if ( options->removable )
	{
		delete_button = gtk_button_new_from_stock( GTK_STOCK_REMOVE );
	}

	gtk_table_attach( GTK_TABLE(list), label, 0,1, yvalue, yvalue+1, GTK_FILL|GTK_EXPAND, 0, 2, 2); /* FIX: GTK3 */
	if ( delete_button )
	{
		gtk_table_attach( GTK_TABLE(list), options->widget, 1,2, yvalue, yvalue+1, GTK_FILL|GTK_EXPAND, 0, 2, 2); /* FIX: GTK3 */
		gtk_table_attach( GTK_TABLE(list), delete_button, 2,3, yvalue, yvalue+1, GTK_EXPAND, 0, 2, 2); /* FIX: GTK3 */
	}
	else
	{
		gtk_table_attach( GTK_TABLE(list), options->widget, 1,3, yvalue, yvalue+1, GTK_FILL|GTK_EXPAND, 0, 2, 2); /* FIX: GTK3 */
	}
	yvalue++;

	g_object_set_data( G_OBJECT(list), "table-y", GUINT_TO_POINTER(yvalue) );

}

/********************************
* RuntimeSetting_Save_Foreach
*
@ name:
@ options:
@ data:
*/
void RuntimeSetting_SaveWidget_Foreach( const gchar * name, RuntimeSettingsStruct * options, gpointer data )
{
	g_return_if_fail( options );
	g_return_if_fail( options->widget );

	if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(options->widget), "GtkEntry" ) )
	{
		const gchar * value_str = gtk_entry_get_text( GTK_ENTRY(options->widget) );
		options->value = g_strdup( value_str );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(options->widget), "GtkSpinButton" ) )
	{
		/* gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(options->widget)) */
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(options->widget), "GtkToggleButton" ) || !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(options->widget), "GtkCheckButton" ) )
	{
		options->value = g_strdup( (gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(options->widget)) ? "true" : "false") );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(options->widget), "GtkHScale" ) )
	{

	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(options->widget), "GtkComboBox" ) || !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(options->widget), "GtkComboBoxEntry" ) )
	{

		if  ( options->internal_type == RUNTIMEOPTION_SECTION )
		{
			gchar * text = Meg_ComboText_GetText( GTK_COMBO_BOX(options->widget) );

			options->value = g_strndup(text, g_utf8_strlen(text, -1) - 4 ); // Strip .txt
			g_free(text);
		}
		else
		{
			options->value =  Meg_ComboText_GetText( GTK_COMBO_BOX(options->widget) );
		}
	}
}

/********************************
* RuntimeSetting_SetDefaultValue
*
@ name:
@ options:
@ data:
*/
void RuntimeSetting_SetDefaultValues( MokoiMapObject * object )
{
	/* Set Default Runtime options */
	if ( object->entity_file && object->entity_language )
	{
		gchar * option_path = g_strdup_printf("/scripts/%s.%s.options", object->entity_file, object->entity_language );

		GHashTable * default_settings = RuntimeParser_Load( option_path );
		g_hash_table_foreach( default_settings, (GHFunc)RuntimeSetting_Append, (gpointer)object->settings );
		g_hash_table_remove_all( default_settings );
		g_free(option_path);


	}
}

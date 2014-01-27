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
	RuntimeSettingsStruct * option = g_new0(RuntimeSettingsStruct, 1);

	option->value = g_strdup(value);
	option->type = g_strdup(type);
	option->removable = FALSE;
	option->deleted = FALSE;
	option->widget = NULL;
	option->internal_type = RuntimeSetting_Type(type);

	return option;
}

/********************************
* RuntimeSettings_FreePointer
*
*/
void RuntimeSettings_FreePointer( gpointer data )
{
	RuntimeSettingsStruct * option = (RuntimeSettingsStruct *)data;
	CLEAR_STRING(option->value)
	CLEAR_STRING(option->type)

	if ( option->widget )
	{
		g_object_unref( option->widget );
	}
}


/********************************
* RuntimeSetting_Copy
*
*/
RuntimeSettingsStruct * RuntimeSetting_Copy( RuntimeSettingsStruct * value )
{
	if ( value == NULL )
		return NULL;

	RuntimeSettingsStruct * option = g_new0(RuntimeSettingsStruct, 1);

	if ( value->value != NULL )
		option->value = g_strdup(value->value);
	else
		option->value = NULL;

	if ( value->type != NULL )
		option->type = g_strdup(value->type);
	else
		option->type = NULL;

	option->removable = FALSE;
	option->deleted = FALSE;
	option->internal_type = RuntimeSetting_Type(option->type);
	option->widget = NULL;
	return option;
}


/********************************
* RuntimeSetting_InsertNew
*
*/
void RuntimeSetting_InsertNew( GHashTable * settings_table, const gchar * key, const gchar * value, const gchar * type )
{
	RuntimeSettingsStruct * option = RuntimeSetting_New( value, type );

	if ( !g_ascii_strcasecmp( key, "id" ) || !g_ascii_strcasecmp(key, "global") || !g_ascii_strcasecmp( key, "entity" ) )
	{
		REPLACE_STRING( option->type, g_strdup("hidden") );
	}

	g_hash_table_insert( settings_table, g_strdup(key), option );
}

/********************************
* RuntimeSetting_Update
*
*/
void RuntimeSetting_Update( GHashTable * settings_table, const gchar * key, const gchar * value, const gchar * type  )
{
	RuntimeSettingsStruct * option = (RuntimeSettingsStruct*)g_hash_table_lookup(settings_table, key);

	if ( option == NULL )
	{
		RuntimeSetting_InsertNew( settings_table, key, value, type );
	}
	else
	{
		REPLACE_STRING_DUPE( option->value, value );

		if ( type ) // Only Change if need
		{
			REPLACE_STRING_DUPE( option->type, type );
		}
	}
}

/********************************
* RuntimeSetting_UpdateBoolean
*
*/
void RuntimeSetting_UpdateBoolean( GHashTable * settings_table, const gchar * key, const gboolean value, const gchar * type )
{
	RuntimeSettingsStruct * option = (RuntimeSettingsStruct*)g_hash_table_lookup(settings_table, key);

	if ( option == NULL )
	{
		RuntimeSetting_InsertNew( settings_table, key, (value ? "true" : "false"), type );
	}
	else
	{
		REPLACE_STRING_DUPE( option->value, (value ? "true" : "false") );

		if ( type ) // Only Change if need
		{
			REPLACE_STRING_DUPE( option->type, type );
		}
	}
}
/********************************
* RuntimeSetting_UpdateValue
*
*/
void RuntimeSetting_UpdateValue( GHashTable * settings_table,const gchar * key, const gint value, const gchar * type )
{
	RuntimeSettingsStruct * option = (RuntimeSettingsStruct*)g_hash_table_lookup(settings_table, key);

	gchar * text = g_strdup_printf("%d", value);

	if ( option == NULL )
	{
		RuntimeSetting_InsertNew( settings_table, key, text, type );
	}
	else
	{
		REPLACE_STRING_DUPE( option->value, text );

		if ( type ) // Only Change if need
		{
			REPLACE_STRING_DUPE( option->type, type );
		}
	}

	g_free(text);
}

/********************************
* RuntimeSetting_GetValue
*
*/
gint RuntimeSetting_GetValue( GHashTable * settings_table, gchar * key )
{
	gint value = 0;
	RuntimeSettingsStruct * option = (RuntimeSettingsStruct*)g_hash_table_lookup(settings_table, key);

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

	CLEAR_STRING( data->value );
	CLEAR_STRING( data->type );

}

/********************************
* RuntimeParser_Append
*
*/
void RuntimeSetting_Append( const gchar * key, RuntimeSettingsStruct * value, GHashTable * table )
{
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
gboolean RuntimeSetting_BooleanCheck( GHashTable * settings_table, gchar * value )
{
	RuntimeSettingsStruct * hash_value = (RuntimeSettingsStruct*)g_hash_table_lookup(settings_table, value);
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
void RuntimeSetting_MenuItem( const gchar * name, RuntimeSettingsStruct * option, GtkWidget * list )
{
	g_return_if_fail( option );

	GtkWidget * item = gtk_menu_item_new_with_label( name );

	gtk_menu_shell_append( GTK_MENU_SHELL(list), GTK_WIDGET(item) );
	g_signal_connect( G_OBJECT(item), "activate", G_CALLBACK(RuntimeSetting_EditPopup), option );
}

/********************************
* RuntimeSetting_Changed_Combo
* Event:
*/
void RuntimeSetting_Changed_Combo( GtkComboBox * widget, gpointer data )
{
	g_return_if_fail( data );

	RuntimeSettingsStruct * option = (RuntimeSettingsStruct *)data;

	RuntimeSetting_SaveWidget_Foreach( NULL, option, NULL );
}

/********************************
* RuntimeSetting_Changed_Toggle
* Event:
*/
void RuntimeSetting_Changed_Toggle( GtkToggleButton * widget, gpointer data )
{
	g_return_if_fail( data );

	RuntimeSettingsStruct * option = (RuntimeSettingsStruct *)data;

	RuntimeSetting_SaveWidget_Foreach( NULL, option, NULL );
}

/********************************
* RuntimeSetting_Changed_Entry
* Event:
*/
void RuntimeSetting_Changed_Entry( GtkEntry * widget, gpointer data )
{
	g_return_if_fail( data );

	RuntimeSettingsStruct * option = (RuntimeSettingsStruct *)data;

	RuntimeSetting_SaveWidget_Foreach( NULL, option, NULL );

}

/********************************
* RuntimeSetting_CreateWidget
* Event:
*/
void RuntimeSetting_CreateWidget( const gchar * name, RuntimeSettingsStruct * option, GtkWidget * list )
{
	g_return_if_fail( option );

	GtkWidget * value_widget;
	switch ( option->internal_type )
	{
		case RUNTIMEOPTION_BOOLEAN:
			value_widget = option->widget = gtk_check_button_new_with_label("Enable");
			if ( option->value )
				gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(value_widget), (!g_ascii_strcasecmp(option->value, "true") ? 1 : 0) );
			break;
		case RUNTIMEOPTION_MUSIC:
			value_widget = option->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( option->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), option->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget),0);
			}
			Meg_ComboFile_Scan( value_widget, "music", NULL, TRUE, 0 );
			break;
		case RUNTIMEOPTION_SOUNDFX:
			value_widget = option->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( option->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), option->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget),0);
			}
			Meg_ComboFile_Scan( value_widget, "soundfx", NULL, TRUE, 0 );
			break;
		case RUNTIMEOPTION_ENTITY:
			value_widget = option->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( option->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), option->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}
			gchar * prefix = NULL;
			if ( option->type )
			{
				prefix = g_strrstr( option->type, "|");
				if ( prefix )
					prefix++;
			}
			Meg_ComboFile_Scan( value_widget, "scripts", prefix, TRUE, 0 );
			break;
		case RUNTIMEOPTION_HIDDEN:
			return;
			break;
		case RUNTIMEOPTION_SECTION:
			value_widget = option->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( option->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), g_strdup_printf("%s.txt",option->value) );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}
			Meg_ComboFile_Scan( value_widget, "sections", ".txt", TRUE, 0 );
			g_object_set_data( G_OBJECT(list), "option-section-widget", value_widget );
			break;
		case RUNTIMEOPTION_MAP:
			value_widget = option->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( option->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), option->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}

			g_object_set_data( G_OBJECT(list), "option-map-widget", value_widget );

			break;
		case RUNTIMEOPTION_MAPENTITY:
			value_widget = option->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( option->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), option->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}

			g_object_set_data( G_OBJECT(list), "option-mapentities-widget", value_widget );
			break;
		default:
			value_widget = option->widget = gtk_entry_new();
			if ( option->value )
				gtk_entry_set_text( GTK_ENTRY(value_widget), option->value );
			break;
	}
}

void RuntimeSetting_CreateWidgetWithSignal( const gchar * name, RuntimeSettingsStruct * option, GtkWidget * list )
{
	g_return_if_fail( option );

	GtkWidget * value_widget;
	switch ( option->internal_type )
	{
		case RUNTIMEOPTION_BOOLEAN:
			value_widget = option->widget = gtk_check_button_new_with_label("Enable");
			if ( option->value )
				gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(value_widget), (!g_ascii_strcasecmp(option->value, "true") ? 1 : 0) );
			g_signal_connect( G_OBJECT(value_widget), "toggled", G_CALLBACK(RuntimeSetting_Changed_Toggle), option );
			break;
		case RUNTIMEOPTION_MUSIC:
			value_widget = option->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( option->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), option->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget),0);
			}
			Meg_ComboFile_Scan( value_widget, "music", NULL, TRUE, 0 );
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(RuntimeSetting_Changed_Combo), option );
			break;
		case RUNTIMEOPTION_SOUNDFX:
			value_widget = option->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( option->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), option->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget),0);
			}
			Meg_ComboFile_Scan( value_widget, "soundfx", NULL, TRUE, 0 );
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(RuntimeSetting_Changed_Combo), option );
			break;
		case RUNTIMEOPTION_ENTITY:
			value_widget = option->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( option->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), option->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}
			gchar * prefix = NULL;
			if ( option->type )
			{
				prefix = g_strrstr( option->type, "|");
				if ( prefix )
					prefix++;
			}
			Meg_ComboFile_Scan( value_widget, "scripts", prefix, TRUE, 0 );
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(RuntimeSetting_Changed_Combo), option );
			break;
		case RUNTIMEOPTION_HIDDEN:
			return;
			break;
		case RUNTIMEOPTION_SECTION:
			value_widget = option->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( option->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), g_strdup_printf("%s.txt",option->value) );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}
			Meg_ComboFile_Scan( value_widget, "sections", ".txt", TRUE, 0 );
			g_object_set_data( G_OBJECT(list), "option-section-widget", value_widget );
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(RuntimeSetting_Changed_Combo), option );
			break;
		case RUNTIMEOPTION_MAP:
			value_widget = option->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( option->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), option->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}

			g_object_set_data( G_OBJECT(list), "option-map-widget", value_widget );
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(RuntimeSetting_Changed_Combo), option );

			break;
		case RUNTIMEOPTION_MAPENTITY:
			value_widget = option->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( option->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), option->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget), 0);
			}

			g_object_set_data( G_OBJECT(list), "option-mapentities-widget", value_widget );
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(RuntimeSetting_Changed_Combo), option );
			break;
		default:
			value_widget = option->widget = gtk_entry_new();
			if ( option->value )
				gtk_entry_set_text( GTK_ENTRY(value_widget), option->value );
			g_signal_connect( G_OBJECT(value_widget), "activate", G_CALLBACK(RuntimeSetting_Changed_Entry), option );
			break;
	}
}


/********************************
* RuntimeSetting_AttachWidget
*
*@ name:
*@ option:
*@ list
*/
void RuntimeSetting_AttachWidget( const gchar * name, RuntimeSettingsStruct * option, GtkWidget * list )
{
	g_return_if_fail( option );

	guint yvalue = GPOINTER_TO_UINT(g_object_get_data( G_OBJECT(list), "table-y" ));
	GtkWidget * label = NULL, * delete_button = NULL;

	switch ( option->internal_type )
	{
		case RUNTIMEOPTION_HIDDEN:
			return;
			break;
		case RUNTIMEOPTION_SECTION: // section widget
			label = gtk_label_new( name );
			g_signal_connect( G_OBJECT(option->widget), "changed", G_CALLBACK(RuntimeSetting_SectionChanged), (gpointer) list );
			break;
		case RUNTIMEOPTION_MAP: // map
			label = gtk_label_new( name );
			g_signal_connect( G_OBJECT(option->widget), "changed", G_CALLBACK(RuntimeSetting_MapChanged), (gpointer) list );
			break;
		case RUNTIMEOPTION_MAPENTITY: // map's entities
			label = gtk_label_new( name );
			break;
		default:
			label = gtk_label_new( name );
			break;
	}

	if ( option->removable )
	{
		delete_button = gtk_button_new_from_stock( GTK_STOCK_REMOVE );
	}

	gtk_table_attach( GTK_TABLE(list), label, 0,1, yvalue, yvalue+1, GTK_FILL|GTK_EXPAND, 0, 2, 2); /* FIX: GTK3 */
	if ( delete_button )
	{
		gtk_table_attach( GTK_TABLE(list), option->widget, 1,2, yvalue, yvalue+1, GTK_FILL|GTK_EXPAND, 0, 2, 2); /* FIX: GTK3 */
		gtk_table_attach( GTK_TABLE(list), delete_button, 2,3, yvalue, yvalue+1, GTK_EXPAND, 0, 2, 2); /* FIX: GTK3 */
	}
	else
	{
		gtk_table_attach( GTK_TABLE(list), option->widget, 1,3, yvalue, yvalue+1, GTK_FILL|GTK_EXPAND, 0, 2, 2); /* FIX: GTK3 */
	}
	yvalue++;

	g_object_set_data( G_OBJECT(list), "table-y", GUINT_TO_POINTER(yvalue) );

}

/********************************
* RuntimeSetting_Save_Foreach
*
@ name:
@ option:
@ data:
*/
void RuntimeSetting_SaveWidget_Foreach( const gchar * name, RuntimeSettingsStruct * option, gpointer data )
{
	g_return_if_fail( option );
	g_return_if_fail( option->widget );

	if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(option->widget), "GtkEntry" ) )
	{
		const gchar * value_str = gtk_entry_get_text( GTK_ENTRY(option->widget) );
		option->value = g_strdup( value_str );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(option->widget), "GtkSpinButton" ) )
	{
		/* gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(option->widget)) */
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(option->widget), "GtkToggleButton" ) || !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(option->widget), "GtkCheckButton" ) )
	{
		option->value = g_strdup( (gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(option->widget)) ? "true" : "false") );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(option->widget), "GtkHScale" ) )
	{

	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(option->widget), "GtkComboBox" ) || !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(option->widget), "GtkComboBoxEntry" ) )
	{

		if  ( option->internal_type == RUNTIMEOPTION_SECTION )
		{
			gchar * text = Meg_ComboText_GetText( GTK_COMBO_BOX(option->widget) );

			option->value = g_strndup(text, g_utf8_strlen(text, -1) - 4 ); // Strip .txt
			g_free(text);
		}
		else
		{
			option->value =  Meg_ComboText_GetText( GTK_COMBO_BOX(option->widget) );
		}
	}
}

/********************************
* RuntimeSetting_SetDefaultValue
*
@ name:
@ option:
@ data:
*/
void RuntimeSetting_SetDefaultValues( DisplayObject *object )
{
	/* Set Default Runtime option */
	if ( MAP_OBJECT_DATA(object)->entity_file && MAP_OBJECT_DATA(object)->entity_language )
	{
		gchar * option_path = g_strdup_printf("/scripts/%s.%s.option", MAP_OBJECT_DATA(object)->entity_file, MAP_OBJECT_DATA(object)->entity_language );

		GHashTable * default_settings = RuntimeParser_Load( option_path );
		g_hash_table_foreach( default_settings, (GHFunc)RuntimeSetting_Append, (gpointer)MAP_OBJECT_DATA(object)->settings );
		g_hash_table_remove_all( default_settings );
		g_free(option_path);


	}
}

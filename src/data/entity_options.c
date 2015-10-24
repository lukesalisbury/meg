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
#include "entity_functions.h"
#include "section_functions.h"
#include "entity_options_parser.h"
#include "entity_options.h"

/* Global Variables */
extern gchar * mokoiBasePath;
extern GError * mokoiError;


/* UI */


/* Functions */


EntitySettingsStruct * EntitySettings_Lookup( GHashTable * hash_table, const gchar * key)
{
	return (EntitySettingsStruct*)g_hash_table_lookup(hash_table, key);

}

/********************************
* EntitySettings_Type
*
*/
guint EntitySettings_Type( const gchar * type )
{
	if (type == NULL)
	{
		return ENTITYOPTION_NONE;
	}
	else if ( !g_ascii_strcasecmp(type, "boolean") )
	{
		return ENTITYOPTION_BOOLEAN;
	}
	else if ( !g_ascii_strcasecmp(type, "music") )
	{
		return ENTITYOPTION_MUSIC;
	}
	else if ( !g_ascii_strcasecmp(type, "soundfx") )
	{
		return ENTITYOPTION_SOUNDFX;
	}
	else if ( !g_ascii_strcasecmp(type, "hidden") )
	{
		return ENTITYOPTION_HIDDEN;
	}
	else if ( g_str_has_prefix(type, "entity") )
	{
		return ENTITYOPTION_ENTITY;
	}
	else if ( !g_ascii_strcasecmp(type, "string") )
	{
		return ENTITYOPTION_STRING;
	}
	else if ( !g_ascii_strcasecmp(type, "sectionmap") )
	{
		return ENTITYOPTION_SECTIONMAP;
	}
	else if ( !g_ascii_strcasecmp(type, "map") )
	{
		return ENTITYOPTION_MAP;
	}
	else if ( !g_ascii_strcasecmp(type, "mapentity") )
	{
		return ENTITYOPTION_MAPENTITY;
	}
	else if ( !g_ascii_strcasecmp(type, "number") )
	{
		return ENTITYOPTION_NUMBER;
	}
	else if ( !g_ascii_strcasecmp(type, "target") )
	{
		return ENTITYOPTION_TARGET;
	}
	else if ( !g_ascii_strcasecmp(type, "internal") )
	{
		return ENTITYOPTION_HIDDEN;
	}
	return ENTITYOPTION_NONE;
}


/********************************
* EntitySettings_New
*
*/
EntitySettingsStruct * EntitySettings_New( const gchar * value, const gchar * type )
{
	EntitySettingsStruct * option = g_new0(EntitySettingsStruct, 1);

	option->value = g_strdup(value);
	option->type = g_strdup(type);
	option->removable = TRUE;
	option->deleted = FALSE;
	option->widget = NULL;
	option->internal_type = EntitySettings_Type(type);

	return option;
}

/********************************
* EntitySettings_FreePointer
*
*/
void EntitySettings_FreePointer( gpointer data )
{
	EntitySettingsStruct * option = (EntitySettingsStruct *)data;
	CLEAR_STRING(option->value)
	CLEAR_STRING(option->type)

	if ( option->widget )
	{
		g_object_unref( option->widget );
	}
}


/********************************
* EntitySettings_Copy
*
*/
EntitySettingsStruct * EntitySettings_Copy( EntitySettingsStruct * value )
{
	if ( value == NULL )
		return NULL;

	EntitySettingsStruct * option = g_new0(EntitySettingsStruct, 1);

	if ( value->value != NULL )
		option->value = g_strdup(value->value);
	else
		option->value = NULL;

	if ( value->type != NULL )
		option->type = g_strdup(value->type);
	else
		option->type = NULL;

	option->removable = value->removable;
	option->deleted = value->deleted;
	option->internal_type = EntitySettings_Type(option->type);
	option->widget = NULL;
	return option;
}


/********************************
* EntitySettings_InsertNew
*
*/
EntitySettingsStruct * EntitySettings_InsertNew( GHashTable * settings_table, const gchar * key, const gchar * value, const gchar * type )
{
	EntitySettingsStruct * option = EntitySettings_New( value, type );

	if ( !g_ascii_strcasecmp( key, "id" ) || !g_ascii_strcasecmp(key, "global") || !g_ascii_strcasecmp( key, "entity" ) )
	{
		REPLACE_STRING( option->type, g_strdup("hidden") );
	}

	g_hash_table_insert( settings_table, g_strdup(key), option );

	return option;
}

/**
 * @brief EntitySettings_UpdateType
 * @param settings_table
 * @param key
 * @param type
 */
void EntitySettings_UpdateType( GHashTable * settings_table, const gchar * key,  const gchar * type  )
{
	EntitySettingsStruct * option = EntitySettings_Lookup(settings_table, key);
	if ( option )
	{
		REPLACE_STRING_DUPE( option->type, type );
	}
}


/********************************
* EntitySettings_Update
*
*/
void EntitySettings_Update( GHashTable * settings_table, const gchar * key, const gchar * value, const gchar * type  )
{
	EntitySettingsStruct * option = EntitySettings_Lookup(settings_table, key);

	if ( option == NULL )
	{
		EntitySettings_InsertNew( settings_table, key, value, type );
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
* EntitySettings_UpdateBoolean
*
*/
void EntitySettings_UpdateBoolean( GHashTable * settings_table, const gchar * key, const gboolean value, const gchar * type )
{
	EntitySettingsStruct * option = EntitySettings_Lookup(settings_table, key);

	if ( option == NULL )
	{
		EntitySettings_InsertNew( settings_table, key, (value ? "true" : "false"), type );
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
* EntitySettings_UpdateValue
*
*/
void EntitySettings_UpdateValue( GHashTable * settings_table,const gchar * key, const gint value, const gchar * type )
{
	EntitySettingsStruct * option = EntitySettings_Lookup(settings_table, key);

	gchar * text = g_strdup_printf("%d", value);

	if ( option == NULL )
	{
		EntitySettings_InsertNew( settings_table, key, text, type );
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
* EntitySettings_GetValue
*
*/
gint EntitySettings_GetValue( GHashTable * settings_table, gchar * key )
{
	gint value = 0;
	EntitySettingsStruct * option = EntitySettings_Lookup(settings_table, key);

	if ( option != NULL && option->value != NULL )
	{
		value = (gint)g_ascii_strtoll(option->value, NULL, 10);
	}
	return value;
}

/********************************
* EntitySettings_Delete
*
*/
void EntitySettings_Delete( EntitySettingsStruct * data )
{
	if ( data == NULL )
		return;

	CLEAR_STRING( data->value );
	CLEAR_STRING( data->type );

}

/********************************
* EntitySettings_Parser_Append
*
*/
void EntitySettings_Append( const gchar * key, EntitySettingsStruct * value, GHashTable * table )
{
	EntitySettingsStruct * setting = (EntitySettingsStruct *)g_hash_table_lookup( table, key );
	if ( setting == NULL )
	{
		g_hash_table_insert( table, g_strdup(key), EntitySettings_Copy(value) );
	}
	else
	{
		setting->removable = value->removable;
		setting->type = g_strdup(value->type);
		setting->internal_type = EntitySettings_Type(value->type);
	}
}



/********************************
* EntitySettings_SectionChanged
*
@
@
*/
void EntitySettings_SectionChanged( GtkComboBox * widget, gchar * default_value )
{
	MokoiWorldFile * section = NULL;
	GtkTreeModel * model = NULL;
	GtkWidget * map_widget = (GtkWidget *)g_object_get_data( G_OBJECT(widget), "options-map-widget" );
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
			MokoiWorldMap * w = (MokoiWorldMap *)list->data;
			gchar * name = g_strdup_printf("%d: %s", w->position.x + (64*w->position.y), w->name );

			Meg_ComboText_AppendText( GTK_COMBO_BOX(map_widget), name );
			list = g_slist_next(list);
		}

		Meg_ComboText_SetIndex_Prefix( GTK_COMBO_BOX(map_widget), default_value, ":");
	}
}

/********************************
* EntitySettings_MapChanged
*
@
@
*/
void EntitySettings_MapChanged( GtkComboBox * widget, gchar * default_value )
{
	GtkTreeModel * model = NULL;
	GtkWidget * map_widget = (GtkWidget *)g_object_get_data( G_OBJECT(widget), "options-mapentities-widget" );

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

	Meg_ComboText_SetIndex( GTK_COMBO_BOX(map_widget), default_value);
	g_strfreev(text_array);
}

/********************************
* EntitySettings_BooleanCheck
*
@
@
*/
gboolean EntitySettings_BooleanCheck( GHashTable * settings_table, gchar * value )
{
	EntitySettingsStruct * hash_value = EntitySettings_Lookup(settings_table, value);
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
* EntitySettings_AddOption
*
@
@
*/
void EntitySettings_AddOption( GtkButton * button, GtkWidget * table )
{
	GtkWidget * dialog, * label, * combo, * vbox;

	/* Create the widgets */
	dialog = gtk_dialog_new_with_buttons( "Add New Option", Meg_Misc_ParentWindow(table), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL );
	label = gtk_entry_new();
	combo = gtk_combo_box_new();
	vbox = gtk_box_new( GTK_ORIENTATION_VERTICAL, 1 );

	Meg_ComboText_Setup( combo, TRUE );

	/**/
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "number" );
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "string" );
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "boolean" );
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "music" );
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "soundfx" );
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "entity" );
	Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), "target" );
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

		GHashTable * settings = g_object_get_data( G_OBJECT(table), "runtime-hashtable" );

		EntitySettingsStruct * option = EntitySettings_InsertNew( settings, title, "", type );


		EntitySettings_CreateWidget( (gchar*)title, option, NULL );
		EntitySettings_AttachWidget( (gchar*)title, option, table );

		gtk_widget_show_all( table );
	}

	gtk_widget_destroy(dialog);

}

/********************************
* EntitySettings_EditPopup
* Edit a single option via a popup dialog
@
@
*/
void EntitySettings_EditPopup(GtkMenuItem *menuitem, gpointer user_data )
{
	EntitySettingsStruct * option = (EntitySettingsStruct*)user_data;
	GtkWidget *dialog, *content_area;

	/* Create the widgets */
	dialog = gtk_dialog_new_with_buttons( "Edit Settings",
										  Meg_Main_GetWindow(),
										  GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL,
										  GTK_STOCK_CLOSE,
										  GTK_RESPONSE_ACCEPT,
										  NULL);
	content_area = gtk_dialog_get_content_area( GTK_DIALOG(dialog) );


	const gchar * menu_text = gtk_menu_item_get_label(menuitem);


	EntitySettings_CreateWidgetWithSignal( menu_text, option, NULL );
	EntitySettings_AttachWidget( menu_text, option, content_area );

	gtk_widget_show_all( content_area );

	if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
	{
		EntitySettings_SaveWidget_Foreach( NULL, option, NULL );
	}

	gtk_widget_destroy( dialog );

}


/********************************
* EntitySettings_MenuItem
* Event:
*/
void EntitySettings_MenuItem( const gchar * name, EntitySettingsStruct * option, GtkWidget * list )
{
	g_return_if_fail( option );

	if ( option->internal_type != ENTITYOPTION_HIDDEN )
	{
		GtkWidget * item = gtk_menu_item_new_with_label( name );

		gtk_menu_shell_append( GTK_MENU_SHELL(list), GTK_WIDGET(item) );
		g_signal_connect( G_OBJECT(item), "activate", G_CALLBACK(EntitySettings_EditPopup), option );
	}
}

/********************************
* EntitySettings_Changed_Combo
* Event:
*/
void EntitySettings_Changed_Combo( GtkComboBox * widget, gpointer data )
{
	g_return_if_fail( data );

	EntitySettingsStruct * option = (EntitySettingsStruct *)data;

	EntitySettings_SaveWidget_Foreach( NULL, option, NULL );
}

/********************************
* EntitySettings_Changed_Toggle
* Event:
*/
void EntitySettings_Changed_Toggle( GtkToggleButton * widget, gpointer data )
{
	g_return_if_fail( data );

	EntitySettingsStruct * option = (EntitySettingsStruct *)data;

	EntitySettings_SaveWidget_Foreach( NULL, option, NULL );
}

/********************************
* EntitySettings_Changed_Entry
* Event:
*/
void EntitySettings_Changed_Entry( GtkEntry * widget, gpointer data )
{
	g_return_if_fail( data );

	EntitySettingsStruct * option = (EntitySettingsStruct *)data;

	EntitySettings_SaveWidget_Foreach( NULL, option, NULL );

}

/********************************
* EntitySettings_Changed_Spin
* Event:

*/
void EntitySettings_Changed_Spin( GtkEntry * widget, gpointer data )
{
	g_return_if_fail( data );

	EntitySettingsStruct * option = (EntitySettingsStruct *)data;

	EntitySettings_SaveWidget_Foreach( NULL, option, NULL );

}


/********************************
* EntitySettings_Changed_Spin
* Event:

*/
void EntitySettings_Target_Dialog( GtkWidget *button, EntitySettingsStruct * option)
{
	g_return_if_fail( option );

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(GUI_ENTITY_TARGET_DIALOG, __func__, __LINE__);
	g_return_if_fail( ui );

	/* Widget */
	GtkWidget * dialog, * combo_world, * combo_grid, *combo_entity;

	dialog = GET_WIDGET( ui, "dialog" );
	combo_world = GET_WIDGET( ui, "combo_world" );
	combo_grid = GET_WIDGET( ui, "combo_grid" );
	combo_entity = GET_WIDGET( ui, "combo_entity" );

	Meg_ComboText_Setup( combo_world, FALSE );
	Meg_ComboText_Setup( combo_grid, FALSE );
	Meg_ComboText_Setup( combo_entity, FALSE );


	Meg_ComboFile_Scan( combo_world, "worlds", ".tsv", TRUE, 0 );

	g_object_set_data( G_OBJECT(combo_world), "options-map-widget", combo_grid );
	g_object_set_data( G_OBJECT(combo_grid), "options-mapentities-widget", combo_entity );

	/* Default values */
	if ( option->internal_type == ENTITYOPTION_TARGET && option->value)
	{
		gchar ** f = g_strsplit(option->value, ":", -1);
		if ( g_strv_length(f) == 3 )
		{

			Meg_ComboText_SetIndex_Prefix( GTK_COMBO_BOX(combo_world), f[0], ".tsv" );
			if ( f[1] )
			{
				EntitySettings_SectionChanged( GTK_COMBO_BOX(combo_world), f[1] );
				if ( f[2] )
				{
					EntitySettings_MapChanged( GTK_COMBO_BOX(combo_grid), f[2] );
				}
			}
		}
		g_strfreev(f);
	}



	/* Signals */
	g_signal_connect( G_OBJECT(combo_world), "changed", G_CALLBACK(EntitySettings_SectionChanged), NULL );
	g_signal_connect( G_OBJECT(combo_grid), "changed", G_CALLBACK(EntitySettings_MapChanged), NULL );

	/* Run Dialog */
	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ) );
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Misc_ParentWindow(button) );

	if ( GTK_RESPONSE_APPLY == gtk_dialog_run( GTK_DIALOG(dialog) ) )
	{
		gchar * text_world = Meg_ComboText_GetText( GTK_COMBO_BOX(combo_world) );
		gchar * text_grid_string = Meg_ComboText_GetText( GTK_COMBO_BOX(combo_grid) );
		gchar * text_entity = Meg_ComboText_GetText( GTK_COMBO_BOX(combo_entity) );
		gchar * text_grid = NULL;
		if ( text_grid_string )
		{
			gchar ** text_array = g_strsplit(text_grid_string, ":", 2);
			if ( g_strv_length(text_array) == 2 )
			{
				text_grid = g_strdup(text_array[0]);
			}
			g_strfreev(text_array);

		}

		if ( text_world )
		{
			gchar * world = STRIP_FILE_EXTENSION(text_world, 4); // Strip .tsv
			option->value = g_strdup_printf("%s:%s:%s", world, (text_grid ? text_grid :""), (text_entity ? text_entity :"") );
			g_free(world);
		}
	}
	gtk_widget_destroy( dialog );

}

/********************************
* EntitySettings_CreateWidget
* Event:
*/
void EntitySettings_CreateWidget(const gchar * name, EntitySettingsStruct * option, MapObjectData *object_data )
{
	g_return_if_fail( option );

	GtkWidget * value_widget;
	switch ( option->internal_type )
	{
		case ENTITYOPTION_NUMBER:
			value_widget = option->widget = gtk_spin_button_new_with_range( -32000.0, 32000.0, 1.0 );
			gtk_spin_button_set_digits( GTK_SPIN_BUTTON(value_widget), 3 );
			if ( option->value )
			{
				gdouble value = g_strtod( option->value, NULL );
				gtk_spin_button_set_value( GTK_SPIN_BUTTON(value_widget), value );
			}
			else
			{
				gtk_spin_button_set_value( GTK_SPIN_BUTTON(value_widget), 0.0 );
			}
			break;
		case ENTITYOPTION_BOOLEAN:
			value_widget = option->widget = gtk_check_button_new_with_label("Enable");
			if ( option->value )
				gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(value_widget), (!g_ascii_strcasecmp(option->value, "true") ? 1 : 0) );
			break;
		case ENTITYOPTION_MUSIC:
			value_widget = option->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( option->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), option->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget),0);
			}
			Meg_ComboFile_Scan( value_widget, "music", NULL, TRUE, 0 );
			break;
		case ENTITYOPTION_SOUNDFX:
			value_widget = option->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );
			if ( option->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), option->value );
				gtk_combo_box_set_active( GTK_COMBO_BOX(value_widget),0);
			}
			Meg_ComboFile_Scan( value_widget, "soundfx", NULL, TRUE, 0 );
			break;
		case ENTITYOPTION_ENTITY:
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
		case ENTITYOPTION_HIDDEN:
			return;
			break;
		case ENTITYOPTION_TARGET:
			value_widget = option->widget = gtk_button_new_with_label( "Select Target" );
			break;
		case ENTITYOPTION_MAPENTITY:
			value_widget = option->widget = gtk_combo_box_new( );
			Meg_ComboText_Setup( value_widget, FALSE );

			Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), "__map__" );
			Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), "__main__" );

			if ( object_data )
			{
				gchar * file_content = NULL;
				gchar * list_file = g_strconcat( mokoiBasePath, "/maps/", object_data->parent->name, ".xml.entities", NULL );

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
								Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), g_strdup(g_strstrip(lines[lc])) );
							}
							lc++;
						}
					}
				}
			}
			else if ( option->value )
			{
				Meg_ComboText_AppendText( GTK_COMBO_BOX(value_widget), option->value );
			}

			if ( option->value )
			{
				Meg_ComboText_SetIndex( GTK_COMBO_BOX(value_widget), option->value );
			}




			break;

		default:
			value_widget = option->widget = gtk_entry_new();
			if ( option->value )
				gtk_entry_set_text( GTK_ENTRY(value_widget), option->value );
			break;
	}
}

void EntitySettings_CreateWidgetWithSignal( const gchar * name, EntitySettingsStruct * option, MapObjectData * object_data )
{
	g_return_if_fail( option );

	GtkWidget * value_widget;

	EntitySettings_CreateWidget( name, option, object_data );

	value_widget = option->widget;

	g_return_if_fail( value_widget );

	switch ( option->internal_type )
	{
		case ENTITYOPTION_NUMBER:
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(EntitySettings_Changed_Spin), option );
			break;
		case ENTITYOPTION_BOOLEAN:
			g_signal_connect( G_OBJECT(value_widget), "toggled", G_CALLBACK(EntitySettings_Changed_Toggle), option );
			break;
		case ENTITYOPTION_MUSIC:
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(EntitySettings_Changed_Combo), option );
			break;
		case ENTITYOPTION_SOUNDFX:
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(EntitySettings_Changed_Combo), option );
			break;
		case ENTITYOPTION_ENTITY:
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(EntitySettings_Changed_Combo), option );
			break;
		case ENTITYOPTION_HIDDEN:
			break;
		case ENTITYOPTION_TARGET:
			g_signal_connect( G_OBJECT(value_widget), "clicked", G_CALLBACK(EntitySettings_Target_Dialog), option );
			break;
		case ENTITYOPTION_MAPENTITY:
			g_signal_connect( G_OBJECT(value_widget), "changed", G_CALLBACK(EntitySettings_Changed_Combo), option );
			break;

		default:
			g_signal_connect( G_OBJECT(value_widget), "activate", G_CALLBACK(EntitySettings_Changed_Entry), option );
			break;
	}
}



/********************************
* EntitySettings_AttachWidget
*
*@ name:
*@ option:
*@ list
*/
void EntitySettings_AttachWidget( const gchar * name, EntitySettingsStruct * option, GtkWidget * list )
{
	g_return_if_fail( option );

	GtkWidget * box = NULL,* label = NULL, * delete_button = NULL;

	box = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 2 );
	switch ( option->internal_type )
	{
		case ENTITYOPTION_HIDDEN:
			return;
			break;
		default:
			label = gtk_label_new( name );
			break;
	}

	gtk_box_pack_start( GTK_BOX(box), label, FALSE, FALSE, 2);
	gtk_box_pack_start( GTK_BOX(box), option->widget, FALSE, TRUE, 2);

	if ( option->removable )
	{
		delete_button = gtk_button_new_from_stock( GTK_STOCK_REMOVE );
		gtk_box_pack_start( GTK_BOX(box), delete_button, FALSE, FALSE, 2);
	}
	gtk_box_pack_start( GTK_BOX(list), box, FALSE, FALSE, 2);

}

/********************************
* EntitySettings_Save_Foreach
*
@ name:
@ option:
@ data:
*/
void EntitySettings_SaveWidget_Foreach( const gchar * name, EntitySettingsStruct * option, gpointer data )
{
	g_return_if_fail( option );

	if ( option->widget )
	{
		if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(option->widget), "GtkEntry" ) )
		{
			const gchar * value_str = gtk_entry_get_text( GTK_ENTRY(option->widget) );
			option->value = g_strdup( value_str );
		}
		else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(option->widget), "GtkSpinButton" ) )
		{
			gint value = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(option->widget) );
			option->value = g_strdup_printf("%d", value);
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
			option->value =  Meg_ComboText_GetText( GTK_COMBO_BOX(option->widget) );
		}
	}
}


/**
 * @brief EntitySettings_DefaultValues
 * @param object_data
 * @return
 */
GHashTable * EntitySettings_DefaultValues(MapObjectData *object_data )
{
	GHashTable * default_settings = NULL;
	/* Set Default Runtime option */
	if ( object_data->entity_file && object_data->entity_language )
	{
		gchar * option_path = g_strdup_printf("/scripts/%s.options", object_data->entity_file );
		default_settings = EntitySettings_Parser_Load( option_path );
		g_free(option_path);
	}
	return default_settings;
}


/********************************
* EntitySettings_SetDefaultValue
*
@ name:
@ option:
@ data:
*/
void EntitySettings_SetDefaultValues(MapObjectData *object_data )
{
	/* Set Default Runtime option */
	if ( object_data->entity_file && object_data->entity_language )
	{
		GHashTable * default_settings = EntitySettings_DefaultValues( object_data );

		/* Append Default Setting to Map Object  */
		g_hash_table_foreach( default_settings, (GHFunc)EntitySettings_Append, (gpointer)object_data->settings );
		g_hash_table_remove_all( default_settings );

		/* Check For Entity Global Flag */
		EntitySettingsStruct * hash_value = EntitySettings_Lookup( object_data->settings, "force-global" );
		if ( hash_value )
		{
			if ( hash_value->value )
			{
				object_data->entity_global = !g_ascii_strcasecmp( hash_value->value, "true" );
			}
		}
	}
}


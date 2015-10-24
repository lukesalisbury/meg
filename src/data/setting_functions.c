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

/* Required Headers */
#include "setting_functions.h"
#include "package.h"
#include "loader_functions.h"

/* Global Variables */
extern GError * mokoiError;
extern GKeyFile * mokoiConfigTable;
extern GHashTable * mokoiSettingsTable;
extern gchar * mokoiSettingsLocked[];
extern GtkListStore * mokoiSettingStore;

/* Events */

/********************************
* Setting_Event_ButtonSave
*
@ button:
@ table:
*/
void Setting_Event_ButtonSave(GtkButton* button, GHashTable * table )
{
	g_hash_table_foreach( table, Setting_SaveForeach, mokoiConfigTable );
	Setting_Save();
}

/********************************
* Setting_Dialog_CustomController
*
@ button:
@ widget:
*/
void Setting_Dialog_CustomController(GtkButton* button, GHashTable * table )
{
	GtkWidget * dialog = gtk_message_dialog_new( Meg_Main_GetWindow(), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_CLOSE, "No interface for modify controller yet.\nVisit http://mokoi.info/docs/Meg/Project#CustomControls to do it manually." );
	gtk_dialog_run( GTK_DIALOG(dialog) );
	gtk_widget_destroy( dialog );

}

/********************************
* Setting_Event_CustomDisplay
*
@ button:
@ widget:
*/
void Setting_Event_CustomDisplay( GtkToggleButton * button, GtkWidget * widget )
{
	if ( gtk_toggle_button_get_active(button) )
	{
		gtk_widget_set_sensitive(widget, TRUE);
	}
	else
	{
		gtk_widget_set_sensitive(widget, FALSE);
	}
}

/********************************
* Setting_Event_SetImage
*
@ button:
@ table:
*/
void Setting_Event_SetImage( GtkButton* button, gpointer data )
{
	GtkWidget * preview = NULL;
	gchar * image_path = NULL, * image_full_path = NULL;
	GtkFileFilter * filter = gtk_file_filter_new();
	GtkWidget * dialog = gtk_file_chooser_dialog_new( "Select new image", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, \
													  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, \
													  GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL );

	preview = g_object_get_data( G_OBJECT(button), "preview_widget" );
	image_path = g_object_get_data( G_OBJECT(button), "image_path" );
	image_full_path = g_build_filename( AL_ProjectPath(), image_path, NULL );


	gtk_file_filter_add_pattern( filter, "*.png" );
	gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(dialog), filter );

	if ( gtk_dialog_run( GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT )
	{
		gchar * filename = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) );
		Meg_FileCopy(filename, image_full_path);
		g_free( filename );

		if ( g_file_test( image_full_path, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR) ) )
		{
			gtk_button_set_label( button, "Change");
			gtk_image_set_from_file( GTK_IMAGE(preview), image_full_path  );
		}
		else
		{
			gtk_button_set_label( button, "Set");
		}
	}
	gtk_widget_destroy( dialog );

	g_free(image_full_path);
}

/**
 * @brief Settings_RefreshPackageWidget
 * @param config_str
 * @param file_extention
 */
void Settings_RefreshPackageWidget( const gchar * config_str, const gchar * file_extention )
{
	GtkWidget * package = g_hash_table_lookup( mokoiSettingsTable, config_str );

	Setting_Package( GTK_COMBO_BOX(package), file_extention );
	if ( package )
	{
		gchar * setting = AL_Setting_GetString(config_str);
		if ( setting )
		{
			Meg_ComboText_SetIndex(GTK_COMBO_BOX(package), setting);
			g_free(setting);
		}
	}
}


/********************************
* Setting_Package
*
@ combo:
@ prefix:
*/
void Setting_Package( GtkComboBox * combo, const gchar * file_extension )
{
	if ( file_extension == NULL )
		return;
	const gchar * current_file;
	gchar * packages_dir;

	GDir * current_directory;
	packages_dir = Meg_Directory_Share("packages");
	current_directory = g_dir_open( packages_dir, 0, &mokoiError );
	if ( mokoiError != NULL )
	{
		g_clear_error( &mokoiError );
	}
	else
	{
		Meg_ComboText_Clear( combo );
		Meg_ComboText_AppendText( combo, g_strdup(MEG_COMBOFILE_NONE) );


		current_file = g_dir_read_name(current_directory);
		while ( current_file != NULL )
		{
			if ( g_str_has_suffix(current_file, file_extension) )
			{
				Meg_ComboText_AppendText( combo, g_strdup(current_file) );
			}
			current_file = g_dir_read_name( current_directory );
		}

		gtk_combo_box_set_active( combo, 0 );
	}
	g_free( packages_dir );



}



/********************************
* Setting_Update
*
*/
gboolean Setting_Update( GtkListStore * store, GtkTreePath * path, GtkTreeIter * iter, gboolean * result )
{
	gchar * key, * string;
	gtk_tree_model_get( GTK_TREE_MODEL(store), iter, 0, &key, 1, &string, -1 );
	if ( !g_key_file_has_key( mokoiConfigTable, "Mokoi", key, NULL ) )
	{
		g_key_file_set_value( mokoiConfigTable, "Mokoi", key, string );
	}
	g_free( key );
	g_free( string );
	return FALSE;
}

/********************************
* Setting_Save
*
*/
gboolean Setting_Save()
{
	GError * local_error = NULL;
	gchar * string = NULL;

	g_hash_table_foreach( mokoiSettingsTable, Setting_SaveForeach, mokoiConfigTable );

	/* Check if display.customsize is set */
	if ( g_key_file_get_boolean( mokoiConfigTable, "Mokoi", "display.customsize", NULL ) )
	{
		g_key_file_set_integer( mokoiConfigTable, "Mokoi", "display.width", AL_Setting_GetNumber("screen.width") );
		g_key_file_set_integer( mokoiConfigTable, "Mokoi", "display.height", AL_Setting_GetNumber("screen.height") );
	}

	string = g_key_file_to_data( mokoiConfigTable, NULL, NULL );

	Meg_file_set_contents( ROOT_FILENAME, g_strstrip(string), -1, &local_error );

	g_free( string );

	AL_Settings_Refresh();

	return Meg_Error_Check( local_error, FALSE, "Setting_Save" );
}


/********************************
* Setting_WidgetRead
* - Update Config from widget
*/
void Setting_WidgetRead(gchar * name, GObject * wid )
{
	if ( !name || !wid )
		return;

	gchar * string = NULL;
	if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkEntry" ) )
	{
		g_key_file_set_string( mokoiConfigTable, "Mokoi", name, gtk_entry_get_text( GTK_ENTRY(wid) ) );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkSpinButton" ) )
	{
		g_key_file_set_integer( mokoiConfigTable, "Mokoi", name, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(wid)) );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkCheckButton" ) )
	{
		g_key_file_set_boolean( mokoiConfigTable, "Mokoi", name, gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(wid)) );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkHScale" ) )
	{
		gint c = (gint)gtk_range_get_value( GTK_RANGE(wid) );
		g_key_file_set_integer( mokoiConfigTable, "Mokoi", name, (c/8)*8 );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkComboBox" ) )
	{
		string = Meg_ComboText_GetText( GTK_COMBO_BOX(wid) );

		if ( !g_strcmp0(name, "language.default") )
		{
			/* TODO save to language.available */
		}
		else if ( g_str_has_prefix(name, "package.") )
		{
			if ( !string )
			{
				g_key_file_remove_key( mokoiConfigTable, "Mokoi", name, NULL );
			}
			else
			{
				if ( Package_ChangeMain(name, string) )
				{
					g_key_file_set_string( mokoiConfigTable, "Mokoi", name, string );
				}
			}
		}
		else if ( string )
		{
			g_key_file_set_string( mokoiConfigTable, "Mokoi", name, string );
		}
		else
		{
			g_key_file_remove_key( mokoiConfigTable, "Mokoi", name, NULL );
		}

	}
}

/********************************
* Setting_WidgetWrite
* - Update Widget with new values
*/
void Setting_WidgetWrite(const gchar * name, GObject * wid )
{
	if ( !name || !wid )
		return;
	if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkEntry" ) )
	{
		gtk_entry_set_text( GTK_ENTRY(wid), AL_Setting_GetString(name) );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkButton" ) )
	{
		gchar * path = NULL;
		GObject * preview_widget = NULL;
		preview_widget = g_object_get_data( wid, "preview_widget" );
		path = g_object_get_data( wid, "image_path" );

		if ( preview_widget && path )
		{
			if ( g_file_test( path, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR) ) )
			{
				gtk_button_set_label( GTK_BUTTON(wid), "Change");
				gtk_image_set_from_file( GTK_IMAGE(preview_widget), path  );
			}
			else
			{
				gtk_button_set_label( GTK_BUTTON(wid), "Set");
			}
		}
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkSpinButton" ) )
	{
		gtk_spin_button_set_value( GTK_SPIN_BUTTON(wid), (gdouble)AL_Setting_GetNumber(name) );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkCheckButton" ) )
	{
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(wid), AL_SettingBoolean(name) );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkHScale" ) )
	{
		gtk_range_set_value( GTK_RANGE(wid), (gdouble)AL_Setting_GetNumber(name) );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkComboBox" ) )
	{
		if ( !g_ascii_strcasecmp("display.mode", name) )
		{
			gchar * setting = AL_Setting_GetString("display.mode");
			if ( !g_ascii_strcasecmp(setting, "OpenGL") )
				gtk_combo_box_set_active( GTK_COMBO_BOX(wid), 1);
			else
				gtk_combo_box_set_active( GTK_COMBO_BOX(wid), 0);
			g_free(setting);
		}
		else if ( !g_ascii_strcasecmp("language.default", name) )
		{
			gchar * setting = AL_Setting_GetString("language.default");
			Meg_ComboText_SetIndex(GTK_COMBO_BOX(wid), setting);
			g_free(setting);
			/* TODO read language.available */
		}
		else
		{
			gchar * setting = AL_Setting_GetString(name);
			Meg_ComboText_SetIndex(GTK_COMBO_BOX(wid), setting);
			g_free(setting);
		}
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkLabel" ) )
	{
		if ( !g_ascii_strcasecmp("project.id", name) )
		{
			gchar * ident = AL_Setting_GetString("project.id");
			gchar * markup = g_markup_printf_escaped( "%s [%X]", ident, Project_GenerateInternalID(ident) );
			gtk_label_set_markup( GTK_LABEL(wid), markup );
			g_free( ident );
			g_free( markup );
		}
		else
		{
			gtk_label_set_markup( GTK_LABEL(wid), AL_Setting_GetString(name) );
		}
	}
	else
	{

	}

}

/********************************
* Setting_WidgetClear
* - Update Widget with new values
*/
void Setting_WidgetClear(gchar * name, GObject * wid )
{
	if ( !name || !wid )
		return;
	if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkEntry" ) )
	{
		gtk_entry_set_text( GTK_ENTRY(wid), "" );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkButton" ) )
	{

	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkSpinButton" ) )
	{
		gtk_spin_button_set_value( GTK_SPIN_BUTTON(wid), 0.0 );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkCheckButton" ) )
	{
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(wid), FALSE );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkHScale" ) )
	{
		gtk_range_set_value( GTK_RANGE(wid), 0.0 );
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkComboBox" ) )
	{
		if ( !g_ascii_strcasecmp("display.mode", name) )
		{
			gtk_combo_box_set_active( GTK_COMBO_BOX(wid), 0);
		}
		else if ( !g_ascii_strcasecmp("language.default", name) )
		{
			/* TODO read language.available */
			gtk_combo_box_set_active( GTK_COMBO_BOX(wid), 0);
		}
		else
		{
			Meg_ComboText_Clear( GTK_COMBO_BOX((wid)) );
		}
	}
	else if ( !g_ascii_strcasecmp( G_OBJECT_TYPE_NAME(wid), "GtkLabel" ) )
	{
		gtk_label_set_markup( GTK_LABEL(wid), "" );
	}
	else
	{

	}

}


/********************************
* Setting_SaveForeach
* - Update Config from widget
@ key:
@ value:
@ key:
*/
void Setting_SaveForeach(gpointer key, gpointer value, gpointer user_data)
{
	gchar * name = (gchar*)key;
	GObject * wid = (GObject*)value;
	Setting_WidgetRead( name, wid );
}


/********************************
* Setting_LoadForeach
* - Update Widget with new values
@ key:
@ value:
@ key:
*/
void Setting_LoadForeach(gpointer key, gpointer value, gpointer user_data)
{
	gchar * name = (gchar*)key;
	GObject * wid = (GObject*)value;
	Setting_WidgetWrite( name, wid );
}


/********************************
* Setting_LoadForeach
* - Update Widget with new values
@ key:
@ value:
@ key:
*/
void Setting_ClearForeach(gpointer key, gpointer value, gpointer user_data)
{
	GObject * wid = (GObject*)value;
	Setting_WidgetClear( NULL, wid );
}


/********************************
* Setting_Unlocked
*
@ key:
*/
gboolean Setting_Unlocked( const char * key )
{
	guint array_count = 0;
	if ( g_strv_length( mokoiSettingsLocked ) )
	{
		while ( mokoiSettingsLocked[array_count] != NULL )
		{
			if ( !g_strcmp0(key, mokoiSettingsLocked[array_count]) )
			{
				return FALSE;
			}
			array_count++;
		}
	}
	return TRUE;
}

/********************************
* Setting_ListAdvance
*
*/
void Setting_ListAdvance( )
{
	guint i = 0;
	gchar * key_value;
	GtkTreeIter iter;
	gchar ** config_list = g_key_file_get_keys( mokoiConfigTable, "Mokoi", NULL, NULL );

	gtk_list_store_clear( mokoiSettingStore );

	while( config_list[i] )
	{
		key_value = g_key_file_get_value( mokoiConfigTable, "Mokoi", config_list[i], NULL );
		gtk_list_store_append( mokoiSettingStore, &iter );
		/* @ store: [ 0=>name, 1=>value, 2=>editable ] */
		gtk_list_store_set( mokoiSettingStore, &iter, 0, config_list[i], 1, key_value, 2, !Setting_Unlocked(config_list[i]), -1 );

		i++;
	}
}

/********************************
* Setting_ClearAdvance
*
*/
void Setting_ClearAdvance( )
{
	gtk_list_store_clear( mokoiSettingStore );
}


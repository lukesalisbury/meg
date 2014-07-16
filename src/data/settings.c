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
#include "game_compiler.h"
#include "loader_functions.h"
#include "setting_functions.h"

/* External Functions */

/* Local Type */

/* Global Variables */
extern GError * mokoiError;
extern GKeyFile * mokoiConfigTable;

/* Local Variables */
GtkListStore * mokoiSettingStore = NULL;
GHashTable * mokoiSettingsTable = NULL;
gchar * mokoiSettingsLocked[] = {
	"project.title",
	"project.id",
	"project.internalid",
	"project.file",
	"player.number",
	"map.width",
	"map.height",
	"screen.width",
	"screen.height",
	"display.width",
	"display.height",
	"display.mode",
	"language.default",
	"debug.able",
	"save.allowed",
	NULL
};

/* UI */

/* Events */

/********************************
* AL_SettingBoolean
*
@ key:
*/
gboolean AL_SettingBoolean( gchar * key )
{
	if ( mokoiConfigTable )
		return g_key_file_get_boolean ( mokoiConfigTable, "Mokoi", key, NULL);
	else
		return TRUE;
}




/********************************
* AL_Setting_Widget
*
@ widget: Container widget for Project Info page
*/
void  AL_Setting_Widget( GtkBuilder * ui )
{
	GtkWidget * custom_display_box;

	custom_display_box = GET_WIDGET( ui, "hbox4" );

	mokoiSettingsTable = g_hash_table_new(g_str_hash, g_str_equal);
	mokoiSettingStore = GET_LISTSTORE( ui, "meg_settings_advance_store" ); /* G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN */

	/* Events */
	SET_OBJECT_SIGNAL( ui, "w_display_custom", "toggled", G_CALLBACK(ToggleButton_Event_WidgetSensitive), custom_display_box );
	SET_OBJECT_SIGNAL( ui, "meg_setting_controller_button", "clicked", G_CALLBACK(Setting_Dialog_CustomController), mokoiSettingsTable );
	SET_OBJECT_SIGNAL( ui, "meg_setting_save", "clicked", G_CALLBACK(Setting_Save), NULL );

	g_hash_table_replace(mokoiSettingsTable, "project.id", gtk_builder_get_object( ui, "w_project_id" ) ); /* GtkLabel */

	g_hash_table_replace(mokoiSettingsTable, "project.title", gtk_builder_get_object( ui, "w_project_title" ) ); /* GtkEntry */
	g_hash_table_replace(mokoiSettingsTable, "project.author", gtk_builder_get_object( ui, "w_project_author" ) ); /* GtkEntry */
	g_hash_table_replace(mokoiSettingsTable, "debug.able", gtk_builder_get_object( ui, "w_debug_able" ) ); /* GtkCheckButton */
	g_hash_table_replace(mokoiSettingsTable, "audio.able", gtk_builder_get_object( ui, "w_audio_able" ) ); /* GtkCheckButton */

	g_hash_table_replace(mokoiSettingsTable, "save.allowed", gtk_builder_get_object( ui, "w_save_able" ) ); /* GtkCheckButton */
	g_hash_table_replace(mokoiSettingsTable, "save.access", gtk_builder_get_object( ui, "w_save_access" ) ); /* GtkCheckButton */

	g_hash_table_replace(mokoiSettingsTable, "map.width", gtk_builder_get_object( ui, "w_map_width" ) ); /* GtkSpinButton */
	g_hash_table_replace(mokoiSettingsTable, "map.height", gtk_builder_get_object( ui, "w_map_height" ) ); /* GtkSpinButton */

	g_hash_table_replace(mokoiSettingsTable, "screen.width", gtk_builder_get_object( ui, "w_screen_width" ) ); /* GtkSpinButton */
	g_hash_table_replace(mokoiSettingsTable, "screen.height", gtk_builder_get_object( ui, "w_screen_height" ) ); /* GtkSpinButton */

	g_hash_table_replace(mokoiSettingsTable, "display.width", gtk_builder_get_object( ui, "w_display_width" ) ); /* GtkSpinButton */
	g_hash_table_replace(mokoiSettingsTable, "display.height", gtk_builder_get_object( ui, "w_display_height" ) ); /* GtkSpinButton */
	g_hash_table_replace(mokoiSettingsTable, "display.fullscreen", gtk_builder_get_object( ui, "w_display_fullscreen" ) ); /* GtkCheckButton */

	g_hash_table_replace(mokoiSettingsTable, "player.number", gtk_builder_get_object( ui, "w_player_number" ) ); /* GtkSpinButton */

	g_hash_table_replace(mokoiSettingsTable, "language.default", gtk_builder_get_object( ui, "w_language_default" ) ); /* GtkComboBox */
	g_hash_table_replace(mokoiSettingsTable, "language.choose", gtk_builder_get_object( ui, "w_language_choose" ) ); /* GtkCheckButton */

	g_hash_table_replace(mokoiSettingsTable, "package.main", gtk_builder_get_object( ui, "w_package_main" ) ); /* GtkComboBox */



	GObject * obj = NULL;
	GObject * preview_widget = NULL;


	obj = gtk_builder_get_object( ui, "w_display_custom" ); /* GtkCheckButton */
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(obj), AL_SettingBoolean("display.customsize") );
	g_hash_table_replace(mokoiSettingsTable, "display.customsize", obj );
	ToggleButton_Event_WidgetSensitive( GTK_TOGGLE_BUTTON(obj), custom_display_box );

	obj = gtk_builder_get_object( ui, "w_display_mode" ); /* GtkComboBox */
	g_hash_table_replace(mokoiSettingsTable, "display.mode", obj );
	Meg_ComboText_AppendText( GTK_COMBO_BOX(obj), "native");
	Meg_ComboText_AppendText( GTK_COMBO_BOX(obj), "OpenGL");

	obj = gtk_builder_get_object( ui, "image_icon" ); /* GtkButton */
	preview_widget = gtk_builder_get_object( ui, "image_icon_preview" );
	g_signal_connect( obj, "clicked", G_CALLBACK(Setting_Event_SetImage), NULL );
	g_object_set_data( obj, "preview_widget", preview_widget);
	g_object_set_data( obj, "image_path", g_strdup("/resources/icon32.png") );

	g_hash_table_replace(mokoiSettingsTable, "image.icon", obj );

	obj = gtk_builder_get_object( ui, "image_banner" ); /* GtkButton */
	preview_widget = gtk_builder_get_object( ui, "image_banner_preview" );
	g_signal_connect( obj, "clicked", G_CALLBACK(Setting_Event_SetImage), NULL );
	g_object_set_data( obj, "preview_widget", preview_widget);
	g_object_set_data( obj, "image_path", g_strdup("/resources/loading.png") );

	g_hash_table_replace(mokoiSettingsTable, "image.banner", obj );



}

/********************************
* AL_Settings_RefreshAdvance
*
@ key:
*/
void AL_Settings_RefreshAdvance()
{
	gtk_list_store_clear( mokoiSettingStore );
	Setting_ListAdvance();
}

/********************************
* AL_Settings_Refresh
*
@ key:
*/
void AL_Settings_Refresh()
{

	/* Packages */
	GtkWidget * package = g_hash_table_lookup( mokoiSettingsTable, "package.main");
	Setting_Package( GTK_COMBO_BOX(package), NULL );
	if ( package )
	{
		gchar * setting = AL_Setting_GetString("package.main");
		if ( setting )
		{
			Meg_ComboText_SetIndex(GTK_COMBO_BOX(package), setting);
			g_free(setting);
		}
	}

	GtkWidget * lang = g_hash_table_lookup( mokoiSettingsTable, "language.default");
	if ( lang )
	{
		GtkListStore * list = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(lang)));
		GtkTreeIter iter;
		char ** lang_files = PHYSFS_enumerateFiles("lang");
		char ** i;

		gtk_list_store_clear(list);
		for (i = lang_files; *i != NULL; i++)
		{
			gchar * file = *i;
			if ( g_str_has_suffix( file, ".txt" ) && !g_str_has_prefix( file, "00" ))
			{
				gchar * name = g_strndup(file, 2);

				gtk_list_store_append( list, &iter );
				gtk_list_store_set( list, &iter, 0, name, 1, name, -1);

			}
		}
		PHYSFS_freeList(lang_files);
		gtk_combo_box_set_active( GTK_COMBO_BOX(lang), 0  );
	}

	g_hash_table_foreach( mokoiSettingsTable, Setting_LoadForeach, mokoiConfigTable );

	Setting_ListAdvance( );

}

/********************************
* AL_Settings_Clear
*
@ key:
*/
void AL_Settings_Clear()
{
	g_hash_table_foreach( mokoiSettingsTable, Setting_ClearForeach, mokoiConfigTable );
	Setting_ClearAdvance();
}

/********************************
* AL_Settings_Clear
*
@ key:
*/
gboolean AL_Setting_Remove(  gchar * key )
{
	if ( Setting_Unlocked(key) )
	{
		GObject * wid = g_hash_table_lookup(mokoiSettingsTable, key);
		g_key_file_set_string( mokoiConfigTable, "Mokoi", key, "" );
		Setting_WidgetWrite( key, wid );
		return g_key_file_remove_key( mokoiConfigTable, "Mokoi", key, NULL);
	}
	else
		return FALSE;
}

/********************************
* AL_Setting_GetString
*
@ key:
*/
gchar * AL_Setting_GetString( gchar * key )
{
	return g_key_file_get_string( mokoiConfigTable, "Mokoi", key, NULL);
}

/********************************
* AL_Setting_GetNumber
*
@ key:
*/
gint AL_Setting_GetNumber( gchar * key )
{
	return g_key_file_get_integer( mokoiConfigTable, "Mokoi", key, NULL );
}

/********************************
* AL_Setting_GetDefaultNumber
*
@ key:
@ default_value:
*/
gint AL_Setting_GetDefaultNumber( gchar * key, gint default_value )
{
	if ( g_key_file_has_key( mokoiConfigTable, "Mokoi", key, NULL ) )
		return g_key_file_get_integer( mokoiConfigTable, "Mokoi", key, NULL );
	else
		return default_value;
}
/********************************
* AL_Setting_GetDouble
*
@ key:
*/
gdouble AL_Setting_GetDouble( gchar * key )
{
	return g_key_file_get_double( mokoiConfigTable, "Mokoi", key, NULL );
}

/********************************
* AL_Settings_Set
*
@ store: [ 0=>name, 1=>value, 2=>editable ]
*/
void AL_Setting_SetString( gchar * key, gchar * value )
{
	GObject * wid = g_hash_table_lookup(mokoiSettingsTable, key);
	g_key_file_set_string( mokoiConfigTable, "Mokoi", key, value );
	Setting_WidgetWrite( key, wid );
}


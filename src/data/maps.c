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
#include "maps.h"
#include "entity_functions.h"
#include "entity_editor.h"
#include "runtime_parser.h"
#include "runtime_options.h"
#include "object_advance.h"
#include "loader_functions.h"

/* External Functions */
GdkPixbuf * Section_GetMapThumbSized( gchar * map, guint x, guint y );

/* Local Type */

/* Global Variables */
//extern guint16 mokoiWorldGrid[256][256];

/* Local Varibles */
guint16 map_bgalpha = 65535;

gchar * map_script_default = "// To ease development, <map_default> may include a Init Function.\n// If You wish to use your own uncomment the next line\n//#define HASINITFUNCTION 1\n#tryinclude <map_default>\n#tryinclude <map_features/standard>\nmain()\n{\n		//Your Code\n}\n";

/* UI */
#include "ui/map_advance.gui.h"
const gchar * mokoiUI_MapAdvance = GUIMAP_ADVANCE

/* Events */


/********************************
* AL_Map_Add
*
@ name: Name for new map.
@ old_path: Path to imported map.
*/
gboolean AL_Map_Add( const gchar * name, const gchar * old_path )
{
	gchar * script_dest = NULL, * map_dest = NULL, * clean_name = NULL;
	gboolean result = FALSE;

	if ( name == NULL && old_path )
	{
		/* Copy Map */
		gchar * basename = g_path_get_basename(old_path);

		clean_name = g_strndup(basename, g_utf8_strlen(basename, -1) -4);

		script_dest = g_strdup_printf("/scripts/maps/%s.mps", clean_name );
		map_dest = g_strdup_printf("/maps/%s.xml", clean_name );

		gchar * content = NULL;
		if ( g_file_get_contents( old_path, &content, NULL, NULL ) )
		{
			result = Map_Copy( map_dest, content );
		}
		g_free( basename );
	}
	else if ( old_path == NULL && name )
	{
		/* Create New Map */
		clean_name = g_strdelimit( (gchar *)name, "_-|> <.\\/\"'!@#$%^&*(){}[]", '_' );

		script_dest = g_strdup_printf("/scripts/maps/%s.mps", clean_name );
		map_dest = g_strdup_printf("/maps/%s.xml", clean_name );

		if ( !Meg_file_test(map_dest, G_FILE_TEST_IS_REGULAR) )
		{
			result = Map_New( map_dest, 1, 1 );
		}
	}

	if ( !result )
	{
		Meg_Log_Print(LOG_ERROR, "Error creating map: '%s'", (name ? name : old_path) );
	}
	else
	{
		Entity_New( script_dest, map_script_default );
	}

	g_free( map_dest );
	g_free( script_dest );
	g_free( clean_name );

	return result;
}

/********************************
* AL_Map_Remove
*
* Alchera: Map Editor Page
*/
gboolean AL_Map_Remove( gchar * name )
{
	Meg_Error_Print( __func__, __LINE__, "Not enable yet");
	return FALSE;
}

/********************************
* AL_Map_Files
* Gets a List of Maps
@ store:  [ 0->name, 1=>extra ]
*/
void AL_Map_Files( GtkListStore * store )
{
	GtkTreeIter iter;

	gtk_list_store_clear( store ); /* Clear previous list */


	char ** directory_listing = PHYSFS_enumerateFiles("/maps/");
	char ** current_file;


	for (current_file = directory_listing; *current_file != NULL; current_file++)
	{
		if ( g_str_has_suffix( *current_file, ".xml") )
		{
			gchar * shortname = g_strndup(*current_file, g_utf8_strlen(*current_file, -1) -4);
			gtk_list_store_append( store, &iter );
			gtk_list_store_set( store, &iter, 0, shortname, 1, "", 2, Section_GetMapThumbSized(shortname, 64, 48), 3, NULL, -1);
		}
	}
	PHYSFS_freeList(directory_listing);

}

/********************************
* AL_Map_Open
*
@ name: map file name
*/
gboolean AL_Map_Open( MapInfo * map_info, GtkWindow * window )
{
	g_return_val_if_fail( map_info, FALSE );
	g_return_val_if_fail( map_info->name, FALSE );

	Map_Open( map_info->name, map_info );

	return ( map_info->data ? TRUE : FALSE );
}


/********************************
* AL_Map_Save
* Save Opened Map
*/
gboolean AL_Map_Save( MapInfo * map_info, GtkWindow * window )
{
	g_return_val_if_fail( map_info, FALSE );
	g_return_val_if_fail( map_info->data, FALSE );


	MokoiMap * mokoi_map = (MokoiMap *)map_info->data;


	/* Save Image of Map */
	cairo_surface_t * cst;

	cst = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, map_info->width, map_info->height );

	if ( cst )
	{
		cairo_t *  cr = cairo_create( cst );

		cairo_save( cr );
		cairo_rectangle( cr, 0, 0, map_info->width, map_info->height );
		gdk_cairo_set_source_rgba( cr, &map_info->colour );
		cairo_fill( cr );
		cairo_restore ( cr );

		GList * scan = scan = g_list_first( mokoi_map->objects );
		while ( scan )
		{
			MokoiMapObject * obj = (MokoiMapObject *)scan->data;
			obj->object->active = FALSE;
			Alchera_DisplayObject_DrawForeach( obj->object, cr );
			scan = g_list_next( scan );
		}
		cairo_destroy( cr );

		cairo_surface_write_to_png( cst, mokoi_map->thumb_filename );
	}

	gdk2rgbacolor( &map_info->colour, &mokoi_map->colour8 );
	return Map_Save( map_info );
}

/********************************
* AL_Map_Options
* Display Map Options
*/
gboolean AL_Map_ContructRuntimeWidget( MapInfo * map_info, GtkWidget * box_runtime )
{
	g_return_val_if_fail( map_info, FALSE );
	g_return_val_if_fail( map_info->data, FALSE );

	Map_GetOptions( map_info );

	/* Options Treeview settings */
	if ( g_hash_table_size(map_info->settings) )
	{
		g_object_set_data( G_OBJECT(box_runtime), "table-y", GUINT_TO_POINTER(0) );
		g_hash_table_foreach( map_info->settings, (GHFunc)RuntimeSetting_CreateWidgetWithSignal, box_runtime );
		g_hash_table_foreach( map_info->settings, (GHFunc)RuntimeSetting_AttachWidget, box_runtime );
	}
	g_object_set_data( G_OBJECT(box_runtime), "runtime-hashtable", map_info->settings );
	return TRUE;
}

/********************************
* AL_Map_Options
* Display Map Options
*/
gboolean AL_Map_Options( MapInfo * map_info, GtkWindow * window )
{
	g_return_val_if_fail( map_info, FALSE );
	g_return_val_if_fail( map_info->data, FALSE );

	gdouble map_size_width = AL_SettingDouble("map.width");
	gdouble map_size_height = AL_SettingDouble("map.height");

	MokoiMap * mokoi_map_data = (MokoiMap *)map_info->data;

	Map_GetOptions( map_info );

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(mokoiUI_MapAdvance, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );

	/* Widgets */
	GtkWidget * dialog, * label, * box_runtime, * spin_w, * spin_h, * button_colour, * button_addoption;
	GtkWidget * combo_wrapmode, * check_centerview, *check_independent, * box_graphics;

	GtkAdjustment * adjust_map_width, * adjust_map_height;

	dialog = GET_WIDGET( ui, "dialog");
	label = GET_WIDGET( ui, "alchera-label");

	/* Setting Widgets */
	spin_w = GET_WIDGET( ui, "spin_w");
	spin_h = GET_WIDGET( ui, "spin_h");
	combo_wrapmode = GET_WIDGET( ui, "combo_wrapmode");
	check_centerview = GET_WIDGET( ui, "check_centerview");
	check_independent = GET_WIDGET( ui, "check_independent");
	button_colour = GET_WIDGET( ui, "button_colour");

	adjust_map_width = GTK_ADJUSTMENT( gtk_builder_get_object(ui, "adjust_map_width") );
	adjust_map_height = GTK_ADJUSTMENT( gtk_builder_get_object(ui, "adjust_map_height") );

	/* Options Widgets */
	box_runtime = GET_WIDGET( ui, "box_runtime");
	button_addoption = GET_WIDGET( ui, "button_addoption");

	/* Graphics Widgets */
	box_graphics = GET_WIDGET( ui, "box_graphics");


	/* Set Up Map Dimension Adjustemnt */
	gtk_adjustment_configure( adjust_map_width,  map_size_width, map_size_width,  map_size_width * 64, map_size_width, 0.0, 0.0);
	gtk_adjustment_configure( adjust_map_height,  map_size_height, map_size_height,  map_size_height * 64, map_size_height, 0.0, 0.0);

	/* Set Default Values */
	Meg_Misc_SetLabel( label, "Map Options", g_path_get_basename(map_info->name), '\n' );

	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(check_centerview), RuntimeSetting_BooleanCheck( map_info->settings, "centerview" ) );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(check_independent), RuntimeSetting_BooleanCheck( map_info->settings, "independent" )  );

	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_w), (gdouble)mokoi_map_data->position.width * map_size_width );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_h), (gdouble)mokoi_map_data->position.height * map_size_height );

	gtk_combo_box_set_active( GTK_COMBO_BOX(combo_wrapmode), RuntimeSetting_GetValue( map_info->settings, "wrap" ) );

	Meg_ColorButton_SetColor( GTK_COLOR_BUTTON(button_colour), &map_info->colour );

	/* Signal */
	g_signal_connect( button_addoption, "clicked", (GCallback)RuntimeSetting_AddOption, box_runtime );


	/* Options Treeview settings */
	if ( g_hash_table_size(map_info->settings) )
	{
		g_object_set_data( G_OBJECT(box_runtime), "table-y", GUINT_TO_POINTER(0) );
		g_hash_table_foreach( map_info->settings, (GHFunc)RuntimeSetting_CreateWidget, box_runtime );
		g_hash_table_foreach( map_info->settings, (GHFunc)RuntimeSetting_AttachWidget, box_runtime );
	}
	g_object_set_data( G_OBJECT(box_runtime), "runtime-hashtable", map_info->settings );


	/* Graphic */
	GHashTable * graphic_sheets = Map_GetReplacableSheets( map_info );
	if ( g_hash_table_size(graphic_sheets) )
	{
		g_object_set_data( G_OBJECT(box_graphics), "table-y", GUINT_TO_POINTER(0) );
		g_hash_table_foreach( graphic_sheets, (GHFunc)Map_ReplacableSheets_Widget_ForEach, box_graphics );
	}


	/* Todo
	Check for Max Width/Height if part of grid
	*/
	gtk_window_set_modal( GTK_WINDOW(dialog), FALSE );
	gtk_window_set_transient_for( GTK_WINDOW(dialog), window );
	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ) );

	gint response_id = gtk_dialog_run( GTK_DIALOG(dialog) );
	if ( response_id == GTK_RESPONSE_APPLY || response_id == 1)
	{
		mokoi_map_data->position.width = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_w) ) / AL_SettingNumber("map.width");
		mokoi_map_data->position.height = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_h) ) / AL_SettingNumber("map.height");

		g_hash_table_foreach( map_info->settings, (GHFunc)RuntimeSetting_SaveWidget_Foreach, NULL );
		g_hash_table_foreach( graphic_sheets, (GHFunc)Map_ReplacableSheets_Update_ForEach, mokoi_map_data );

		RuntimeSetting_UpdateValue( map_info->settings, "wrap", gtk_combo_box_get_active( GTK_COMBO_BOX(combo_wrapmode) ) );
		RuntimeSetting_UpdateValue( map_info->settings, "wrap", gtk_combo_box_get_active( GTK_COMBO_BOX(combo_wrapmode) ) );

		RuntimeSetting_UpdateBoolean( map_info->settings, "centerview", gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_centerview) ) );
		RuntimeSetting_UpdateBoolean( map_info->settings, "independent", gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_independent) ) );

		Meg_ColorButton_GetColor( GTK_COLOR_BUTTON(button_colour), &map_info->colour );

		Map_Save( map_info );

		if ( response_id == 1 )
		{
			EntityEditor_New( mokoi_map_data->entity_filename );
		}

	}

	gtk_widget_destroy( dialog );

	return TRUE;
}

/********************************
* AL_Map_Layers
*
@ store:  [ 0->name, 1=>visible, 2->value ]
*/
void AL_Map_Layers( GtkListStore * store )
{
	GtkTreeIter iter;
	gtk_list_store_clear( store );
	gtk_list_store_append( store, &iter );
	gtk_list_store_set( store, &iter, 1, TRUE, 0, AL_SettingString("string.layer0"), 2, 0,-1 );
	gtk_list_store_append( store, &iter );
	gtk_list_store_set( store, &iter, 1, TRUE, 0, AL_SettingString("string.layer1"), 2, 1,-1 );
	gtk_list_store_append( store, &iter );
	gtk_list_store_set( store, &iter, 1, TRUE, 0, AL_SettingString("string.layer2"), 2, 2,-1 );
	gtk_list_store_append( store, &iter );
	gtk_list_store_set( store, &iter, 1, TRUE, 0, AL_SettingString("string.layer3"), 2, 3,-1 );
	gtk_list_store_append( store, &iter );
	gtk_list_store_set( store, &iter, 1, TRUE, 0, AL_SettingString("string.layer4"), 2, 4,-1 );
	gtk_list_store_append( store, &iter );
	gtk_list_store_set( store, &iter, 1, TRUE, 0, AL_SettingString("string.layer5"), 2, 5,-1 );
	gtk_list_store_append( store, &iter );
	gtk_list_store_set( store, &iter, 1, TRUE, 0, AL_SettingString("string.layer6"), 2, 6,-1 );
}



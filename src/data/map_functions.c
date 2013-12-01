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
#include "setting_functions.h"
#include "runtime_parser.h"
#include "runtime_options.h"
#include "loader_functions.h"

/* Global Functions  */
gboolean Map_ParseXML( MapInfo * map_info, gchar * content );

/* Local Functions  */

/* Global Variables */
extern GError * mokoiError;
extern GKeyFile * mokoiConfigTable;

/* Local Variables */


/* UI */
#include "ui/map_chooser.gui.h"
const gchar * mokoiUI_MapChooser = GUIMAP_CHOOSER;


/********************************
* MapObject_Type
*
@ type_name:
*/
gchar MapObject_Type( const gchar * type_name )
{
	if ( !g_ascii_strcasecmp(type_name, "sprite") )
		return 's';
	else if ( !g_ascii_strcasecmp(type_name, "animation") )
		return 's';
	else if ( !g_ascii_strcasecmp(type_name, "rect") )
		return 'r';
	else if ( !g_ascii_strcasecmp(type_name, "line") )
		return 'l';
	else if ( !g_ascii_strcasecmp(type_name, "text") )
		return 't';
	else if ( !g_ascii_strcasecmp(type_name, "circle") )
		return 'c';
	else if ( !g_ascii_strcasecmp(type_name, "polygon") )
		return 'p';
	else if ( !g_ascii_strcasecmp(type_name, "canvas") )
		return 'M';


	g_warning("MapObject_Type: Error %s\n", type_name );
	return 'U';

}

/********************************
* MapObject_TypeName
*
@ type_name:
*/
gchar * MapObject_TypeName( gchar type )
{
	if ( type == 's' )
		return "sprite";
	else if ( type == 'r')
		return "rect";
	else if ( type == 'l')
		return "line";
	else if ( type == 't')
		return "text";
	else if ( type == 'c')
		return "circle";
	else if ( type == 'p')
		return "polygon";
	else if (type == 'M')
		return "canvas";
	else
		return "unknown";

}


/********************************
* Map_Free
* Delete MokoiMap data with optional save.
@ map:
@ save:
*/
void Map_Free( MapInfo * map_info, gboolean save )
{
	g_return_if_fail( map_info );
	g_return_if_fail( map_info->data );



	g_hash_table_destroy( map_info->settings );

	if ( map_info->file_type == 0 )
	{
		MokoiMap * mokoi_map_data = (MokoiMap *)map_info->data;


		/*mokoi_map_data->objects = g_list_sort_with_data( mokoi_map_data->objects, (GCompareDataFunc)Alchera_DisplayObject_Order, NULL );*/
		GList * scan = NULL;
		scan = g_list_first( mokoi_map_data->objects );
		while ( scan )
		{
			MokoiMapObject * object = (MokoiMapObject *)scan->data;
			g_hash_table_destroy( object->settings );
			g_free( object->name );
			scan = g_list_next( scan );
		}
		g_list_free( mokoi_map_data->objects );


		g_free( mokoi_map_data->entity_filename );
		g_free( mokoi_map_data->thumb_filename );
		g_free( mokoi_map_data );
	}
	else if ( map_info->file_type == 1 )
	{

	}

}


/********************************
* Map_Open
*
@ file: full path to map
*/
gboolean Map_Open( gchar * file, MapInfo * map_info )
{
	g_return_val_if_fail( map_info, FALSE );

	MokoiMap * mokoi_map = NULL;
	gchar * runtime_file, * thumbnail_file, * content;

	/* Inital Map Settings */
	runtime_file = g_strdup_printf( "/scripts/maps/%s.%s.options", file, "mps" );
	thumbnail_file = g_strdup_printf( "/maps/%s.png", file );

	map_info->name = g_strdup( file );
	map_info->settings = RuntimeParser_Load( runtime_file );

	mokoi_map = map_info->data = g_new0(MokoiMap, 1);
	mokoi_map->position = (GdkRectangle){0,0,1,1};
	mokoi_map->colour8 = (rgbaColour){255,255,255,255};

	mokoi_map->xml_filename = g_strdup_printf( "/maps/%s.xml", file );
	mokoi_map->entity_filename = g_strdup_printf( "/scripts/maps/%s.%s", file, "mps" );
	mokoi_map->thumb_filename = PHYSFS_buildLocalFilename( thumbnail_file );

	/* Parse the XML file */
	if ( Meg_file_get_contents( mokoi_map->xml_filename, &content, NULL, NULL ) )
	{
		Map_ParseXML( map_info, content );
	}

	/* Match value to one's Meg uses */
	rgba2gdkcolor(&mokoi_map->colour8, &map_info->colour);

	map_info->width = mokoi_map->position.width * AL_SettingNumberDefault("map.width", 1);
	map_info->height = mokoi_map->position.height * AL_SettingNumberDefault("map.height", 1);

	g_free( runtime_file );
	g_free( thumbnail_file );
	g_free( content );

	return TRUE;
}

/********************************
* Map_New
*
@ file:
@ width:
@ height:
*/
gboolean Map_New( gchar * file, guint width, guint height )
{
	GString * content = g_string_new( "<map xmlns=\"http://mokoi.info/projects/mokoi\">\n" );
	g_string_append_printf( content, "<settings>\n" );
	g_string_append_printf( content, "<dimensions width=\"%ud\" height=\"%ud\" />\n", width, height );
	g_string_append_printf( content, "<color red=\"%ud\" blue=\"%ud\" green=\"%ud\" mode=\"0\" />\n", 128, 128, 128 );
	g_string_append_printf( content, "</settings>\n" );
	g_string_append_printf( content, "</map>" );

	Meg_file_set_contents( file, content->str, -1, &mokoiError ); /* Write map file */
	g_string_free( content, TRUE );
	if ( mokoiError )
	{
		Meg_Error_Print( __func__, __LINE__, "Map could not be saved. Reason: %s", mokoiError->message );
		g_clear_error( &mokoiError );
		return FALSE;
	}
	return TRUE;
}

/********************************
* Map_Copy
*
@ file:
@ width:
@ height:
*/
gboolean Map_Copy( gchar * file, gchar * content )
{
	Meg_file_set_contents( file, content, -1, &mokoiError ); /* Write map file */
	if ( mokoiError )
	{
		Meg_Error_Print( __func__, __LINE__, "Map could not be saved. Reason: %s", mokoiError->message );
		g_clear_error( &mokoiError );
		return FALSE;
	}
	return TRUE;
}

/********************************
* Map_Setting_Foreach
*
@ key:
@ value:
@ content:
*/
void Map_Setting_Foreach( gchar* key, RuntimeSettingsStruct * value, GString * content )
{
	if ( value )
	{
		gchar * value_str = NULL;
		if ( value->value )
		{
			value_str = g_markup_escape_text(value->value, -1);
		}
		else
		{
			value_str = "";
		}

		if ( !g_ascii_strcasecmp( value_str, "(null)" ) )
		{
			return;
		}

		if ( !g_ascii_strcasecmp( (gchar*)key, "entity" ) )
		{
			gchar ** file = g_strsplit( value_str,".", 2);
			if ( g_strv_length(file) )
			{
				g_string_append_printf( content, "\t<setting key=\"%s\" value=\"%s\" type=\"hidden\"/>\n", (gchar*)key, value_str );
			}
			g_strfreev( file );
		}
		else
		{
			if ( value->type )
			{
				g_string_append_printf( content, "\t<setting key=\"%s\" value=\"%s\" type=\"%s\"/>\n", (gchar*)key, value_str, value->type );
			}
			else
			{
				g_string_append_printf( content, "\t<setting key=\"%s\" value=\"%s\" />\n", (gchar*)key, value_str );
			}
		}
		g_free( value_str );
	}
}

/********************************
* Map_SaveObject
*
@ object:
@ map_string:
@ entity_string:
*/
void Map_SaveObject( MokoiMapObject * object, GString * map_string , GString * entity_string )
{
	gint x,y,w,h,z,l;
	GSList * point_scan = NULL;
	DisplayObject * display_object = NULL;


	if ( object->object->type == DT_DELETE )
		return;

	x = (gint)object->object->x;
	y = (gint)object->object->y;
	l = object->object->layer;
	z = l * 1000;

	if ( object->type == 'l' )
	{
		// Line Object use w/h as 2nd x/y
		w = (gint)(object->object->w + object->object->x);
		h = (gint)(object->object->h + object->object->y);
	}
	else
	{
		w = (gint)object->object->w;
		h = (gint)object->object->h;
	}

	g_string_append_printf( map_string, "<object value=\"%s\" type=\"%s\"", object->name, MapObject_TypeName(object->type) );
	if ( object->object_id && object->object_id[0] != '*' ) // * is old code for random
	{
		g_string_append_printf( map_string, " id=\"%s\"", object->object_id );
		/* Add Entity to list */
		g_string_append_printf( entity_string, "%s\t%d\t%d\n", object->object_id, (gint)object->object->x, (gint)object->object->y );
	}
	g_string_append_printf( map_string, ">\n" );
	g_string_append_printf( map_string, "\t<position x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\" z=\"%d\" l=\"%d\" r=\"%d\" f=\"%d\"/>\n", x, y, w, h, z, l, object->object->rotate*90, object->object->flip);
	g_string_append_printf( map_string, "\t<color red=\"%d\" blue=\"%d\" green=\"%d\" alpha=\"%d\" />\n", object->colour8.red, object->colour8.blue, object->colour8.green, object->colour8.alpha );

	g_hash_table_foreach( object->settings, (GHFunc) Map_Setting_Foreach, map_string );


	/* Write Entity */
	if ( object->entity_file  )
	{
		g_string_append_printf( map_string, "\t<entity value=\"%s\" language=\"%s\" global=\"%s\"/>\n", object->entity_file, object->entity_language, (object->entity_global ? "true" : "false") );
	}

	/* Write Path */
	if ( object->object->path  )
	{
		point_scan = object->object->path;
		g_string_append_printf( map_string, "\t<path>\n");
		while ( point_scan )
		{
			display_object = (DisplayObject *)point_scan->data;
			g_string_append_printf( map_string, "\t\t<point x=\"%d\" y=\"%d\" ms=\"%d\"/>\n", (gint)display_object->x, (gint)display_object->y, 10 );
			point_scan = g_slist_next( point_scan );
		}
		g_string_append_printf( map_string, "\t</path>\n");
	}


	if ( object->type == 'p' )
	{
		g_string_append_printf( map_string, "\t<option points=\"%u\"/>\n", g_slist_length(object->object->shape) );

		point_scan = object->object->shape;
		while ( point_scan )
		{
			display_object = (DisplayObject *)point_scan->data;
			g_string_append_printf( map_string, "\t<point x=\"%d\" y=\"%d\"/>\n", (gint)display_object->x, (gint)display_object->y );
			point_scan = g_slist_next(point_scan);
		}
	}


	g_string_append_printf( map_string, "</object>\n");


}

/********************************
* Map_Save
*
@ map:
*/
gboolean Map_Save( MapInfo * map_info )
{
	g_return_val_if_fail( map_info != NULL, FALSE );
	g_return_val_if_fail( map_info->data != NULL, FALSE );


	MokoiMap * mokoi_map = (MokoiMap*) map_info->data;
	GList * scan = NULL;
	GString * map_entities = g_string_new(""); // TSV listing of Entities, used in the server and editor
	GString * map_xml = g_string_new("<map xmlns=\"http://mokoi.info/projects/mokoi\">\n");

	/* Settings */
	g_string_append_printf( map_xml, "<settings>\n");
	g_string_append_printf( map_xml, "\t<dimensions width=\"%d\" height=\"%d\" />\n",  mokoi_map->position.width, mokoi_map->position.height);
	g_string_append_printf( map_xml, "\t<color red=\"%d\" blue=\"%d\" green=\"%d\" mode=\"0\" />\n", mokoi_map->colour8.red, mokoi_map->colour8.blue, mokoi_map->colour8.green);

	g_hash_table_foreach(map_info->settings, (GHFunc)RuntimeParser_SaveString, (gpointer)map_xml);

	g_string_append_printf( map_xml, "</settings>\n");

	/* Objects */
	scan = g_list_first( mokoi_map->objects );
	while ( scan )
	{
		Map_SaveObject( (MokoiMapObject *)scan->data, map_xml, map_entities );
		scan = g_list_next(scan);
	}
	g_string_append_printf( map_xml, "</map>");

	/* Write map file */
	Meg_file_set_contents( mokoi_map->xml_filename, map_xml->str, -1, &mokoiError);
	if ( mokoiError )
	{
		Meg_Error_Print( __func__, __LINE__, "Map could not be saved. Reason: %s", mokoiError->message );
		g_clear_error( &mokoiError );
		return FALSE;
	}

	/* Map's Named Entity */
	if ( map_entities->len )
	{
		gchar * list_file = g_strconcat( mokoi_map->xml_filename, ".entities", NULL );
		Meg_file_set_contents(list_file, map_entities->str, -1, &mokoiError);

		if ( mokoiError )
		{
			Meg_Error_Print( __func__, __LINE__, "Map Entity list could not be saved. Reason: %s", mokoiError->message );
			g_clear_error( &mokoiError );
		}
		g_free( list_file );
	}

	RuntimeParser_Save( mokoi_map->entity_filename, map_info->settings ); // Save Map Entity option file.
	return TRUE;
}

/********************************
* MapChooser_UpdatePreview
*
@ widget: Image widget
@ combo: combo box list of names
*/
void MapChooser_UpdatePreview( GtkComboBox * combo, GtkWidget * widget )
{
	GdkPixbuf * image = NULL, * scaled = NULL;

	gchar * name = Meg_ComboText_GetText( combo );
	gchar * src = g_strdup_printf( "/maps/%s.png",  name );

	image = Megdk_pixbuf_new_from_file( src, NULL );

	if ( image )
	{
		scaled = gdk_pixbuf_scale_simple(image, gdk_pixbuf_get_width(image)/4, gdk_pixbuf_get_height(image)/4, GDK_INTERP_BILINEAR );
		gtk_image_set_from_pixbuf( GTK_IMAGE(widget), scaled );
		g_object_unref(image);
	}
	else
	{
		gtk_image_set_from_stock( GTK_IMAGE(widget), GTK_STOCK_MISSING_IMAGE, GTK_ICON_SIZE_DIALOG );
	}

	g_free( src );
	g_free( name );
}

/********************************
* MapChooser_Dialog
*
*/
gchar * MapChooser_Dialog( gchar * title, gchar * selected_item, gchar * additional_option )
{
	gchar * map = NULL;
	GtkWidget * dialog, * combo_maps, * image_preview, * label_title;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(mokoiUI_MapChooser, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );

	/* Widget */
	dialog = GET_WIDGET( ui, "dialog");
	label_title = GET_WIDGET( ui, "label_title" );
	combo_maps = GET_WIDGET( ui, "combo_maps" );
	image_preview = GET_WIDGET( ui, "image_preview" );

	/* Signal */
	g_signal_connect( G_OBJECT(combo_maps), "changed", G_CALLBACK(MapChooser_UpdatePreview), image_preview );

	/* Add Maps to combo box */
	if ( additional_option )
		Meg_ComboText_AppendText( GTK_COMBO_BOX(combo_maps), additional_option );

	Meg_ComboFile_Scan( combo_maps, "/maps/", ".xml", FALSE, 4 );

	if ( selected_item )
		Meg_ComboText_SetIndex( GTK_COMBO_BOX(combo_maps), selected_item );

	gtk_label_set_text( GTK_LABEL(label_title), title );


	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ) );
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Main_GetWindow() );

	gint result = gtk_dialog_run( GTK_DIALOG(dialog) );

	if ( result == GTK_RESPONSE_APPLY )
	{
		map = Meg_ComboText_GetText( GTK_COMBO_BOX(combo_maps) );
	}
	gtk_widget_destroy( dialog );
	return map;

}

/********************************
* Map_SetStartingPoint
*
*/
gboolean Map_SetStartingPoint( )
{
	gchar * current_map = g_key_file_get_string( mokoiConfigTable, "Mokoi", "map.start", NULL );
	gchar * map = MapChooser_Dialog( "Start Map", current_map, NULL );

	if ( map != NULL )
	{
		g_key_file_set_value( mokoiConfigTable, "Mokoi", "map.start", map );
		Setting_Save();
		g_free(map);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



/********************************
* Map_GetOptions
*
*/
void Map_GetOptions( MapInfo * map_info )
{
	/* Load Default Settings */
	GHashTable * default_settings = RuntimeParser_Load( "/map.options" );
	g_hash_table_foreach( default_settings, (GHFunc)RuntimeSetting_Append, (gpointer)map_info->settings );
	g_hash_table_remove_all( default_settings );
}


/********************************
* Map_GetReplacableSheets
*
*/
GHashTable * Map_GetReplacableSheets( MapInfo * map_info )
{
	g_return_val_if_fail( map_info != NULL, FALSE );
	g_return_val_if_fail( map_info->data != NULL, FALSE );


	MokoiMap * mokoi_map_data = (MokoiMap*) map_info->data;
	GHashTable * graphic_sheets = g_hash_table_new( g_str_hash, g_str_equal );
	GRegex * regex = g_regex_new( "^\\w+(\\d+).png:", 0, 0, NULL );

	if ( mokoi_map_data->objects )
	{
		GList * q = mokoi_map_data->objects;
		while ( q )
		{
			MokoiMapObject * object = (MokoiMapObject *)(q->data);
			if ( object->type == 's' )
			{
				if ( g_regex_match(regex, object->name, 0 , NULL) )
				{
					gchar ** ident_split = g_strsplit_set(object->name, ":", 2);

					if ( ident_split )
					{
						g_hash_table_insert( graphic_sheets, g_strdup(ident_split[0]), gtk_entry_new() );
					}

					g_strfreev(ident_split);
				}
			}

			q = g_list_next( q );
		}
	}

	g_regex_unref( regex );

	return graphic_sheets;
}

/********************************
* Map_GetReplacableSheets
*
*/
void Map_ReplacableSheets_Widget_ForEach( gchar * key, GtkWidget * text_sheet, GtkWidget * list )
{
	guint yvalue = GPOINTER_TO_UINT(g_object_get_data( G_OBJECT(list), "table-y" ));
	GtkWidget * label_key = NULL;


	label_key = gtk_label_new( key );

	gtk_entry_set_text( GTK_ENTRY(text_sheet), key );
	g_object_set_data( G_OBJECT(text_sheet), "key_name", key );


	gtk_table_attach( GTK_TABLE(list), label_key, 0,1, yvalue, yvalue+1, GTK_FILL|GTK_EXPAND, 0, 2, 2); /* FIX: GTK3 */
	gtk_table_attach( GTK_TABLE(list), text_sheet, 1,2, yvalue, yvalue+1, GTK_FILL|GTK_EXPAND, 0, 2, 2);/* FIX: GTK3 */

	yvalue++;

	g_object_set_data( G_OBJECT(list), "table-y", GUINT_TO_POINTER(yvalue) );

}

/********************************
* Map_GetReplacableSheets
*
*/
void Map_ReplacableSheets_Update_ForEach( gchar * key, GtkWidget * text_sheet, MokoiMap * mokoi_map_data )
{
	const gchar * value = gtk_entry_get_text( GTK_ENTRY(text_sheet) );

	if ( g_ascii_strcasecmp(key, value) )
	{
		GRegex * regex = g_regex_new( key, 0, 0, NULL );

		if ( mokoi_map_data->objects )
		{
			GList * q = mokoi_map_data->objects;
			while ( q )
			{
				MokoiMapObject * object = (MokoiMapObject *)(q->data);
				if ( object->type == 's' )
				{
					if ( g_regex_match_full(regex, object->name, g_utf8_strlen(key,-1), 0, 0, NULL, NULL) )
					{
						gchar * new_name = g_regex_replace_literal( regex, object->name, -1, 0, value, 0, NULL );
						g_free(object->name);
						object->name = new_name;
						MapObject_UpdateSprite( object );
					}
				}

				q = g_list_next( q );
			}
		}
		g_regex_unref( regex );
	}
}

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
#include "entity_options_parser.h"
#include "entity_options.h"
#include "maps.h"
#include "loader_functions.h"

/* Required Headers */
#include <zlib.h>
#include <sys/stat.h>
#include <glib/gstdio.h>

enum MapParserModes {
	MPM_NEW,
	MPM_ROOT,
	MPM_SETTING,
	MPM_OBJECT,
	MPM_PATH,
	MPM_POINTS
};

/* Global Functions  */


/* Local Functions  */
void map_parse_handler_start( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, gpointer user_data, GError **error);
void map_parse_handler_end( GMarkupParseContext *context, const gchar *element_name, gpointer user_data, GError **error);
void map_dimension_parse_start( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, gpointer user_data, GError **error);
DisplayObjectTypes VirtualObject_Type( const gchar * type_name );

/* Global Variables */


/* Local Variables */
gint map_parse_mode = MPM_NEW;

static GMarkupParser map_parser_xml = { map_parse_handler_start, map_parse_handler_end, NULL, NULL, NULL};
static GMarkupParser map_dimension_parser = { map_dimension_parse_start, NULL, NULL, NULL, NULL};


/********************************
* MapObject_SetDefaultSettingOptions
*
*/
void MapObject_SetDefaultSettingOptions( MapObjectData * object_data )
{
	switch ( object_data->type )
	{
		case 's':
			break;
		case 't':
			EntityOption_InsertNew( object_data->settings, "text-string", "-1", "hidden" );
			break;
		case 'l':
			break;
		case 'r':
			break;
		case 'p':
			break;
		case 'c':
			break;
		default:
			break;
	}
}


/********************************
* MapObject_UpdateDisplayObject
*
*/
void MapObject_UpdateDisplayObject( char internal_type, DisplayObject * object )
{
	switch ( internal_type )
	{
		case 's':
			object->type = DT_IMAGE;
			break;
		case 't':
			object->type = DT_TEXT;
			object->w = 64;
			object->h = 8;
			object->tw = 0;
			object->th = 0;
			object->resizable = FALSE;
			break;
		case 'l':
			object->type = DT_LINE;
			object->tw = 1;
			object->th = 1;
			break;
		case 'r':
			object->type = DT_RECTANGLE;
			object->tw = 1;
			object->th = 1;
			break;
		case 'p':
			object->type = DT_POLYGON;
			object->resizable = FALSE;
			break;
		case 'c':
			object->type = DT_CIRCLE;
			object->tw = 1;
			object->th = 1;
			break;
		default:
			break;
	}
}

/********************************
* map_parser_colour
*
*/
void map_parser_colour( rgbaColour * colour, gboolean use_alpha, const gchar **attribute_names, const gchar **attribute_values)
{
	colour->red = colour->blue = colour->green = colour->alpha = 255;
	for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
	{
		if ( !g_ascii_strcasecmp (*attribute_names, "red") )
			colour->red = (guint8)g_ascii_strtoull(*attribute_values, NULL, 10);
		else if ( !g_ascii_strcasecmp (*attribute_names, "blue") )
			colour->blue = (guint8)g_ascii_strtoull(*attribute_values, NULL, 10);
		else if ( !g_ascii_strcasecmp (*attribute_names, "green") )
			colour->green = (guint8)g_ascii_strtoull(*attribute_values, NULL, 10);
		else if ( !g_ascii_strcasecmp (*attribute_names, "alpha") )
		{
			if ( use_alpha )
			{
				colour->alpha = (guint8)g_ascii_strtoull(*attribute_values, NULL, 10);
			}
		}
	}
}

/********************************
* map_dimension_parse_start
*
*/
void map_dimension_parse_start( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, gpointer user_data, GError **error )
{
	if ( g_ascii_strcasecmp(element_name, "dimensions") == 0 )
	{
		for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
		{
			if ( g_ascii_strcasecmp (*attribute_names, "width") == 0 )
				((GdkRectangle*)user_data)->width = atoi(*attribute_values);
			else if ( g_ascii_strcasecmp (*attribute_names, "height") == 0 )
				((GdkRectangle*)user_data)->height = atoi(*attribute_values);
		}
	}
}

/********************************
* map_parse_handler_start_settings
*
*/
void map_parse_handler_start_settings( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, MapInfo * map_info, GError **error )
{
	/* Settings */
	/* <settings>
	 *
	 * </settings>
	 */
	if ( g_ascii_strcasecmp (element_name, "color") == 0 )
	{
		map_parser_colour(&MAP_DATA(map_info)->colour8, FALSE, attribute_names, attribute_values);
	}
	else if ( g_ascii_strcasecmp (element_name, "option") == 0 )
	{
		gchar * key = NULL, *value = NULL, *type = NULL;
		for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
		{
			if ( g_ascii_strcasecmp(*attribute_names, "key") == 0 )
				key = g_strdup(*attribute_values);
			else if ( g_ascii_strcasecmp(*attribute_names, "value") == 0 )
				value = g_strdup(*attribute_values);
			else if ( g_ascii_strcasecmp(*attribute_names, "type") == 0 )
				type = g_strdup(*attribute_values);
		}

		if ( key && value )
		{
			if ( g_ascii_strcasecmp( value, "(null)" ) )
				g_hash_table_insert( map_info->settings, (gpointer)key, EntityOption_New(value, type) );
			else
				g_hash_table_insert( map_info->settings, (gpointer)key, EntityOption_New("", type) );
		}

	}
	else if ( g_ascii_strcasecmp (element_name, "dimensions") == 0 )
	{
		for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
		{
			if ( g_ascii_strcasecmp (*attribute_names, "width") == 0 )
				MAP_DATA(map_info)->position.width = atoi(*attribute_values);
			else if ( g_ascii_strcasecmp (*attribute_names, "height") == 0 )
				MAP_DATA(map_info)->position.height = atoi(*attribute_values);
			if ( g_ascii_strcasecmp (*attribute_names, "x") == 0 )
				MAP_DATA(map_info)->position.x = atoi(*attribute_values);
			else if ( g_ascii_strcasecmp (*attribute_names, "y") == 0 )
				MAP_DATA(map_info)->position.y = atoi(*attribute_values);
		}
	}

}

/********************************
* map_parse_handler_start_path_element
*
*/
void map_parse_handler_start_path_element( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, MapInfo * map_info, GError **error )
{
	/* Create Path Object Object */
	DisplayObject * object_display = g_new0(DisplayObject, 1);
	object_display->layer = 42;
	object_display->w = object_display->h = 16;
	object_display->type = DT_PATHITEM;
	object_display->resizable = FALSE;
	for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
	{
		if ( g_ascii_strcasecmp (*attribute_names, "x") == 0 )
			object_display->x = atoi(*attribute_values);
		else if ( g_ascii_strcasecmp (*attribute_names, "y") == 0 )
			object_display->y = atoi(*attribute_values);
	}


	/* Append Path */
	GList * last_object = NULL;
	DisplayObject * object = NULL;

	last_object = g_list_last( map_info->display_list );
	if ( last_object )
	{
		object = (DisplayObject*)last_object->data;
		object->path = g_slist_append( object->path, object_display );
	}
}

/********************************
* map_parse_handler_start_object_element
*
*/
void map_parse_handler_start_object_element( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, MapInfo * map_info, GError **error )
{
	DisplayObject * object_display = NULL;

	/* Get Last Object */
	GList * last_object = g_list_last( map_info->display_list );
	object_display = (DisplayObject *)last_object->data;

	/* Update Object */
	object_display->resizable = TRUE;
	if ( g_ascii_strcasecmp (element_name, "position") == 0 )
	{
		for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
		{
			if ( *attribute_names[0] == 'x' )
				object_display->x = g_ascii_strtod(*attribute_values, NULL);
			else if ( *attribute_names[0] == 'y' )
				object_display->y = g_ascii_strtod(*attribute_values, NULL);
			else if ( *attribute_names[0] == 'l' )
				object_display->layer = (gint)g_ascii_strtod(*attribute_values, NULL);
			else if ( *attribute_names[0] == 'w' )
				object_display->w = g_ascii_strtod(*attribute_values, NULL);
			else if ( *attribute_names[0] == 'h' )
				object_display->h = g_ascii_strtod(*attribute_values, NULL);
			else if ( *attribute_names[0] == 'r' )
				object_display->rotate = atoi(*attribute_values)/90;
			else if ( *attribute_names[0] == 'f' )
				object_display->is_flipped = (gboolean)g_ascii_strtod(*attribute_values, NULL);
		}

		if ( object_display->type == DT_LINE )
		{
			object_display->w -= object_display->x;
			object_display->h -= object_display->y;
		}
	}
	else if ( g_ascii_strcasecmp(element_name, "entity") == 0 )
	{
		if ( MAP_OBJECT_DATA(object_display) )
		{
			for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
			{
				if ( !g_ascii_strcasecmp( *attribute_names, "value" ) )
				{
					MAP_OBJECT_DATA(object_display)->entity_file = g_strdup(*attribute_values);
				}
				else if ( !g_ascii_strcasecmp( *attribute_names, "global" ) )
				{
					MAP_OBJECT_DATA(object_display)->entity_global = !g_ascii_strcasecmp(*attribute_values, "true");
				}
				else if ( !g_ascii_strcasecmp( *attribute_names, "language" ) )
				{
					MAP_OBJECT_DATA(object_display)->entity_language = g_strdup(*attribute_values);
				}
			}
		}
	}
	else if ( g_ascii_strcasecmp(element_name, "color") == 0 )
	{
		if ( MAP_OBJECT_DATA(object_display) )
		{
			map_parser_colour(&MAP_OBJECT_DATA(object_display)->colour8, TRUE, attribute_names, attribute_values);
			object_display->colour.red = (gdouble)MAP_OBJECT_DATA(object_display)->colour8.red / 255.0;
			object_display->colour.blue = (gdouble)MAP_OBJECT_DATA(object_display)->colour8.blue / 255.0;
			object_display->colour.green = (gdouble)MAP_OBJECT_DATA(object_display)->colour8.green / 255.0;
			object_display->colour.alpha = (gdouble)MAP_OBJECT_DATA(object_display)->colour8.alpha / 255.0;
		}
	}
	else if ( g_ascii_strcasecmp(element_name, "setting") == 0 )
	{
		if ( map_info->file_type == 0 )
		{
			const gchar * key = NULL, *value = NULL, *type = NULL;
			for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
			{
				if ( g_ascii_strcasecmp(*attribute_names, "key") == 0 )
					key = *attribute_values;
				else if ( g_ascii_strcasecmp(*attribute_names, "value") == 0 )
					value = *attribute_values;
				else if ( g_ascii_strcasecmp(*attribute_names, "type") == 0 )
					type = *attribute_values;
			}

			if ( key && value )
			{
				if ( !g_ascii_strcasecmp( key, "id" ) )
				{
					if ( value[0] == '*')
						MAP_OBJECT_DATA(object_display)->object_name = "";
					else
						MAP_OBJECT_DATA(object_display)->object_name = g_strdup(value);
				}
				else if ( !g_ascii_strcasecmp( key, "entity" ) )
				{
					gchar ** file = g_strsplit( value, ".", 2);
					if ( g_strv_length(file) == 2 )
					{
						MAP_OBJECT_DATA(object_display)->entity_file = g_strdup(file[0]);
						MAP_OBJECT_DATA(object_display)->entity_language = g_strdup(file[1]);
					}
					g_strfreev( file );
				}
				else if ( !g_ascii_strcasecmp( key, "global" ) )
				{
					MAP_OBJECT_DATA(object_display)->entity_global = !g_ascii_strcasecmp(value, "true");
				}
				else
				{
					EntityOption_Update( MAP_OBJECT_DATA(object_display)->settings, key, (g_ascii_strcasecmp( value, "(null)" ) == 0 ? "" : value), type );
				}
			}
		}
	}
	else if ( g_ascii_strcasecmp(element_name, "path") == 0 )
	{
		map_info->parse_mode = MPM_PATH;
	}
	else if ( g_ascii_strcasecmp(element_name, "point") == 0 )
	{
		DisplayObject * object_point = g_new0(DisplayObject, 1);

		object_point->type = DT_POINT;
		object_point->layer = 43;
		object_point->w = object_point->h = 16;
		object_point->resizable = FALSE;
		for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
		{
			if ( g_ascii_strcasecmp (*attribute_names, "x") == 0 )
				object_point->x = atoi(*attribute_values);
			else if ( g_ascii_strcasecmp (*attribute_names, "y") == 0 )
				object_point->y = atoi(*attribute_values);
		}

		if ( object_point->x > object_display->w )
		{
			object_display->w  = object_point->x;
		}
		if ( object_point->y > object_display->h )
		{
			object_display->h  = object_point->y;
		}

		object_display->shape = g_slist_append( object_display->shape, object_point );
	}
}

/********************************
* map_parse_handler_start_root_element
*
*/
void map_parse_handler_start_root_element( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, MapInfo * map_info, GError **error )
{
	if ( g_ascii_strcasecmp( element_name, "settings" ) == 0 )
	{
		map_info->parse_mode = MPM_SETTING;
	}
	else if ( g_ascii_strcasecmp( element_name, "object" ) == 0 )
	{
		map_info->parse_mode = MPM_OBJECT;

		/* Created Display Object */
		DisplayObject * object_display = NULL;

		/*  */
		if ( map_info->file_type == 0 )
		{
			MapObjectData * object_data = MapObjectData_New(NULL);

			object_data->parent = map_info;

			for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
			{
				if ( g_ascii_strcasecmp(*attribute_names, "type") == 0 )
					object_data->type = MapObject_Type(*attribute_values);
				else if ( g_ascii_strcasecmp (*attribute_names, "value") == 0 )
					object_data->name = g_strdup(*attribute_values);
				else if ( g_ascii_strcasecmp(*attribute_names, "id") == 0 )
					object_data->object_name = g_strdup(*attribute_values);
				else if ( g_ascii_strcasecmp(*attribute_names, "parent") == 0 || g_ascii_strcasecmp(*attribute_names, "global") == 0 )
					EntityOption_InsertNew( object_data->settings, *attribute_names, *attribute_values, "hidden" );
			}

			object_display = Alchera_DisplayObject_New(object_data, &MapObjectData_FreePointer);

			MapObject_UpdateDisplayObject(object_data->type, object_display);
			MapObject_SetDefaultSettingOptions( object_data );

			if ( object_data->type == 't' )
			{
				object_display->text = g_strdup(object_data->name);
			}

		}
		else if ( map_info->file_type == 1 )
		{
			VirtualObjectData * virtual_object = g_new0(VirtualObjectData, 1);

			for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
			{
				if ( g_ascii_strcasecmp(*attribute_names, "type") == 0 )
					virtual_object->type = MapObject_Type(*attribute_values);
				else if ( g_ascii_strcasecmp (*attribute_names, "value") == 0 )
					virtual_object->name = g_strdup(*attribute_values);
				else if ( g_ascii_strcasecmp(*attribute_names, "id") == 0 )
					virtual_object->ident = g_strdup(*attribute_values);

			}

			object_display = Alchera_DisplayObject_New(virtual_object, &VirtualObjectData_FreePointer);
			MapObject_UpdateDisplayObject(virtual_object->type, object_display);

		}
		else
		{
			object_display = Alchera_DisplayObject_New(NULL, NULL);
		}

		object_display->id = map_info->id_counter++;
		map_info->display_list = g_list_append(map_info->display_list, object_display);
	}
	else
	{

	}
}

/********************************
* map_parse_handler_start
*
*/
void map_parse_handler_start( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, gpointer user_data, GError **error )
{
	MapInfo * map_info = (MapInfo *)user_data;

	if ( map_info->parse_mode == MPM_SETTING && map_info->file_type == 0)
	{
		/* Settings */
		map_parse_handler_start_settings( context, element_name, attribute_names, attribute_values, map_info, error );
	}
	else if ( map_info->parse_mode == MPM_PATH )
	{
		map_parse_handler_start_path_element( context, element_name, attribute_names, attribute_values, map_info, error );
	}
	else if ( map_info->parse_mode == MPM_OBJECT )
	{
		map_parse_handler_start_object_element( context, element_name, attribute_names, attribute_values, map_info, error );
	}
	else
	{
		/* Root Element */
		map_parse_handler_start_root_element( context, element_name, attribute_names, attribute_values, map_info, error );
	}
}

/********************************
* map_parse_handler_end
*
*/
void map_parse_handler_end( GMarkupParseContext * context, const gchar *element_name, gpointer user_data, GError **error )
{
	MapInfo * map_info = (MapInfo *)user_data;
	if ( g_ascii_strcasecmp( element_name, "settings" ) == 0 )
	{
		map_info->parse_mode = MPM_ROOT;
	}
	else if ( g_ascii_strcasecmp (element_name, "object") == 0 )
	{
		map_info->parse_mode = MPM_ROOT;
	}
	else if ( g_ascii_strcasecmp (element_name, "path") == 0 )
	{
		map_info->parse_mode = MPM_OBJECT;
	}
}

/********************************
* Map_ParseXML
*
@ map:
*/
gboolean Map_ParseXML( MapInfo * map_info, gchar * content )
{
	GError * error  = NULL;
	GMarkupParseContext * ctx;

	ctx = g_markup_parse_context_new( &map_parser_xml, (GMarkupParseFlags)0, map_info, NULL );
	if ( g_markup_parse_context_parse( ctx, content, -1, &error ) )
	{
		g_markup_parse_context_end_parse( ctx, NULL );
	}
	else
	{
		Meg_Error_Print( __func__, __LINE__, "Parsing setting error. Reason: %s", error->message );
		g_clear_error(&error);
	}
	g_markup_parse_context_free( ctx );

	return TRUE;
}


/********************************
* Map_GetDimensions
*
@ file:
@ width:
@ height:
*/
gboolean Map_GetDimensions( gchar * file, gint  * width, gint * height )
{
	GdkRectangle rect = { 0, 0, 1, 1 };
	GMarkupParseContext * ctx;
	gchar * content = NULL;
	gchar * dest = g_strdup_printf( "/maps/%s.xml", file );

	if ( Meg_file_get_contents( dest, &content, NULL, NULL ) )
	{
		ctx = g_markup_parse_context_new( &map_dimension_parser, (GMarkupParseFlags)0, &rect, NULL );
		g_markup_parse_context_parse( ctx, content, -1, NULL );
		g_markup_parse_context_end_parse( ctx, NULL );
		g_markup_parse_context_free( ctx );
	}

	*width = rect.width;
	*height = rect.height;

	g_free( dest );

	return TRUE;

}


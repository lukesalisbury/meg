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
#include "resources_functions.h"
#include "entity_functions.h"
#include "animation_functions.h"
#include "runtime_parser.h"
#include "runtime_options.h"

/* Global Variables */
extern GError * mokoiError;


/* Local Variables */

/* Local Function */


/*-------------------------------
	Sprite Handling Code
-------------------------------*/

/********************************
* Sprite_Find
*
*/
GSList * Sprite_Find( GSList * first, gchar * file_name )
{
	GSList * list_scan = first;
	while ( list_scan )
	{
		if ( !g_ascii_strcasecmp(((SheetObject *)list_scan->data)->display_name, file_name) )
		{
			return list_scan;
		}
		list_scan = g_slist_next( list_scan );
	}
	return NULL;
}



/********************************
* Sprite_Get
*
*/
SheetObject * Sprite_Get(gchar * name, gchar * sheet, gboolean load)
{
	SheetObject * sprite = NULL;
	Spritesheet * parent = Sheet_Get( sheet, FALSE );

	if ( parent )
	{
		if ( load )
		{
			Sheet_Load( parent );
		}
		GSList * child = Sprite_Find(parent->children, name);

		if ( child != NULL )
		{
			sprite = (SheetObject *)child->data;
			if ( SPRITE_DATA(sprite)->animation && SPRITE_DATA(sprite)->image_loaded == FALSE )
				SpriteAnimation_Build(sprite);
		}
		else
		{
			Meg_Log_Print( LOG_NONE,"Sprite_Get: Sprite (%s) not found\n", name);
		}
	}
	else
	{
		Meg_Log_Print( LOG_NONE,"Sprite_Get: Parent (%s) not found\n", sheet);
	}


	return sprite;
}

/********************************
* Sprite_GetFull
*
*/
SheetObject * Sprite_GetFull( gchar * full_ident, gboolean load )
{
	SheetObject * sprite = NULL;
	gchar ** ident_split = g_strsplit_set(full_ident, ":", 2);

	if ( ident_split )
	{
		sprite = Sprite_Get(ident_split[1], ident_split[0], load);
	}

	g_strfreev( ident_split );

	return sprite;
}


/********************************
* Sprite_GetPixbuf
*
*/
GdkPixbuf * Sprite_GetPixbuf(gchar * name, gchar * sheet)
{
	SheetObject * sprite = Sprite_Get(name, sheet, TRUE);
	if ( sprite )
	{
		if ( SPRITE_DATA(sprite)->image )
		{
			g_object_ref(SPRITE_DATA(sprite)->image);
			return SPRITE_DATA(sprite)->image;
		}
		else
		{
			SpriteAnimation_Build(sprite);
			if ( SPRITE_DATA(sprite)->image )
			{
				g_object_ref(SPRITE_DATA(sprite)->image);
				return SPRITE_DATA(sprite)->image;
			}
		}
	}
	return NULL;
}

/********************************
* Sprite_GetPixbuf
*
*/
GdkPixbuf * Sheet_GetPixbuf(gchar * name, Spritesheet * sheet )
{
	SheetObject * sprite = NULL;
	GSList * child = Sprite_Find(sheet->children, name);

	if ( child != NULL )
	{
		sprite = (SheetObject *)child->data;
		if ( SPRITE_DATA(sprite)->image )
		{
			g_object_ref(SPRITE_DATA(sprite)->image);
			return SPRITE_DATA(sprite)->image;
		}
		else
		{
			SpriteAnimation_Build(sprite);
			if ( SPRITE_DATA(sprite)->image )
			{
				g_object_ref(SPRITE_DATA(sprite)->image);
				return SPRITE_DATA(sprite)->image;
			}
		}
	}
	return NULL;
}


/*-------------------------------
	Animations Handling Code
-------------------------------*/

/********************************
* Animation_Get
* Returns 'name' AnimationDetail
*
*/
AnimationDetail * Animation_Get( gchar * name )
{
	if ( !name )
		return NULL;

	SheetObject * sprite = Sprite_GetFull(name, 1);

	if ( sprite )
	{
		return SPRITE_DATA(sprite)->animation;
	}
	return NULL;
}



/*-------------------------------
	Map Object Handling Code
-------------------------------*/

/********************************
* MapObject_UpdateSprite
*
*/
SheetObject * MapObject_UpdateSprite( DisplayObject * object )
{
	SheetObject * sprite = Sprite_GetFull( MAP_OBJECT_DATA(object)->name, TRUE );

	if ( object->image )
	{
		g_object_unref( object->image );
	}

	if ( !sprite )
	{
		object->type = DT_NONE;
	}
	else if ( SPRITE_DATA(sprite)->animation )
	{
		MAP_OBJECT_DATA(object)->type = 's';
		if ( SPRITE_DATA(sprite)->animation )
		{
			object->image = SPRITE_DATA(sprite)->image;
			object->type = DT_IMAGE;
			object->tw = SPRITE_DATA(sprite)->animation->w;
			object->th = SPRITE_DATA(sprite)->animation->h;
			object->animation_timeout = TRUE;

		}
		else if ( SPRITE_DATA(sprite)->image )
		{
			Meg_Log_Print( LOG_NONE, "No animation image for '%s' animation, using static image instead.", MAP_OBJECT_DATA(object)->name );
			object->image = SPRITE_DATA(sprite)->image;
			object->tw = gdk_pixbuf_get_width( object->image );
			object->th = gdk_pixbuf_get_height( object->image );
			object->type = DT_IMAGE;
		}
		else
			Meg_Log_Print( LOG_NONE,"No static image for '%s' animation", MAP_OBJECT_DATA(object)->name );
	}
	else if ( !SPRITE_DATA(sprite)->image )
	{
		object->type = DT_NONE;
	}
	else
	{
		MAP_OBJECT_DATA(object)->type = 's';

		object->type = DT_IMAGE;
		object->image = SPRITE_DATA(sprite)->image;
		object->tw = gdk_pixbuf_get_width( object->image );
		object->th = gdk_pixbuf_get_height( object->image );
	}

	if ( sprite )
	{
		gchar ** ident_split = g_strsplit_set(MAP_OBJECT_DATA(object)->name, ":", 2);
		guint c = 0;
		while ( c < 8 )
		{
			if ( SPRITE_DATA(sprite)->childrens[c].position != -1 && SPRITE_DATA(sprite)->childrens[c].name != NULL )
			{
				SheetObject * border_image = Sprite_Get( SPRITE_DATA(sprite)->childrens[c].name, ident_split[0], FALSE );
				Alchera_DisplayObject_AddBorder( object, SPRITE_DATA(border_image)->image, SPRITE_DATA(sprite)->childrens[c].position, SPRITE_DATA(sprite)->childrens[c].repeat );
				g_object_ref( SPRITE_DATA(border_image)->image );
			}
			c++;
		}
		g_strfreev(ident_split);

	}

	g_object_ref( object->image );

	return sprite;
}

/********************************
* MapObject_AddSimplePolygonPoint
*
*/
inline void MapObject_AddSimplePolygonPoint( DisplayObject * object, gint x, gint y)
{
	DisplayObject * obj_point = g_new0(DisplayObject, 1);
	obj_point->type = DT_POINT;
	obj_point->layer = 43;
	obj_point->w = obj_point->h = 16;
	obj_point->x = x;
	obj_point->y = y;
	obj_point->resizable = FALSE;
	object->shape = g_slist_append( object->shape, obj_point );
}



/********************************
* MapObject_SetInitialType
*
*/
inline char MapObject_SetInitialType( DisplayObject * object, gchar * name, gdouble area_width, gdouble area_height )
{
	char internal_type = 's';
	if ( !g_ascii_strcasecmp( name, "Rectangle") )
	{
		internal_type = 'r'; // Internal Type
		object->type = DT_RECTANGLE; // Type for Widget
		object->w = area_width;
		object->h = area_height;
	}
	else if ( !g_ascii_strcasecmp( name, "Circle") )
	{
		internal_type = 'c';
		object->type = DT_CIRCLE;
		object->w = area_width;
		object->h = area_height;
	}
	else if ( !g_ascii_strcasecmp( name, "Polygon") )
	{
		internal_type = 'p';
		object->type = DT_POLYGON;
		object->w = area_width;
		object->h = area_height;
		object->resizable = FALSE;


		MapObject_AddSimplePolygonPoint( object, area_width/2, 0);
		MapObject_AddSimplePolygonPoint( object, area_width, area_height);
		MapObject_AddSimplePolygonPoint( object, 0, area_height);
	}
	else if ( !g_ascii_strcasecmp( name, "Text") )
	{
		internal_type = 't';

		object->type = DT_TEXT;
		object->text = g_strdup("New Text");
		object->resizable = FALSE;

		object->w = 64;
		object->h = 8;
		object->tw = 64;
		object->th = 8;
	}
	else if ( !g_ascii_strcasecmp( name, "Line") )
	{
		internal_type = 'l';
		object->type = DT_LINE;
		object->w = area_width;
		object->h = area_height;
	}
	else
	{
		internal_type = 's';
	}

	return internal_type;
}



/********************************
* MapObject_New
*
*/
DisplayObject * MapObject_New( gchar * name, gdouble area_width, gdouble area_height )
{
	DisplayObject * object = NULL;
	MapObjectData * object_data = NULL;

	/* Set a position area */
	if ( area_width <= 0.0 )
		area_width = 32.0;

	if ( area_height <= 0.0 )
		area_height = 32.0;

	object_data = MapObjectData_New( name );
	object = Alchera_DisplayObject_New( object_data, &MapObjectData_FreePointer );

	object_data->type = MapObject_SetInitialType( object, name, area_width, area_height );

	if ( object_data->type == 't' )
	{
		RuntimeSetting_InsertNew( object_data->settings, "number", "-1", "hidden" );
	}
	else if ( object_data->type == 's' )
	{
		SheetObject * sprite = MapObject_UpdateSprite( object );

		/* Set Default Runtime options */
		if ( sprite && SPRITE_DATA(sprite)->entity )
		{
			RuntimeSetting_SetDefaultValues( object );

			gchar ** file = g_strsplit( SPRITE_DATA(sprite)->entity, ".", 2);
			if ( g_strv_length(file) == 2 )
			{
				REPLACE_STRING( object_data->entity_file, g_strdup(file[0]) );
				REPLACE_STRING( object_data->entity_language, g_strdup(file[1]) );
			}
			g_strfreev( file );
		}

	}

	return object;
}



/********************************
* VirtualObject_UpdateSprite
*
*/
SheetObject * VirtualObject_UpdateSprite( DisplayObject * object )
{
	SheetObject * sprite = Sprite_GetFull( VIRTUAL_OBJECT_DATA(object)->name, TRUE );

	if ( object->image )
	{
		g_object_unref( object->image );
	}

	if ( !sprite )
	{
		object->type = DT_NONE;
	}
	else if ( !SPRITE_DATA(sprite)->image )
	{
		object->type = DT_NONE;
	}
	else
	{
		VIRTUAL_OBJECT_DATA(object)->type = 's';

		object->type = DT_IMAGE;
		object->image = SPRITE_DATA(sprite)->image;
		object->tw = gdk_pixbuf_get_width( object->image );
		object->th = gdk_pixbuf_get_height( object->image );
	}


	g_object_ref( object->image );

	return sprite;
}

/********************************
* VirtualObject_New
*
*/
DisplayObject * VirtualObject_New(gchar * name, gdouble area_width, gdouble area_height )
{

	/* Set a position area */
	if ( area_width <= 0.0 )
		area_width = 32.0;

	if ( area_height <= 0.0 )
		area_height = 32.0;

	//
	DisplayObject * object = NULL;
	MapObjectData * object_data = MapObjectData_New( name );

	object = Alchera_DisplayObject_New( object_data, &MapObjectData_FreePointer );

	object_data->type = MapObject_SetInitialType( object, name, area_width, area_height );



	if ( object_data->type == 's' )
	{
		VirtualObject_UpdateSprite( object );

	}

	return object;

}

/********************************
* VirtualObject_Type
*
@ type_name:
*/
DisplayObjectTypes VirtualObject_Type( const gchar * type_name )
{
	if ( !g_ascii_strcasecmp(type_name, "sprite") )
		return DT_IMAGE;
	else if ( !g_ascii_strcasecmp(type_name, "rect") )
		return DT_RECTANGLE;
	else if ( !g_ascii_strcasecmp(type_name, "line") )
		return DT_LINE;
	else if ( !g_ascii_strcasecmp(type_name, "text") )
		return DT_TEXT;
	else if ( !g_ascii_strcasecmp(type_name, "circle") )
		return DT_CIRCLE;
	else if ( !g_ascii_strcasecmp(type_name, "polygon") )
		return DT_POLYGON;

	g_warning("VirtualObject_Type: Error %s\n", type_name );
	return DT_NONE;

}



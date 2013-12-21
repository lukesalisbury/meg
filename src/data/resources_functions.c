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
GdkPixbuf * SpriteSheet_GetPixbuf(gchar * name, Spritesheet * sheet )
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
SheetObject * MapObject_UpdateSprite( MokoiMapObject * object )
{
	SheetObject * sprite = NULL;
	gchar ** ident_split = g_strsplit_set(object->name, ":", 2);

	if ( ident_split )
	{
		sprite = Sprite_Get(ident_split[1], ident_split[0], TRUE);
	}
	else
	{
		return sprite;
	}

	if ( object->object->image )
	{
		g_object_unref( object->object->image );
	}

	if ( !sprite )
	{
		object->object->type = DT_NONE;
	}
	else if ( SPRITE_DATA(sprite)->animation )
	{
		object->type = 's';
		if ( SPRITE_DATA(sprite)->animation )
		{
			object->object->image = SPRITE_DATA(sprite)->image;
			object->object->type = DT_IMAGE;
			object->object->tw = SPRITE_DATA(sprite)->animation->w;
			object->object->th = SPRITE_DATA(sprite)->animation->h;
			object->object->timeout = TRUE;
			//g_timeout_add(gdk_pixbuf_animation_iter_get_delay_time((GdkPixbufAnimationIter *)object->object->data ), (GSourceFunc)Meg_MapObject_PushAnimation, (gpointer)object->object);

		}
		else if ( SPRITE_DATA(sprite)->image )
		{
			Meg_Log_Print( LOG_NONE, "No animation image for '%s' animation, using static image instead.", object->name);
			object->object->image = SPRITE_DATA(sprite)->image;
			object->object->tw = gdk_pixbuf_get_width( SPRITE_DATA(sprite)->image );
			object->object->th = gdk_pixbuf_get_height( SPRITE_DATA(sprite)->image );
			object->object->type = DT_IMAGE;
		}
		else
			Meg_Log_Print( LOG_NONE,"No static image for '%s' animation", object->name);
	}
	else if ( !SPRITE_DATA(sprite)->image )
	{
		object->object->type = DT_NONE;
	}
	else
	{
		object->type = 's';
		object->object->type = DT_IMAGE;
		object->object->image = SPRITE_DATA(sprite)->image;
		object->object->supports_path = TRUE;

		object->object->tw = gdk_pixbuf_get_width( SPRITE_DATA(sprite)->image );
		object->object->th = gdk_pixbuf_get_height( SPRITE_DATA(sprite)->image );
	}

	if ( sprite )
	{
		guint c = 0;
		while ( c < 8 )
		{
			if ( SPRITE_DATA(sprite)->childrens[c].position != -1 && SPRITE_DATA(sprite)->childrens[c].name != NULL )
			{
				SheetObject * border_image = Sprite_Get( SPRITE_DATA(sprite)->childrens[c].name, ident_split[0], FALSE );
				Alchera_DisplayObject_AddBorder( object->object, SPRITE_DATA(border_image)->image, SPRITE_DATA(sprite)->childrens[c].position, SPRITE_DATA(sprite)->childrens[c].repeat );
				g_object_ref( SPRITE_DATA(border_image)->image );
			}
			c++;
		}

	}

	g_strfreev(ident_split);


	g_object_ref( object->object->image );

	return sprite;
}



/********************************
* MapObject_New
*
*/
MokoiMapObject * MapObject_New(gchar * name, gdouble area_width, gdouble area_height )
{
	MokoiMapObject * object = g_new0(MokoiMapObject, 1);
	object->type = 'u';
	object->name = g_strdup(name);
	object->settings = g_hash_table_new(g_str_hash, g_str_equal);
	object->object = g_new0(DisplayObject, 1);
	object->object->rotate = 0;
	object->object->type = DT_OTHER;
	object->object->resizable = TRUE;
	object->object->supports_path = FALSE;
	object->object->flip = 0;
	object->object->colour.red = object->object->colour.blue \
							= object->object->colour.green \
							= object->object->colour.alpha \
							= 1.0;
	object->colour8.red = object->colour8.blue \
			= object->colour8.green \
			= object->colour8.alpha \
			= 255;

	if ( area_width <= 0.0 )
		area_width = 32.0;

	if ( area_height <= 0.0 )
		area_height = 32.0;

	if ( !g_ascii_strcasecmp( name, "Rectangle") )
	{
		object->type = 'r'; // Internal Type
		object->object->type = DT_RECTANGLE; // Type for Widget
		object->object->w = area_width;
		object->object->h = area_height;
	}
	else if ( !g_ascii_strcasecmp( name, "Circle") )
	{
		object->type = 'c';
		object->object->type = DT_CIRCLE;
		object->object->w = area_width;
		object->object->h = area_height;
	}
	else if ( !g_ascii_strcasecmp( name, "Polygon") )
	{

		object->type = 'p';
		object->object->type = DT_POLYGON;
		object->object->w = area_width;
		object->object->h = area_height;
		object->object->resizable = FALSE;

		DisplayObject * objn = g_new0(DisplayObject, 1);
		object->object->shape = g_slist_append( object->object->shape, objn );
		objn->type = DT_POINT;
		objn->layer = 43;
		objn->w = objn->h = 16;
		objn->x = area_width/2;
		objn->y = 0;
		objn->resizable = FALSE;

		objn = g_new0(DisplayObject, 1);
		object->object->shape = g_slist_append( object->object->shape, objn );
		objn->type = DT_POINT;
		objn->layer = 43;
		objn->w = objn->h = 16;
		objn->x = area_width;
		objn->y = area_height;
		objn->resizable = FALSE;

		objn = g_new0(DisplayObject, 1);
		object->object->shape = g_slist_append( object->object->shape, objn );
		objn->type = DT_POINT;
		objn->layer = 43;
		objn->w = objn->h = 16;
		objn->x = 0;
		objn->y = area_height;
		objn->resizable = FALSE;

	}
	else if ( !g_ascii_strcasecmp( name, "Text") )
	{
		object->type = 't';
		object->object->type = DT_TEXT;
		object->object->data = g_strdup("New Text");
		object->object->resizable = FALSE;

		object->object->w = 64;
		object->object->h = 8;
		object->object->tw = 64;
		object->object->th = 8;
		RuntimeSettingsStruct * option = RuntimeSetting_New( "-1", "" );
		g_hash_table_replace( object->settings, "number", (gpointer)option );
	}
	else if ( !g_ascii_strcasecmp( name, "Line") )
	{
		object->type = 'l';
		object->object->type = DT_LINE;
		object->object->data = NULL;
		object->object->w = area_width;
		object->object->h = area_height;
	}
	else
	{
		object->type = 's';
		SheetObject * sprite = MapObject_UpdateSprite( object );

		/* Set Default Runtime options */
		if ( sprite && SPRITE_DATA(sprite)->entity )
		{
			RuntimeSetting_SetDefaultValues( object );

			gchar ** file = g_strsplit( SPRITE_DATA(sprite)->entity, ".", 2);
			if ( g_strv_length(file) == 2 )
			{
				REPLACE_STRING( object->entity_file, g_strdup(file[0]) );
				REPLACE_STRING( object->entity_language, g_strdup(file[1]) );
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
void VirtualObject_UpdateSprite( VirtualObject * object )
{
	SheetObject * sprite = NULL;
	gchar ** ident_split = g_strsplit_set(object->name, ":", 2);

	if ( ident_split )
	{
		sprite = Sprite_Get(ident_split[1], ident_split[0], TRUE);
	}
	else
	{
		return;
	}

	if ( object->object->image )
	{
		g_object_unref( object->object->image );
	}

	if ( !sprite )
	{
		object->object->type = DT_NONE;
	}
	else if ( SPRITE_DATA(sprite)->animation )
	{
		object->type = 's';
		if ( SPRITE_DATA(sprite)->animation )
		{
			object->object->image = SPRITE_DATA(sprite)->image;
			object->object->type = DT_IMAGE;
			object->object->tw = SPRITE_DATA(sprite)->animation->w;
			object->object->th = SPRITE_DATA(sprite)->animation->h;
			object->object->timeout = TRUE;
		}
		else if ( SPRITE_DATA(sprite)->image )
		{
			Meg_Log_Print( LOG_NONE, "No animation image for '%s' animation, using static image instead.", object->name);
			object->object->image = SPRITE_DATA(sprite)->image;
			object->object->tw = gdk_pixbuf_get_width( SPRITE_DATA(sprite)->image );
			object->object->th = gdk_pixbuf_get_height( SPRITE_DATA(sprite)->image );
			object->object->type = DT_IMAGE;
		}
		else
			Meg_Log_Print( LOG_NONE,"No static image for '%s' animation", object->name);
	}
	else if ( !SPRITE_DATA(sprite)->image )
	{
		object->object->type = DT_NONE;
	}
	else
	{
		object->type = 's';
		object->object->type = DT_IMAGE;
		object->object->image = SPRITE_DATA(sprite)->image;
		object->object->supports_path = TRUE;

		object->object->tw = gdk_pixbuf_get_width( SPRITE_DATA(sprite)->image );
		object->object->th = gdk_pixbuf_get_height( SPRITE_DATA(sprite)->image );
	}

	if ( sprite )
	{
		guint c = 0;
		while ( c < 8 )
		{
			if ( SPRITE_DATA(sprite)->childrens[c].position != -1 && SPRITE_DATA(sprite)->childrens[c].name != NULL )
			{
				SheetObject * border_image = Sprite_Get( SPRITE_DATA(sprite)->childrens[c].name, ident_split[0], FALSE );
				Alchera_DisplayObject_AddBorder( object->object, SPRITE_DATA(border_image)->image, SPRITE_DATA(sprite)->childrens[c].position, SPRITE_DATA(sprite)->childrens[c].repeat );
				g_object_ref( SPRITE_DATA(border_image)->image );
			}
			c++;
		}

	}

	g_strfreev(ident_split);


	g_object_ref( object->object->image );

	return;
}

/********************************
* VirtualObject_New
*
*/
VirtualObject * VirtualObject_New(gchar * name, gdouble area_width, gdouble area_height )
{
	VirtualObject * object = g_new0(VirtualObject, 1);
	object->type = 'u';
	object->name = g_strdup(name);
	object->object = g_new0(DisplayObject, 1);
	object->object->rotate = 0;
	object->object->type = DT_OTHER;
	object->object->resizable = TRUE;
	object->object->supports_path = FALSE;
	object->object->flip = 0;
	object->object->colour.red = object->object->colour.blue \
							= object->object->colour.green \
							= object->object->colour.alpha \
							= 1.0;

	if ( area_width <= 0.0 )
		area_width = 32.0;

	if ( area_height <= 0.0 )
		area_height = 32.0;

	if ( !g_ascii_strcasecmp( name, "Rectangle") )
	{
		object->type = 'r'; // Internal Type
		object->object->type = DT_RECTANGLE; // Type for Widget
		object->object->w = area_width;
		object->object->h = area_height;
	}
	else if ( !g_ascii_strcasecmp( name, "Circle") )
	{
		object->type = 'c';
		object->object->type = DT_CIRCLE;
		object->object->w = area_width;
		object->object->h = area_height;
	}
	else if ( !g_ascii_strcasecmp( name, "Polygon") )
	{

		object->type = 'p';
		object->object->type = DT_POLYGON;
		object->object->w = area_width;
		object->object->h = area_height;
		object->object->resizable = FALSE;

		DisplayObject * objn = g_new0(DisplayObject, 1);
		object->object->shape = g_slist_append( object->object->shape, objn );
		objn->type = DT_POINT;
		objn->layer = 43;
		objn->w = objn->h = 16;
		objn->x = area_width/2;
		objn->y = 0;
		objn->resizable = FALSE;

		objn = g_new0(DisplayObject, 1);
		object->object->shape = g_slist_append( object->object->shape, objn );
		objn->type = DT_POINT;
		objn->layer = 43;
		objn->w = objn->h = 16;
		objn->x = area_width;
		objn->y = area_height;
		objn->resizable = FALSE;

		objn = g_new0(DisplayObject, 1);
		object->object->shape = g_slist_append( object->object->shape, objn );
		objn->type = DT_POINT;
		objn->layer = 43;
		objn->w = objn->h = 16;
		objn->x = 0;
		objn->y = area_height;
		objn->resizable = FALSE;

	}
	else if ( !g_ascii_strcasecmp( name, "Text") )
	{
		object->type = 't';
		object->object->type = DT_TEXT;
		object->object->data = g_strdup("New Text");
		object->object->resizable = FALSE;

		object->object->w = 64;
		object->object->h = 8;
		object->object->tw = 64;
		object->object->th = 8;

	}
	else if ( !g_ascii_strcasecmp( name, "Line") )
	{
		object->type = 'l';
		object->object->type = DT_LINE;
		object->object->data = NULL;
		object->object->w = area_width;
		object->object->h = area_height;
	}
	else
	{
		object->type = 's';
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



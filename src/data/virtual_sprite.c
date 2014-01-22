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
#include "widgets/widget_map.h"
#include "gtk_compat.h"
#include "maps.h"
#include "virtual_sprite.h"

/* External Functions */
gboolean Map_ParseXML( MapInfo * map_info, gchar * content );
gchar * MapObject_TypeName( gchar type );


/* Global Variables */


/* Local Variables */
Spritesheet * virtual_spritesheet = NULL;

/* UI */


/********************************
* VirtualSprite_LoadXML
*
*/
MapInfo * VirtualSprite_LoadXML( gchar * id )
{
	MapInfo * map_info = g_new0(MapInfo, 1);
	gchar * file_path = NULL, * content = NULL;

	map_info->file_type = 1;
	map_info->name = g_strdup(id);


	file_path = g_strdup_printf( "/sprites/virtual/%s.xml", id  );

	if ( Meg_file_get_contents( file_path, &content, NULL, NULL ) )
	{
		Map_ParseXML( map_info, content );

		/* Update display_list */

		DisplayObject * object = NULL;
		GList * scan = g_list_first( map_info->display_list );
		while ( scan )
		{
			object = (DisplayObject *)scan->data;

			switch ( MAP_OBJECT_DATA(object)->type )
			{
				case 's':
					MapObject_UpdateSprite( object );
					break;
				case 't':
				case 'l':
				case 'r':
				case 'p':
				case 'c':
					object->type = MapObject_Internal2DisplayObjectType(MAP_OBJECT_DATA(object)->type);
					break;
				default:

					break;
			}



			map_info->width = MAX( map_info->width, object->x + object->w );
			map_info->height = MAX( map_info->height, object->y + object->h );


			scan = g_list_next( scan );
		}

	}

	g_free(file_path);
	g_free(content);
	return map_info;
}

/********************************
* VirtualSprite_BuildPixbuf
*
*/
GdkPixbuf *  VirtualSprite_BuildPixbuf( gchar * id )
{
	GdkPixbuf * image = NULL;
	MapInfo * map_info = NULL;

	if ( !g_ascii_strcasecmp( id, "Add New" ) )
	{
		return NULL;
	}
	map_info = VirtualSprite_LoadXML( id );


	if ( map_info->width > 0 && map_info->height > 0 )
	{
		/* Save Image of Map */
		cairo_surface_t * cst;

		cst = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, map_info->width, map_info->height );

		if ( cst )
		{
			cairo_t *  cr = cairo_create( cst );

			GList * scan = scan = g_list_first( map_info->display_list );
			while ( scan )
			{
				DisplayObject * obj = (DisplayObject *)scan->data;
				Alchera_DisplayObject_DrawForeach( obj, cr );
				scan = g_list_next( scan );
			}
			cairo_destroy( cr );

			image = gdk_pixbuf_get_from_surface( cst, 0, 0, map_info->width, map_info->height );
			cairo_surface_write_to_png(cst, "temp-cairo.png");
			gdk_pixbuf_save( image, "temp-pixbuf.png", "png", NULL, NULL);
		}

		Map_Free( map_info, FALSE );
	}



	return image;
}

/********************************
* VirtualSprite_SaveXML
*
*/
gboolean VirtualSprite_SaveXML( MapInfo * map_info )
{
	g_return_val_if_fail( map_info != NULL, FALSE );

	GList * scan = NULL;
	GString * map_string = g_string_new("<map xmlns=\"http://mokoi.info/projects/mokoi\">\n");

	g_string_append( map_string, "\t<settings>\n" );

	g_string_append_printf( map_string, "\t\t<dimensions width=\"%d\" height=\"%d\"/>\n", map_info->width, map_info->height);

	g_string_append( map_string, "\t</settings>\n" );

	/* Objects */
	scan = g_list_first( map_info->display_list );
	while ( scan )
	{
		gint x,y,w,h,z,l;

		VirtualObjectData * object = NULL;
		DisplayObject * display_object = NULL;


		display_object = (DisplayObject *)scan->data;
		g_return_val_if_fail( display_object != NULL, FALSE );

		object = (VirtualObjectData *)display_object->data;
		g_return_val_if_fail( object != NULL, FALSE );

		if ( display_object->type != DT_DELETE )
		{
			x = (gint)display_object->x;
			y = (gint)display_object->y;
			l = display_object->layer;
			z = l * 1000;

			if ( display_object->type == DT_LINE )
			{
				// Line Object use w/h as 2nd x/y
				w = (gint)(display_object->w + display_object->x);
				h = (gint)(display_object->h + display_object->y);
			}
			else
			{
				w = (gint)display_object->w;
				h = (gint)display_object->h;
			}
			if ( object->ident )
			{
				g_string_append_printf( map_string, "\t<object value=\"%s\" type=\"%s\" id=\"%s\">\n", object->name, MapObject_TypeName(object->type), object->ident );
			}
			else
			{
				g_string_append_printf( map_string, "\t<object value=\"%s\" type=\"%s\">\n", object->name, MapObject_TypeName(object->type) );
			}
			g_string_append_printf( map_string, "\t\t<position x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\" z=\"%d\" l=\"%d\" r=\"%d\" f=\"%d\"/>\n", x, y, w, h, z, l, display_object->rotate*90, display_object->flip);

			if ( display_object->type == DT_POLYGON )
			{
				GSList * point_scan = NULL;
				DisplayObject * point_object = NULL;
				g_string_append_printf( map_string, "\t\t<option points=\"%u\"/>\n", g_slist_length(display_object->shape) );

				point_scan = display_object->shape;
				while ( point_scan )
				{
					point_object = (DisplayObject *)point_scan->data;
					g_string_append_printf( map_string, "\t\t<point x=\"%d\" y=\"%d\"/>\n", (gint)point_object->x, (gint)point_object->y );
					point_scan = g_slist_next(point_scan);
				}
			}

			g_string_append_printf( map_string, "\t</object>\n");
		}

		scan = g_list_next(scan);
	}
	g_string_append_printf( map_string, "</map>");

	/* Write map file */
	gchar * file_path = g_strdup_printf( "/sprites/virtual/%s.xml", map_info->name );
	Meg_file_set_contents( file_path, map_string->str, -1, NULL);
	g_free( file_path );

	VirtualSpriteSheet_Insert( map_info->name, TRUE );

	return TRUE;
}

/********************************
* VirtualSprite_GetInfo
*
*/
MapInfo * VirtualSprite_GetInfo( gchar * id )
{
	MapInfo * map_info = NULL;

	if ( !g_ascii_strcasecmp( id, "Add New" ) )
	{
		map_info = g_new0(MapInfo, 1);
		map_info->name = g_strdup( "NewVirtual" );
		map_info->width = 64;
		map_info->height = 64;
		map_info->file_type = 1;
		map_info->data = g_new0(MapData, 1);
	}
	else
	{
		map_info = VirtualSprite_LoadXML( id );

	}

	return map_info;
}

/********************************
* VirtualSprite_Save
*
*/
void VirtualSprite_Save( GtkButton * button, MapInfo * map_info )
{
	VirtualSprite_SaveXML( map_info );
}

/********************************
* VirtualSprite_UpdateName
*
*/
void VirtualSprite_UpdateName(GtkLabel * label, MapInfo * map_info)
{
	g_return_if_fail( map_info );

	gchar * marked_up_text = NULL;

	marked_up_text = g_markup_printf_escaped( "%s <a href=\"\">rename</a>", map_info->name );
	gtk_label_set_markup( label , marked_up_text );
	g_free( marked_up_text );

}



/********************************
* VirtualSprite_FindCompare
*
*/
gint VirtualSprite_FindCompare(gconstpointer a, gconstpointer b)
{
	SheetObject * list_object = (SheetObject *)a;
	gchar * file_name = (gchar*)b;

	return g_ascii_strcasecmp( list_object->display_name, file_name );
}

/********************************
* VirtualSpriteSheet_Insert
*
*/
gboolean VirtualSpriteSheet_Insert( gchar * name, gboolean visible )
{
	GSList * scan = NULL;

	scan = g_slist_find_custom( virtual_spritesheet->children, name, VirtualSprite_FindCompare );

	if ( !scan )
	{
		SheetObject * sprite = NULL;
		SpriteData * sprite_data = g_new0(SpriteData, 1);

		sprite = SheetObject_New( (gpointer)sprite_data, &SpriteData_FreePointer );

		sprite->display_name = g_strdup(name);
		sprite->parent_sheet = g_strdup("Virtual");
		sprite->ident_string = g_strdup_printf( "Virtual:%s", sprite->display_name );
		sprite->visible = visible;

		virtual_spritesheet->children = g_slist_append( virtual_spritesheet->children, (gpointer)sprite );

		return TRUE;
	}

	return FALSE;

}


/********************************
* VirtualSprite_UpdateSpriteInfo
*
*/
gboolean VirtualSprite_UpdateSpriteInfo( gchar * name )
{
	GSList * scan = NULL;

	scan = g_slist_find_custom( virtual_spritesheet->children, name, VirtualSprite_FindCompare );

	if ( scan )
	{
		SheetObject * sprite = (SheetObject*)scan->data;

		SPRITE_DATA(sprite)->image = VirtualSprite_BuildPixbuf( name );
		SPRITE_DATA(sprite)->image_loaded = TRUE;

		return TRUE;
	}

	return FALSE;

}

/********************************
* VirtualSpriteSheet_Get
*
*/
Spritesheet * VirtualSpriteSheet_Get(gboolean update_sprite_image)
{
	if ( virtual_spritesheet == NULL)
	{
		virtual_spritesheet = g_new0(Spritesheet, 1);
		virtual_spritesheet->file = g_strdup("Virtual");
		virtual_spritesheet->image_loaded = FALSE;
		virtual_spritesheet->visible = TRUE;
		virtual_spritesheet->children = NULL;
		virtual_spritesheet->image = NULL;
		virtual_spritesheet->ref++;

		/* Spritesheet loading */
		gchar ** files = PHYSFS_enumerateFiles("/sprites/virtual/");
		gchar ** current;

		for ( current = files; *current != NULL; current++ )
		{
			if ( g_str_has_suffix( *current, ".xml" ) )
			{
				gchar * file_name = g_strndup(*current, g_utf8_strlen(*current, -1) - 4 ); // Strip .xml
				VirtualSpriteSheet_Insert( file_name, TRUE );
				g_free(file_name);
			}
		}
		PHYSFS_freeList(files);

		VirtualSpriteSheet_Insert( "Add New", FALSE );

	}

	if ( update_sprite_image )
	{
		GSList * scan = NULL;

		scan = virtual_spritesheet->children;

		while ( scan )
		{
			SheetObject * sprite = (SheetObject*)scan->data;
			if ( sprite )
			{
				SPRITE_DATA(sprite)->image = VirtualSprite_BuildPixbuf( sprite->display_name );
				SPRITE_DATA(sprite)->image_loaded = ( SPRITE_DATA(sprite)->image != NULL );
			}
			scan = g_slist_next( scan );
		}
	}


	return virtual_spritesheet;
}



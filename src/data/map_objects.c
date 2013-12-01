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
#include "object_advance.h"
#include "sheets_functions.h"
#include "animation_functions.h"

/* External Functions */


/* Local Type */
typedef enum {
	OT_RECTANGLE,
	OT_CIRCLE,
	OT_LINE,
	OT_TEXT,
	OT_POLYGON
} ObjectImageTypes;

/* Global Variables */
extern GSList * mokoiSpritesheets;

/* Local Variables */
GdkPixbuf * mokoiObjectImage[4] = { NULL, NULL, NULL, NULL };
GdkPixbuf * mokoiObjectMissing = NULL;

/* UI */

/* Events */


/********************************
* AL_Object_List
* Get the current map list of display objects
*/
GList * AL_Object_List( MapInfo * map )
{
	MokoiMap * mokoi_map_data = (MokoiMap *)map->data;

	/* MokoiMapObject hold the Full Details about the Map Object.
	   This includes the <DisplayObject> used on the map editor.
	*/
	MokoiMapObject * obj = NULL;
	GList * scan = g_list_first( mokoi_map_data->objects );

	while ( scan )
	{
		obj = (MokoiMapObject *)scan->data;

		obj->object->tw = obj->object->th = 1;
		obj->object->type = DT_NONE;
		obj->object->timeout = FALSE;
		switch ( obj->type )
		{
			case 's':
				MapObject_UpdateSprite( obj );
				break;
			case 't':
				obj->object->type = DT_TEXT;
				obj->object->data = (gpointer)g_strdup( obj->name );
				break;
			case 'l':
				obj->object->type = DT_LINE;
				break;
			case 'r':
				obj->object->type = DT_RECTANGLE;
				break;
			case 'p':
				obj->object->type = DT_POLYGON;
				obj->object->resizable = FALSE;
				break;
			case 'c':
				obj->object->type = DT_CIRCLE;
				break;
			default:
				obj->object->type = DT_OTHER;
				break;
		}

		if ( obj->object->image )
		{
			obj->object->type = DT_IMAGE;
			obj->object->tw = gdk_pixbuf_get_width( obj->object->image );
			obj->object->th = gdk_pixbuf_get_height( obj->object->image );
		}

		if ( obj->object->type == DT_IMAGE || obj->object->type == DT_ANIMATION )
		{
			if ( obj->object->w < 1.00 )
			{
				obj->object->w = obj->object->tw;
			}
			if ( obj->object->h < 1.00 )
			{
				obj->object->h = obj->object->th;
			}
		}


		obj->object->active = obj->object->tile = FALSE;
		obj->object->id = obj->id;
		map->displayList = g_list_append(map->displayList, obj->object);
		scan = g_list_next( scan );
	}
	return NULL;
}


/********************************
* DisplayObject_UpdateInformation
*
*/
void DisplayObject_UpdateInformation( DisplayObject * object, gchar * ident, gdouble x, gdouble y, gdouble w, gdouble h, gint z)
{
	if ( !object )
		return;

	object->x = x;
	object->y = y;
	object->layer = z;
	object->tw = object->th = 1;
	object->w = w;
	object->h = h;


	if ( object->type == DT_IMAGE || object->type == DT_ANIMATION )
	{
		if ( object->image )
		{
			object->tw = gdk_pixbuf_get_width( object->image );
			object->th = gdk_pixbuf_get_height( object->image );
			if ( object->w <= 0.00 )
			{
				object->w = object->tw;
			}
			if ( object->h <= 0.00 )
			{
				object->h = object->th;
			}
		}

	}
	else
	{
		if ( object->w <= 0.00 )
		{
			object->w = 16.0;
		}
		if ( object->h <= 0.00 )
		{
			object->h = 16.0;
		}
	}
}
/********************************
* AL_Object_Add
* Add object to current map
@ ident: resource id of object.
*/
DisplayObject * AL_Object_Add( MapInfo * map, gchar * ident, gdouble x, gdouble y, gdouble w, gdouble h, gint z)
{
	DisplayObject * object = NULL;


	if ( map->file_type == 0 )
	{
		MokoiMapObject * map_obj = NULL;
		MokoiMap * mokoi_map_data = (MokoiMap *)map->data;

		map_obj = MapObject_New( ident, w, h );

		if ( !map_obj )
			return object;

		object = map_obj->object;

		map_obj->parent = map;
		mokoi_map_data->objects = g_list_append(mokoi_map_data->objects, (gpointer)map_obj);
		map_obj->id = object->id = mokoi_map_data->id_count++;

	}
	else if ( map->file_type == 1 )
	{
		VirtualObject * virt_obj = NULL;
		VirtualObjectList * virtual_object_list = (VirtualObjectList *)map->data;

		virt_obj = VirtualObject_New( ident, w, h );

		if ( !virt_obj )
			return object;


		object = virt_obj->object;

		virtual_object_list->objects = g_list_append(virtual_object_list->objects, (gpointer)virt_obj );
		object->id = virtual_object_list->id_count++;

	}

	if ( object )
	{
		DisplayObject_UpdateInformation( object, ident, x, y, w, h, z );
	}

	return object;
}

/********************************
* AL_Object_Remove
*
@ id: id for display object.
*/
G_MODULE_EXPORT gboolean AL_Object_Remove( MapInfo * map, gint id )
{
	MokoiMap * mokoi_map_data = (MokoiMap *)map->data;
	if ( mokoi_map_data )
	{
		GList * object = g_list_nth( mokoi_map_data->objects, id );
		if ( object != NULL )
		{
			MokoiMapObject * obj = (MokoiMapObject *)object->data;
			/*if ( obj->object->data != NULL )
				g_free(obj->object->data);
				*/
			obj->object->type = DT_DELETE;
			return TRUE;
		}
	}
	return FALSE;
}

/********************************
* AL_Object_Update
*
@ map:
@ id:
*/
G_MODULE_EXPORT gboolean AL_Object_Update( MapInfo * map, gint id, gdouble x, gdouble y, gdouble w, gdouble h, gint z )
{
	return FALSE;
}



void RuntimeSetting_MenuItem( gchar * name, RuntimeSettingsStruct * options, GtkWidget * list );
/********************************
* AL_Object_Advance
*
@ map:
@ id:
*/
GtkWidget * Object_GetSettingMenu(MapInfo * map, guint id )
{
    MokoiMap * mokoi_map_data = (MokoiMap *)map->data;
    GList * object = g_list_nth(mokoi_map_data->objects, id);
    GtkWidget * menu_widget = NULL;

    if ( object != NULL )
    {
        MokoiMapObject * current = (MokoiMapObject*)object->data;

        if ( g_hash_table_size(current->settings) )
        {
            menu_widget = gtk_menu_new();
            g_hash_table_foreach( current->settings, (GHFunc)RuntimeSetting_MenuItem, menu_widget );
        }
    }
    return menu_widget;
}

/********************************
* AL_Object_Advance
*VirtualSprite_DialogDisplay
@ map:
@ id:
*/
G_MODULE_EXPORT gboolean AL_Object_Advance( MapInfo * map, gint id, GtkWindow * window )
{
	gboolean result = FALSE;
	MokoiMap * mokoi_map_data = (MokoiMap *)map->data;
	GList * object = g_list_nth(mokoi_map_data->objects, id);

	if ( object != NULL )
	{
		MokoiMapObject * current = (MokoiMapObject*)object->data;

		if ( current->type == 's' || current->type == 'a' )
			result = ObjectAdvance_Sprite( current, window );
		else if ( current->type == 't' )
			result = ObjectAdvance_Text( current, window );
		else if ( current->type == 'r' || current->type == 'c' || current->type == 'p' )
			result = ObjectAdvance_Shape( current, window );
		else if ( current->type == 'l' )
			result = ObjectAdvance_Line( current, window );
		else if ( current->type == 'M' || current->type == 'p' )
			result = ObjectAdvance_File( current, window );
		else
			Meg_Error_Print( __func__, __LINE__, "No Options for that object type. (%c)", current->type);
	}
	else
	{
		Meg_Error_Print( __func__, __LINE__, "AL_Object_AdvanceObject can't find object.");
	}

	return result;
}

/********************************
* AL_Object_Get
*
@ map:
@ id:
*/
G_MODULE_EXPORT DisplayObject * AL_Object_Get( MapInfo * map, gint id)
{
	return NULL;
}

/********************************
* AL_Object_Groups
* Updates ComboBox 'list' with list of object groups.
@ list: [ 0=>name ]
*/
G_MODULE_EXPORT void AL_Object_Groups(GtkListStore * list , gboolean display_virtual)
{
	GtkTreeIter iter;

	g_slist_foreach( mokoiSpritesheets, (GFunc)Sheets_ListEachVisible, list );

	gtk_list_store_append( list, &iter );
	gtk_list_store_set( list, &iter, 0, g_strdup("Shapes"), -1 );

	if ( display_virtual )
	{
		gtk_list_store_append( list, &iter );
		gtk_list_store_set( list, &iter, 0, g_strdup("Virtual"), -1 );
	}

}

/********************************
* AL_Object_Groups
* Get List of children in object group
@ list: [ 0=>Icon 1=>Ident, Auto Advance, Type, Width, Height
*/
void AL_Object_Available( GtkListStore * list, gchar * parent )
{
	if ( parent == NULL )
		return;

	if ( !mokoiObjectMissing )
	{
		mokoiObjectMissing = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, 32, 32 );
		gdk_pixbuf_fill( mokoiObjectMissing, 0xFF0000FF );
	}

	MokoiSheet * sheet = NULL;
	GSList * scan;
	guint s_width = 0, s_height = 0;
	GtkTreeIter iter;

	if ( !g_ascii_strcasecmp( parent, "Shapes") )
	{
		GtkIconTheme * icon_theme;
		icon_theme = gtk_icon_theme_get_default();
		if ( !mokoiObjectImage[OT_RECTANGLE] )
		{
			mokoiObjectImage[OT_RECTANGLE] = gtk_icon_theme_load_icon( icon_theme, "draw-rectangle", 22, GTK_ICON_LOOKUP_GENERIC_FALLBACK, NULL );
			if (!mokoiObjectImage[OT_RECTANGLE])
			{
				mokoiObjectImage[OT_RECTANGLE] = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, 22, 22 );
				gdk_pixbuf_fill( mokoiObjectImage[OT_RECTANGLE], 0xAADD00FF );
			}
		}
		if ( !mokoiObjectImage[OT_CIRCLE] )
		{
			mokoiObjectImage[OT_CIRCLE] = gtk_icon_theme_load_icon( icon_theme, "draw-ellipse", 22, GTK_ICON_LOOKUP_GENERIC_FALLBACK, NULL );
			if (!mokoiObjectImage[OT_CIRCLE])
			{
				mokoiObjectImage[OT_CIRCLE] = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, 22, 22 );
				gdk_pixbuf_fill( mokoiObjectImage[OT_CIRCLE], 0xAADDDDFF );
			}
		}
		if ( !mokoiObjectImage[OT_LINE] )
		{
			mokoiObjectImage[OT_LINE]= gtk_icon_theme_load_icon( icon_theme, "draw-line", 22, GTK_ICON_LOOKUP_GENERIC_FALLBACK, NULL );
			if (!mokoiObjectImage[OT_LINE])
			{
				mokoiObjectImage[OT_LINE] = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, 4, 22 );
				gdk_pixbuf_fill( mokoiObjectImage[OT_LINE], 0xFFDDAAFF );
			}
		}
		if ( !mokoiObjectImage[OT_TEXT] )
		{
			mokoiObjectImage[OT_TEXT]= gtk_icon_theme_load_icon( icon_theme, "draw-text", 22, GTK_ICON_LOOKUP_GENERIC_FALLBACK, NULL );
			if (!mokoiObjectImage[OT_TEXT])
			{
				mokoiObjectImage[OT_TEXT] = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, 22, 22 );
				gdk_pixbuf_fill( mokoiObjectImage[OT_TEXT], 0xDDDDDDFF );
			}
		}
		if ( !mokoiObjectImage[OT_POLYGON] )
		{
			mokoiObjectImage[OT_POLYGON]= gtk_icon_theme_load_icon( icon_theme, "draw-text", 22, GTK_ICON_LOOKUP_GENERIC_FALLBACK, NULL );
			if (!mokoiObjectImage[OT_POLYGON])
			{
				mokoiObjectImage[OT_POLYGON] = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, 22, 22 );
				gdk_pixbuf_fill( mokoiObjectImage[OT_POLYGON], 0xDDDDDDFF );
			}
		}



		/*0=>Icon 1=>Ident, Auto Advance, Type, Width, Height  */
		gtk_list_store_append( list, &iter );
		gtk_list_store_set( list, &iter, 0, mokoiObjectImage[OT_RECTANGLE], 1, "Rectangle", 2, "Rectangle", 3, TRUE, 4, DT_RECTANGLE, 5, 1, 6, 1, -1 );
		gtk_list_store_append( list, &iter );
		gtk_list_store_set( list, &iter, 0, mokoiObjectImage[OT_CIRCLE], 1, "Circle", 2, "Circle", 3, TRUE, 4, OT_CIRCLE, 5, 1, 6, 1, -1 );
		gtk_list_store_append( list, &iter );
		gtk_list_store_set( list, &iter, 0, mokoiObjectImage[OT_LINE], 1, "Line", 2, "Line", 3, TRUE, 4, OT_LINE, 5, 1, 6, 1, -1 );
		gtk_list_store_append( list, &iter );
		gtk_list_store_set( list, &iter, 0, mokoiObjectImage[OT_TEXT], 1, "Text", 2, "Text", 3, TRUE, 4, OT_TEXT, 5, 1, 6, 1, -1 );
		gtk_list_store_append( list, &iter );
		gtk_list_store_set( list, &iter, 0, mokoiObjectImage[OT_POLYGON], 1, "Polygon", 2, "Polygon", 3, TRUE, 4, OT_POLYGON, 5, 1, 6, 1, -1 );

	}
	else if ( !g_ascii_strcasecmp( parent, "Virtual") )
	{
		/* Spritesheet loading */
		char ** files = PHYSFS_enumerateFiles("/sprites/virtual/");
		char ** current;

		for (current = files; *current != NULL; current++)
		{
			if ( g_str_has_suffix( *current, ".xml" ) )
			{
				gchar * file_name = g_strndup(*current, g_utf8_strlen(*current, -1) - 4 ); // Strip .xml
				GdkPixbuf * image = VirtualSprite_BuildPixbuf(file_name);

				gtk_list_store_append( list, &iter );
				gtk_list_store_set( list, &iter, 1, g_strdup(file_name), 2, g_strdup_printf("virtual:%s",file_name), 3, FALSE, 4, DT_IMAGE, -1 );
				gtk_list_store_set( list, &iter, 0, image, 5, gdk_pixbuf_get_width( image ), 6, gdk_pixbuf_get_height( image ), -1 );


				g_free(file_name);
			}
		}
		PHYSFS_freeList(files);

	}
	else
	{
		sheet = Sheet_Get(parent, FALSE);

		if ( sheet )
		{
			Sheet_Load( sheet );
			scan = sheet->children;
			while ( scan )
			{
				MokoiSprite * sprite = (MokoiSprite *)scan->data;

				s_width = sprite->detail->position.width;
				s_height = sprite->detail->position.height;

				if ( sprite->animation )
				{
					gtk_list_store_append( list, &iter );
					gtk_list_store_set( list, &iter, 1, sprite->detail->name, 2, sprite->ident, 3, FALSE, 4, DT_IMAGE, -1 );
					if ( !sprite->image )
					{
						SpriteAnimation_Build(sprite);
					}
					if ( sprite->image )
					{
						s_width = gdk_pixbuf_get_width( sprite->image );
						s_height = gdk_pixbuf_get_height( sprite->image );
						gtk_list_store_set( list, &iter, 0, sprite->image, 5, s_width, 6, s_height, -1 );
					}
					else
					{
						gtk_list_store_set( list, &iter, 0, mokoiObjectMissing, 5, 32, 6, 32, -1 );
					}
				}
				else if ( sprite->visible )
				{
					gtk_list_store_append( list, &iter );
					gtk_list_store_set( list, &iter, 1, sprite->detail->name, 2, sprite->ident, 3, FALSE, 4, DT_IMAGE, -1 );
					if ( sprite->image )
					{
						s_width = gdk_pixbuf_get_width(sprite->image);
						s_height = gdk_pixbuf_get_height(sprite->image);
						gtk_list_store_set( list, &iter, 0, sprite->image, 5, s_width, 6, s_height, -1 );
					}
					else
					{
						gtk_list_store_set( list, &iter, 0, mokoiObjectMissing, 5, 32, 6, 32, -1 );
					}
				}

				scan = g_slist_next( scan );
			}
		}
		else
		{
			Meg_Error_Print( __func__, __LINE__, "Sheet %s not found.", parent);
		}
	}
}



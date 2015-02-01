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
#include "maps.h"
#include "object_advance.h"
#include "sheets_functions.h"
#include "animation_functions.h"
#include "virtual_sprite.h"


/* External Functions */
DisplayObjectTypes MapObject_Internal2DisplayObjectType( char internal_type );
void EntityOption_MenuItem( gchar * name, EntityOptionStruct * options, GtkWidget * list );

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
GdkPixbuf * mokoiObjectImage[5] = { NULL, NULL, NULL, NULL, NULL };
GdkPixbuf * mokoiObjectMissing = NULL;

/* UI */

/* Events */


/********************************
* AL_Object_List
* Get the current map list of display objects
*/
GList * AL_Object_List( MapInfo * map_info )
{
	/* Load resources if needed*/
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
				//object->type = MapObject_Internal2DisplayObjectType(MAP_OBJECT_DATA(object)->type);
				break;
			default:
				break;
		}
		scan = g_list_next(scan);
	}

	return map_info->display_list;

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
DisplayObject * AL_Object_Add( MapInfo * map_info, gchar * ident, gdouble x, gdouble y, gdouble w, gdouble h, gint z)
{
	DisplayObject * object = NULL;

	if ( map_info->file_type == 0 )
	{
		object = MapObject_New( ident, w, h );
	}
	else if ( map_info->file_type == 1 )
	{
		object = VirtualObject_New( ident, w, h );
	}

	object->id = map_info->id_counter++;

	if ( object )
	{
		DisplayObject_UpdateInformation( object, ident, x, y, w, h, z );

		Alchera_DisplayObject_RefreshBorder( object );

		if ( object->type == 's' )
		{
			Alchera_DisplayObject_SetName( object, object->name);
		}
		else
		{
			Alchera_DisplayObject_SetName( object, NULL );
		}
	}

	return object;
}

/********************************
* AL_Object_Remove
*
@ id: id for display object.
*/
gboolean AL_Object_Remove( MapInfo * map_info, gint id )
{
	DisplayObject * object = AL_Object_Get( map_info, id );

	if ( object )
	{
		if ( object->free )
		{
			object->free(object->data);
			object->data = NULL;
		}

		if ( object->shape )
		{
			g_slist_free_full(object->shape, g_free);
		}


		object->type = DT_DELETE;
		return TRUE;
	}

	return FALSE;
}

/********************************
* AL_Object_Update
*
@ map:
@ id:
*/
gboolean AL_Object_Update( MapInfo * map_info, gint id, gdouble x, gdouble y, gdouble w, gdouble h, gint z )
{
	return FALSE;
}

/********************************
* Object_OpenAdvance
*
@ map:
@ id:
*/
void AL_Object_OpenAdvance( GtkWidget * widget, MapInfo * map_info )
{
	GtkWindow * window = Meg_Main_GetWindow();
	DisplayObject * object = map_info->selected;

	if ( object != NULL )
	{
		if ( MAP_OBJECT_DATA(object)->type == 's'  )
			ObjectAdvance_Sprite( object, window );
		else if ( MAP_OBJECT_DATA(object)->type == 't' )
			ObjectAdvance_Text( object, window );
		else if ( MAP_OBJECT_DATA(object)->type == 'r' || MAP_OBJECT_DATA(object)->type == 'c' || MAP_OBJECT_DATA(object)->type == 'p' )
			ObjectAdvance_Shape( object, window );
		else if ( MAP_OBJECT_DATA(object)->type == 'l' )
			ObjectAdvance_Line( object, window );
		else if ( MAP_OBJECT_DATA(object)->type == 'M' || MAP_OBJECT_DATA(object)->type == 'p' )
			ObjectAdvance_File( object, window );
		else
			Meg_Error_Print( __func__, __LINE__, "No Options for that object type. (%c)", MAP_OBJECT_DATA(object)->type);
	}
	else
	{
		Meg_Error_Print( __func__, __LINE__, "AL_Object_OpenAdvance can't find object.");
	}
}


/********************************
* AL_Object_Advance
*
@ map:
@ id:
*/
GtkWidget * Object_GetSettingMenu( MapInfo * map_info, guint id )
{
	GtkWidget * menu_widget = NULL;
	DisplayObject * object = AL_Object_Get(map_info, id);
	MapObjectData * map_object = NULL;

	if ( object != NULL )
    {
		if ( object->type > DT_DELETE )
		{
			menu_widget = gtk_menu_new();
			map_object = MAP_OBJECT_DATA(object);

			/* Add Advance */
			GtkWidget * sub_menu_advance = gtk_menu_item_new_with_label( "Advance" );
			g_signal_connect( G_OBJECT(sub_menu_advance), "activate", G_CALLBACK(AL_Object_OpenAdvance), map_info );
			gtk_menu_shell_append( GTK_MENU_SHELL(menu_widget), sub_menu_advance );
			gtk_widget_show(sub_menu_advance);

			if ( g_hash_table_size( map_object->settings) )
			{
				GtkWidget * sep = gtk_separator_menu_item_new();
				gtk_menu_shell_append( GTK_MENU_SHELL(menu_widget), sep );
				gtk_widget_show(sep);

				g_hash_table_foreach( map_object->settings, (GHFunc)EntityOption_MenuItem, menu_widget );


			}
		}


    }
    return menu_widget;
}


/********************************
* AL_Object_Advance
*
@ map:
@ id:
*/
G_MODULE_EXPORT gboolean AL_Object_Advance( MapInfo * map_info, gint id, GtkWindow * window )
{
	gboolean result = FALSE;
	DisplayObject * object = AL_Object_Get( map_info, id );


	if ( object->type > DT_DELETE )
	{
		if ( MAP_OBJECT_DATA(object)->type == 's'  )
			result = ObjectAdvance_Sprite( object, window );
		else if ( MAP_OBJECT_DATA(object)->type == 't' )
			result = ObjectAdvance_Text( object, window );
		else if ( MAP_OBJECT_DATA(object)->type == 'r' || MAP_OBJECT_DATA(object)->type == 'c' || MAP_OBJECT_DATA(object)->type == 'p' )
			result = ObjectAdvance_Shape( object, window );
		else if ( MAP_OBJECT_DATA(object)->type == 'l' )
			result = ObjectAdvance_Line( object, window );
		else if ( MAP_OBJECT_DATA(object)->type == 'M' || MAP_OBJECT_DATA(object)->type == 'p' )
			result = ObjectAdvance_File( object, window );
		else
			Meg_Error_Print( __func__, __LINE__, "No Options for that object type. (%c)", MAP_OBJECT_DATA(object)->type);
	}


	return result;
}

/********************************
* AL_Object_Get
*
@ map:
@ id:
*/
G_MODULE_EXPORT DisplayObject * AL_Object_Get( MapInfo * map_info, gint id )
{
	DisplayObject * object = NULL;
	GList * scan = g_list_first( map_info->display_list );

	while ( scan )
	{
		object = (DisplayObject *)scan->data;

		if ( object->id == id )
		{
			return object;
		}

		scan = g_list_next(scan);
	}

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

	Spritesheet * sheet = NULL;
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
			mokoiObjectImage[OT_LINE]= gtk_icon_theme_load_icon( icon_theme, "draw-path", 22, GTK_ICON_LOOKUP_GENERIC_FALLBACK, NULL );
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
			mokoiObjectImage[OT_POLYGON]= gtk_icon_theme_load_icon( icon_theme, "draw-polygon", 22, GTK_ICON_LOOKUP_GENERIC_FALLBACK, NULL );
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
	else
	{
		sheet = Sheet_Get(parent, FALSE);

		if ( sheet )
		{
			Sheet_Load( sheet );
			scan = sheet->children;
			while ( scan )
			{
				SheetObject * sprite = (SheetObject *)scan->data;

				s_width = sprite->position.width;
				s_height = sprite->position.height;

				if ( SPRITE_DATA(sprite)->animation )
				{
					gtk_list_store_append( list, &iter );
					gtk_list_store_set( list, &iter, 1, sprite->display_name, 2, sprite->ident_string, 3, FALSE, 4, DT_IMAGE, -1 );
					if ( !SPRITE_DATA(sprite)->image )
					{
						SpriteAnimation_Build(sprite);
					}
					if ( SPRITE_DATA(sprite)->image )
					{
						s_width = gdk_pixbuf_get_width( SPRITE_DATA(sprite)->image );
						s_height = gdk_pixbuf_get_height( SPRITE_DATA(sprite)->image );
						gtk_list_store_set( list, &iter, 0, SPRITE_DATA(sprite)->image, 5, s_width, 6, s_height, -1 );
					}
					else
					{
						gtk_list_store_set( list, &iter, 0, mokoiObjectMissing, 5, 32, 6, 32, -1 );
					}
				}
				else if ( sprite->visible )
				{
					gtk_list_store_append( list, &iter );
					gtk_list_store_set( list, &iter, 1, sprite->display_name, 2, sprite->ident_string, 3, FALSE, 4, DT_IMAGE, -1 );
					if ( SPRITE_DATA(sprite)->image )
					{
						s_width = gdk_pixbuf_get_width(SPRITE_DATA(sprite)->image);
						s_height = gdk_pixbuf_get_height(SPRITE_DATA(sprite)->image);
						gtk_list_store_set( list, &iter, 0, SPRITE_DATA(sprite)->image, 5, s_width, 6, s_height, -1 );
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



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
#include "section_functions.h"
#include <string.h>

/* Global Function */
gboolean AL_Map_Add( const gchar * name, const gchar * old_path );

/* Global Variables */
extern GError * mokoiError;
extern GList * mokoiWorldsList;

/* Local Variables */

/* UI */

const gchar * mokoiUI_WorldAdd = GUI_WORLD_ADD;
extern const gchar * mokoiUI_MapChooser;

/********************************
* Section_GetMapThumb
*
@ map:
*/
GdkPixbuf * Section_GetMapThumb( gchar * map )
{
	gchar * src = NULL;
	GdkPixbuf * image = NULL, * scaled = NULL;

	src = g_strdup_printf( "/maps/thumbs/%s.png", map );
	image = Megdk_pixbuf_new_from_file( src, NULL );

	if ( image )
	{
		scaled = gdk_pixbuf_scale_simple(image, gdk_pixbuf_get_width(image)/4, gdk_pixbuf_get_height(image)/4, GDK_INTERP_BILINEAR );
		g_object_unref(image);
	}

	g_free( src );

	return scaled;
}

/********************************
* Section_GetMapThumbSized
*
@ map:
@ x:
@ y:
*/
GdkPixbuf * Section_GetMapThumbSized( gchar * map, guint x, guint y )
{
	gchar * src = NULL;
	GdkPixbuf * image = NULL, * scaled = NULL;

	src = g_strdup_printf( "/maps/thumbs/%s.png", map );
	image = Megdk_pixbuf_new_from_file( src, NULL );

	if ( image )
	{
		scaled = gdk_pixbuf_scale_simple(image, x, y, GDK_INTERP_BILINEAR );
		g_object_unref(image);
	}

	g_free( src );

	return scaled;
}

/********************************
* Section_MapPreview
*
@ combo: combo box list of names
@ widget: Image widget
*/
void Section_MapPreview( GtkComboBox * combo, GtkWidget * widget )
{
	gchar * map = NULL;
	gchar * src = NULL;
	GdkPixbuf * image = NULL, * scaled = NULL;

	map = Meg_ComboText_GetText( combo );
	src = g_strdup_printf( "/maps/%s.png", map );
	image = Megdk_pixbuf_new_from_file( src, NULL );
	g_free( src );

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

}

/********************************
* Section_Get
*
@ section: name of section
- Returns the MokoiSectionFile or NULL;
*/
MokoiWorldFile * Section_Get( gchar * section )
{
	gchar * filename = NULL;
	MokoiWorldFile * current = NULL;
	GList * scan = mokoiWorldsList;

	filename = g_strdup_printf( "/worlds/%s", section );

	while ( scan )
	{
		current = (MokoiWorldFile *)scan->data;
		if ( !g_ascii_strcasecmp(current->name, filename) )
		{
			return current;
		}
		scan = g_list_next(scan);
	}

	current = Section_Load( section );
	mokoiWorldsList = g_list_append( mokoiWorldsList, current );
	return current;
}

/********************************
* Section_Free
*
*
*/
void Section_Free( MokoiWorldFile * world )
{
	g_free( world->name );
	g_slist_free( world->maps );
}

/********************************
* Section_Load
*
*
*/
MokoiWorldFile * Section_Load( gchar * section )
{
	gchar * content;
	guint8 x, y;
	MokoiWorldFile * new_world = g_new0(MokoiWorldFile, 1);

	memset( &new_world->grid, 65535, 4096 ); /* Reset Grid */
	new_world->width = 64;
	new_world->height = 64;

	new_world->name  = g_strdup_printf( "/worlds/%s", section );
	Meg_file_get_contents( new_world->name, &content, NULL, &mokoiError );

	if ( !Meg_Error_Check( mokoiError, TRUE, (gchar*)__func__ ) )
	{
		gchar ** lines = g_strsplit_set( content, "\n", -1);
		gint lc = 0;
		if ( g_strv_length( lines ) )
		{
			while ( lines[lc] != NULL )
			{
				lines[lc] = g_strstrip( lines[lc] );
				gchar ** array = g_strsplit( lines[lc], "\t", 5 );
				if ( g_strv_length(array) >= 4 )
				{
					if ( array[0][0] == '#' )
					{
						new_world->type = strtol( array[1], NULL, 10) ;

						// TODO
						new_world->width = 64;
						new_world->height = 64;
						//new_world->width = strtol( array[2], NULL, 10);
						//new_world->height = strtol( array[3], NULL, 10);
					}
					else
					{
						MokoiWorldMap * new_map = g_new0(MokoiWorldMap, 1);

						new_map->name = g_strdup( array[0] );
						new_map->position.x = strtol( array[1], NULL, 10);
						new_map->position.y = strtol( array[2], NULL, 10);
						if ( g_strv_length(array) == 5 )
						{
							new_map->position.width = strtol( array[3], NULL, 10);
							new_map->position.height = strtol( array[4], NULL, 10);
						}
						new_map->image = FALSE;

						Map_GetDimensions( new_map->name, &new_map->position.width, &new_map->position.height );

						/* Add map and get it's ID */
						new_world->maps = g_slist_append( new_world->maps, new_map );
						new_map->id = g_slist_index( new_world->maps, new_map );

						for ( x = 0; x < new_map->position.width; x++ )
						{
							for ( y = 0; y < new_map->position.height; y++ )
							{
								if ( (new_map->position.x + x) < 64 && (new_map->position.y + y) < 64 )
								{
									new_world->grid[new_map->position.x + x][new_map->position.y + y] = new_map->id;
								}
							}
						}

					}
				}
				g_strfreev( array );
				lc++;
			}
		}
		g_strfreev( lines );
	}
	g_free(content);

	return new_world;
}

/********************************
* Section_Save
*
*/
gboolean Section_Save( MokoiWorldFile * world )
{
	g_return_val_if_fail( world, FALSE );

	GString * content = g_string_new("");
	GSList * scan = world->maps;

	g_string_append_printf( content, "#\t%d\t%d\t%d\n", world->type, world->width, world->height );
	while ( scan )
	{
		MokoiWorldMap * map = (MokoiWorldMap *)scan->data;
		if ( map->id != 65535 )
		{
			g_string_append_printf( content, "%s\t%d\t%d\t%d\t%d\n", map->name, map->position.x, map->position.y, map->position.width, map->position.height );
		}
		scan = g_slist_next( scan );
	}

	Meg_file_set_contents( world->name, content->str, -1, &mokoiError );
	g_string_free( content, TRUE );

	return TRUE;
}

/**
 * @brief Section_CreateNewMap
 * @param world
 * @param window
 * @return
 */
gchar * Section_CreateNewMap( MokoiWorldFile * world, GtkWindow * window )
{
	gchar * map = NULL;
	GtkWidget * dialog, * frame, * vbox, * entry;

	dialog = gtk_dialog_new_with_buttons("New Map", window, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	frame = gtk_frame_new( "Map Name" );
	entry = gtk_entry_new();
	vbox = gtk_dialog_get_content_area( GTK_DIALOG(dialog) );

	gtk_frame_set_shadow_type( GTK_FRAME(frame), GTK_SHADOW_NONE );

	gtk_box_pack_start( GTK_BOX(vbox), frame, TRUE, TRUE, 0 );
	gtk_container_add( GTK_CONTAINER(frame), entry );

	gtk_widget_show_all(vbox);

	gint result = gtk_dialog_run( GTK_DIALOG(dialog) );
	if ( result == GTK_RESPONSE_ACCEPT )
	{
		const gchar * new_file_name = gtk_entry_get_text( GTK_ENTRY(entry) );
		if ( g_utf8_strlen( new_file_name, -1) )
		{
			if ( AL_Map_Add(new_file_name, NULL) )
			{
				map = g_strdup( new_file_name );
			}
		}
	}

	gtk_widget_destroy( dialog );

	return map;
}

/********************************
* Section_AddMap
*
@ x:
@ y:
*/
gchar * Section_NewMap( MokoiWorldFile * world, guint8 x, guint8 y, GtkWindow * window )
{
	g_return_val_if_fail( world, NULL );

	gchar * map = MapChooser_Dialog( "Map Selection", NULL, "New Map" );

	if ( map )
	{
		if ( g_ascii_strcasecmp( map, "New Map" ) == 0 )
		{
			g_free( map );
			map = Section_CreateNewMap( world, window );
		}

		if ( map )
		{
			MokoiWorldMap * new_map = g_new0( MokoiWorldMap, 1 );
			new_map->name = g_strdup(map);
			new_map->position.x = x;
			new_map->position.y = y;
			new_map->position.width = 1;
			new_map->position.height = 1;
			new_map->image = NULL;
			Map_GetDimensions( new_map->name, &new_map->position.width, &new_map->position.height );


			/* Add map and get it's ID */
			world->maps = g_slist_append( world->maps, new_map );
			new_map->id = g_slist_index( world->maps, new_map );

			for ( x = 0; x < new_map->position.width; x++ )
			{
				for ( y = 0; y < new_map->position.height; y++ )
				{
					if ( (new_map->position.x + x) < 64 && (new_map->position.y + y) < 64 )
						world->grid[new_map->position.x + x][new_map->position.y + y] = new_map->id;
				}
			}
			Section_Save(world);
		}

	}
	return map;
}

/********************************
* Section_RemoveMap
*
@ x:
@ y:
*/
void Section_RemoveMap( MokoiWorldFile * world, guint8 x, guint8 y, GtkWindow * window )
{
	g_return_if_fail( world );

	GtkWidget * dialog = NULL;
	MokoiWorldMap * item;
	gint id = -1;
	guint8 ox, oy;

	x = CLAMP(x, 0, 63);
	y = CLAMP(y, 0, 63);

	id = world->grid[x][y];
	if ( id != 65535 )
	{
		dialog = gtk_message_dialog_new( window, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Are you sure your wish to removed this map from World?" );
		switch ( gtk_dialog_run( GTK_DIALOG(dialog)) )
		{
			case GTK_RESPONSE_YES:
			{
				item = (MokoiWorldMap*)g_slist_nth_data( world->maps, id );
				if ( item )
				{
					x = (guint8)item->position.x;
					y = (guint8)item->position.y;
					for ( ox = 0; ox < item->position.width; ox++ )
					{
						for ( oy = 0; oy < item->position.height; oy++ )
						{
							if ( (item->position.x + x) < 64 && (item->position.y + y) < 64 )
								world->grid[ x + ox ][ y + oy ] = 65535;
						}
					}
					item->id = 65535;
					Meg_Main_PrintStatus("Removed Map '%s' from World", item->name);
					Section_Save(world);
				}

				break;
			}
			default:
				break;
		}
		gtk_widget_destroy( dialog );
	}
}

/**
 * @brief Section_MapName
 * @param world
 * @param x
 * @param y
 * @param window
 * @return name of map
 */
gchar * Section_MapName( MokoiWorldFile * world, guint8 x, guint8 y, GtkWindow * window )
{
	g_return_val_if_fail( world, NULL );

	MokoiWorldMap * item;
	gint id = -1;

	x = CLAMP(x, 0, 63);
	y = CLAMP(y, 0, 63);
	id = world->grid[x][y];

	if ( id != 65535 )
	{
		item = (MokoiWorldMap*)g_slist_nth_data( world->maps, id);
		if ( item )
		{
			return  g_strdup(item->name);
		}
		return NULL;
	}
	else
	{

	}
	return NULL;
}


/********************************
* Section_Available
*
@ name:
@ area:
*/
gboolean Section_Available( MokoiWorldFile * world, gchar * name, GdkRectangle * area )
{
	g_return_val_if_fail( world, FALSE );

	MokoiWorldMap * map;
	GSList * frames = world->maps;
	while( frames )
	{
		map = (MokoiWorldMap*)frames->data;
		if ( g_ascii_strcasecmp( map->name, name ) == 0 )
		{
			guint8 x = (guint8)map->position.x, y = (guint8)map->position.y;
			area->x = map->position.x;
			area->y = map->position.y;
			area->width = x - map->position.x;
			area->height = y - map->position.y;
		}
		frames = g_slist_next( frames );
	}
	return TRUE;
}


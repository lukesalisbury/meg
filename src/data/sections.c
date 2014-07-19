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
#include <glib/gstdio.h>
#include "gtk_compat.h"

/* Required Headers */
#include "maps.h"
#include "section_functions.h"

/* External Functions */

/* Local Type */

/* Global Variables */
extern GError * mokoiError;

/* Local Variables */
GList * mokoiWorldsList = NULL;

/* UI */

/* Events */


/********************************
* AL_MapSection_Open (Exported Function)
*
* Alchera: Map Overview Page
*/
guint AL_MapSection_Open( gchar * file, GtkWindow * window )
{
	MokoiWorldFile * section = Section_Get( file );
	return g_list_index( mokoiWorldsList, section );
}

/********************************
* AL_MapSection_Save (Exported Function)
*
* Alchera: Map Overview Page
*/
gboolean AL_MapSection_Save( guint id, GtkWindow * window )
{
	MokoiWorldFile * world = (MokoiWorldFile*)g_list_nth_data(mokoiWorldsList, id);
	return Section_Save( world );
}

/********************************
* AL_MapSection_Close (Exported Function)
*
* Alchera: Map Overview Page
*/
gboolean AL_MapSection_Close( guint id, GtkWindow * window )
{
	MokoiWorldFile * world = (MokoiWorldFile*)g_list_nth_data(mokoiWorldsList, id);
	Section_Free( world );
	return TRUE;
}

/********************************
* AL_MapSection_MoveMap (Exported Function)
*
* Alchera: Map Overview Page
*/
gchar * AL_MapSection_Pressed( guint id, GtkWindow * window, guint x, guint y, guint button, GdkEventType event )
{
	MokoiWorldFile * world = (MokoiWorldFile*)g_list_nth_data(mokoiWorldsList, id);

	g_return_val_if_fail( world, NULL );

	gchar * map_name = NULL;
	guint8 grid_x = 0, grid_y = 0;
	guint grid_w = AL_Setting_GetNumber("map.width");
	guint grid_h = AL_Setting_GetNumber("map.height");

	grid_w = ( grid_w > 4 ? grid_w/4 : 64 );
	grid_h = ( grid_h > 4 ? grid_h/4 : 48 );

	grid_x = x / grid_w;
	grid_y = y / grid_h;

	if ( button == GDK_BUTTON_PRIMARY )
	{
		map_name = Section_MapName( world, grid_x, grid_y, window );
		if ( !map_name )
		{
			Section_NewMap( world, grid_x, grid_y, window );
		}
	}
	else if ( button == GDK_BUTTON_SECONDARY )
	{
		Section_RemoveMap( world, grid_x, grid_y, window );
	}
	return map_name;
}

/********************************
* AL_MapSection_SizeRequest (Exported Function)
*
* Alchera: Map Overview Page
*/
void AL_MapSection_SizeRequest( guint id, gint * width, gint * height )
{
	MokoiWorldFile * world = (MokoiWorldFile*)g_list_nth_data(mokoiWorldsList, id);

	g_return_if_fail( world );

	if ( width )
	{
		*width = world->width*64;
	}

	if ( height )
	{
		*height = world->height*48;
	}
}

/********************************
* AL_MapSection_Redraw (Exported Function)
*
* Alchera: Map Overview Page
*/
gboolean AL_MapSection_Redraw( guint id, cairo_t * cr, gint width, gint height )
{
	MokoiWorldFile * world = (MokoiWorldFile*)g_list_nth_data(mokoiWorldsList, id);

	g_return_val_if_fail( world, FALSE );

	GSList * scan = NULL;
	MokoiWorldMap * current = NULL;
	gchar * label_text = NULL;

	GdkRectangle screen_area;

	guint grid_x = 0, grid_y = 0;
	guint grid_w = AL_Setting_GetNumber("map.width");
	guint grid_h = AL_Setting_GetNumber("map.height");

	cairo_text_extents_t label_extents;


	grid_w = ( grid_w > 4 ? grid_w/4 : 64 );
	grid_h = ( grid_h > 4 ? grid_h/4 : 48 );


	screen_area.x = screen_area.y = 0;
	screen_area.width /= width / grid_w;
	screen_area.height /= height / grid_w;




	scan = world->maps;
	while ( scan )
	{
		current = (MokoiWorldMap *)scan->data;
		if ( current->id != 65535 )
		{
			if ( !current->image )
			{
				current->image = Section_GetMapThumb( current->name );
			}
			if ( current->image )
			{
				gdk_cairo_set_source_pixbuf(cr, current->image, current->position.x * grid_w, current->position.y * grid_h);
				cairo_paint(cr);
			}
			cairo_set_source_rgba( cr, 0, 0, 0 , 0.5);
			cairo_rectangle(cr, current->position.x * grid_w + 5, current->position.y * grid_h + 5, current->position.width * grid_w - 10, current->position.height * grid_h - 10);
			cairo_set_line_width(cr, 6);
			cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
			if ( !current->image )
			{
				cairo_stroke_preserve(cr);
				cairo_set_source_rgba(cr, 1, 1, 1, 0.5);
				cairo_fill(cr);
			}
			else
			{
				cairo_stroke(cr);
			}
			label_text = g_strdup_printf("%d: %s", (current->position.y*64)+current->position.x,current->name);
			cairo_set_source_rgba(cr, 1, 1, 1, 0.75);
			cairo_set_font_size(cr, 13);
			cairo_text_extents(cr, label_text, &label_extents);
			cairo_rectangle(cr, current->position.x * grid_w + 10, current->position.y * grid_h + 10, label_extents.width + 2, label_extents.height + 2);
			cairo_fill(cr);
			cairo_set_source_rgb( cr, 0, 0, 0 );
			cairo_move_to(cr, (current->position.x * grid_w) + 10, (current->position.y * grid_h) + 23);
			cairo_show_text(cr, label_text);
			g_free(label_text);
		}

/*
		if ( world->type )
		{
			rect.y = 20;
			label_text = g_strdup_printf("Floor %d of 16", (current->position.y*64)+current->position.x );
			cairo_set_source_rgba(cr, 1, 1, 1, 0.75);
			cairo_set_font_size(cr, 13);
			cairo_text_extents(cr, "text", &extents);
			cairo_rectangle(cr, 0, 0, extents.width + 2, extents.height + 2);
			cairo_fill(cr);
			cairo_set_source_rgb( cr, 0, 0, 0 );
			cairo_show_text(cr, text);
			g_free( label_text );
		}
*/



		scan = g_slist_next( scan );
	}

	/* Draw Grid */
	cairo_set_source_rgb( cr, 0, 0, 0 );
	while ( grid_x < width )
	{
		cairo_move_to(cr, grid_x, 0);
		cairo_line_to(cr, grid_x, height);
		grid_x += grid_w;
	}
	while ( grid_y < height )
	{
		cairo_move_to(cr, 0, grid_y);
		cairo_line_to(cr, width, grid_y);
		grid_y += grid_h;
	}
	cairo_set_line_width(cr, 0.5);
	cairo_stroke(cr);


	return FALSE;
}


/********************************
* AL_Worlds_Files (Exported Function)
*
@ store:  [ 0->name, 1->subtext, 2->image, 3->window pointer ]
*/
void AL_Worlds_Files( GtkListStore * store )
{
	GtkTreeIter iter;

	/* Section Icon */
	GdkPixbuf * i = gdk_pixbuf_new( GDK_COLORSPACE_RGB, FALSE, 8, 16, 16);
	gdk_pixbuf_fill( i, 0xffffffff);

	/* Clear previous list */
	gtk_list_store_clear( store );


	/* Scan Directory */
	char ** directory_listing = PHYSFS_enumerateFiles("/worlds/");
	char ** listed_file;
	gchar * current_file = NULL;

	for (listed_file = directory_listing; *listed_file != NULL; listed_file++)
	{
		current_file = *listed_file;
		if ( g_str_has_suffix( current_file, ".tsv" ) )
		{
			gtk_list_store_append(store, &iter);
			gtk_list_store_set(store, &iter, 0, g_strdup(current_file), 1, "", 2, i, 3, NULL, -1);
			Section_Get( current_file );
		}
	}
	PHYSFS_freeList(directory_listing);

}

/********************************
* AL_Worlds_Add (Exported Function)
*
* Alchera: Map Overview Page
*/
gboolean AL_Worlds_Add( const gchar * name )
{
	gboolean successful = FALSE;
	gchar * clean_section, * dest;

	clean_section = g_strdup(name);

	g_strdelimit( clean_section, "_-|> <.\\/\"'!@#$%^&*(){}[]", '_' );

	dest = g_strdup_printf( "/worlds/%s.tsv", clean_section );

	if ( !Meg_file_test(dest, G_FILE_TEST_IS_REGULAR) )
	{
		Meg_file_set_contents( dest, "#\t0\t64\t64\n", -1, NULL);
		successful = TRUE;
	}

	g_free(dest);
	g_free(clean_section);

	return successful;
}

/********************************
* AL_Worlds_Remove (Exported Function)
*
* Alchera: Map Overview Page
*/
gboolean AL_Worlds_Remove( gchar * name )
{
	gchar * src = g_strdup_printf( "/worlds/%s", name );
	if ( Meg_file_test(src, G_FILE_TEST_IS_REGULAR) )
	{
		PHYSFS_delete( src );
	}
	g_free(src);

	return TRUE;
}


/********************************
* AL_MapSection_Type (Exported Function)
*
* Alchera: Map Overview Page
*/
guint8 AL_MapSection_Type( guint id, guint8 *type, GtkWindow * window )
{
	MokoiWorldFile * world = (MokoiWorldFile*)g_list_nth_data(mokoiWorldsList, id);

	if ( type != NULL )
	{
		world->type = *type;
	}

	return world->type;
}





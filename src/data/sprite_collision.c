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
#include <glib.h>
#include <glib/gstdio.h>

/* External Functions */
GSList * Path_FileLoad( gchar * file );
gboolean Path_FileSave( GSList * data, gchar * file );

/* Global Variables */
extern GError * mokoiError;
extern GSList * mokoiSpritesheets;

/* Local Variables */
GdkRectangle Mokoi_SpriteCollision;


/********************************
* SpriteCollision_Read
*
*/
gboolean SpriteCollision_Read( SheetObject * sprite, GtkListStore * list )
{
	GtkTreeIter iter;
	guint8 rect = 0;
	for ( rect = 0; rect < 7; rect++ )
	{
		if ( SPRITE_DATA(sprite)->collisions[rect].width )
		{
			gtk_list_store_append( list, &iter );
			gtk_list_store_set( list, &iter, 0, rect, 1, SPRITE_DATA(sprite)->collisions[rect].x, 2, SPRITE_DATA(sprite)->collisions[rect].y, 3, SPRITE_DATA(sprite)->collisions[rect].width, 4, SPRITE_DATA(sprite)->collisions[rect].height, 5, NULL, -1);
		}
	}
	return TRUE;
}


/********************************
* SpriteCollision_Load
* OLD Method
*/
gboolean SpriteCollision_Load( SheetObject * sprite, gchar * file )
{
	GdkRectangle collision;
	gint c = 0;
	gchar * sheet_file = g_strdup_printf("/sprites/collisions/%s", file);

	PHYSFS_File * handle = PHYSFS_openRead( sheet_file );
	if ( handle )
	{
		do
		{
			gint16 temp_buffer = 0;
			PHYSFS_readSBE16( handle, &temp_buffer ); collision.x = temp_buffer;
			PHYSFS_readSBE16( handle, &temp_buffer ); collision.y = temp_buffer;
			PHYSFS_readSBE16( handle, &temp_buffer ); collision.width = temp_buffer;
			PHYSFS_readSBE16( handle, &temp_buffer ); collision.height = temp_buffer;
			if (  c < 7 )
			{
				if ( !SPRITE_DATA(sprite)->collisions[c].width )
				{
					SPRITE_DATA(sprite)->collisions[c] = collision;
					g_print( "%s %d x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\"\n", file, c, SPRITE_DATA(sprite)->collisions[c].x, SPRITE_DATA(sprite)->collisions[c].y, SPRITE_DATA(sprite)->collisions[c].width, SPRITE_DATA(sprite)->collisions[c].height );
				}
				c++;
			}
		} while ( !PHYSFS_eof( handle )  );
		PHYSFS_close(handle);
		return TRUE;
	}
	return FALSE;
}


/********************************
* SpriteCollision_Write
*
*/
gboolean SpriteCollision_Write( SheetObject * sprite, GtkTreeModel * data )
{
	guint8 c = 0;
	GdkRectangle rect;
	GtkTreeIter iter;
	if ( gtk_tree_model_get_iter_first( data, &iter ) )
	{
		do
		{
			gtk_tree_model_get( data, &iter, 0, &c, 1, &rect.x, 2, &rect.y, 3, &rect.width, 4, &rect.height, -1);
			if ( c < 7 )
				SPRITE_DATA(sprite)->collisions[c] = rect;

		} while ( gtk_tree_model_iter_next( data, &iter ) );
		return TRUE;
	}
	return FALSE;
}


/********************************
* SpriteCollision_Add
*
*/
void SpriteCollision_Add( GtkButton * widget, GtkComboBox * combobox )
{
	GtkTreeModel * model = gtk_combo_box_get_model( combobox );
	gint c = gtk_tree_model_iter_n_children( model, NULL );

	if ( c < 7 )
	{
		GtkTreeIter iter;
		gtk_list_store_append( GTK_LIST_STORE(model), &iter);
		gtk_list_store_set( GTK_LIST_STORE(model), &iter, 0, c, 1, 0, 2, 0, 3, 0, 4, 0, -1);
		gtk_combo_box_set_active( combobox, c );
	}
}


/********************************
* SpriteCollision_Draw
*
*/

gboolean SpriteCollision_Draw( GtkWidget * widget, cairo_t * cr, GtkComboBox * combobox )
{
	if ( gtk_combo_box_get_active( combobox ) == -1 )
	{
		return FALSE;
	}

	GdkRectangle rect;
	GtkTreeIter iter;
	SheetObject * sprite = g_object_get_data( G_OBJECT(widget), "sprite" );
	guint width = 5, height = 5, x = 0, y = 0;


	if ( !gtk_combo_box_get_active_iter( combobox, &iter ) )
	{
		return FALSE;
	}

	gtk_tree_model_get( gtk_combo_box_get_model( combobox ), &iter, 1, &rect.x, 2, &rect.y, 3, &rect.width, 4, &rect.height, -1);

	cairo_scale(cr, 4.0, 4.0);

	if ( SPRITE_DATA(sprite)->image )
	{
		width = gdk_pixbuf_get_width( SPRITE_DATA(sprite)->image );
		height = gdk_pixbuf_get_height( SPRITE_DATA(sprite)->image );
		gtk_widget_set_size_request( widget, width*4, height*4 );

		cairo_save( cr );
		cairo_rectangle( cr, 0, 0, width, height );
		gdk_cairo_set_source_pixbuf( cr, SPRITE_DATA(sprite)->image, 0, 0 );
		cairo_fill( cr );
		cairo_restore( cr );

		cairo_save( cr );
		cairo_set_source_rgba( cr, 0, 0, 0, 0.6 );
		cairo_set_line_width( cr, 0.4 );
		while ( x < width )
		{
			cairo_move_to( cr, x, 0 );
			cairo_line_to( cr, x, height );
			cairo_stroke( cr );
			x += 2;
		}
		while ( y < height )
		{
			cairo_move_to( cr, 0, y );
			cairo_line_to( cr, width, y );
			cairo_stroke( cr );
			y += 2;
		}
		cairo_restore( cr );

		cairo_save( cr );
		cairo_set_source_rgba( cr, 1.0, 0, 0 , 0.5);
		cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
		cairo_set_line_width(cr, 1.0);
		cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
		cairo_stroke( cr );
		cairo_restore( cr );

		cairo_save( cr );
		cairo_set_source_rgba( cr, 0.6, 0.6, 0, 0.5);
		cairo_rectangle(cr, Mokoi_SpriteCollision.x, Mokoi_SpriteCollision.y, Mokoi_SpriteCollision.width, Mokoi_SpriteCollision.height);
		cairo_set_line_width(cr, 1.0);
		cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
		cairo_fill( cr );
		cairo_restore( cr );

	}
	return TRUE;


}

/********************************
* SpriteCollision_Expose
*
*/
gboolean SpriteCollision_Expose(  GtkWidget * widget, GdkEventExpose * event, GtkComboBox * combobox )
{
	cairo_t * cr;

	cr = gdk_cairo_create( gtk_widget_get_window(widget) );

	if ( !cr )
	{
		Meg_Main_PrintStatus("gdk_cairo_create failed ");
		return FALSE;
	}


	return SpriteCollision_Draw( widget, cr, combobox );

}

/********************************
* SpriteCollision_Update
*
*/
void SpriteCollision_Update( GtkComboBox * combobox, GtkWidget * widget )
{
	gtk_widget_queue_draw(widget);
}

/********************************
* SpriteCollision_ButtonMoved
*
*/
gboolean SpriteCollision_ButtonMoved( GtkWidget * widget , GdkEventMotion * event,  GtkComboBox * combobox )
{
	gint active = gtk_combo_box_get_active( combobox );
	if ( active == -1 )
	{
		return FALSE;
	}

	gdouble x = (event->x / 4.0);
	gdouble y = (event->y / 4.0);

	Mokoi_SpriteCollision.width = (gint)x - Mokoi_SpriteCollision.x;
	Mokoi_SpriteCollision.height = (gint)y - Mokoi_SpriteCollision.y;

	gtk_widget_queue_draw( widget );
	return TRUE;
}

/********************************
* SpriteCollision_ButtonPressed
*
*/
gboolean SpriteCollision_ButtonPressed( GtkWidget * widget , GdkEventButton * event,  GtkComboBox * combobox )
{
	gint active = gtk_combo_box_get_active( combobox );
	if ( active == -1 )
	{
		return FALSE;
	}

	gdouble x = (event->x / 4.0);
	gdouble y = (event->y / 4.0);
	if ( event->button == 1 )
	{
		if ( event->type == GDK_BUTTON_RELEASE )
		{
			Mokoi_SpriteCollision.width = (gint)x - Mokoi_SpriteCollision.x;
			Mokoi_SpriteCollision.height = (gint)y - Mokoi_SpriteCollision.y;
			gtk_widget_queue_draw( widget );

			GtkWidget * dialog = gtk_dialog_new_with_buttons( "Replace Rectangle",
				Meg_Misc_ParentWindow(widget),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_STOCK_YES, GTK_RESPONSE_ACCEPT,
				GTK_STOCK_NO, GTK_RESPONSE_REJECT,
				NULL );
			GtkWidget * entry = gtk_label_new( "" );

			gchar * markup = g_markup_printf_escaped( "<b>New Rectangle</b>\nX: %d\nY: %d\nWidth: %d\nHeight: %d", Mokoi_SpriteCollision.x, Mokoi_SpriteCollision.y, Mokoi_SpriteCollision.width, Mokoi_SpriteCollision.height );

			gtk_label_set_markup( GTK_LABEL(entry), markup );

			gtk_container_add( GTK_CONTAINER(gtk_dialog_get_content_area( GTK_DIALOG(dialog) )), entry );
			gtk_widget_show( entry );
			if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
			{
				GtkTreeIter iter;
				if ( gtk_combo_box_get_active_iter( combobox, &iter ) )
				{
					gtk_list_store_set( GTK_LIST_STORE(gtk_combo_box_get_model( combobox )), &iter, 1, Mokoi_SpriteCollision.x, 2, Mokoi_SpriteCollision.y, 3, Mokoi_SpriteCollision.width, 4, Mokoi_SpriteCollision.height, -1);
				}
			}
			gtk_widget_destroy( dialog );

			Mokoi_SpriteCollision.x = Mokoi_SpriteCollision.y = Mokoi_SpriteCollision.width = Mokoi_SpriteCollision.height = 0;
			gtk_widget_queue_draw( widget );
		}
		else if ( event->type == GDK_BUTTON_PRESS )
		{
			Mokoi_SpriteCollision.x = (gint)x;
			Mokoi_SpriteCollision.y = (gint)y;
			Mokoi_SpriteCollision.width = Mokoi_SpriteCollision.height = 0;
			gtk_widget_queue_draw( widget );
		}
	}
	return FALSE;
}


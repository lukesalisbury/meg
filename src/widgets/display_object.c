/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#define REPLACE_STRING_DUPE(s,v)	g_free( s ); if (v) { s = g_strdup(v); } else { s = NULL; }

/* Global Header */
#include "display_object.h"

/* Required Headers */
#if GTK_MAJOR_VERSION == 2
#include "gdkkeysyms-backward.h"
#endif


/* GtkWidget */


/* Global Variables */
guint16	mapGridValue = 16;

/* External Functions */
DisplayObject * AL_Object_Add( MapInfo * map_info, gchar * indent, gdouble x, gdouble y, gdouble w, gdouble h, gint z );
DisplayObject * AL_Object_Get( MapInfo * map_info, gint id);
gboolean AL_Object_Remove( MapInfo * map_info, gint id);
gboolean AL_Object_Update( MapInfo * map_info, gint id, gdouble x, gdouble y, gdouble w, gdouble h, gint z );
gboolean AL_Object_Advance( MapInfo * map_info, gint id, GtkWindow * window );
cairo_font_face_t * Meg_GetBitmapFont();

/* UI */


/* Functions */
gboolean Alchera_DisplayObject_Advance( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window )
{
	if ( obj && map_info )
	{
		return AL_Object_Advance( map_info, obj->id , parent_window );
	}
	return FALSE;
}

gboolean Alchera_DisplayObject_LayerUp( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window )
{
	if ( obj )
	{
		obj->layer++;
		return TRUE;
	}
	return FALSE;
}

gboolean Alchera_DisplayObject_LayerDown( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window )
{
	if ( obj )
	{
		obj->layer--;
		return TRUE;
	}
	return FALSE;
}

gboolean Alchera_DisplayObject_Rotate( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window )
{
	if ( obj )
	{
		obj->rotate++;
		if ( obj->rotate == 4 )
		{
			obj->rotate = 0;
		}
		return TRUE;
	}
	return FALSE;
}


gboolean Alchera_DisplayObject_Remove( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window )
{
	if ( !obj && !map_info )
		return FALSE;

	/* Confirm deleting Dialog */
	GtkWidget * delete_dialog = gtk_message_dialog_new( parent_window,
		GTK_DIALOG_MODAL,
		GTK_MESSAGE_QUESTION,
		GTK_BUTTONS_YES_NO,
		"Are you sure your wish to removed this object?"
	);

	gint result = gtk_dialog_run( GTK_DIALOG(delete_dialog) );
	gtk_widget_destroy( delete_dialog );

	if (result == GTK_RESPONSE_YES)
	{
		return AL_Object_Remove( map_info, obj->id );

	}

	return FALSE;
}


gboolean Alchera_DisplayObject_PathRemove( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window )
{
	if ( obj )
	{
		if ( obj->path )
		{
			GtkWidget * delete_dialog = gtk_message_dialog_new( parent_window,
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO,
				"Do wish to remove the path on this object?"
			);
			gint result = gtk_dialog_run( GTK_DIALOG(delete_dialog) );
			gtk_widget_destroy( delete_dialog );

			if ( result == GTK_RESPONSE_YES )
			{
				g_slist_free( obj->path );
				obj->path = NULL;
				return TRUE;
			}
		}
	}

	return FALSE;
}

gboolean Alchera_DisplayObject_Path( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window )
{
	if ( obj )
	{
		if ( !obj->path )
		{
			GtkWidget * delete_dialog = gtk_message_dialog_new( parent_window,
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO,
				"Do wish to add a path to this object?"
			);
			gint result = gtk_dialog_run( GTK_DIALOG(delete_dialog) );
			gtk_widget_destroy( delete_dialog );

			if ( result == GTK_RESPONSE_YES )
			{
				DisplayObject * objn = g_new0(DisplayObject, 1);
				objn->x = obj->x;
				objn->y = obj->y;
				objn->w = objn->h = 16;
				objn->type = DT_PATHITEM;
				obj->path = g_slist_append(obj->path, obj);
				return TRUE;
			}

		}
		else
		{
			return TRUE;
		}
	}

	return FALSE;
}

gboolean Alchera_DisplayObject_Flip( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window )
{
	if ( obj && map_info )
	{
		obj->is_flipped = !obj->is_flipped;
		return TRUE;
	}
	return FALSE;
}


gboolean Alchera_DisplayObject_KeyInput( MapInfo * map_info, DisplayObject * obj, guint key, gboolean resize_modifer, GtkWindow * parent_window )
{
	g_return_val_if_fail( obj, FALSE );
	g_return_val_if_fail( map_info, FALSE );

	if ( key == GDK_KEY_Up )
	{
		//gtk_alchera_map_undo_push( map, obj );
		if ( resize_modifer )
		{
			gdouble height = (gdouble)obj->th;
			obj->h -= height;
			if (obj->h <= height)
			{
				obj->tiled_image = FALSE;
				obj->h = height;
			}
			else
				obj->tiled_image = TRUE;
		}
		else
		{
			obj->y -= Alchera_DisplayObject_StandardMovement( 0, 0 );
		}
	}
	else if ( key == GDK_KEY_Down )
	{
		//gtk_alchera_map_undo_push( map, obj );
		if ( resize_modifer )
		{
			gdouble height = (gdouble)obj->th;
			obj->h += height;
			if (obj->h <= height)
			{
				obj->tiled_image = FALSE;
				obj->h = height;
			}
			else
				obj->tiled_image = TRUE;
		}
		else
		{
			obj->y += Alchera_DisplayObject_StandardMovement( 0, 0 );
		}
	}
	else if ( key == GDK_KEY_Left )
	{
		//gtk_alchera_map_undo_push( map, obj );
		if ( resize_modifer )
		{
			gdouble width = (gdouble)obj->tw;
			obj->w -= width;
			if (obj->w <= width)
			{
				obj->tiled_image = FALSE;
				obj->w = width;
			}
			else
				obj->tiled_image = TRUE;
		}
		else
		{
			obj->x -= Alchera_DisplayObject_StandardMovement( 0, 0 );
		}
	}
	else if ( key == GDK_KEY_Right )
	{
		//gtk_alchera_map_undo_push( map, obj );
		if ( resize_modifer )
		{
			gdouble width = (gdouble)obj->tw;
			obj->w += width;
			if (obj->w <= width)
			{
				obj->tiled_image = FALSE;
				obj->w = width;
			}
			else
				obj->tiled_image = TRUE;
		}
		else
		{
			obj->x += Alchera_DisplayObject_StandardMovement( 0, 0 );
		}
	}
	return TRUE;
}


gboolean Alchera_DisplayObject_Attach( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window )
{
	if ( obj && map_info )
	{
		return AL_Object_Advance( map_info, obj->id , parent_window );
	}
	return FALSE;
}

/********************************
* Alchera_DisplayObject_Add
*
*/
DisplayObject * Alchera_DisplayObject_Add(MapInfo * info, gchar * ident, gdouble x, gdouble y, gdouble w, gdouble h, guint z)
{
	DisplayObject * obj = NULL;

	obj = AL_Object_Add( info, ident, x, y, w, h, z );

	return obj;
}



/********************************
* Event:
* Result:
*/
guint Alchera_DisplayObject_StandardMovement( guint state, gboolean mapAlign )
{
	if ( mapAlign )
		return mapGridValue;
	if ( state & GDK_CONTROL_MASK )
		return 8;
	else
		return 1;
}


/********************************
* Alchera_DisplayObject_Order
* Handle the display order for Maps
* See http://library.gnome.org/devel/glib/stable/glib-Doubly-Linked-Lists.html#GCompareDataFunc
*/
gint Alchera_DisplayObject_Order( gconstpointer a, gconstpointer b, gpointer data )
{
	DisplayObject * q, * t;
	gboolean sort_y = FALSE;
	q = (DisplayObject*)a;
	t = (DisplayObject*)b;

	if ( data )
	{
		sort_y = TRUE;
	}

	if( q->layer > t->layer )
		return 1;
	else if( q->layer < t->layer )
		return -1;
	else // layers are equal
	{
		if ( sort_y )
		{
			if( q->y > t->y )
				return 1;
			else if( q->y < t->y )
				return -1;

			if( q->x > t->x )
				return 1;
			else if( q->x < t->x )
				return -1;

		}

		//Just Order by the order the object was placed on the map
		if( q->id > t->id )
			return 1;
		else if( q->id < t->id )
			return -1;

	}
	return 0;
}


/********************************
* Alchera_DisplayObject_DrawBorderPixbuf
*
*/
void Alchera_DisplayObject_DrawBorderPixbuf(DisplayObject * object, cairo_t *cr)
{
	if ( !object->border )
		return;

	DisplayObjectBorder * border = NULL;
	GList * border_list = object->border;
	while ( border_list )
	{
		cairo_pattern_t * pattern;
		border = (DisplayObjectBorder *)border_list->data;
		cairo_rectangle( cr, object->x + border->x, object->y + border->y, border->w, border->h );
		gdk_cairo_set_source_rgba( cr, &object->colour );
		gdk_cairo_set_source_pixbuf( cr, border->image, object->x + border->x, object->y + border->y );
		pattern = cairo_get_source( cr );
		cairo_pattern_set_extend( pattern, CAIRO_EXTEND_REPEAT);
		cairo_fill( cr );

		border_list = g_list_next(border_list);
	}

}


/********************************
* Alchera_DisplayObject_DrawPixbuf
*
*/
void Alchera_DisplayObject_DrawPixbuf(DisplayObject * object, cairo_t *cr)
{
	if ( !object->image )
		return;
	cairo_pattern_t * pattern;
	cairo_matrix_t matrix;

	cairo_rectangle( cr, object->x, object->y, (object->rotate % 2 ? object->h : object->w), (object->rotate % 2 ? object->w : object->h) );
	gdk_cairo_set_source_rgba( cr, &object->colour );

	if ( object->rotate )
	{
		cairo_translate( cr, object->x, object->y);
		cairo_rotate( cr, object->rotate * G_PI / 2 );
		cairo_translate( cr, -object->x, -object->y );
	}

	//cairo_set_operator( cr, CAIRO_OPERATOR_OVER );
	gdk_cairo_set_source_pixbuf( cr, object->image, object->x, object->y );
	pattern = cairo_get_source( cr );
	cairo_pattern_set_extend( pattern, CAIRO_EXTEND_REPEAT);
	if ( object->is_flipped )
	{
		cairo_matrix_init_scale( &matrix, -1, 1 );
		cairo_matrix_translate( &matrix, -object->x, -object->y );
		cairo_pattern_set_matrix( pattern, &matrix );
	}
	cairo_fill( cr );


	Alchera_DisplayObject_DrawBorderPixbuf(object, cr);
}



/********************************
* Alchera_DisplayObject_DrawAnimation
*
*/
void Alchera_DisplayObject_DrawAnimation( DisplayObject * object, cairo_t * cr )
{
	// Disabled for now
	/*
	if ( !object->data )
		return;

	cairo_pattern_t * pattern;
	cairo_matrix_t matrix;
	GdkPixbuf * image = gdk_pixbuf_animation_iter_get_pixbuf( (GdkPixbufAnimationIter *)object->data );

	cairo_rectangle( cr, object->x, object->y, (object->rotate % 2 ? object->h : object->w), (object->rotate % 2 ? object->w : object->h) );
	gdk_cairo_set_source_rgba( cr, &object->colour );
	if ( object->rotate )
	{
		cairo_translate( cr, object->x, object->y);
		cairo_rotate( cr, object->rotate * G_PI / 2 );
		cairo_translate( cr, -object->x, -object->y );
	}

	gdk_cairo_set_source_pixbuf( cr, image, object->x, object->y );
	pattern = cairo_get_source( cr );
	cairo_pattern_set_extend( pattern, CAIRO_EXTEND_REPEAT);
	if ( object->flip )
	{
		cairo_matrix_init_scale( &matrix, -1, 1 );
		cairo_matrix_translate( &matrix, -object->x, -object->y );
		cairo_pattern_set_matrix( pattern, &matrix );
	}
	cairo_fill( cr );
	*/
}

/********************************
* Alchera_DisplayObject_DrawRectangle
*
*/
void Alchera_DisplayObject_DrawRectangle(DisplayObject* object, cairo_t *cr)
{
	cairo_rectangle( cr, object->x, object->y, object->w, object->h );
	gdk_cairo_set_source_rgba( cr, &object->colour );
	cairo_fill( cr );
}


/********************************
* Alchera_DisplayObject_DrawPolygon
*
*/
void Alchera_DisplayObject_DrawPolygon( DisplayObject * object, cairo_t * cr )
{
	gdk_cairo_set_source_rgba( cr, &object->colour );

	gdouble w = 0;
	gdouble h = 0;
	GSList * list  = object->shape;

	cairo_new_path( cr );
	while ( list )
	{
		DisplayObject * point = (DisplayObject*) list->data;
		cairo_line_to( cr, object->x + point->x, object->y + point->y );

		w = MAX( w, point->x );
		h = MAX( h, point->y );

		list = g_slist_next(list);
	}

	if ( w > 0 )
		object->w = w;
	if ( h > 0 )
		object->h = h;

	cairo_close_path( cr );
	cairo_fill( cr );
}

/********************************
* Alchera_DisplayObject_DrawCircle
*
*/
void Alchera_DisplayObject_DrawCircle( DisplayObject * object, cairo_t * cr )
{
	double width = object->w / 2.0;
	double height = object->h / 2.0;
	cairo_translate( cr, object->x + width, object->y + height );
	cairo_scale( cr, width, height );
	cairo_arc( cr, 0.0, 0.0, 1.0, 0.0, 2 * G_PI );
	gdk_cairo_set_source_rgba( cr, &object->colour );
	cairo_fill( cr );

}

/********************************
* Alchera_DisplayObject_DrawLine
*
*/
void Alchera_DisplayObject_DrawLine(DisplayObject* object, cairo_t *cr)
{
	cairo_move_to( cr, object->x, object->y );
	cairo_line_to( cr, object->x + object->w, object->y + object->h );
	gdk_cairo_set_source_rgba( cr, &object->colour );
	cairo_stroke( cr );
}

/********************************
* Alchera_DisplayObject_DrawBitText
*
*/
void Alchera_DisplayObject_DrawBitText(DisplayObject* object, cairo_t *cr)
{
	if ( !object->text )
		return;


	cairo_text_extents_t extents;
	cairo_set_font_face( cr, Meg_GetBitmapFont() );// TODO MODIFY

	cairo_set_font_size( cr, 8.0 );
	cairo_text_extents( cr, object->text, &extents );
	cairo_move_to( cr, object->x , object->y + extents.height );
	gdk_cairo_set_source_rgba( cr, &object->colour );
	object->w = extents.width;
	object->h = extents.height;
	cairo_show_text( cr, object->text );
}

/********************************
* Alchera_DisplayObject_DrawText
*
*/
void Alchera_DisplayObject_DrawText(DisplayObject* object, cairo_t *cr)
{
	if ( !object->text )
		return;
	cairo_text_extents_t extents;
	cairo_select_font_face( cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL );
	cairo_set_font_size( cr, 10.0 );
	cairo_text_extents( cr, object->text, &extents );
	cairo_move_to( cr, object->x , object->y + extents.height );
	gdk_cairo_set_source_rgba( cr, &object->colour );
	object->w = extents.width;
	object->h = extents.height;
	cairo_show_text( cr, object->text );

}

/********************************
* Alchera_DisplayObject_DrawPath
*
*/
void Alchera_DisplayObject_DrawPath(DisplayObject* object, cairo_t *cr)
{
	cairo_move_to( cr, object->x, object->y );
	cairo_line_to( cr, object->x + object->w, object->y + object->h );
	gdk_cairo_set_source_rgba( cr, &object->colour );
	cairo_stroke( cr );
}

/********************************
* Alchera_DisplayObject_DrawForeach
*
*/
void Alchera_DisplayObject_DrawForeach(DisplayObject * object, cairo_t *cr )
{
	if ( object->type == DT_DELETE )
	{
		return;
	}
	cairo_save( cr );
	switch ( object->type )
	{
		case DT_IMAGE:
			Alchera_DisplayObject_DrawPixbuf( object, cr );
			break;
		case DT_TEXT:
			Alchera_DisplayObject_DrawBitText( object, cr );
			break;
		case DT_RECTANGLE:
			Alchera_DisplayObject_DrawRectangle( object, cr );
			break;
		case DT_CIRCLE:
			Alchera_DisplayObject_DrawCircle( object, cr );
			break;
		case DT_POLYGON:
			Alchera_DisplayObject_DrawPolygon( object, cr );
			break;
		case DT_LINE:
			Alchera_DisplayObject_DrawLine( object, cr );
			break;
		case DT_ANIMATION:
			Alchera_DisplayObject_DrawAnimation( object, cr );
			break;
		default:
			Alchera_DisplayObject_DrawRectangle( object, cr );
			break;
	}
	cairo_restore( cr );

	if ( object->active )
	{
		cairo_save( cr );

		if ( object->rotate % 2 )
		{
			cairo_rectangle( cr, object->x, object->y, object->h, object->w );
		}
		else
		{
			cairo_rectangle( cr, object->x, object->y, object->w, object->h );
		}

		cairo_set_source_rgba( cr, .2, .6, 1, 0.5 );
		cairo_fill_preserve( cr );

		cairo_set_source_rgba( cr, .2, .6, 1, 1.0 );
		cairo_set_line_width( cr , 0.5);
		cairo_stroke( cr );
		cairo_restore( cr );

		if ( object->resizable )
		{
			cairo_save( cr );
			cairo_set_source_rgba( cr, 0, 0, 0, 0.5 );
			if ( object->rotate % 2 )
				cairo_rectangle( cr, object->x + object->h - 4, object->y + object->w - 4, 6, 6 );
			else
				cairo_rectangle( cr, object->x + object->w - 4, object->y + object->h - 4, 6, 6 );
			cairo_fill( cr );
			cairo_restore( cr );
		}
	}
}


/********************************
* Alchera_Map_AnimationIdle
*
*/
void Alchera_Map_AnimationIdle(DisplayObject * object, cairo_t *cr)
{
	if ( object->type != DT_ANIMATION )
	{
		return;
	}
	cairo_save ( cr );
	Alchera_DisplayObject_DrawAnimation( object, cr );
	cairo_restore ( cr );
	if (object->active)
	{
		static double map_active_dashes[] = {25.0,  /* ink */
				10.0,  /* skip */
				25.0,  /* ink */
				10.0   /* skip*/
		};
		static gint map_active_num  = sizeof (map_active_dashes)/sizeof(map_active_dashes[0]);
		cairo_save ( cr );
		cairo_set_dash (cr, map_active_dashes, map_active_num, 0.0);

		if ( object->rotate % 2 )
		{
			cairo_rectangle( cr, object->x, object->y, object->h, object->w );
		}
		else
		{
			cairo_rectangle( cr, object->x, object->y, object->w, object->h );
		}

		cairo_set_source_rgba (cr, 0, 0, 1, 0.2);
		cairo_fill_preserve ( cr );

		cairo_set_source_rgba (cr, 0, 0, 0, 0.5);
		cairo_stroke ( cr );
		cairo_restore ( cr );
	}
}


gboolean Alchera_Map_PointCollided( DisplayObject * a, gdouble x, gdouble y, guint z)
{
	/*if (z != a->z)
		return 0;*/
	if ( x < (a->x - 8.0) )
		return 0;
	if ( y < (a->y - 8.0) )
		return 0;
	if ( x > (a->x + 8.0) )
		return 0;
	if ( y > (a->y + 8.0) )
		return 0;
	return 1;
}

gboolean Alchera_Map_RectCollided( gdouble ox, gdouble oy, gdouble w, gdouble h, gdouble x, gdouble y )
{
	if ( x < ox )
		return 0;
	if ( y < oy )
		return 0;
	if ( x > (ox + w) )
		return 0;
	if ( y > (oy + h) )
		return 0;
	return 1;
}



/********************************
* Meg_DisplayObject_UpdateBorder
*
*/
void Meg_DisplayObject_UpdateBorder( DisplayObject * obj, DisplayObjectBorder * border )
{
	g_return_if_fail(obj != NULL);
	g_return_if_fail(border != NULL);

	gint width = 0, height = 0;
	//gint parent_width = 0, parent_height = 0;

	width = gdk_pixbuf_get_width( border->image );
	height = gdk_pixbuf_get_height( border->image );

	//parent_width = gdk_pixbuf_get_width( obj->image );
	//parent_height = gdk_pixbuf_get_height( obj->image );

	border->x = border->y = 0;
	border->w = (gdouble)width;
	border->h = (gdouble)height;

	/* Single, Scaled Single, Repeat, Repeat Horizontal, Repeat Vertically */
	switch ( border->position )
	{
		case 0:// "top-left"
		{
			border->x -= (gdouble)width;
			border->y -= (gdouble)height;
			break;
		}
		case 1:// "top"
		{
			border->y -= (gdouble)height;
			if ( border->repeat == 2 ) // Repeat
			{
				border->w = obj->w;
				border->mode = CAIRO_EXTEND_REPEAT;
			}
			else if ( border->repeat == 1 ) // Scaled
			{
				border->w = obj->w;
			}
			break;
		}
		case 2:// top-right"
		{
			border->x += (gdouble)obj->w;
			border->y -= (gdouble)height;
			break;
		}
		case 3:// "right"
		{
			border->x += (gdouble)obj->w;
			if ( border->repeat == 2 ) // Repeat
			{
				border->h = obj->h;
				border->mode = CAIRO_EXTEND_REPEAT;
			}
			else if ( border->repeat == 1 ) // Scaled
			{
				border->h = obj->h;
			}
			break;
		}
		case 4:// "bottom-right"
		{
			border->x += (gdouble)obj->w;
			border->y += (gdouble)obj->h;
			break;
		}
		case 5: //"bottom"
		{
			border->y += (gdouble)obj->h;
			if ( border->repeat == 2 ) // Repeat
			{
				border->w = obj->w;
				border->mode = CAIRO_EXTEND_REPEAT;
			}
			else if ( border->repeat == 1 ) // Scaled
			{
				border->w = obj->w;
			}
			break;
		}
		case 6: //"bottom-left"
		{
			border->x -= (gdouble)width;
			border->y += (gdouble)obj->h;
			break;
		}
		case 7: //"left"
		{
			border->x -= (gdouble)width;
			if ( border->repeat == 2 ) // Repeat
			{
				border->h = obj->h;
				border->mode = CAIRO_EXTEND_REPEAT;
			}
			else if ( border->repeat == 1 ) // Scaled
			{
				border->h = obj->h;
			}
			break;
		}
		default:
		break;
	}

}
gboolean Alchera_DisplayObject_AddBorder( DisplayObject * obj, GdkPixbuf * image, gint position, guint repeat )
{
	g_return_val_if_fail(obj, FALSE);
	g_return_val_if_fail(position != -1, FALSE);
	g_return_val_if_fail(image, FALSE);

	DisplayObjectBorder * border = g_new0(DisplayObjectBorder, 1);

	border->image = image;
	border->mode = CAIRO_EXTEND_NONE;
	border->repeat = repeat;
	border->position = position;

	Meg_DisplayObject_UpdateBorder( obj, border );


	obj->border = g_list_append( obj->border, (gpointer)border );

	return TRUE;
}


void Alchera_DisplayObjectBorder_Free( DisplayObjectBorder * border )
{
	g_return_if_fail( border );

	g_object_unref( border->image );

}

/********************************
* Alchera_DisplayObject_RefreshBorder
*
*/
void Alchera_DisplayObject_RefreshBorder(DisplayObject * object )
{
	if ( !object->border )
		return;

	DisplayObjectBorder * border = NULL;
	GList * border_list = object->border;
	while ( border_list )
	{
		border = (DisplayObjectBorder *)border_list->data;
		Meg_DisplayObject_UpdateBorder( object, border );
		border_list = g_list_next(border_list);
	}

}



/********************************
* Alchera_DisplayObject_New
*
*/
DisplayObject * Alchera_DisplayObject_New( gpointer data, gboolean (*free)(gpointer) )
{
	DisplayObject * object = g_new0(DisplayObject, 1);

	if ( data )
	{
		object->data = data;
		object->free = free;
	}

	object->rotate = 0;
	object->type = DT_OTHER;
	object->resizable = TRUE;
	object->rotatable = FALSE;
	object->supports_path = FALSE;
	object->is_flipped = 0;
	object->colour.red = object->colour.blue = object->colour.green = object->colour.alpha = 1.0;

	object->name = g_strdup("Unknown");

	return object;
}

gchar * display_object_type_names[] = { "None", "Deleted", "Image", "Animation", "Rectangle", "Circle", "Line", "Text", "Polygon", "Other", "Other", "Other", NULL };
void Alchera_DisplayObject_SetName( DisplayObject * object, gchar * name )
{
	if ( name )
	{
		REPLACE_STRING_DUPE(object->name, name);
	}
	else
	{
		REPLACE_STRING_DUPE(object->name, display_object_type_names[object->type]);
	}
}



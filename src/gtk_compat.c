/****************************
Copyright © 2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "gtk_compat.h"


#if GTK_MAJOR_VERSION == 2
void gdk_cairo_set_source_rgba( cairo_t *cr, const GdkRGBA * rgba )
{
	GdkColor c;

	c.red = (guint16)(rgba->red * 65535.0);
	c.blue = (guint16)(rgba->blue * 65535.0);
	c.green = (guint16)(rgba->green * 65535.0);
	c.pixel = 0;

	gdk_cairo_set_source_color( cr, &c );

}

guint32 cairo_surface_get_pixel( cairo_surface_t * surface, gint src_x, gint src_y )
{
	guint32 colour = 0xFA118ac4;
	guchar * data = cairo_image_surface_get_data(surface);
	gint stride = cairo_image_surface_get_stride(surface);

	guchar * pixel = NULL;

	if ( data )
	{
		pixel = data + (src_y * stride) + src_x * 4;

		colour = pixel[0] << 24 | pixel[1] << 16 | pixel[2] << 8 | pixel[3];
	}

	return colour;
}

gboolean gdk_pixbuf_set_pixel( GdkPixbuf * pixbuf, gint src_x, gint src_y, guint32 colour )
{
	guchar * data = gdk_pixbuf_get_pixels(pixbuf);
	gint stride = gdk_pixbuf_get_rowstride(pixbuf);
	guchar * pixel = NULL;

	if ( data )
	{
		pixel = data + (src_y * stride) + src_x * 4;

		pixel[0] = colour;
		return TRUE;
	}
	return FALSE;
}

GdkPixbuf * gdk_pixbuf_get_from_surface( cairo_surface_t * surface, gint src_x, gint src_y, gint width, gint height)
{
	g_return_val_if_fail( surface, NULL );
	g_return_val_if_fail( width > 0, NULL );
	g_return_val_if_fail( height > 0, NULL );

	GdkPixbuf * pixbuf = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, width, height);

	g_return_val_if_fail( pixbuf, NULL );

	gint y,x;
	for ( y = 0; y < height; y++ )
	{
		for ( x = 0; x < width; x++ )
		{
			guint32 c = cairo_surface_get_pixel( surface, src_x+x, src_y+y );
			gdk_pixbuf_set_pixel( pixbuf, src_x, src_y, c );
		}
	}


	return pixbuf;
}


#endif

#if !GLIB_CHECK_VERSION(2,28,0)
void g_slist_free_full( GSList * list, GDestroyNotify free_func )
{
	GSList * el;

	if ( free_func )
	{
		el = list;
		while ( el != NULL )
		{
			free_func(el);
			el = g_slist_next(el);
		}
	}

	g_slist_free(list);
}

void g_list_free_full( GList * list, GDestroyNotify free_func )
{
	GList * el;

	if ( free_func )
	{
		el = list;
		while ( el != NULL )
		{
			free_func(el);
			el = g_list_next(el);
		}
	}

	g_list_free(list);
}
#endif


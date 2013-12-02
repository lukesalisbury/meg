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

GdkPixbuf * gdk_pixbuf_get_from_surface( cairo_surface_t * surface, gint src_x, gint src_y, gint width, gint height)
{
	GdkPixbuf * pixbuf = NULL;
	GdkPixmap * pixmap = NULL;
	cairo_t * cr = NULL;

	g_return_val_if_fail( surface, NULL );
	g_return_val_if_fail( width > 0, NULL );
	g_return_val_if_fail( height > 0, NULL );

	pixmap = gdk_pixmap_new(NULL, width, height, 32);
	cr = gdk_cairo_create(pixmap);
	cairo_set_source_surface(cr, surface, (gdouble)src_x, (gdouble)src_y );
	cairo_paint(cr);
	pixbuf = gdk_pixbuf_get_from_drawable(NULL, pixmap, NULL, 0, 0, 0, 0, width, height);



	cairo_destroy(cr);
	g_object_unref(pixmap);
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


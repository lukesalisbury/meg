/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef _DISPLAY_OBJECT_H_
#define _DISPLAY_OBJECT_H_

typedef struct _DisplayObject DisplayObject;

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "gtk_compat.h"
#include "map_info.h"

typedef enum {
	DT_NONE,
	DT_DELETE,
	DT_IMAGE,
	DT_ANIMATION,
	DT_RECTANGLE,
	DT_CIRCLE,
	DT_LINE,
	DT_TEXT,
	DT_POLYGON,
	DT_POINT,
	DT_PATH,
	DT_PATHITEM,
	DT_EMBEDEDFILE,
	DT_OTHER
} DisplayObjectTypes;

typedef struct {
	GdkPixbuf * image;
	gdouble x, y, w, h;
	gint position;
	guint repeat;
	cairo_extend_t mode;
} DisplayObjectBorder;


struct _DisplayObject {

	DisplayObjectTypes type;
	gdouble x, y, w, h, z_offset;
	guint tw, th, layer, id, rotate;
	gboolean active, animation_timeout, tiled_image, resizable, rotatable, is_flipped, supports_path;

	GdkRGBA colour;

	GSList * path;
	GSList * shape;
	GList * border; /*<DisplayObjectBorder>*/

	GdkPixbuf * image;
	gchar * text;
	gchar * name;
	gpointer data;

	gboolean (*draw_function) ( cairo_t *cr );
	gboolean (*free) ( gpointer data );

};




DisplayObject * Alchera_DisplayObject_New( gpointer data, gboolean (*free)(gpointer) );
void Alchera_DisplayObject_SetName( DisplayObject * object, gchar * name );

void Alchera_DisplayObject_DrawForeach( DisplayObject * object, cairo_t *cr );
gint Alchera_DisplayObject_Order( gconstpointer a, gconstpointer b, gpointer data );
guint Alchera_DisplayObject_StandardMovement( guint state, gboolean mapAlign );

DisplayObject * Alchera_DisplayObject_Add( MapInfo * info, gchar * ident, gdouble x, gdouble y, gdouble w, gdouble h, guint z );

gboolean Alchera_DisplayObject_Advance( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window );
gboolean Alchera_DisplayObject_LayerUp( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window );
gboolean Alchera_DisplayObject_LayerDown( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window );
gboolean Alchera_DisplayObject_Rotate( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window );
gboolean Alchera_DisplayObject_Flip( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window );
gboolean Alchera_DisplayObject_Remove( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window );
gboolean Alchera_DisplayObject_Path( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window );
gboolean Alchera_DisplayObject_Attach( MapInfo * map_info, DisplayObject * obj, GtkWindow * parent_window );
gboolean Alchera_DisplayObject_KeyInput( MapInfo * map_info, DisplayObject * obj, guint key, gboolean resize_modifer, GtkWindow * parent_window );

gboolean Alchera_DisplayObject_AddBorder( DisplayObject * obj, GdkPixbuf * image, gint position, guint repeat );
void Alchera_DisplayObject_RefreshBorder( DisplayObject * object );

#endif

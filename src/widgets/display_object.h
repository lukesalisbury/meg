/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef _DISPLAY_OBJECT_H_
#define _DISPLAY_OBJECT_H_

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "gtk_compat.h"


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

typedef struct {
	DisplayObjectTypes type;
	gdouble x, y, w, h;
	guint tw, th, layer, id, rotate;
	gboolean active, timeout, tile, resizable, rotation, flip, supports_path;
	GdkRGBA colour;
	gpointer data;
	GdkPixbuf * image;
	GSList * path;
	GSList * shape;
	GList * border; /*<DisplayObjectBorder>*/
	gboolean (* custom_draw) (cairo_t *cr);
} DisplayObject;

typedef struct {
	GdkRectangle shape;
	guint id;
	gboolean show_rect;
	gchar * name;
	gpointer data;
} MapSection;

typedef struct {
	DisplayObject * selected;
	GList * displayList;
	GHashTable * settings;
	gchar * name;
	guint32 visible;
	guint32 id_counter;
	guint width, height;
	GdkRGBA colour;
	gpointer data;
	guint8 parse_mode;
	guint8 file_type;
} MapInfo;

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

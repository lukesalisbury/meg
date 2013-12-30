
#ifndef __MAP_H
#define __MAP_H

#include <gtk/gtk.h>
#include <cairo.h>
#include "display_object.h"

typedef struct
{
	gboolean active, rot;
	gdouble x, y, w, h, offset_x, offset_y;
	guint mode, id, sw, sh; /* mode 0: normal, 1: new object hovering, 2: new object click & drag; 3: moving object; 4: points */
	gchar * ident;
} MapSelection;

typedef enum WidgetMapMouse {
	MAP_NONE = 0,
	MAP_PLACEOBJECT = 1,
	MAP_NEWOBJECT = 2,
	MAP_MOVINGOBJECT = 3,
	MAP_ADDPOINT = 4,
	MAP_RESIZEOBJECT = 5
} WidgetMapMouse;


typedef enum WidgetMapMode {
	WIDGETMAP_NORMAL = 0,
	WIDGETMAP_PATH = 1,
	WIDGETMAP_POINTS = 2
} WidgetMapMode;


G_BEGIN_DECLS


#define GTK_TYPE_ALCHERA_MAP			(gtk_alchera_map_get_type())
#define GTK_ALCHERA_MAP(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_ALCHERA_MAP, AlcheraMap))
#define GTK_ALCHERA_MAP_CLASS(obj)		(G_TYPE_CHECK_CLASS_CAST((obj), GTK_ALCHERA_MAP,  AlcheraMapClass))
#define GTK_IS_ALCHERA_MAP(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_ALCHERA_MAP))
#define GTK_IS_ALCHERA_MAP_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE((obj), GTK_TYPE_ALCHERA_MAP))
#define GTK_ALCHERA_MAP_GET_CLASS		(G_TYPE_INSTANCE_GET_CLASS((obj), GTK_TYPE_ALCHERA_MAP, AlcheraMapClass))


typedef struct _AlcheraMap AlcheraMap;
typedef struct _AlcheraMapClass AlcheraMapClass;


struct _AlcheraMap {
	GtkDrawingArea widget;
	gdouble mouse[2];
	gdouble scale_width, scale_height, scale;
	guint map_width, map_height;
	guint32 visible;
	guint16 grid_value;
	guint8 current_layer;
	gboolean align;

	MapSelection selection_region;
	DisplayObject * selected;
	DisplayObject * parent;
	guint32 object_held;
	MapInfo * info;
	gpointer data;

	guint32 grab_time;
	GSList * undo;

	GSList * path;

	WidgetMapMode map_mode;


	/* Support Widgets */
	GtkListStore * objects_list;
	GtkStatusbar * status_widget;
	GtkWidget * menu_widget;

};
//typedef struct _AlcheraMap AlcheraMap;

struct _AlcheraMapClass {
	GtkDrawingAreaClass parent_class;
};

GtkWidget * gtk_alchera_map_new( MapInfo * data );
#if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION  < 14
GtkType gtk_alchera_map_get_type(void);
#else
GType gtk_alchera_map_get_type(void);
#endif

void gtk_alchera_map_set_scale( AlcheraMap * wid, gdouble scale );
void gtk_alchera_map_set_selected_object( AlcheraMap *wid, DisplayObject * obj );
void gtk_alchera_map_set_dimension( AlcheraMap * wid, guint width, guint height );
void gtk_alchera_map_set_support_widget( AlcheraMap * wid, GtkListStore * store, GtkStatusbar * status, GtkWidget * object_menu  );

void gtk_alchera_map_edit_path(AlcheraMap * map);

void gtk_alchera_map_refresh( AlcheraMap * wid );
void gtk_alchera_map_change_scale( AlcheraMap * wid, gdouble value );
void gtk_alchera_map_set_align( AlcheraMap * wid, gboolean able );


GList * gtk_alchera_map_get_list( AlcheraMap * wid );
DisplayObject * gtk_alchera_map_get_selected( AlcheraMap * wid );
MapInfo * gtk_alchera_map_get_info( AlcheraMap * wid );

void gtk_alchera_map_undo_push( AlcheraMap * wid, DisplayObject * moved );
void gtk_alchera_map_undo( AlcheraMap * wid );

gboolean gtk_alchera_map_get_visibility(  AlcheraMap * wid, guint8 layer );
guint8 gtk_alchera_map_get_layer(  AlcheraMap * wid );

void gtk_alchera_map_set_visibility(  AlcheraMap * wid, guint8 layer, gboolean value );
void gtk_alchera_map_set_layer(  AlcheraMap * wid, guint8 value);

gboolean gtk_alchera_map_set_new_object( AlcheraMap * wid, gchar * ident, gint width, gint height, GdkPixbuf * object );

GtkWidget * gtk_alchera_map_get_menu_widget( AlcheraMap * wid );
void gtk_alchera_map_select_background_colour( AlcheraMap * wid, GdkRGBA colour );

G_END_DECLS

#endif

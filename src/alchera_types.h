#ifndef _ALCHERA_TYPES_H_
#define _ALCHERA_TYPES_H_

#include <gtk/gtk.h>
#include <glib.h>
#include "widgets/display_object.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GET_WIDGET(b,n)	GTK_WIDGET( gtk_builder_get_object( (b), (n) ) )
#define GET_LISTSTORE(b,n)	GTK_LIST_STORE( gtk_builder_get_object( (b), (n) ) )
#define GET_LABEL_WIDGET(b,n)	GTK_LABEL( gtk_builder_get_object( (b), (n) ) )
#define SET_OBJECT_SIGNAL(b,n,e,c,d)	g_signal_connect( gtk_builder_get_object( (b), (n) ), (e), (c), (d));
#define SET_OBJECT_SIGNAL_SWAP(b,n,e,c,d)	g_signal_connect_swapped( gtk_builder_get_object( (b), (n) ), (e), (c), (d));
#define SET_MENU_SIGNAL(b,n,c)	g_signal_connect( gtk_builder_get_object( (b), (n) ), "activate", (c), NULL);

#define STRIP_FILE_EXTENSION(t,n)	g_strndup(t, g_utf8_strlen(t, -1) - n );
#define REPLACE_STRING(s,v)	g_free( s ); s = v;
#define REPLACE_STRING_DUPE(s,v)	g_free( s ); s = g_strdup(v);

typedef struct {
	guint id;
	gchar * name;
	GdkRectangle position;
	gboolean show_rect;
	gpointer data;
} SheetObject;

typedef struct {
	GdkPixbuf * image;
	gboolean image_loaded;
	gchar * file;
	gint64 file_size;
	GSList * children; /*<SheetObject>*/
	gboolean visible;
	SheetObject * selected;
	gpointer data;
} Spritesheet;



typedef enum LogStyle
{
	LOG_NONE,
	LOG_BOLD,
	LOG_ERROR,
	LOG_WARNING,
	LOG_FINE
} LogStyle;

typedef struct
{
	gchar * name;
	gchar * arguments;
	gchar * info;
	gpointer user_data;
} EditorDatabaseListing;

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

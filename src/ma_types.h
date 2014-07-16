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
#define GET_SPIN_WIDGET(b,n)	GTK_SPIN_BUTTON( gtk_builder_get_object( (b), (n) ) )

#define SET_OBJECT_SIGNAL(b,n,e,c,d)	g_signal_connect( gtk_builder_get_object( (b), (n) ), (e), (c), (d));
#define SET_OBJECT_SIGNAL_SWAP(b,n,e,c,d)	g_signal_connect_swapped( gtk_builder_get_object( (b), (n) ), (e), (c), (d));
#define SET_MENU_SIGNAL(b,n,c)	g_signal_connect( gtk_builder_get_object( (b), (n) ), "activate", (c), NULL);

#define STRIP_FILE_EXTENSION(t,n)	g_strndup(t, g_utf8_strlen(t, -1) - n );
#define REPLACE_STRING(s,v)	g_free( s ); s = v;
#define REPLACE_STRING_DUPE(s,v)	g_free( s ); if (v) { s = g_strdup(v); } else { s = NULL; }
#define CLEAR_STRING(s)	if (s) { g_free( s ); s = NULL; }

typedef struct {
	guint id;
	gchar * ident_string;
	gchar * display_name;
	gchar * parent_sheet;

	GdkRectangle position;
	gboolean show_rect;
	gboolean visible;

	guint ref;
	gpointer data;
	gboolean (*free)(gpointer data);
} SheetObject;

typedef struct {

	gchar * file;
	gint64 file_size;

	GSList * children; /*<SheetObject>*/
	gboolean visible;
	SheetObject * selected;

	GdkPixbuf * image;
	gboolean image_loaded;

	guint ref;
	gpointer data;
	gboolean (*free)(gpointer data);
} Spritesheet;

typedef struct {
	GdkRectangle shape;
	guint id;
	gboolean show_rect;
	gchar * name;
	gpointer data;
} MapSection;


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
	gchar * arguments_string;
	gchar * info;
	GList * arguments;
	gpointer user_data;
} EditorDatabaseListing;

SheetObject * SheetObject_New( gpointer data, gboolean (*free)(gpointer) );
gboolean SheetObject_Clear( SheetObject * object );
gboolean SheetObject_Free( SheetObject * object );
SheetObject * SheetObject_Ref( SheetObject * object );
SheetObject * SheetObject_Unref( SheetObject * object );

Spritesheet * Spritesheet_New( gpointer data, gboolean (*free)(gpointer) );
gboolean Spritesheet_Free( Spritesheet * object );
Spritesheet * Spritesheet_Ref( Spritesheet * object );
Spritesheet * Spritesheet_Unref( Spritesheet * object );


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

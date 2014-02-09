/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Global Header */
#include "global.h"
#include <gdk/gdkkeysyms.h>

/* Required Headers */
#include "ma_section.h"
#include "ma_map_section.h"

/* GtkWidget */

/* Global Variables */


/* External Functions */
void Meg_MapEdit_OpenFilename( gchar * file );


/* UI */

/********************************
* Event: Map Section press
* Result:
*/
gboolean Meg_MapSection_LayoutPressed( GtkWidget * widget, GdkEventButton * event, GtkWidget * window )
{
	gchar * new_id = NULL;
	guint32 section_id = GPOINTER_TO_UINT( g_object_get_data( G_OBJECT(window), "alchera-section-id" ) );

	new_id = AL_MapSection_Pressed( section_id, Meg_Main_GetWindow(window), (guint)event->x, (guint)event->y, event->button, event->type );

	if ( new_id )
	{
		if ( event->button == 1 )
		{
			if ( event->type == GDK_2BUTTON_PRESS )
			{
				Meg_MapEdit_OpenFilename( new_id );
			}
		}
		g_free( new_id );
	}
	gtk_widget_queue_draw( widget );
	return FALSE;
}


/********************************
* Event:
* Result:
*/
void Meg_MapSection_ComboType( GtkComboBox * combo, GtkWidget * window )
{
	GtkTreeIter iter;
	guint8 value = 0;

	guint32 section_id = GPOINTER_TO_UINT( g_object_get_data( G_OBJECT(window), "alchera-section-id" ) );

	if ( gtk_combo_box_get_active_iter( GTK_COMBO_BOX(combo), &iter) )
	{
		GtkTreeModel * store = gtk_combo_box_get_model( GTK_COMBO_BOX(combo) );
		gtk_tree_model_get(store, &iter, 0, &value, -1 );

		AL_MapSection_Type(section_id, &value, Meg_Main_GetWindow(window) );
	}
}


/********************************
* Meg_MapSection_KeyPress
* Event:
* Result:
*/
void Meg_MapSection_KeyPressed( GtkWidget * widget, GdkEventKey * event, GtkWidget * window )
{
	gchar * new_id = NULL;
	guint32 section_id = GPOINTER_TO_UINT( g_object_get_data( G_OBJECT(window), "alchera-section-id" ) );

	new_id = AL_MapSection_Pressed( section_id, Meg_Main_GetWindow(window), 0, 0, event->keyval, event->type );
	if ( new_id )
	{
		g_free( new_id );
		gtk_widget_queue_draw( widget );
	}
}


/********************************
* Event: Map Overview Refresh
* Result:
*/
gboolean Meg_MapSection_LayoutDraw( GtkWidget * overview, cairo_t * cr, GtkWidget * window )
{
	guint32 section_id = GPOINTER_TO_UINT( g_object_get_data( G_OBJECT(window), "alchera-section-id" ) );

	#if GTK_MAJOR_VERSION == 2
	GtkAllocation wigdet_al;
	gtk_widget_get_allocation(overview, &wigdet_al);
	AL_MapSection_Redraw( section_id, cr, wigdet_al.width, wigdet_al.height );
	#else
	AL_MapSection_Redraw( section_id, cr, gtk_widget_get_allocated_width(overview), gtk_widget_get_allocated_height(overview) );
	#endif

	return FALSE;
}

/********************************
* Event: Map Overview Refresh
* Result:
*/
gboolean Meg_MapSection_LayoutExpose( GtkWidget * overview, GdkEventExpose *event, GtkWidget * widget )
{
	cairo_t * cr = gdk_cairo_create( gtk_widget_get_window(overview) );
	if ( !cr )
	{
		Meg_Main_PrintStatus("gdk_cairo_create failed ");
		return FALSE;
	}
	Meg_MapSection_LayoutDraw( overview, cr, widget );
	return FALSE;
}

/********************************
* Event:
* Result:
*/
void Meg_MapSection_ButtonSave( GtkButton * button, GtkWidget * window  )
{
	gchar * section_name;
	gint result = 0;
	GtkWidget * dialog;
	GtkWindow * parent_window = Meg_Main_GetWindow( window );

	section_name = (gchar*)g_object_get_data( G_OBJECT(window), "alchera-section-name" );

	dialog = gtk_message_dialog_new( parent_window, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "Save '%s'", section_name  );
	gtk_dialog_add_buttons( GTK_DIALOG(dialog), GTK_STOCK_YES, 1, GTK_STOCK_NO, 2, GTK_STOCK_CANCEL, 0, NULL );
	result = gtk_dialog_run( GTK_DIALOG(dialog) );
	gtk_widget_destroy( dialog );

	if ( result == 1 )
	{
		guint32 section_id = GPOINTER_TO_UINT( g_object_get_data( G_OBJECT(window), "alchera-section-id" ) );
		AL_MapSection_Save( section_id, parent_window );
	}
}

/********************************
* Event:
* Result:
*/
void Meg_MapSection_ButtonZoomOut( GtkButton * button, GtkWidget * window  )
{

}

/********************************
* Event:
* Result:
*/
void Meg_MapSection_ButtonZoomNormal( GtkButton * button, GtkWidget * window  )
{

}

/********************************
* Event:
* Result:
*/
void Meg_MapSection_ButtonZoomIn( GtkButton * button, GtkWidget * window  )
{

}


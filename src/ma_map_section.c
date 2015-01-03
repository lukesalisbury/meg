/****************************
Copyright © 2007-2014 Luke Salisbury
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
#include "ma_map.h"
#include "ma_map_section.h"
/* GtkWidget */

/* Global Variables */
extern GtkWidget * sectionNotebook;
extern GtkListStore * sectionFileStore;

/* External Functions */
void Meg_MapEdit_OpenFilename( gchar * file );

/* */
gboolean Meg_MapSection_LayoutDraw( GtkWidget * widget, cairo_t * cr, GtkWidget * overview );
gboolean Meg_MapSection_LayoutExpose( GtkWidget * widget, GdkEventExpose *event, GtkWidget * overview );
gboolean Meg_MapSection_CloseTab( GtkWidget *event_box, GdkEventButton *event, GtkWidget * overview );

/* UI */

const gchar * alchera_map_overview_ui = GUI_MAP_SECTION;


/********************************
* Meg_MapSection_New
* Initialises new Map Section/Overview Tab
*/
GtkWidget * Meg_MapSection_Open( gchar * file )
{
	GtkWidget * widget_overview, * label_text, * display_overview;

	GError * error = NULL;
	GtkBuilder * ui = gtk_builder_new();
	if ( !gtk_builder_add_from_string( ui, alchera_map_overview_ui, -1, &error ) )
	{
		Meg_Error_Print( __func__, __LINE__, "UI creation error: %s", error->message );
		return NULL;
	}

	widget_overview = GET_WIDGET( ui, "overview" );
	label_text = GET_WIDGET( ui, "alchera-label");
	display_overview = GET_WIDGET( ui, "display_overview" );

	/* Tab Settings */
	g_signal_connect( G_OBJECT(widget_overview), "key-release-event", G_CALLBACK(Meg_Main_HelpRequest), NULL);
	g_object_set_data( G_OBJECT(widget_overview), "meg-help-page", g_strdup(PROGRAMSHELPDIRECTORY"/Worlds.xml") );
	g_object_ref( widget_overview );

	/* Set names */
	Meg_Misc_SetLabel( label_text, "World:", file, ' ' );

	guint32 section_id = 0;
	section_id = AL_MapSection_Open( file, Meg_Misc_ParentWindow( sectionNotebook ) );

	gint width = -1, height = -1;
	AL_MapSection_SizeRequest( section_id, &width, &height );
	gtk_widget_set_size_request( display_overview, width, height );
	//gtk_widget_get_preferred_size

	//GtkRequisition requisition;
	//gtk_widget_size_request(display_overview,&requisition);/* FIX: GTK3 */

	/* Signals */
#if GTK_MAJOR_VERSION == 2
	g_signal_connect( display_overview, "expose-event", G_CALLBACK(Meg_MapSection_LayoutExpose), widget_overview );
#else
	g_signal_connect( display_overview, "draw", G_CALLBACK(Meg_MapSection_LayoutDraw), widget_overview );
#endif
	g_signal_connect( display_overview, "button-press-event", G_CALLBACK(Meg_MapSection_LayoutPressed), widget_overview );
	g_signal_connect( display_overview, "key-release-event", G_CALLBACK(Meg_MapSection_LayoutPressed), widget_overview );
	g_signal_connect( display_overview, "button-release-event", G_CALLBACK(Meg_MapSection_LayoutPressed), widget_overview );
	g_signal_connect( display_overview, "key-press-event", G_CALLBACK(Meg_MapSection_LayoutPressed), widget_overview );
	g_signal_connect( display_overview, "popup-menu", G_CALLBACK(Meg_MapSection_LayoutPressed), widget_overview );

	SET_OBJECT_SIGNAL(ui, "button_save", "clicked", G_CALLBACK(Meg_MapSection_ButtonSave), widget_overview );
	SET_OBJECT_SIGNAL(ui, "button_zoomin", "clicked", G_CALLBACK(Meg_MapSection_ButtonZoomIn), widget_overview );
	SET_OBJECT_SIGNAL(ui, "button_zoom", "clicked", G_CALLBACK(Meg_MapSection_ButtonZoomNormal), widget_overview );
	SET_OBJECT_SIGNAL(ui, "button_zoomout", "clicked", G_CALLBACK(Meg_MapSection_ButtonZoomOut), widget_overview );

	SET_OBJECT_SIGNAL(ui, "combo_type", "changed", G_CALLBACK(Meg_MapSection_ComboType), widget_overview );

	/* Notebook Label */
	GtkWidget * notebook_label, * notebook_box, * notebook_close, * notebook_image;

	notebook_label = gtk_label_new(NULL);
	Meg_Misc_SetLabel( notebook_label, "Section", file, ' ' );

	notebook_close = gtk_event_box_new();
	notebook_image = gtk_image_new_from_stock( GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU );
	gtk_container_add( GTK_CONTAINER(notebook_close), notebook_image);
	g_signal_connect( G_OBJECT(notebook_close), "button_press_event", G_CALLBACK(Meg_MapSection_CloseTab), widget_overview);

	notebook_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);

	gtk_container_add( GTK_CONTAINER(notebook_box), notebook_label);
	gtk_container_add( GTK_CONTAINER(notebook_box), notebook_close);
	gtk_widget_show_all( notebook_box );

	g_object_set_data( G_OBJECT(widget_overview), "alchera-section-id", GUINT_TO_POINTER(section_id) );
	g_object_set_data( G_OBJECT(widget_overview), "alchera-section-name", g_strdup(file) );


	gtk_notebook_append_page( GTK_NOTEBOOK(sectionNotebook), widget_overview, notebook_box );
	gtk_widget_queue_draw(widget_overview);
	return widget_overview;
}

/********************************
* Meg_MapSection_Close
*
*/
gboolean Meg_MapSection_Close( GtkWidget *widget, GdkEvent *event, gpointer user_data )
{
	gint result = 0;
	GtkWidget * dialog = gtk_message_dialog_new( Meg_Misc_ParentWindow( widget ), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "Save '%s'", gtk_window_get_title( GTK_WINDOW(widget) ) );
	gtk_dialog_add_buttons( GTK_DIALOG(dialog), GTK_STOCK_YES, 1, GTK_STOCK_NO, 2, GTK_STOCK_CANCEL, 0, NULL );
	result = gtk_dialog_run( GTK_DIALOG(dialog) );
	gtk_widget_destroy( dialog );

	if ( result == 0 )
	{
		return TRUE;
	}
	else
	{
		GtkTreeIter iter;
		if ( gtk_tree_model_get_iter_from_string( GTK_TREE_MODEL(sectionFileStore), &iter,(gchar*)g_object_get_data( G_OBJECT(widget), "alchera-list-path") ) )
		{
			gtk_list_store_set( sectionFileStore, &iter, 1, "", 3, NULL, -1);
		}
		if ( result == 1 )
		{
			guint32 section_id = GPOINTER_TO_UINT( g_object_get_data( G_OBJECT(widget), "alchera-section-id" ) );
			AL_MapSection_Save( section_id, Meg_Misc_ParentWindow( widget ) );
		}
	}
	return FALSE;
}

/********************************
* Meg_MapSection_CloseTab
*
*/
gboolean Meg_MapSection_CloseTab( GtkWidget *event_box, GdkEventButton *event, GtkWidget * overview )
{
	gint result = 0;
	GtkWidget * dialog = gtk_message_dialog_new( Meg_Misc_ParentWindow( overview ), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "Save '%s'", (gchar*)g_object_get_data( G_OBJECT(overview), "alchera-section-name" ) );
	gtk_dialog_add_buttons( GTK_DIALOG(dialog), GTK_STOCK_YES, 1, GTK_STOCK_NO, 2, GTK_STOCK_CANCEL, 0, NULL );
	result = gtk_dialog_run( GTK_DIALOG(dialog) );
	gtk_widget_destroy( dialog );

	if ( result == 0 )
	{
		return TRUE;
	}
	else
	{
		GtkTreeIter iter;
		if ( gtk_tree_model_get_iter_from_string( GTK_TREE_MODEL(sectionFileStore), &iter,(gchar*)g_object_get_data( G_OBJECT(overview), "alchera-list-path") ) )
		{
			gtk_list_store_set( sectionFileStore, &iter, 1, "", 3, NULL, -1);
		}
		if ( result == 1 )
		{

				guint32 section_id = GPOINTER_TO_UINT( g_object_get_data( G_OBJECT(overview), "alchera-section-id" ) );
				AL_MapSection_Save( section_id, Meg_Misc_ParentWindow( overview ) );

		}
		gint page = gtk_notebook_page_num( GTK_NOTEBOOK(sectionNotebook), overview);
		if ( page != -1 )
		{
			gtk_notebook_remove_page( GTK_NOTEBOOK(sectionNotebook), page );
		}

	}
	return FALSE;
}


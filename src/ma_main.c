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
#include <glib/gprintf.h>
#include "widgets/cursors.h"
#include "ma_web.h"
#include "ma_widgets.h"

#if (GTK_MAJOR_VERSION == 2)
#include "gdkkeysyms-backward.h"
#endif

#ifdef apple
#include <gtkmacintegration/gtk-mac-menu.h>
#endif

/* GtkWidget */
GtkWidget * alchera_main_window = NULL;
GtkWidget * alchera_about_window = NULL;
GtkWidget * alchera_main_frame = NULL;
GtkWidget * alchera_main_toolbar = NULL;
GtkWidget * alchera_main_statusbar = NULL;
GtkWidget * alchera_main_textlog = NULL;
GtkWidget * meg_main_empty = NULL;

/* Global Variables */
extern gchar * project_file_path;

/* Local Variables */
GtkTextTag * alchera_log_bold, * alchera_log_error, * alchera_log_fine;
GdkPixbuf * alchera_map_cursor = NULL;
const GtkTargetEntry meg_window_drop_targets = { "text/uri-list", GTK_TARGET_OTHER_APP, 1 };

/* Internal Functions */

/* External Functions */
void Meg_Help_Open( gchar * file );
gboolean Meg_Event_Drop( GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer user_data );
void Meg_Event_DropReceived( GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint ttype, guint time, gpointer user_data );
gboolean Funclist_Scan( );

/* UI */

#include "ui/main_window.gui.h"
const gchar * alchera_main_ui = GUIMAIN_WINDOW

/********************************
* Meg_Main_ToolbarClicked
*
*/
void Meg_Main_ToolbarClicked(GtkToolButton * toolbutton, gpointer user_data)
{
	if ( project_file_path )
	{
		GList * child_list = gtk_container_get_children( GTK_CONTAINER(alchera_main_frame) );
		GtkWidget * child = (GtkWidget *)g_list_nth_data( child_list, 0 );
		if ( child )
		{
			gtk_container_remove( GTK_CONTAINER(alchera_main_frame), child );
		}
		gtk_container_add( GTK_CONTAINER(alchera_main_frame), GTK_WIDGET(user_data) );
		gtk_widget_show_all( GTK_WIDGET(user_data));
	}
}

/********************************
* Meg_Main_GetWindow
*
*/
GtkWindow * Meg_Main_GetWindow()
{
	if ( alchera_main_window )
		return GTK_WINDOW(alchera_main_window);
	else
		return NULL;
}

/********************************
* Meg_Main_GetWidget
*
*/
GtkWidget * Meg_Main_GetWidget()
{
	return alchera_main_window;
}

/********************************
* Meg_Main_SetStatus
*
*/
 guint Meg_Main_SetStatus( GString * status_text )
{
	gtk_statusbar_pop( GTK_STATUSBAR(alchera_main_statusbar), 1 );
	return gtk_statusbar_push( GTK_STATUSBAR(alchera_main_statusbar), 1, status_text->str );
}



/********************************
* Meg_Main_HelpRequest
* Event:
* Result:
*/
gboolean Meg_Main_HelpRequest( GtkWidget * widget, GdkEventKey *event, gpointer user_data )
{
	if ( event->keyval == 0xFFbe )
	{
		GtkWidget * child = NULL;
		GList * child_list = gtk_container_get_children( GTK_CONTAINER(alchera_main_frame) );
		if ( g_list_length(child_list) )
		{
			child = (GtkWidget *)g_list_nth_data( child_list, 0 );
			gchar * page = g_object_get_data( G_OBJECT(child), "meg-help-page" );
			if ( page )
			{
				Meg_Help_Open( page );
			}
		}
	}
	return FALSE;
}


/********************************
* Meg_Main_PrintStatus
*
*/
void Meg_Main_PrintStatus(const gchar * format, ...)
{
	gchar * log = g_new0(gchar, 255);
	va_list argptr;
	va_start( argptr, format );
	g_vsnprintf(log, 255, format, argptr);
	gtk_statusbar_pop( GTK_STATUSBAR(alchera_main_statusbar), 1 );
	gtk_statusbar_push( GTK_STATUSBAR(alchera_main_statusbar), 1, log );
	va_end( argptr );
	g_free(log);
}

/********************************
* Meg_Log_Enable
*
*/
void Meg_Log_Enable()
{
	Meg_Log_SetBuffer( gtk_text_view_get_buffer( GTK_TEXT_VIEW(alchera_main_textlog) ) );
}

/********************************
* Meg_Main_Init
*
*/
gboolean Meg_Main_Init()
{
	GTimer * yimer = g_timer_new();
	g_timer_start(yimer);
	GError * error = NULL;

	g_print("----Meg_Main_Init: ");

	GtkAccelGroup * accel_group = NULL;

	GtkBuilder * ui = gtk_builder_new();
	if ( !gtk_builder_add_from_string(ui, alchera_main_ui, -1, &error) )
	{
		Meg_Error_Print( __func__, __LINE__, "UI creation error: %s", error->message );
		return FALSE;
	}

	alchera_main_window = GET_WIDGET(ui, "alchera_main_window");
	alchera_main_frame = GET_WIDGET(ui, "alchera_main_frame");
	alchera_main_toolbar = GET_WIDGET(ui, "alchera_main_toolbar");
	alchera_main_statusbar = GET_WIDGET(ui, "alchera_main_statusbar");
	alchera_main_textlog = GET_WIDGET(ui, "alchera_main_textlog");
	meg_main_empty = GET_WIDGET(ui, "meg_empty_label");

#ifdef apple
	/* OS X Global menu */
	GtkWidget * menu_bar = GET_WIDGET(ui, "menubar1");
	gtk_widget_hide(menu_bar);
	gtk_mac_menu_set_menu_bar( GTK_MENU_SHELL(menu_bar) );
#endif

	/* Add Signal to Widget */
	g_signal_connect( alchera_main_window, "delete-event", G_CALLBACK(Meg_Event_CloseProgram), NULL);
	g_signal_connect( alchera_main_window, "key-release-event", G_CALLBACK(Meg_Main_HelpRequest), NULL);

	//SET_OBJECT_SIGNAL(b,n,e,c,d)

	SET_MENU_SIGNAL(ui, "action_about", G_CALLBACK(Meg_Event_AboutProgram) );
	SET_MENU_SIGNAL(ui, "action_preferences", G_CALLBACK(Meg_Dialog_Preference) );
	SET_MENU_SIGNAL(ui, "action_quit", G_CALLBACK(Meg_Event_CloseProgram) );

	SET_MENU_SIGNAL(ui, "action_open", G_CALLBACK(Meg_Event_OpenProject) );
	SET_MENU_SIGNAL(ui, "action_close", G_CALLBACK(Meg_Event_CloseProject) );
	SET_MENU_SIGNAL(ui, "action_compile", G_CALLBACK(Meg_Event_Compile) );
	SET_MENU_SIGNAL(ui, "action_play", G_CALLBACK(Meg_Event_Play) );
	SET_MENU_SIGNAL(ui, "action_patch", G_CALLBACK(Meg_Event_CreatePatch) );

	SET_MENU_SIGNAL(ui, "action_export", G_CALLBACK(Meg_Event_Export) );
	SET_MENU_SIGNAL(ui, "action_import", G_CALLBACK(Meg_Event_Import) );

	SET_MENU_SIGNAL(ui, "action_web_forum", G_CALLBACK(Meg_Event_WebForum) );
	SET_MENU_SIGNAL(ui, "action_web_bug", G_CALLBACK(Meg_Event_WebBug) );
	SET_MENU_SIGNAL(ui, "action_web_manual", G_CALLBACK(Meg_Event_WebManual) );

	SET_MENU_SIGNAL(ui, "action_package_update", G_CALLBACK(Meg_Event_PackageCheck) );



	accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group( GTK_WINDOW(alchera_main_window), accel_group);

	gtk_widget_add_accelerator(GET_WIDGET(ui, "menu_play"), "activate", accel_group, GDK_KEY_p, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(GET_WIDGET(ui, "menu_save"), "activate", accel_group, GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

	/* Add Drop and drag support */
	gtk_drag_dest_set( alchera_main_window, GTK_DEST_DEFAULT_ALL, &meg_window_drop_targets, 1,GDK_ACTION_MOVE|GDK_ACTION_COPY);
	g_signal_connect( alchera_main_window, "drag-drop", G_CALLBACK(Meg_Event_Drop), NULL);
	g_signal_connect( alchera_main_window, "drag-data-received", G_CALLBACK(Meg_Event_DropReceived),NULL);

	/* Create Cursor */
	alchera_map_cursor = gdk_pixbuf_add_alpha( gdk_pixbuf_new_from_inline( -1, dnd_cursor_copy, FALSE, NULL ), FALSE, 0, 0, 0 );

	/* Add Play button to toolbar */
	//GtkWidget * widget_play_icon = gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_LARGE_TOOLBAR);
	GtkWidget * widget_play_icon = gtk_image_new_from_icon_name(PAGE_ICON_PLAY, GTK_ICON_SIZE_LARGE_TOOLBAR);

	GtkToolItem * button_play = gtk_tool_button_new(widget_play_icon, "Play Game");
	g_signal_connect(button_play, "clicked", G_CALLBACK(Meg_Event_Play), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(alchera_main_toolbar), button_play, 0);


	g_print("%f\n", g_timer_elapsed(yimer, NULL) );

	g_print("----Funclist_Scan: ");
	Funclist_Scan( );
	g_print("%f\n", g_timer_elapsed(yimer, NULL) );


	/* Tab pages */

	g_print("----Meg_Log_Init: ");
	Meg_Main_AddSection( GET_WIDGET(ui, "scrolledwindow3"), "Log", PAGE_ICON_LOG );
	g_print("%f\n", g_timer_elapsed(yimer, NULL) );

	g_print("----MegWidget_Questions_Create: ");
	MegWidget_Questions_Create();
	g_print("%f\n", g_timer_elapsed(yimer, NULL) );


	g_print("----MegWidget_Help_Create: ");
	MegWidget_Help_Create();
	g_print("%f\n", g_timer_elapsed(yimer, NULL) );

	g_print("----MegWidget_EntityList_Create: ");
	MegWidget_EntityList_Create();
	g_print("%f\n", g_timer_elapsed(yimer, NULL) );

	g_print("----MegWidget_Audio_Create: ");
	MegWidget_Audio_Create();
	g_print("%f\n", g_timer_elapsed(yimer, NULL) );

	g_print("----MegWidget_Section_Create: ");
	MegWidget_Section_Create();
	g_print("%f\n", g_timer_elapsed(yimer, NULL) );


	g_print("----MegWidget_Map_Create: ");
	MegWidget_Map_Create();
	g_print("%f\n", g_timer_elapsed(yimer, NULL) );

	g_print("----MegWidget_Spritesheet_Create: ");
	MegWidget_Spritesheet_Create();
	g_print("%f\n", g_timer_elapsed(yimer, NULL) );

	g_print("----MegWidget_Language_Create: ");
	MegWidget_Language_Create();
	g_print("%f\n", g_timer_elapsed(yimer, NULL) );


	g_print("----MegWidget_Project_Create: ");
	MegWidget_Project_Create();
	g_print("%f\n", g_timer_elapsed(yimer, NULL) );


	g_timer_destroy(yimer);

	if ( project_file_path == NULL )
		Meg_Event_OpenProject();

	/* Create Log System */
	Meg_Log_Enable();
	Meg_Log_Append( LOG_BOLD, PROGRAM_TITLE" Log " );

	return TRUE;
}


/********************************
* Meg_Main_AddSection
*
*/
GtkWidget * Meg_Main_AddSection( GtkWidget * section_widget, gchar * section_title, gchar * section_icon )
{
	GtkToolItem * toolbutton = NULL;
	GtkWidget * icon_widget = NULL;

	if ( section_icon )
	{
		//icon_widget = gtk_image_new_from_stock(section_icon, GTK_ICON_SIZE_LARGE_TOOLBAR);
		icon_widget = gtk_image_new_from_icon_name(section_icon, GTK_ICON_SIZE_LARGE_TOOLBAR);
	}
	toolbutton = gtk_tool_button_new(icon_widget, section_title);

	g_signal_connect(toolbutton, "clicked", G_CALLBACK (Meg_Main_ToolbarClicked), section_widget);

	gtk_toolbar_insert(GTK_TOOLBAR(alchera_main_toolbar), toolbutton, 0);
	gtk_widget_show_all(alchera_main_toolbar);

	return GTK_WIDGET(toolbutton);
}



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

/* Required Headers */
#include "ma_spritesheets.h"

/* GtkWidget */
GtkWidget * meg_sheet_maindisplay;
GtkWidget * meg_sheet_sheetcombo;
GtkWidget * meg_sheet_spriteslist;
GtkWidget * meg_sheet_modelabel = NULL;
GtkWidget * meg_sheet_colourbutton = NULL;
GtkWidget * meg_sheet_modebutton = NULL;
GtkWidget * meg_sheet_label_filesize = NULL;
GtkWidget * meg_sheet_check_visible = NULL;

/* Global Variables */
GtkTreeViewColumn * meg_sheet_deletecolumn = NULL;
GtkListStore * meg_sheet_spriteslist_store = NULL;

extern GList * sheetList;
extern guint16 sheetGridValue;
extern gboolean sheetModeSelect;
extern gdouble sheetScale;
guint32 sheetAlignValue = 8;

/* External Functions */

/* Local Functions */
void MegWidget_Spritesheet_Refresh( GtkWidget * widget, gpointer user_data );

/* UI */

const gchar * alchera_sheet_ui = GUI_PAGE_SHEET;

/********************************
* MegWidget_Spritesheet_Create
*
@
*/
void MegWidget_Spritesheet_Create()
{
	GtkWidget * widget, * button_color;
	GError * error = NULL;

	GtkBuilder * ui = gtk_builder_new();
	if ( !gtk_builder_add_from_string( ui, alchera_sheet_ui, -1, &error ) )
	{
		Meg_Error_Print( __func__, __LINE__, "UI creation error: %s", error->message );
		return;
	}

	/* Widgets */
	widget = GET_WIDGET( ui, "meg_sheeteditor_widget");
	meg_sheet_sheetcombo = GET_WIDGET( ui, "sheet_combo");
	meg_sheet_maindisplay = GET_WIDGET( ui, "meg_sheeteditor_display");
	meg_sheet_spriteslist = GET_WIDGET( ui, "sprites_list");
	meg_sheet_colourbutton = GET_WIDGET( ui, "button_color");
	meg_sheet_modebutton = GET_WIDGET( ui, "button_mode");
	meg_sheet_modelabel = GET_WIDGET( ui, "label_mode");
	meg_sheet_label_filesize = GET_WIDGET( ui, "label_filesize");
	meg_sheet_check_visible = GET_WIDGET( ui, "check_visible");
	button_color = GET_WIDGET( ui, "button_color");
	meg_sheet_deletecolumn = (GtkTreeViewColumn *)gtk_builder_get_object( ui, "treeviewcolumn1" );


	g_object_ref( widget );


	/* Widget Settings */
#if GTK_MAJOR_VERSION == 2
	g_signal_connect( meg_sheet_maindisplay, "expose-event", G_CALLBACK(Meg_Spritesheet_ViewExpose), NULL );
#else
	g_signal_connect( meg_sheet_maindisplay, "draw", G_CALLBACK(Meg_Spritesheet_ViewDraw), NULL );
#endif
	g_object_set_data_full( G_OBJECT(meg_sheet_maindisplay), "sheet-image", NULL, g_object_unref );
	g_object_set_data_full( G_OBJECT(meg_sheet_maindisplay), "sheet-name", NULL, g_free );
	g_object_set_data( G_OBJECT(meg_sheet_maindisplay), "sheet-data", NULL );

	/* Signals */
	/* SET_OBJECT_SIGNAL(b,n,e,c,d) */

	g_signal_connect( widget, "realize", G_CALLBACK(MegWidget_Spritesheet_Refresh), meg_sheet_sheetcombo );
	g_signal_connect( meg_sheet_maindisplay, "button-press-event", G_CALLBACK(Meg_Spritesheet_ViewMouse), meg_sheet_maindisplay );
	g_signal_connect( meg_sheet_maindisplay, "button-release-event", G_CALLBACK(Meg_Spritesheet_ViewMouse), meg_sheet_maindisplay );
	g_signal_connect( meg_sheet_maindisplay, "motion-notify-event", G_CALLBACK(Meg_Spritesheet_DrawMouse), meg_sheet_maindisplay );

	g_signal_connect( meg_sheet_spriteslist, "row-activated", G_CALLBACK(Meg_Spritesheet_SpriteSelect), meg_sheet_maindisplay );

	g_signal_connect( meg_sheet_sheetcombo, "changed", G_CALLBACK(Meg_Spritesheet_SheetChanged), meg_sheet_maindisplay );

	g_signal_connect( meg_sheet_check_visible, "toggled", G_CALLBACK(Meg_Spritesheet_SetVisible), meg_sheet_maindisplay );

	SET_OBJECT_SIGNAL( ui, "button_add", "clicked", G_CALLBACK(Meg_Spritesheet_SheetAdd), meg_sheet_sheetcombo );
	SET_OBJECT_SIGNAL( ui, "button_remove", "clicked", G_CALLBACK(Meg_Spritesheet_SheetRemove), meg_sheet_sheetcombo );
	SET_OBJECT_SIGNAL( ui, "button_edit", "clicked", G_CALLBACK(Meg_Spritesheet_SheetEdit), meg_sheet_sheetcombo );

	SET_OBJECT_SIGNAL( ui, "button_mode", "toggled", G_CALLBACK(Meg_Spritesheet_SetMode), meg_sheet_maindisplay );
	SET_OBJECT_SIGNAL( ui, "button_align", "toggled", G_CALLBACK(Meg_Spritesheet_SetAlign), meg_sheet_maindisplay );

	g_signal_connect( button_color, "color-set", G_CALLBACK(Meg_Spritesheet_ButtonColourSet), meg_sheet_maindisplay );

	SET_OBJECT_SIGNAL( ui, "button_zoomin", "clicked", G_CALLBACK(Meg_Spritesheet_ButtonZoomIn), meg_sheet_maindisplay );
	SET_OBJECT_SIGNAL( ui, "button_zoom", "clicked", G_CALLBACK(Meg_Spritesheet_ButtonZoomNormal), meg_sheet_maindisplay );
	SET_OBJECT_SIGNAL( ui, "button_zoomout", "clicked", G_CALLBACK(Meg_Spritesheet_ButtonZoomOut), meg_sheet_maindisplay );

	/* tab settings */
	g_object_set_data( G_OBJECT(widget), "meg-help-page", g_strdup(PROGRAMSHELPDIRECTORY"/Spritesheet.xml") );
	Meg_Main_AddSection( widget, "Sprite Sheets", PAGE_ICON_SPRITE );

	g_object_unref(ui);

	g_signal_emit_by_name( G_OBJECT(button_color), "color-set" );
}

/********************************
* MegWidget_Spritesheet_Destroy
*
*/
gboolean MegWidget_Spritesheet_Destroy(GtkWidget * widget, GdkEvent * event, gpointer user_data )
{
	return TRUE;
}

/********************************
* MegWidget_Spritesheet_Refresh
*
*/
void MegWidget_Spritesheet_Refresh( GtkWidget * widget, gpointer user_data )
{
	sheetModeSelect = !gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(meg_sheet_modebutton) );
	sheetGridValue = g_key_file_get_integer( Meg_Preference_Storage(), "numeric", "gridvalue", NULL );
	sheetAlignValue = g_key_file_get_integer( Meg_Preference_Storage(), "numeric", "alignvalue", NULL );
}

/********************************
* MegWidget_Spritesheet_Init
*
*/
void MegWidget_Spritesheet_Init()
{
	MegWidget_Spritesheet_SetSheetList( );
}


/********************************
* MegWidget_Spritesheet_Close
*
*/
void MegWidget_Spritesheet_Close()
{
	GtkTreeModel * treemodel = gtk_combo_box_get_model( GTK_COMBO_BOX(meg_sheet_sheetcombo) );

	/* Clear List */
	gtk_list_store_clear( GTK_LIST_STORE(treemodel) );

	MegWidget_Spritesheet_SetFile(NULL);
}


/********************************
* MegWidget_Spritesheet_SetSheetList
*

*/
void MegWidget_Spritesheet_SetSheetList()
{
	GtkTreeModel * treemodel = gtk_combo_box_get_model( GTK_COMBO_BOX(meg_sheet_sheetcombo) );

	/* Clear List */
	gtk_list_store_clear( GTK_LIST_STORE(treemodel) );

	/* Update List */
	AL_Sheet_List( GTK_LIST_STORE(treemodel), TRUE );

}

/********************************
* MegWidget_Spritesheet_SetSheetList
*
*/
void MegWidget_Spritesheet_RefreshSpriteList( )
{
	GtkTreeIter iter;
	Spritesheet * spritesheet = g_object_get_data( G_OBJECT(meg_sheet_maindisplay), "sheet-data" );
	if ( spritesheet )
	{
		GtkTreeModel * treemodel = gtk_tree_view_get_model( GTK_TREE_VIEW(meg_sheet_spriteslist) );

		/* Clear Sprite List */
		gtk_list_store_clear( GTK_LIST_STORE(treemodel) );


		/* Sprite Treeview */
		GSList * list_scan = spritesheet->children;
		while ( list_scan )
		{
			SheetObject * map_object = (SheetObject *)list_scan->data;
			gtk_list_store_append( GTK_LIST_STORE(treemodel), &iter );
			gtk_list_store_set( GTK_LIST_STORE(treemodel), &iter, 0, map_object->display_name, -1 );
			list_scan = g_slist_next( list_scan );
		}

	}
}

/********************************
* MegWidget_Spritesheet_SetSheetList
*

*/
void MegWidget_Spritesheet_SetFile( Spritesheet * spritesheet )
{
	g_object_set_data( G_OBJECT(meg_sheet_maindisplay), "sheet-data", spritesheet );
	if ( spritesheet )
	{
		MegWidget_Spritesheet_RefreshSpriteList();
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(meg_sheet_check_visible), spritesheet->visible );

		if ( spritesheet->image_loaded )
		{

			//Appears to be a glib bug
			//gchar * markup = g_markup_printf_escaped("%s %" G_GINT64_FORMAT " bytes", "File Size:", spritesheet->file_size );
			gchar * markup = g_strdup_printf("%s %" G_GINT64_FORMAT " bytes", "File Size:", spritesheet->file_size );
            gtk_label_set_markup( GTK_LABEL(meg_sheet_label_filesize), markup );
            g_free( markup );

            if ( spritesheet->image )
            {
                gint width = gdk_pixbuf_get_width(spritesheet->image);
                gint height = gdk_pixbuf_get_height(spritesheet->image);

				width = (gint)((gdouble)width * sheetScale);
				height = (gint)((gdouble)height * sheetScale);

                gtk_widget_set_size_request( meg_sheet_maindisplay, width, height);


            }

			Meg_Spritesheet_DisplayScale(meg_sheet_maindisplay);
		}

		spritesheet->selected = NULL;

	}
	else
	{
		gtk_widget_queue_draw( meg_sheet_maindisplay );
	}

}

/********************************
* MegWidget_Spritesheet_Import
*/
void MegWidget_Spritesheet_Import( gchar * filename )
{
	g_return_if_fail(filename);



	GtkWidget * dialog = gtk_message_dialog_new(
				Meg_Main_GetWindow(),
				GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_INFO,
				GTK_BUTTONS_NONE,
				"Do you wish to import %s",
				filename
	);

	gtk_dialog_add_buttons( GTK_DIALOG(dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);

	if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
	{
		if ( AL_Sheet_Add( filename ) )
		{
			MegWidget_Spritesheet_SetFile( NULL );
			MegWidget_Spritesheet_SetSheetList( );
		}
	}
	gtk_widget_destroy( dialog );

}


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
#include "ma_map_edit.h"
#include "ma_map_settings.h"

#include "widgets/map_info.h"

/* GtkWidget */
extern GtkListStore * mapFileStore;
extern GtkWidget * alchera_main_statusbar;
extern const GtkTargetEntry alchera_map_drop_target;
extern GtkWidget * mapNotebook;

/* Global Variables */

/* External Functions */

/* UI */

const gchar * alchera_map_edit_ui = GUI_MAP_EDIT;

/********************************
* Meg_MapEdit_GetMapInfo
*
*/
MapInfo * Meg_MapEdit_GetMapInfo( GtkWidget * widget )
{
	GtkWidget * map = (GtkWidget *)g_object_get_data( G_OBJECT(widget), "alchera-map" );
	return gtk_alchera_map_get_info( GTK_ALCHERA_MAP(map) );
}

/********************************
* Meg_MapEdit_GetMapWidget
*
*/
GtkWidget * Meg_MapEdit_GetMapWidget( GtkWidget * widget )
{
	GtkWidget * map = (GtkWidget *)g_object_get_data( G_OBJECT(widget), "alchera-map" );
	return map;
}

/********************************
* Alchera_Map_OpenFilename
*
*/
void Meg_MapEdit_OpenFilename( gchar * file )
{
	GtkTreeIter iter;
	GtkWidget * window = NULL;
	GtkTreePath * path = NULL;
	GtkTreeModel * model = GTK_TREE_MODEL(mapFileStore);
	gboolean search = FALSE;
	gchar * filename = NULL;

	if ( gtk_tree_model_get_iter_first(model, &iter) )
	{
		do
		{
			gtk_tree_model_get( model, &iter, 0, &filename, 3, &window, -1);
			if ( !g_ascii_strcasecmp( filename, file ) )
			{
				if ( window )
				{
					gint page = gtk_notebook_page_num( GTK_NOTEBOOK(mapNotebook), window);
					if ( page != -1 )
					{
						gtk_notebook_set_current_page( GTK_NOTEBOOK(mapNotebook), page );
					}
				}
				else
				{
					path = gtk_tree_model_get_path( model, &iter );
					window = Meg_MapEdit_Open( file );
					gtk_list_store_set( mapFileStore, &iter, 1, "<i>opened</i>", 3, window, -1);
					g_object_set_data( G_OBJECT(window), "alchera-list-path", gtk_tree_path_to_string(path) );
					gtk_tree_path_free( path );
				}
				g_free(filename);
				return;
			}
			g_free(filename);

			search = gtk_tree_model_iter_next( model, &iter );

		}while ( search );
	}
}

/********************************
* Meg_MapEdit_Open
* Initialises new Map Editor Tab
*/
GtkWidget * Meg_MapEdit_Open( gchar * file )
{
	gint tab_page = -1;
	MapInfo * map_info = NULL;
	MapEditAdvanceWidget * settings_widgets = NULL;

	gdouble map_size_width = AL_Setting_GetDouble("map.width");
	gdouble map_size_height = AL_Setting_GetDouble("map.height");

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create( alchera_map_edit_ui, __func__, __LINE__ );
	g_return_val_if_fail( ui, NULL );

	/* Create MapInfo data */
	map_info = g_new0(MapInfo, 1);
	map_info->name = g_strdup(file);

	if ( !AL_Map_Open( map_info, Meg_Main_GetWindow() ) )
	{
		GtkWidget * dialog = gtk_message_dialog_new( Meg_Main_GetWindow(), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Error loading map '%s'", file );
		gtk_dialog_run( GTK_DIALOG(dialog) );
		gtk_widget_destroy( dialog );

		MapInfo_Destroy( map_info );
		return NULL;
	}

	/* Widgets */
	GtkWidget * widget_editor, * label_text, * widget_map, * iconview_objects, * combo_groups, * combo_layers, * viewport_display, * object_menu, * box_virtualkeys, * button_objectmenu, * status_bar;
	GtkAdjustment * adjust_map_width, * adjust_map_height;
	GtkListStore * store_layers, * store_current_objects, * store_groups;

	widget_editor = GET_WIDGET( ui, "widget_editor" );
	label_text = GET_WIDGET( ui, "alchera-label");

	iconview_objects = GET_WIDGET( ui, "iconview_objects" );
	combo_layers = GET_WIDGET( ui, "combo_layers" );
	combo_groups = GET_WIDGET( ui, "combo_groups" );
	viewport_display = GET_WIDGET( ui, "viewport_display");
	box_virtualkeys = GET_WIDGET( ui, "box_virtualkeys" );
	object_menu = GET_WIDGET( ui, "object_menu");
	button_objectmenu = GET_WIDGET(ui, "button_objectmenu");
	status_bar  = GET_WIDGET(ui, "status_bar");

	store_layers = GET_LISTSTORE( ui, "store_layers" );
	store_current_objects = GET_LISTSTORE( ui, "store_current_objects" );
	store_groups = GET_LISTSTORE( ui, "store_groups" );

	adjust_map_width = GTK_ADJUSTMENT( gtk_builder_get_object(ui, "adjust_map_width") );
	adjust_map_height = GTK_ADJUSTMENT( gtk_builder_get_object(ui, "adjust_map_height") );

	settings_widgets = g_new0(MapEditAdvanceWidget, 1);
	settings_widgets->spin_w = GET_WIDGET( ui, "spin_w");
	settings_widgets->spin_h = GET_WIDGET( ui, "spin_h");
	settings_widgets->combo_wrapmode = GET_WIDGET( ui, "combo_wrapmode");
	settings_widgets->check_centerview = GET_WIDGET( ui, "check_centerview");
	settings_widgets->check_independent = GET_WIDGET( ui, "check_independent");
	settings_widgets->button_colour = GET_WIDGET( ui, "button_colour");
	settings_widgets->box_runtime = GET_WIDGET( ui, "box_runtime");

	/* Alchera Map widget */
	widget_map = gtk_alchera_map_new( map_info );
	if ( GTK_IS_ALCHERA_MAP(widget_map) )
	{
		gtk_alchera_map_set_support_widget( GTK_ALCHERA_MAP(widget_map), store_current_objects, GTK_STATUSBAR(status_bar), object_menu );
		gtk_widget_set_size_request( GTK_WIDGET(viewport_display), MIN(GTK_ALCHERA_MAP(widget_map)->info->width, 640), MIN(GTK_ALCHERA_MAP(widget_map)->info->height, 480) );
	}
	gtk_container_add( GTK_CONTAINER(viewport_display), widget_map );


	/* Settings Widgets */
	gtk_adjustment_configure( adjust_map_width,  map_size_width, map_size_width,  map_size_width * 64, map_size_width, 0.0, 0.0);
	gtk_adjustment_configure( adjust_map_height,  map_size_height, map_size_height,  map_size_height * 64, map_size_height, 0.0, 0.0);

	Meg_MapSettings_Create( widget_map, settings_widgets );

	/* Object Data */
	g_object_set_data( G_OBJECT(widget_editor), "alchera-map", widget_map );
	g_object_set_data( G_OBJECT(combo_layers), "alchera-map", widget_map );
	g_object_set_data( G_OBJECT(widget_editor), "alchera-map-name", g_strdup(file) );

	/* Settings */
	Meg_Misc_SetLabel( label_text, "Map:", file, ' ' );
	gtk_menu_tool_button_set_menu( GTK_MENU_TOOL_BUTTON(button_objectmenu), object_menu );
	gtk_icon_view_enable_model_drag_source( GTK_ICON_VIEW(iconview_objects), GDK_BUTTON1_MASK, &alchera_map_drop_target, 1, GDK_ACTION_COPY );
	AL_Object_List( map_info );

	/* Layers */
	AL_Map_Layers( store_layers );

	/* Display Object Groups */
	if ( store_groups  )
	{
		gtk_list_store_clear( store_groups );
		AL_Object_Groups( store_groups, TRUE );
	}

	/* Signals */

	SET_OBJECT_SIGNAL( ui, "button_visible", "clicked", G_CALLBACK(Meg_MapEdit_ToggleLayer), combo_layers );

	g_signal_connect( G_OBJECT(widget_editor), "key-press-event", G_CALLBACK(Meg_MapEdit_KeyPress), combo_layers);
	g_signal_connect( G_OBJECT(combo_groups), "changed", G_CALLBACK(Meg_MapEdit_GroupChanged), iconview_objects );

	SET_OBJECT_SIGNAL( ui, "button_nextgroup", "clicked", G_CALLBACK(Meg_MapEdit_GroupChangedNext), combo_groups );
	SET_OBJECT_SIGNAL( ui, "button_prevgroup", "clicked", G_CALLBACK(Meg_MapEdit_GroupChangedPrev), combo_groups );
	SET_OBJECT_SIGNAL( ui, "button_showkey", "clicked", G_CALLBACK(Meg_MapEdit_ButtonKeys), box_virtualkeys );

	g_signal_connect( G_OBJECT(combo_layers), "changed", G_CALLBACK(Meg_MapEdit_LayerChanged), widget_map );
	g_signal_connect( G_OBJECT(iconview_objects), "item-activated", G_CALLBACK(Meg_MapEdit_ObjectSelected), widget_map );
	g_signal_connect( G_OBJECT(iconview_objects), "drag-data-get", G_CALLBACK(Meg_MapEdit_ObjectDrop), widget_map );
	g_signal_connect( G_OBJECT(button_objectmenu), "show-menu", G_CALLBACK(Meg_MapEdit_EditorMenu), widget_map );

	SET_OBJECT_SIGNAL( ui, "button_save", "clicked", G_CALLBACK(Meg_MapEdit_ButtonSave), widget_map );
	SET_OBJECT_SIGNAL( ui, "button_visible", "clicked", G_CALLBACK(Meg_MapEdit_UpdateLayers), widget_map );
	SET_OBJECT_SIGNAL( ui, "button_zoomin", "clicked", G_CALLBACK(Meg_MapEdit_ButtonZoomIn), widget_map );
	SET_OBJECT_SIGNAL( ui, "button_zoom", "clicked", G_CALLBACK(Meg_MapEdit_ButtonZoomNormal), widget_map );
	SET_OBJECT_SIGNAL( ui, "button_zoomout", "clicked", G_CALLBACK(Meg_MapEdit_ButtonZoomOut), widget_map );
	SET_OBJECT_SIGNAL( ui, "button_align", "toggled", G_CALLBACK(Meg_MapEdit_ButtonAlign), widget_map );

	SET_OBJECT_SIGNAL( ui, "treeview_objects", "row-activated", G_CALLBACK(Meg_MapEdit_DisplayObjectAdvance), widget_map );

	SET_OBJECT_SIGNAL( ui, "action_layer_tofront" , "activate", G_CALLBACK(Meg_MapObject_LayerUp), widget_map );
	SET_OBJECT_SIGNAL( ui, "action_layer_toback" , "activate", G_CALLBACK(Meg_MapObject_LayerDown), widget_map );
	SET_OBJECT_SIGNAL( ui, "action_object_remove" , "activate", G_CALLBACK(Meg_MapObject_Remove), widget_map );

	SET_OBJECT_SIGNAL( ui, "action_object_path" , "activate", G_CALLBACK(Meg_MapObject_Path), widget_map );
	SET_OBJECT_SIGNAL( ui, "action_object_attach" , "activate", G_CALLBACK(Meg_MapObject_Attach), widget_map );
	SET_OBJECT_SIGNAL( ui, "action_path_modify" , "activate", G_CALLBACK(Meg_MapObject_PathModify), widget_map );

	SET_OBJECT_SIGNAL( ui, "action_object_keyleft" , "activate", G_CALLBACK(Meg_MapObject_KeyLeft), widget_map );
	SET_OBJECT_SIGNAL( ui, "action_object_keyright" , "activate", G_CALLBACK(Meg_MapObject_KeyRight), widget_map );

	SET_OBJECT_SIGNAL( ui, "action_object_keyup" , "activate", G_CALLBACK(Meg_MapObject_KeyUp), widget_map );
	SET_OBJECT_SIGNAL( ui, "action_object_keydown" , "activate", G_CALLBACK(Meg_MapObject_KeyDown), widget_map );

	SET_OBJECT_SIGNAL( ui, "action_object_width_decrease" , "activate", G_CALLBACK(Meg_MapObject_WidthDecrease), widget_map );
	SET_OBJECT_SIGNAL( ui, "action_object_width_increase" , "activate", G_CALLBACK(Meg_MapObject_WidthIncrease), widget_map );
	SET_OBJECT_SIGNAL( ui, "action_object_height_decrease" , "activate", G_CALLBACK(Meg_MapObject_HeightDecrease), widget_map );
	SET_OBJECT_SIGNAL( ui, "action_object_height_increase" , "activate", G_CALLBACK(Meg_MapObject_HeightIncrease), widget_map );

	SET_OBJECT_SIGNAL( ui, "action_map_graphics" , "activate", G_CALLBACK(Meg_MapSettings_GraphicSwitch), widget_map );
	SET_OBJECT_SIGNAL( ui, "action_map_openscript" , "activate", G_CALLBACK(Meg_MapSettings_OpenEntity), widget_map );

	SET_OBJECT_SIGNAL( ui, "action_map_undo" , "activate", G_CALLBACK(Meg_MapEdit_Undo), widget_map );
	SET_OBJECT_SIGNAL( ui, "action_resortobject" , "activate", G_CALLBACK(Meg_MapEdit_Resort), widget_map );




	/* Help */
	g_signal_connect( G_OBJECT(widget_editor), "key-release-event", G_CALLBACK(Meg_Main_HelpRequest), NULL);
	g_object_set_data( G_OBJECT(widget_editor), "meg-help-page", g_strdup(PROGRAMSHELPDIRECTORY"/MapEditor.xml") );

	/* Combo boxes */
	gtk_combo_box_set_active( GTK_COMBO_BOX(combo_layers), 0 );
	gtk_combo_box_set_active( GTK_COMBO_BOX(combo_groups), 0 );

	/* Notebook Label */
	GtkWidget * notebook_label, * notebook_box, * notebook_close, * notebook_image;

	/* Tab Widget */
	notebook_label = gtk_label_new(NULL);
	notebook_box = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 2 );
	notebook_close = gtk_event_box_new();
	notebook_image = gtk_image_new_from_stock( GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU );

	gtk_container_add( GTK_CONTAINER(notebook_close), notebook_image);
	gtk_container_add( GTK_CONTAINER(notebook_box), notebook_label);
	gtk_container_add( GTK_CONTAINER(notebook_box), notebook_close);

	/* Tab Events */
	g_signal_connect( G_OBJECT(notebook_close), "button_press_event", G_CALLBACK(Meg_MapEdit_CloseTab), widget_editor);

	/* Tab Setup */
	Meg_Misc_SetLabel( notebook_label, "Map", file, ' ' );
	gtk_widget_show_all( notebook_box );

	/* Append to notebook */
	tab_page = gtk_notebook_append_page( GTK_NOTEBOOK(mapNotebook), widget_editor, notebook_box );
	gtk_widget_show_all( widget_editor );
	g_object_ref( widget_editor );

	if ( tab_page != -1 )
	{
		gtk_notebook_set_current_page( GTK_NOTEBOOK(mapNotebook), tab_page );
	}

	return widget_editor;
}

/********************************
* Meg_MapEdit_CloseTab
*
*/
gboolean Meg_MapEdit_CloseTab( GtkWidget * event_box , GdkEvent *event, GtkWidget * widget )
{
	gint result = 0;
	GtkWidget * dialog = gtk_message_dialog_new( Meg_Misc_ParentWindow( widget ), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "Save '%s'", (gchar*)g_object_get_data( G_OBJECT(widget), "alchera-map-name" ) );
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
		if ( gtk_tree_model_get_iter_from_string( GTK_TREE_MODEL(mapFileStore), &iter,(gchar*)g_object_get_data( G_OBJECT(widget), "alchera-list-path") ) )
		{
			gtk_list_store_set( mapFileStore, &iter, 1, "", 3, NULL, -1);
		}
		if ( result == 1 )
		{
			AL_Map_Save( Meg_MapEdit_GetMapInfo(widget), Meg_Misc_ParentWindow( widget ) );
		}
		gint page = gtk_notebook_page_num( GTK_NOTEBOOK(mapNotebook), widget);
		if ( page != -1 )
		{
			gtk_notebook_remove_page( GTK_NOTEBOOK(mapNotebook), page );
		}
	}
	return FALSE;
}






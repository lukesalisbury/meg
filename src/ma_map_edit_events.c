/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Map Edit Window Events */

/* Global Header */
#include "global.h"
#include <gdk/gdk.h>
#if (GTK_MAJOR_VERSION == 2)
#include "gdkkeysyms-backward.h"
#endif

/* Required Headers */
#include "ma_map.h"
#include "ma_map_edit.h"
#include "ma_map_section.h"
#include "ma_map_settings.h"


/* Global Variables */
extern GtkListStore * mapFileStore;
extern GtkListStore * sectionFileStore;

/* Local Variables */


/* Functions */


/********************************
* Meg_MapEdit_KeyPress
* Event: Watches for 0 to 9 keypresses to change layer
* Result:
*/
gboolean Meg_MapEdit_KeyPress( GtkWidget * widget, GdkEventKey * event, GtkComboBox * layers )
{
	if ( event->keyval >= GDK_KEY_0 && event->keyval <= GDK_KEY_9)
	{
		AlcheraMap * map = GTK_ALCHERA_MAP( g_object_get_data( G_OBJECT(layers), "alchera-map" ) );
		gtk_combo_box_set_active( layers, event->keyval-48 );
		gtk_alchera_map_set_layer( map, event->keyval-48 );
	}
	return FALSE;
}

/********************************
* Meg_MapEdit_GroupChanged
* Event: Change Sheet
* Result: Updates Icon View
*/
void Meg_MapEdit_GroupChanged( GtkComboBox * widget, GtkWidget * iconview )
{
	if ( iconview )
	{
		GtkTreeModel * list = gtk_icon_view_get_model( GTK_ICON_VIEW(iconview) );
		gchar * select_text = Meg_ComboText_GetText( widget );
		gtk_list_store_clear( GTK_LIST_STORE(list) );
		AL_Object_Available( GTK_LIST_STORE(list), select_text );
	}
}


/********************************
* Meg_MapEdit_GroupChangedNext
* Event: Change Sheet
* Result: Updates Icon View
*/
void Meg_MapEdit_GroupChangedNext( GtkWidget * widget, GtkComboBox * combo_group )
{
	if ( combo_group )
	{
		GtkTreeModel * model =  gtk_combo_box_get_model(combo_group);
		gint index = gtk_combo_box_get_active(combo_group);
		gint maxindex = gtk_tree_model_iter_n_children( model, NULL ) - 1;

		if ( index == -1 )
			index = 0;
		else if ( index >= maxindex )
			index = 0;
		else
			index++;
		gtk_combo_box_set_active( combo_group, index );
	}
}

/********************************
* Meg_MapEdit_GroupChangedPrev
* Event: Change Sheet
* Result: Updates Icon View
*/
void Meg_MapEdit_GroupChangedPrev( GtkWidget * widget, GtkComboBox * combo_group )
{
	if ( combo_group )
	{
		GtkTreeModel * model =  gtk_combo_box_get_model(combo_group);
		gint index = gtk_combo_box_get_active(combo_group);
		gint maxindex = gtk_tree_model_iter_n_children( model, NULL ) - 1;

		if ( index <= 0)
			index = maxindex;
		else if ( index >= maxindex )
			index = 0;
		else
			index--;
		gtk_combo_box_set_active( combo_group, index );
	}
}

/********************************
* Event: Meg_MapEdit_LayerChanged
* Result:

*/
void Meg_MapEdit_LayerChanged( GtkComboBox * layers, GtkWidget * map_widget )
{
	if ( GTK_IS_COMBO_BOX(layers) )
	{
		gboolean active;
		guint layer = 0;
		GtkTreeIter iter;
		GtkTreeModel * list = gtk_combo_box_get_model( layers );

		if ( gtk_combo_box_get_active_iter( layers, &iter ) )
		{
			gtk_tree_model_get( list, &iter, 1, &active, 2, &layer, -1 );
			gtk_alchera_map_set_visibility( GTK_ALCHERA_MAP(map_widget), (guint8)layer, active);
			gtk_alchera_map_set_layer( GTK_ALCHERA_MAP(map_widget), layer );
			gtk_widget_queue_draw( map_widget );
		}
	}
}

/********************************
* Event:
* Result:
*/
void Meg_MapEdit_ToggleLayer(  GtkToolButton * toolbutton, GtkWidget * layers )
{
	gboolean active;
	guint layer = 0;
	GtkTreeIter iter;
	GtkTreeModel * list = gtk_combo_box_get_model( GTK_COMBO_BOX(layers) );

	gtk_combo_box_get_active_iter( GTK_COMBO_BOX(layers), &iter );
	gtk_tree_model_get( list, &iter, 1, &active, 2, &layer, -1 );

	active = !active;

	gtk_list_store_set( GTK_LIST_STORE(list), &iter, 1, active, -1); /* set new value */

	AlcheraMap * map = GTK_ALCHERA_MAP( g_object_get_data( G_OBJECT(layers), "alchera-map" ) );

	gtk_alchera_map_set_visibility( map, (guint8)layer, active );
}

/********************************
* Event:
* Result:
*/
void Meg_MapEdit_UpdateLayers( GtkToolButton * toolbutton, GtkWidget * map_widget )
{

}

/********************************
* Event:
* Result:
*/
void Meg_MapEdit_ButtonAlign( GtkToggleToolButton * widget, GtkWidget * map_widget )
{
	AlcheraMap * map = GTK_ALCHERA_MAP(map_widget);
	map->align = gtk_toggle_tool_button_get_active( widget );
	gtk_alchera_map_refresh( map );
}

/********************************
* Event:
* Result:
*/
void Meg_MapEdit_ButtonSave( GtkButton *widget, GtkWidget * map_widget )
{
	AlcheraMap * map = GTK_ALCHERA_MAP(map_widget);
	AL_Map_Save( map->info, Meg_Misc_ParentWindow( map_widget ) );
}

/********************************
* Event:
* Result:
*/
void Meg_MapEdit_ButtonOptions( GtkButton * widget, GtkWidget * map_widget )
{
	AlcheraMap * map = GTK_ALCHERA_MAP(map_widget);
	if ( AL_Map_Options( map->info, Meg_Misc_ParentWindow( map_widget ) ) )
	{
		Meg_MapSettings_Refresh( map_widget );
		gtk_alchera_map_refresh( map );
	}
}

/********************************
* Event:
* Result:
*/
void Meg_MapEdit_ButtonZoomOut( GtkButton *widget, GtkWidget * map_widget )
{
	gtk_alchera_map_change_scale( GTK_ALCHERA_MAP(map_widget), -0.3 );
	gtk_alchera_map_refresh( GTK_ALCHERA_MAP(map_widget) );
}

/********************************
* Event:
* Result:
*/
void Meg_MapEdit_ButtonZoomNormal( GtkButton *widget, GtkWidget * map_widget )
{
	gtk_alchera_map_set_scale( GTK_ALCHERA_MAP(map_widget), 1.0 );
	gtk_alchera_map_refresh( GTK_ALCHERA_MAP(map_widget) );
}

/********************************
* Event:
* Result:
*/
void Meg_MapEdit_ButtonZoomIn( GtkButton *widget, GtkWidget * map_widget )
{
	gtk_alchera_map_change_scale( GTK_ALCHERA_MAP(map_widget), 0.3 );
	gtk_alchera_map_refresh( GTK_ALCHERA_MAP(map_widget) );
}

/********************************
* Event:
* Result:
*/
void Meg_MapEdit_ButtonKeys( GtkButton * widget, GtkWidget * box )
{
	gboolean visible = gtk_widget_get_visible(box);

	gtk_widget_set_visible( box, !visible );
}

/********************************
* Event: Object Droped
* Result:
*/
void Meg_MapEdit_ObjectDrop( GtkWidget * widget, GdkDragContext * drag_context, GtkSelectionData * data, guint info, guint time, gpointer user_data)
{
	gchar * ident = NULL;
	GtkTreeIter iter;
	GtkTreeModel * model;
	GtkTreePath * path = NULL;
	GList * items = NULL;

	items = gtk_icon_view_get_selected_items( GTK_ICON_VIEW(widget) );

	if ( items )
	{
		path = (GtkTreePath *)items->data;
		model = gtk_icon_view_get_model( GTK_ICON_VIEW(widget) );
		if ( gtk_tree_model_get_iter( model, &iter, path ) )
		{
			gtk_tree_model_get( model, &iter, 2, &ident, -1 );
			gtk_selection_data_set_text(data, ident, -1);
		}
	}
	g_list_foreach( items, (GFunc)gtk_tree_path_free, NULL );
	g_list_free( items );
}

/********************************
* Event: Double clicking on Icon
* Result: To bring up New Object Placing
*/
void Meg_MapEdit_ObjectSelected( GtkIconView * icon_view, GtkTreePath *path, GtkWidget * map_widget )
{
	if ( !map_widget )
		g_error("No Map Widget Found");

	GtkTreeIter iter;
	GtkTreeModel * model;
	gchar *ident;
	gint width, height, type;
	GdkPixbuf * object = NULL;

	model = gtk_icon_view_get_model( icon_view );
	gtk_tree_model_get_iter( model, &iter, path );
	gtk_tree_model_get( model, &iter, 2, &ident, 4, &type, 5, &width, 6, &height, -1 );

	if ( type == DT_IMAGE || type == DT_ANIMATION )
	{
		if ( ident )
		{
			object = AL_GetSprite(ident);
		}
	}

	gtk_alchera_map_set_new_object( GTK_ALCHERA_MAP(map_widget), ident, width, height, object );

	g_free( ident );
}

/********************************
* Meg_MapEdit_Undo
* Event:
* Result:
*/
void Meg_MapEdit_Undo( GtkAction * action, AlcheraMap * map )
{
	if ( map)
	{
		gtk_alchera_map_undo( map );
		gtk_alchera_map_refresh( map );
	}
}





/********************************
* Event: Double clicking on Icon
* Result: To bring up New Object Placing

G_MODULE_EXPORT gboolean Meg_MapEdit_ObjectSelectedKey( GtkWidget * layout, GdkEventKey * event, GtkIconView * icon_view )
{
	if ( event->keyval == 0xff0d )
	{
		GList * items = gtk_icon_view_get_selected_items(icon_view);

		if (items && items->data != NULL)
		{
			GtkTreePath * path = (GtkTreePath*)items->data;
			gtk_icon_view_item_activated(icon_view, path);
		}
		g_list_foreach(items, (GFunc)gtk_tree_path_free, NULL);
		g_list_free(items);
	}
	return FALSE;
}
*/


/********************************
* Meg_MapEdit_DisplayObjectAdvance
* Event:
* Result:
*/
void Meg_MapEdit_DisplayObjectAdvance( GtkTreeView * tree_view, GtkTreeViewColumn *column, GtkTreePath *path, GtkWidget * map_widget )
{
	GtkTreeIter iter;
	GtkTreeModel * model;
	GtkTreeSelection * selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(tree_view) );

	if ( gtk_tree_selection_get_selected(selection, &model, &iter) )
	{
		gint id = -1;
		gtk_tree_model_get (model, &iter, 0, &id, -1);
		if ( id != -1 )
		{
			AlcheraMap * map = GTK_ALCHERA_MAP(map_widget);
			if ( AL_Object_Advance( gtk_alchera_map_get_info( map ), id, GTK_WINDOW(gtk_widget_get_toplevel( map_widget )) ) )
			{
				gtk_alchera_map_refresh( map );
			}
		}
	}
}

/********************************
* Meg_MapEdit_EditorMenu
* Event:
* Result:
*/
void Meg_MapEdit_EditorMenu( GtkMenuToolButton * button, GtkWidget * map_widget )
{
	AlcheraMap * map = GTK_ALCHERA_MAP(map_widget);
	gtk_menu_tool_button_set_menu( GTK_MENU_TOOL_BUTTON(button), gtk_alchera_map_get_menu_widget(map) );
}





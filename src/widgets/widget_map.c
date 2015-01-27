#include "widget_map.h"
#include <string.h>
#include <gdk/gdk.h>
#include "cursors.h"


#if GTK_MAJOR_VERSION == 2
#include "gdkkeysyms-backward.h"
#endif

#ifndef GDK_BUTTON_PRIMARY
#define GDK_BUTTON_PRIMARY      (1)
#endif
#ifndef GDK_BUTTON_SECONDARY
#define GDK_BUTTON_SECONDARY    (3)
#endif

#define MAP_POINT_OFFSET 8
#define MAP_PATHPOINT_OFFSET 8

typedef struct
{
	DisplayObject * obj;
	guint id;
	gdouble x, y, w, h;
} UndoMapObject;

GdkPixbuf * alchera_map_cursor = NULL;
extern GKeyFile * meg_pref_storage;

gchar * display_object_type_names[] = { "None", "Deleted", "Image", "Animation", "Rectangle", "Circle", "Line", "Text", "Polygon", "Other", "Other", "Other", NULL };
const GtkTargetEntry alchera_map_drop_target = { "text/plain", GTK_TARGET_SAME_APP, 1 };

GtkWidget * Object_GetSettingMenu( MapInfo * map_info, guint id );

G_DEFINE_TYPE( AlcheraMap, gtk_alchera_map, GTK_TYPE_DRAWING_AREA );

/*  Helper Functions */

/********************************
* gtk_alchera_pointer_rect_collided
*
* Result:
*/
gboolean gtk_alchera_pointer_rect_collided( gdouble ox, gdouble oy, gdouble w, gdouble h, gdouble x, gdouble y )
{
	if ( x < ox )
		return 0;
	if ( y < oy )
		return 0;
	if ( x > (ox + w) )
		return 0;
	if ( y > (oy + h) )
		return 0;
	return 1;
}

/********************************
* gtk_alchera_object_collided
*/
gboolean gtk_alchera_object_collided( AlcheraMap * map, DisplayObject * a, gdouble x, gdouble y, guint8 z)
{
	if ( !gtk_alchera_map_get_visibility(map, a->layer) && z != 255 )
		return 0;
	/*if (z != a->z)
		return 0;*/
	if ( x < a->x )
		return 0;
	if ( y < a->y)
		return 0;
	if ( a->type == DT_LINE )
	{
		gdouble x2 = ( a->w > 2  ? a->w : 4 );
		gdouble y2 = ( a->h > 2  ? a->h : 4 );
		if ( x > (a->x + x2) )
			return 0;
		if ( y > (a->y + y2) )
			return 0;
	}
	else if ( a->rotate == 0 || a->rotate == 2 )
	{
		if ( x > (a->x + a->w) )
			return 0;
		if ( y > (a->y + a->h) )
			return 0;
	}
	else
	{
		if ( x > (a->x + a->h) )
			return 0;
		if ( y > (a->y + a->w) )
			return 0;
	}
	return 1;
}

/********************************
* gtk_alchera_object_each
*
*/
void gtk_alchera_object_each( DisplayObject * object, GtkListStore * store )
{
	GtkTreeIter iter;

	gtk_list_store_append( store, &iter );
	gtk_list_store_set( store, &iter,
			0, object->id,
			1, display_object_type_names[object->type],
			2, object->x,
			3, object->y,
			4, object->layer,
			-1);
}

/********************************
* gtk_alchera_map_toplevel_window
*/
GtkWindow * gtk_alchera_map_toplevel_window( GtkWidget * widget )
{
	return GTK_WINDOW( gtk_widget_get_toplevel( widget ));
}

/********************************
* gtk_alchera_map_clear_selection_region
*/
void gtk_alchera_map_clear_selection_region( GtkWidget * widget, AlcheraMap * map )
{
	map->selection_region.mode = MAP_NONE;
	map->selection_region.id = 0;
	map->selection_region.offset_x = 0;
	map->selection_region.offset_y = 0;
#if GTK_MAJOR_VERSION == 2
	gdk_pointer_ungrab( map->grab_time );
#else
	gdk_device_ungrab( gtk_get_current_event_device(), map->grab_time );
#endif
	gdk_window_set_cursor( gtk_widget_get_window(widget), NULL );
	gtk_grab_remove(widget);
}

/********************************
* gtk_alchera_map_status_text
*/
void gtk_alchera_map_status_text( GtkStatusbar * wid, const gchar * format, ... )
{
	if ( wid )
	{
		gchar * log = g_new0(gchar, 255);
		va_list argptr;
		va_start( argptr, format );
		g_vsnprintf(log, 255, format, argptr);
		gtk_statusbar_pop( wid, 1 );
		gtk_statusbar_push( wid, 1, log );
		va_end( argptr );
		g_free(log);
	}
}

/********************************
* gtk_alchera_map_selection_timeout
*/
gboolean gtk_alchera_map_selection_timeout( GtkWidget * widget )
{
	g_return_val_if_fail(widget != NULL, FALSE);
	g_return_val_if_fail(GTK_IS_ALCHERA_MAP(widget), FALSE);

	AlcheraMap * map = GTK_ALCHERA_MAP(widget);

	if ( map->object_held )
	{
		map->object_held = 0;
		map->selection_region.mode = MAP_MOVINGOBJECT;

		GdkCursor * cursor;
		cursor = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_FLEUR);
		gdk_window_set_cursor( gtk_widget_get_window(widget), cursor);
		#if GTK_MAJOR_VERSION > 2
		g_object_unref(cursor);
		#else
		gdk_cursor_unref(cursor);
		#endif
	}
	else if ( !map->selected )
	{
		map->selection_region.active = FALSE;
		map->selection_region.id = 0;
		gtk_alchera_map_status_text( map->status_widget, "..............");
	}
	return FALSE;
}

/********************************
* gtk_alchera_map_select_background_colour
*/
void gtk_alchera_map_select_background_colour( AlcheraMap * wid, GdkRGBA colour )
{
	g_return_if_fail(wid != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(wid));

	wid->info->colour = colour;
}

/********************************
* helper for gtk_alchera_map_draw_gridlines function
*/
void gtk_alchera_map_draw_gridlines( AlcheraMap * map, cairo_t * cr  )
{
	if ( map->align )
	{
		gint x = 0, y = 0;
		cairo_save( cr );
		cairo_set_source_rgba( cr, 0, 0, 0, 0.8);
		cairo_set_line_width( cr, 1);
		while ( x < map->info->width )
		{
			cairo_move_to( cr, x, 0);
			cairo_line_to( cr, x, map->info->height);
			cairo_stroke( cr );
			x += map->grid_value;
		}
		while ( y < map->info->height )
		{
			cairo_move_to( cr, 0, y);
			cairo_line_to( cr, map->info->width, y);
			cairo_stroke( cr );
			y += map->grid_value;
		}
		cairo_restore( cr );
	}

}

/******************************************
 * gtk_alchera_map_split_selected_point
 *
 *
 */
void gtk_alchera_map_split_selected_point( GtkMenuItem * menuitem, gpointer user_data)
{
	g_return_if_fail(user_data != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(user_data));

	AlcheraMap * map = GTK_ALCHERA_MAP(user_data);

	g_return_if_fail(map->parent != NULL);
	g_return_if_fail(map->parent->shape != NULL);

	/* Get Current Position */
	guint position = 0;
	GSList * points = map->parent->shape;
	while( points )
	{
		DisplayObject * obj = (DisplayObject *)points->data;
		if ( obj == map->selected )
			break;
		else
		{
			points = g_slist_next(points);
			position++;
		}
	}

	DisplayObject * prev_point = NULL, * next_point = NULL;
	DisplayObject * new_prev_point = NULL, * new_next_point = NULL;

	if ( position > 0 )
	{
		prev_point = (DisplayObject *)g_slist_nth_data(map->parent->shape, position - 1);
		next_point = (DisplayObject *)g_slist_nth_data(map->parent->shape, position + 1);
	}
	else
	{
		next_point = (DisplayObject *)g_slist_nth_data(map->parent->shape, position + 1);
	}

	if ( !next_point )
	{
		next_point = (DisplayObject *)g_slist_nth_data(map->parent->shape, 0);
	}

	g_return_if_fail(next_point != NULL);

	new_next_point = g_new0(DisplayObject, 1);
	new_next_point->type = DT_POINT;
	new_next_point->layer = 43;
	new_next_point->w = new_next_point->h = 16;
	new_next_point->resizable = FALSE;

	new_next_point->x = (map->selected->x + next_point->x)/2;
	new_next_point->y = (map->selected->y + next_point->y)/2;

	if ( prev_point )
	{
		new_prev_point = Alchera_DisplayObject_New( NULL, NULL );
		new_prev_point->type = DT_POINT;
		new_prev_point->layer = 43;
		new_prev_point->w = new_next_point->h = 16;
		new_prev_point->resizable = FALSE;

		new_prev_point->x = (map->selected->x + prev_point->x)/2;
		new_prev_point->y = (map->selected->y + prev_point->y)/2;

		map->parent->shape = g_slist_remove( map->parent->shape, map->selected );

		map->parent->shape = g_slist_insert( map->parent->shape, new_next_point, position );
		map->parent->shape = g_slist_insert( map->parent->shape, new_prev_point, position );

		map->selected = new_prev_point;

	}
	else
	{
		map->parent->shape = g_slist_insert(map->parent->shape, new_next_point, position + 1 );
	}
	gtk_widget_queue_draw( GTK_WIDGET(map) );
}

/******************************************
 * gtk_alchera_map_remove_selected_point
 * Remove Select point in a polygon
 *
 */
void gtk_alchera_map_remove_selected_point( GtkMenuItem * menuitem, gpointer user_data)
{
	g_return_if_fail(user_data != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(user_data));

	AlcheraMap * map = GTK_ALCHERA_MAP(user_data);

	g_return_if_fail(map->parent != NULL);
	g_return_if_fail(map->parent->shape != NULL);

	DisplayObject * first = (DisplayObject *)g_slist_nth_data(map->parent->shape, 0);

	if ( first != map->selected )
		map->parent->shape = g_slist_remove( map->parent->shape, map->selected );

	map->selected = first;
	gtk_widget_queue_draw( GTK_WIDGET(map) );
}

/******************************************
 * gtk_alchera_map_remove_selected_path
 * Remove Select point in a path
 *
 */
void gtk_alchera_map_remove_selected_path( GtkMenuItem * menuitem, gpointer user_data)
{
	g_return_if_fail(user_data != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(user_data));

	AlcheraMap * map = GTK_ALCHERA_MAP(user_data);

	g_return_if_fail(map->parent != NULL);
	g_return_if_fail(map->parent->path != NULL);

	DisplayObject * first = (DisplayObject *)g_slist_nth_data(map->parent->path, 0);

	if ( first != map->selected )
	{
		map->parent->path = g_slist_remove( map->parent->path, map->selected );
	}

	map->selected = first;

	gtk_widget_queue_draw( GTK_WIDGET(map) );
}

/******************************************
 * gtk_alchera_map_add_path_point
 *
 *
 */
void gtk_alchera_map_add_path_point( GtkMenuItem * menuitem, gpointer user_data)
{
	g_return_if_fail(user_data != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(user_data));

	AlcheraMap * map = GTK_ALCHERA_MAP(user_data);

	g_return_if_fail(map->parent != NULL);
	g_return_if_fail(map->parent->path != NULL);

	map->selection_region.mode = MAP_ADDPOINT;

	gtk_alchera_map_status_text( map->status_widget, "Click add a new path point. Right click or Escape to cancel");
}

/******************************************
 * gtk_alchera_map_cancel_edit_path
 *
 *
 */
void gtk_alchera_map_cancel_edit_path( GtkMenuItem * menuitem, gpointer user_data)
{
	g_return_if_fail(user_data != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(user_data));

	AlcheraMap * map = GTK_ALCHERA_MAP(user_data);

	map->selection_region.mode = MAP_NONE;
	map->map_mode = WIDGETMAP_NORMAL;

	map->selected = map->parent = NULL;
	gtk_widget_queue_draw(GTK_WIDGET(map));

}

/********************************
* gtk_alchera_map_object_rotate
* Event:
* Result:
*/
void gtk_alchera_map_object_rotate( GtkAction * action, AlcheraMap * map )
{
	g_return_if_fail(map != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(map));

	DisplayObject * obj = gtk_alchera_map_get_selected( map );

	if ( obj )
	{
		obj->rotate++;
		if ( obj->rotate == 4 )
		{
			obj->rotate = 0;
		}

		gtk_alchera_map_refresh( map );
	}
}

/********************************
* gtk_alchera_map_object_flip
* Event:
* Result:
*/
void gtk_alchera_map_object_flip( GtkToggleAction * action, AlcheraMap * map )
{
	g_return_if_fail(map != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(map));

	DisplayObject * obj = gtk_alchera_map_get_selected( map );

	if ( obj )
	{
		obj->is_flipped = !obj->is_flipped;
		gtk_alchera_map_refresh( map );
	}

}

/******************************************
 * gtk_alchera_map_object_submenu
 * Sets up menu for different Display Objects
 *
 */
GtkWidget * gtk_alchera_map_object_submenu( AlcheraMap * map, DisplayObject * selected )
{
	GtkWidget * widget = NULL;

	if ( map->map_mode == WIDGETMAP_PATH )
	{
		widget = gtk_menu_new();
		if ( selected && selected->type == DT_PATHITEM )
		{
			GtkWidget * menu_item = gtk_menu_item_new_with_label("Remove Current Point");
			g_signal_connect( G_OBJECT(menu_item), "activate", G_CALLBACK(gtk_alchera_map_remove_selected_path), map );
			gtk_menu_shell_append( GTK_MENU_SHELL(widget), menu_item );
			gtk_widget_show(menu_item);
		}
		else
		{
			GtkWidget * menu_item2 = gtk_menu_item_new_with_label("Add Path Point");
			g_signal_connect( G_OBJECT(menu_item2), "activate", G_CALLBACK(gtk_alchera_map_add_path_point), map );
			gtk_menu_shell_append( GTK_MENU_SHELL(widget), menu_item2 );
			gtk_widget_show(menu_item2);
		}

		GtkWidget * menu_item = gtk_menu_item_new_with_label("Exit Path Editing");
		g_signal_connect( G_OBJECT(menu_item), "activate", G_CALLBACK(gtk_alchera_map_cancel_edit_path), map );
		gtk_menu_shell_append( GTK_MENU_SHELL(widget), menu_item );
		gtk_widget_show(menu_item);
	}
	else if ( selected )
	{
		if ( selected->type == DT_POINT )
		{
			widget = gtk_menu_new();

			GtkWidget * menu_item = gtk_menu_item_new_with_label("Remove Point");
			g_signal_connect( G_OBJECT(menu_item), "activate", G_CALLBACK(gtk_alchera_map_remove_selected_point), map );
			gtk_menu_shell_append( GTK_MENU_SHELL(widget), menu_item );
			gtk_widget_show(menu_item);

			GtkWidget * menu_item2 = gtk_menu_item_new_with_label("Split Point");
			g_signal_connect( G_OBJECT(menu_item2), "activate", G_CALLBACK(gtk_alchera_map_split_selected_point), map );
			gtk_menu_shell_append( GTK_MENU_SHELL(widget), menu_item2 );
			gtk_widget_show(menu_item2);
		}

		else
		{
			widget = map->menu_widget;
			GList * list = gtk_container_get_children( GTK_CONTAINER(map->menu_widget) );
			if ( list )
			{
				GtkWidget * menu_item = GTK_WIDGET(list->data);
				gtk_menu_item_set_label( GTK_MENU_ITEM(menu_item), display_object_type_names[selected->type]);

				/* Remove old sub menu */
				gtk_menu_item_set_submenu( GTK_MENU_ITEM(menu_item), NULL );

				if ( selected->type == DT_POLYGON )
				{
					GtkWidget * sub_menu = gtk_menu_new();

					GtkWidget * sub_menu_item = gtk_menu_item_new_with_label( "Convert to Sprite" );
					//g_signal_connect( G_OBJECT(sub_menu_item), "activate", G_CALLBACK(Alchera_DisplayObject_PolygonToSprite), map );
					gtk_menu_shell_append( GTK_MENU_SHELL(sub_menu), sub_menu_item );
					gtk_widget_show(sub_menu_item);
					gtk_menu_item_set_submenu( GTK_MENU_ITEM(menu_item), sub_menu );
				}
				else if ( selected->type == DT_IMAGE )
				{
					GtkWidget * sub_menu = gtk_menu_new();

					GtkWidget * sub_menu_mirror = gtk_menu_item_new_with_label( "Flip Image" );
					GtkWidget * sub_menu_rotate = gtk_menu_item_new_with_label( "Rotate" );

					g_signal_connect( G_OBJECT(sub_menu_mirror), "activate", G_CALLBACK(gtk_alchera_map_object_flip), map );
					g_signal_connect( G_OBJECT(sub_menu_rotate), "activate", G_CALLBACK(gtk_alchera_map_object_rotate), map );


					gtk_menu_shell_append( GTK_MENU_SHELL(sub_menu), sub_menu_mirror );
					gtk_menu_shell_append( GTK_MENU_SHELL(sub_menu), sub_menu_rotate );

					gtk_widget_show(sub_menu_mirror);
					gtk_widget_show(sub_menu_rotate);

					gtk_menu_item_set_submenu( GTK_MENU_ITEM(menu_item), sub_menu );
				}


				/* Create Sub menu for setting */
				GtkWidget * setting_menu_item = GTK_WIDGET(g_list_nth_data(list, 1));
				GtkWidget * setting_sub_menu = Object_GetSettingMenu( map->info, selected->id );
				if ( setting_menu_item )
				{
					if ( setting_sub_menu  )
					{
						gtk_widget_show_all( GTK_WIDGET(setting_sub_menu) );
						gtk_menu_item_set_submenu( GTK_MENU_ITEM(setting_menu_item), setting_sub_menu );
					}
					else
					{
						gtk_menu_item_set_submenu( GTK_MENU_ITEM(setting_menu_item), NULL );
					}
				}
			}
		}
	}

	if ( widget == NULL )
	{
		widget = gtk_menu_new();

		GtkWidget * menuitem = gtk_menu_item_new_with_label("No Object Selected");
		gtk_menu_shell_append( GTK_MENU_SHELL(widget), menuitem );
		gtk_widget_show(menuitem);
	}

	return widget;

}

/******************************************
 * gtk_alchera_map_edit_path
 *
 *
 */
void gtk_alchera_map_edit_path( AlcheraMap * map )
{
	g_return_if_fail(map != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(map));

	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = gtk_alchera_map_toplevel_window( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_Path( gtk_alchera_map_get_info( map ), obj, window ) )
	{
		map->map_mode = WIDGETMAP_PATH;
		map->parent = map->selected;
		map->selected = NULL;
		gtk_alchera_map_status_text( map->status_widget, "Editting Path.");
	}

}

/******************************************
 * gtk_alchera_map_select_object
 *
 *
 */
void gtk_alchera_map_select_object( GtkWidget * widget, AlcheraMap * map, GdkEventButton *event, DisplayObject * object, DisplayObject * parent_object )
{
	if ( !object )
	{
		gtk_alchera_map_status_text( map->status_widget, "Click to select an Object.");
	}

	if ( object->type == DT_DELETE )
	{
		gtk_alchera_map_status_text( map->status_widget, "Click to select an Object.");
		return;
	}

	map->selected = object;
	map->parent = parent_object;
	map->object_held = event->time;

	if ( object->type == DT_POINT )
	{
		map->selection_region.offset_x = map->parent->x + map->selected->x - (event->x / map->scale);
		map->selection_region.offset_y = map->parent->y +map->selected->y - (event->y / map->scale);
		map->selection_region.x = map->parent->x + map->selected->x;
		map->selection_region.y = map->parent->y + map->selected->y;
	}
	else
	{
		map->selection_region.offset_x = map->selected->x - (event->x / map->scale);
		map->selection_region.offset_y = map->selected->y - (event->y / map->scale);
		map->selection_region.x = map->selected->x;
		map->selection_region.y = map->selected->y;
	}

	map->selection_region.w = map->selected->w;
	map->selection_region.h = map->selected->h;
	map->selection_region.id = g_timeout_add(250, (GSourceFunc)gtk_alchera_map_selection_timeout, (gpointer)widget);


	object->active = TRUE;
	parent_object->active = TRUE;

	if ( map->map_mode != WIDGETMAP_NORMAL ) // In special mode already, leave it
		return;

	if ( object->shape )
	{
		map->map_mode = WIDGETMAP_POINTS;
		gtk_alchera_map_status_text( map->status_widget, "Click to cancel editing point mode.");
	}
	else
	{
		map->map_mode = WIDGETMAP_NORMAL;
		gtk_alchera_map_status_text( map->status_widget, "Resize Object by holding Shift and the arrow keys. 'F' to Flip Object. 'Space' to rotate. Right Click for more options ");

	}

	if ( object == parent_object && map->info )
	{
		map->info->selected = object;
	}
}


/* Widget Events */
static gboolean gtk_alchera_map_draw( GtkWidget * widget, cairo_t * cr  )
{
	g_return_val_if_fail(widget != NULL, FALSE);
	g_return_val_if_fail(GTK_IS_ALCHERA_MAP(widget), FALSE);

	AlcheraMap * map = GTK_ALCHERA_MAP(widget);

	cairo_scale( cr, map->scale, map->scale );
	cairo_set_source_rgba( cr, 1, 1, 1, 1 );

	/* Box */
	cairo_save( cr );
	cairo_set_source_rgba( cr, 0.1, 0.1, 0.1, 1);
	cairo_set_line_width( cr, 2.0);
	cairo_move_to( cr, (gdouble)map->info->width-1, 0);
	cairo_rel_line_to( cr, 0, (gdouble)map->info->height-1);
	cairo_move_to( cr, 0, (gdouble)map->info->height-1);
	cairo_rel_line_to( cr, (gdouble)map->info->width-1, 0);
	cairo_stroke( cr );
	cairo_restore( cr );


	/* Background Colour */
	cairo_save( cr );
	cairo_rectangle( cr, 0, 0, map->info->width, map->info->height);
	gdk_cairo_set_source_rgba(cr, &map->info->colour);
	cairo_fill( cr );
	cairo_restore( cr );

	if ( map->info )
	{
		gtk_list_store_clear(map->objects_list);
		if ( map->info->display_list )
		{
			GList * list = g_list_first( map->info->display_list );
			while( list )
			{
				DisplayObject * obj = (DisplayObject *)list->data;
				if ( gtk_alchera_map_get_visibility( map, (guint8)obj->layer ) )
				{
					Alchera_DisplayObject_DrawForeach( obj, cr );
				}
				gtk_alchera_object_each( obj, map->objects_list );
				list = g_list_next(list);
			}
		}
	}

	gtk_alchera_map_draw_gridlines( map, cr );

	if ( map->selection_region.active )
	{
		cairo_save( cr );
		if ( map->selection_region.rot )
			cairo_rectangle( cr, map->selection_region.x, map->selection_region.y, map->selection_region.h, map->selection_region.w);
		else
			cairo_rectangle( cr, map->selection_region.x, map->selection_region.y, map->selection_region.w, map->selection_region.h);
		cairo_set_source_rgba( cr, 1, 1, 1, 0.2);
		cairo_fill_preserve( cr );

		cairo_set_source_rgba( cr, 1, 1, 1, 0.8);
		cairo_stroke( cr );
		cairo_restore( cr );
	}

	if ( map->map_mode != WIDGETMAP_NORMAL )
	{
		cairo_save( cr );
		cairo_set_source_rgba( cr, 0.1, 0.1, 0.1, 0.5);
		cairo_rectangle( cr, 0, 0, map->info->width, map->info->height);
		cairo_fill( cr );
		cairo_restore( cr );

		if ( map->map_mode == WIDGETMAP_POINTS )
		{
			if ( map->parent && map->parent->shape )
			{
				GSList * points = map->parent->shape;
				cairo_save( cr );
				cairo_set_line_width( cr, 0.5);
				while( points )
				{
					DisplayObject * obj = (DisplayObject *)points->data;
					if ( obj == map->selected )
						cairo_set_source_rgba( cr, 0.1, 0.1, 0.1, 0.8 );
					else
						cairo_set_source_rgba( cr, 1.0, 1.0, 1.0, 0.8 );
					cairo_rectangle( cr, map->parent->x +  obj->x -4.0, map->parent->y + obj->y - 4.0, 8.0, 8.0 );
					cairo_fill_preserve( cr );

					cairo_set_source_rgba( cr, 1, 1, 1, 0.8);
					cairo_stroke( cr );
					points = g_slist_next(points);
				}

				cairo_restore( cr );
			}

		}
		else if ( map->map_mode == WIDGETMAP_PATH )
		{
			if ( map->parent && map->parent->path )
			{
				GSList * slist = map->parent->path;
				while( slist )
				{
					DisplayObject * obj = (DisplayObject *)slist->data;
					slist = g_slist_next( slist ); // Peek at next object
					if ( slist )
					{
						DisplayObject * objnext = (DisplayObject *)slist->data;

						cairo_save( cr );
						cairo_set_source_rgba( cr, 1.0, 1.0, 0, 0.8 );
						cairo_set_line_width( cr, 2 );
						if ( obj == map->selected )
							cairo_move_to( cr, map->selection_region.x, map->selection_region.y);
						else
							cairo_move_to( cr, obj->x, obj->y);
						if ( objnext == map->selected )
							cairo_line_to( cr, map->selection_region.x, map->selection_region.y );
						else
							cairo_line_to( cr, objnext->x, objnext->y );
						cairo_stroke( cr );
						cairo_restore( cr );
					}
					cairo_save( cr );

					if ( obj == map->selected )
					{
						cairo_set_source_rgba( cr, 0.1, 0.1, 0.5, 0.8 );
						cairo_rectangle( cr, map->selection_region.x-4.0, map->selection_region.y-4.0, 8.0, 8.0 );
					}
					else
					{
						cairo_set_source_rgba( cr, 0.5, 0.5, 0, 0.8 );
						cairo_rectangle( cr, obj->x-4.0, obj->y-4.0, 8.0, 8.0 );
					}
					cairo_fill( cr );
					cairo_restore( cr );
				}
			}

		}

	}

	return FALSE;
}

#if GTK_MAJOR_VERSION == 2
static gboolean gtk_alchera_map_expose( GtkWidget *widget, GdkEventExpose *event )
{
	cairo_t * cr;
	gboolean result = FALSE;

	g_return_val_if_fail(event != NULL, FALSE);
	g_return_val_if_fail(GTK_IS_ALCHERA_MAP(widget), FALSE);
	/*g_return_val_if_fail(!GDK_IS_DRAWABLE(gtk_widget_get_window(widget)), FALSE);*/

	cr = gdk_cairo_create( gtk_widget_get_window(widget) );
	if ( !cr )
		return FALSE;

	result = gtk_alchera_map_draw( widget, cr );
	cairo_destroy( cr );
	return result;
}
#endif

/* Movement Event */
/**********************************
* Event: Mouse Moved
*
*/
static gboolean gtk_alchera_map_motion_notify( GtkWidget *widget, GdkEventMotion *event )
{
	AlcheraMap * map = GTK_ALCHERA_MAP(widget);
	gboolean continue_motion = FALSE;

	gdouble event_x = map->mouse[0] = event->x / map->scale;
	gdouble event_y = map->mouse[1] = event->y / map->scale;

	if ( map->selection_region.mode == MAP_NEWOBJECT ) /* new object click & drag */
	{
		map->selection_region.w = (event_x) - map->selection_region.x;
		map->selection_region.h = (event_y) - map->selection_region.y;
		if (map->selection_region.w < map->selection_region.sw)
			map->selection_region.w = map->selection_region.sw;
		else
			map->selection_region.w = (gdouble)((gint)(map->selection_region.w / map->selection_region.sw)) * map->selection_region.sw;
		if (map->selection_region.h < map->selection_region.sh)
			map->selection_region.h = map->selection_region.sh;
		else
			map->selection_region.h = (gdouble)((gint)(map->selection_region.h / map->selection_region.sh)) * map->selection_region.sh;
		map->selection_region.active = TRUE;

		gtk_widget_queue_draw(widget);
		gtk_alchera_map_status_text( map->status_widget, "Placing Map Object at %dx%d by %dx%d. Release Mouse button to set object", (gint)map->selection_region.x, (gint)map->selection_region.y, (gint)map->selection_region.w, (gint)map->selection_region.h);
		continue_motion = TRUE;
	}
	else if ( map->selection_region.mode == MAP_MOVINGOBJECT ) /* moving object */
	{
		map->selection_region.x = (event_x) + map->selection_region.offset_x;
		map->selection_region.y = (event_y) + map->selection_region.offset_y;
		map->selection_region.rot = (map->selected->rotate % 2) ? TRUE : FALSE ;
		map->selection_region.active = TRUE;
		if ( map->align )
		{
			map->selection_region.x =(gdouble)(((gint)map->selection_region.x / map->grid_value) * map->grid_value);
			map->selection_region.y =(gdouble)(((gint)map->selection_region.y / map->grid_value) * map->grid_value);
		}

		gtk_widget_queue_draw(widget);
		continue_motion = TRUE;
	}
	else if ( map->selection_region.mode == MAP_RESIZEOBJECT ) /* moving object */
	{
		if ( map->selected )
		{
			if ( map->selected->resizable && map->selected->th != 0 && map->selected->tw != 0 ) //
			{
				map->selection_region.rot = (map->selected->rotate % 2) ? TRUE : FALSE ;

				if ( map->selection_region.rot )
				{
					map->selection_region.h = (gdouble)(((guint)((event_x) - map->selection_region.x) / map->selected->th) * map->selected->th);
					map->selection_region.w = (gdouble)(((guint)((event_y) - map->selection_region.y) / map->selected->tw) * map->selected->tw);
				}
				else
				{
					map->selection_region.w = (gdouble)(((guint)((event_x) - map->selection_region.x) / map->selected->tw) * map->selected->tw);
					map->selection_region.h = (gdouble)(((guint)((event_y) - map->selection_region.y) / map->selected->th) * map->selected->th);
				}
				if ( map->selection_region.w < (gdouble)map->selected->tw )
					map->selection_region.w = (gdouble)map->selected->tw;
				if ( map->selection_region.w < 1.0 )
					map->selection_region.w = (gdouble)map->selected->tw;
				if ( map->selection_region.h < (gdouble)map->selected->th )
					map->selection_region.h = (gdouble)map->selected->th;
				if ( map->selection_region.h < 1.0 )
					map->selection_region.h = (gdouble)map->selected->th;

				map->selection_region.active = TRUE;



				gtk_widget_queue_draw(widget);
			}
			continue_motion = TRUE;
		}
	}
	else if ( map->selection_region.mode == MAP_PLACEOBJECT )
	{
		if ( map->selection_region.ident )
		{
			gtk_alchera_map_status_text( map->status_widget, "Click to start placing Map Object at x:%0.f y:%0.f ", event_x, event_y);

			map->selection_region.x = event_x;
			map->selection_region.y = event_y;
			if ( map->align )
			{
				map->selection_region.x =(gdouble)(((gint)map->selection_region.x / map->grid_value) * map->grid_value);
				map->selection_region.y =(gdouble)(((gint)map->selection_region.y / map->grid_value) * map->grid_value);
			}
			map->selection_region.active = TRUE;
			gtk_widget_queue_draw( widget );
		}
		else
			map->selection_region.mode = MAP_NONE;
	}


	return continue_motion;
}

/* Input Event */
gboolean gtk_alchera_map_grab( GtkWidget *widget, GdkEventButton *event )
{

	gtk_widget_grab_focus( widget );

	return TRUE;
}

static void gtk_alchera_map_button_press_placeobject( AlcheraMap * map, GtkWidget * widget, GdkEventButton * event, gdouble event_x, gdouble event_y )
{
	if ( event->button == GDK_BUTTON_PRIMARY )
	{
		map->selection_region.x = event_x;
		map->selection_region.y = event_y;

		if ( map->align )
		{
			map->selection_region.x =(gdouble)(((gint)map->selection_region.x / map->grid_value) * map->grid_value);
			map->selection_region.y =(gdouble)(((gint)map->selection_region.y / map->grid_value) * map->grid_value);
		}

		map->selection_region.mode = MAP_NEWOBJECT;
		map->selection_region.active = TRUE;
	}

}

/**********************************
* gtk_alchera_map_button_press
* Event: Mouse Button Released
*
*/
static gboolean gtk_alchera_map_button_press( GtkWidget *widget, GdkEventButton *event )
{
	AlcheraMap * map = GTK_ALCHERA_MAP(widget);


	gdouble event_x = event->x / map->scale;
	gdouble event_y = event->y / map->scale;

	if ( map->selection_region.mode == MAP_PLACEOBJECT ) /* Start Drag */
	{
		gtk_alchera_map_button_press_placeobject( map, widget, event, event_x, event_y );
	}
	else if ( map->selection_region.mode == MAP_NEWOBJECT ) /* End Drag */
	{

	}
	else if ( map->selection_region.mode == MAP_MOVINGOBJECT )
	{

	}
	else if ( map->selection_region.mode == MAP_RESIZEOBJECT )
	{

	}
	else if ( map->selection_region.mode == MAP_ADDPOINT ) /* Add point */
	{

	}
	else /* Select */
	{
		if ( event->button == GDK_BUTTON_SECONDARY ) // Right Click
		{
			GtkWidget * menu_widget = gtk_alchera_map_object_submenu( map, map->selected );
			gtk_menu_popup( GTK_MENU(menu_widget), NULL, NULL, NULL, NULL, event->button, event->time);
		}
		else if ( event->button == GDK_BUTTON_PRIMARY )
		{
			if ( !map->object_held )
			{
				/* Check for Collisions */
				if ( map->map_mode == WIDGETMAP_NORMAL ) /* Normal Mode */
				{
					if ( map->selected )
					{
						if ( map->selected->resizable )
						{
							map->selection_region.rot = (map->selected->rotate % 2) ? TRUE : FALSE ;
							if ( map->selection_region.rot)
							{
								if ( gtk_alchera_pointer_rect_collided( (map->selected->x + map->selected->h - 4), (map->selected->y + map->selected->w - 4), 6.0, 6.0, event_x, event_y)  )
								{
									map->selection_region.mode = MAP_RESIZEOBJECT;
									map->selection_region.x = map->selected->x;
									map->selection_region.y = map->selected->y;
									return TRUE;
								}
							}
							else
							{
								if ( gtk_alchera_pointer_rect_collided( (map->selected->x + map->selected->w - 4), (map->selected->y + map->selected->h - 4), 6.0, 6.0, event_x, event_y )  )
								{
									map->selection_region.mode = MAP_RESIZEOBJECT;
									map->selection_region.x = map->selected->x;
									map->selection_region.y = map->selected->y;
									return TRUE;
								}
							}
						}
						map->selected->active = FALSE;

						gtk_widget_queue_draw_area(widget, (gint)map->selected->x-8,  (gint)map->selected->y - 8,  (gint)map->selected->w + 8,  (gint)map->selected->h+ 8);

						/* reset selection */
						map->selected = NULL;
						map->info->selected = NULL;
						map->object_held = 0;


					}

					if ( !map->selected )
					{
						GList * list_scan = g_list_last(map->info->display_list);
						while( list_scan )
						{
							DisplayObject * object = (DisplayObject *)list_scan->data;
							object->active = FALSE;
							if ( gtk_alchera_object_collided(map, object, event_x, event_y, gtk_alchera_map_get_layer(map) ) )
							{
								gtk_widget_queue_draw(widget);
								gtk_alchera_map_select_object( widget, map, event, object, object );
								list_scan = NULL;
							}
							else
							{
								list_scan = g_list_previous(list_scan);
							}
						}
						if ( !map->selected )
							gtk_alchera_map_status_text( map->status_widget, "Click to select an object.");
					}
				}
				else if ( map->map_mode == WIDGETMAP_POINTS )
				{
					map->selected = NULL;
					if ( map->parent && map->parent->shape )
					{
						GSList * point_scan = map->parent->shape;
						if ( point_scan )
						{
							while( point_scan )
							{
								DisplayObject * object = (DisplayObject *)point_scan->data;
								object->active = FALSE;
								if ( gtk_alchera_object_collided(map, object, (-map->parent->x + event_x) + MAP_POINT_OFFSET, (-map->parent->y + event_y) + MAP_POINT_OFFSET, 255 ) )
								{
									gtk_widget_queue_draw(widget);
									gtk_alchera_map_select_object( widget, map, event, object, map->parent );
									point_scan = NULL;
								}
								else
								{
									point_scan = g_slist_next(point_scan);
								}
							}
						}
					}
					if ( !map->selected )
						gtk_alchera_map_status_text( map->status_widget, "Click to select an polygon point.");

				}
				else if ( map->map_mode == WIDGETMAP_PATH )
				{
					if ( map->selected )
						map->selected->active = FALSE;

					map->selected = NULL;
					if ( map->parent && map->parent->path )
					{
						GSList * point_scan = g_slist_nth( map->parent->path, 1);
						if ( point_scan )
						{
							while( point_scan )
							{
								DisplayObject * object = (DisplayObject *)point_scan->data;
								object->active = FALSE;
								if ( gtk_alchera_object_collided(map, object, event_x + MAP_PATHPOINT_OFFSET, event_y + MAP_PATHPOINT_OFFSET, 255 ) )
								{
									gtk_widget_queue_draw(widget);
									gtk_alchera_map_select_object( widget, map, event, object, map->parent );
									point_scan = NULL;
								}
								else
								{
									point_scan = g_slist_next(point_scan);
								}
							}
						}
					}

					if ( !map->selected )
						gtk_alchera_map_status_text( map->status_widget, "Click to select an path point.");
				}

				/* If nothing is selected, reset the mode back to normal */
				if ( !map->selected && map->map_mode != WIDGETMAP_PATH )
				{
					map->map_mode = WIDGETMAP_NORMAL;
					map->selection_region.mode = MAP_NONE;
					map->selection_region.active = FALSE;
					map->parent = NULL;
					gtk_widget_queue_draw(widget);
				}
			}
			else
			{
				map->object_held = 0;
			}
		}
		return TRUE;
	}

	gtk_widget_queue_draw( widget );
	return FALSE;
}

static void gtk_alchera_map_button_release_newobject( AlcheraMap * map, GtkWidget * widget, GdkEventButton * event, gdouble event_x, gdouble event_y )
{
	if ( event->button == GDK_BUTTON_PRIMARY )
	{
		map->selection_region.active = FALSE;
		if ( map->selection_region.ident )
		{
			map->selection_region.w = event_x;
			map->selection_region.h = event_y;
			map->selection_region.w -= map->selection_region.x;
			map->selection_region.h -= map->selection_region.y;

			if (map->selection_region.w < map->selection_region.sw)
				map->selection_region.w = map->selection_region.sw;
			else
				map->selection_region.w = (gdouble)((gint)(map->selection_region.w / map->selection_region.sw)) * map->selection_region.sw;
			if (map->selection_region.h < map->selection_region.sh)
				map->selection_region.h = map->selection_region.sh;
			else
				map->selection_region.h = (gdouble)((gint)(map->selection_region.h / map->selection_region.sh)) * map->selection_region.sh;

			/* Add New Object */
			DisplayObject * obj = Alchera_DisplayObject_Add( map->info, map->selection_region.ident, map->selection_region.x, map->selection_region.y, map->selection_region.w, map->selection_region.h, gtk_alchera_map_get_layer(map) );
			if ( obj )
			{
				map->info->display_list = g_list_insert_sorted_with_data( map->info->display_list, (gpointer)obj, (GCompareDataFunc)Alchera_DisplayObject_Order, NULL);

				/* Remove Active Flag from prevuios selected object */
				if ( map->selected )
				{
					map->selected->active = FALSE;
					map->map_mode = WIDGETMAP_NORMAL;
				}

				map->selected = obj;
			}

			/* If Shift is pressed, continue adding objects */
			if ( event->state & GDK_SHIFT_MASK )
			{
				map->selection_region.mode = MAP_PLACEOBJECT;
			}
			else
			{
				if ( map->selection_region.ident )
				{
					g_free(map->selection_region.ident);
					map->selection_region.ident = NULL;
				}
				gtk_alchera_map_clear_selection_region( widget, map );
			}
		}
		else
		{
			gtk_alchera_map_clear_selection_region( widget, map );
		}
	}
}

static void gtk_alchera_map_button_release_movingobject( AlcheraMap * map, GtkWidget * widget, GdkEventButton * event, gdouble event_x, gdouble event_y )
{
	if ( event->button == GDK_BUTTON_PRIMARY )
	{
		gtk_alchera_map_undo_push( map, map->selected );
		gtk_widget_queue_draw_area(widget, (gint)map->selected->x-8,  (gint)map->selected->y - 8,  (gint)map->selected->w + 8,  (gint)map->selected->h+ 8);

		if ( map->selected->type == DT_POINT )
		{
			map->selected->x = (map->selection_region.x) - map->parent->x;
			map->selected->y = (map->selection_region.y) - map->parent->y;
		}
		else
		{
			map->selected->x = (map->selection_region.x);
			map->selected->y = (map->selection_region.y);
		}

		if ( map->selected->path )
		{
			/* If path exist, change the first position of it */
			DisplayObject * objn = g_slist_nth_data( map->selected->path, 0);
			if (objn)
			{
				objn->x = map->selected->x;
				objn->y = map->selected->y;
			}
		}
		gtk_widget_queue_draw_area(widget, (gint)map->selected->x-8,  (gint)map->selected->y - 8,  (gint)map->selected->w + 8,  (gint)map->selected->h+ 8);

		map->selection_region.active = FALSE;

		gtk_alchera_map_clear_selection_region( widget, map );

	}
}

static void gtk_alchera_map_button_release_resizeobject( AlcheraMap * map, GtkWidget * widget, GdkEventButton * event, gdouble event_x, gdouble event_y )
{
	if ( event->button == GDK_BUTTON_PRIMARY )
	{
		if ( map->selected->resizable )
		{
			gtk_alchera_map_undo_push( map, map->selected );

			map->selected->w = map->selection_region.w;
			map->selected->h = map->selection_region.h;

			Alchera_DisplayObject_RefreshBorder(map->selected);

			map->selection_region.active = FALSE;

			gtk_alchera_map_clear_selection_region( widget, map );

			gtk_widget_queue_draw_area(widget, (gint)map->selected->x-8,  (gint)map->selected->y - 8,  (gint)map->selected->w + 8,  (gint)map->selected->h + 8);
		}
	}
}

static void gtk_alchera_map_button_release_addpoint( AlcheraMap * map, GtkWidget * widget, GdkEventButton * event, gdouble event_x, gdouble event_y )
{
	if ( event->button == GDK_BUTTON_PRIMARY )
	{
		DisplayObject * objn = g_new0(DisplayObject, 1);
		map->parent->path = g_slist_append( map->parent->path, objn );
		objn->x = event_x;
		objn->y = event_y;
		objn->w = objn->h = 16;
		objn->type = DT_PATHITEM;
		objn->layer = 255;

		map->selection_region.mode = MAP_ADDPOINT;
		map->selection_region.active = FALSE;
		map->selected = (DisplayObject *)g_slist_nth_data(map->parent->path, 0);

		gtk_widget_queue_draw(widget);
	}

	if ( event->button == GDK_BUTTON_SECONDARY )
	{
		map->selection_region.mode = MAP_NONE;

		gtk_alchera_map_status_text( map->status_widget, "Select path point to move.");
	}
}

/**********************************
* gtk_alchera_map_button_release
* Event: Mouse Button Released
*
*/
static gboolean gtk_alchera_map_button_release( GtkWidget *widget, GdkEventButton *event )
{
	AlcheraMap * map = GTK_ALCHERA_MAP(widget);
	gtk_widget_grab_focus( widget );

	gdouble event_x = event->x / map->scale;
	gdouble event_y = event->y / map->scale;


	if ( map->selection_region.mode == MAP_PLACEOBJECT ) /* Start Drag */
	{
	}
	else if ( map->selection_region.mode == MAP_NEWOBJECT ) /* End Drag */
	{
		gtk_alchera_map_button_release_newobject( map,  widget, event, event_x, event_y );
	}
	else if ( map->selection_region.mode == MAP_MOVINGOBJECT )
	{
		gtk_alchera_map_button_release_movingobject( map,  widget, event, event_x, event_y );
	}
	else if ( map->selection_region.mode == MAP_RESIZEOBJECT )
	{
		gtk_alchera_map_button_release_resizeobject( map,  widget, event, event_x, event_y );
	}
	else if ( map->selection_region.mode == MAP_ADDPOINT ) /* Add point */
	{
		gtk_alchera_map_button_release_addpoint( map,  widget, event, event_x, event_y );
	}
	else /* Select */
	{
		if ( event->button == GDK_BUTTON_PRIMARY )
		{
			if ( !map->object_held && event->type == GDK_BUTTON_PRESS )
			{

			}
			else
			{
				map->object_held = 0;
			}
		}

		return TRUE;
	}
	gtk_widget_queue_draw( widget );
	return FALSE;
}

/**********************************
* gtk_alchera_map_button_press
* Event: Mouse Button press
*
*/
static gboolean gtk_alchera_map_key_release( GtkWidget * widget, GdkEventKey *event )
{
	AlcheraMap * map = GTK_ALCHERA_MAP(widget);

	if ( event->keyval == GDK_KEY_Escape )
	{
		if ( map->selection_region.mode == MAP_ADDPOINT )
		{
			map->map_mode = WIDGETMAP_POINTS;
		}
		else
		{
			map->map_mode = WIDGETMAP_NORMAL;
		}


		/*
		if ( gdk_pointer_is_grabbed () )
		{
#if GTK_MAJOR_VERSION == 2
			gdk_pointer_ungrab( map->grab_time );
#else
			gdk_device_ungrab( gtk_get_current_event_device(), map->grab_time );
#endif
			gdk_window_set_cursor( gtk_widget_get_window(widget), NULL );
			gtk_grab_remove(widget);
		}
		*/
		map->selection_region.mode = MAP_NONE;
		map->selection_region.active = FALSE;
		gtk_widget_queue_draw( widget );
		return FALSE;
	}
	gboolean shift_pressed = (event->state & GDK_SHIFT_MASK ? TRUE : FALSE);

	if ( !map->selected )
		return FALSE;
	if ( event->keyval == GDK_KEY_space )
	{
		Alchera_DisplayObject_Rotate( map->info, map->selected, gtk_alchera_map_toplevel_window(widget) );
	}
	else if ( event->keyval == GDK_KEY_f )
	{
		Alchera_DisplayObject_Flip( map->info, map->selected, gtk_alchera_map_toplevel_window(widget) );
	}
	else if ( event->keyval == GDK_KEY_Up )
	{
		gtk_alchera_map_undo_push( map, map->selected );
		if ( shift_pressed )
		{
			gdouble height = (gdouble)map->selected->th;
			map->selected->h -= height;
			if (map->selected->h <= height)
			{
				map->selected->tiled_image = FALSE;
				map->selected->h = height;
			}
			else
				map->selected->tiled_image = TRUE;
		}
		else
			map->selected->y -= Alchera_DisplayObject_StandardMovement(event->state, map->align );
	}
	else if ( event->keyval == GDK_KEY_Down )
	{
		gtk_alchera_map_undo_push( map, map->selected );
		if ( shift_pressed )
		{
			gdouble height = (gdouble)map->selected->th;
			map->selected->h += height;
			if (map->selected->h <= height)
			{
				map->selected->tiled_image = FALSE;
				map->selected->h = height;
			}
			else
				map->selected->tiled_image = TRUE;
		}
		else
			map->selected->y += Alchera_DisplayObject_StandardMovement(event->state, map->align );
	}
	else if ( event->keyval == GDK_KEY_Left )
	{
		gtk_alchera_map_undo_push( map, map->selected );
		if ( shift_pressed )
		{
			gdouble width = (gdouble)map->selected->tw;
			map->selected->w -= width;
			if (map->selected->w <= width)
			{
				map->selected->tiled_image = FALSE;
				map->selected->w = width;
			}
			else
				map->selected->tiled_image = TRUE;
		}
		else
			map->selected->x -= Alchera_DisplayObject_StandardMovement(event->state, map->align );
	}
	else if ( event->keyval == GDK_KEY_Right )
	{
		gtk_alchera_map_undo_push( map, map->selected );
		if (shift_pressed)
		{
			gdouble width = (gdouble)map->selected->tw;
			map->selected->w += width;
			if (map->selected->w <= width)
			{
				map->selected->tiled_image = FALSE;
				map->selected->w = width;
			}
			else
				map->selected->tiled_image = TRUE;
		}
		else
			map->selected->x += Alchera_DisplayObject_StandardMovement(event->state, map->align );
	}
	else if ( event->keyval == GDK_KEY_Delete )
	{
		if ( Alchera_DisplayObject_Remove( map->info, map->selected, gtk_alchera_map_toplevel_window(widget) ) )
		{
			map->map_mode = WIDGETMAP_NORMAL;
			map->selection_region.mode = MAP_NONE;
			map->selection_region.active = FALSE;
		}
	}
	else if ( event->keyval == GDK_KEY_Undo || (event->keyval == GDK_KEY_z && event->state & GDK_CONTROL_MASK) )
	{
		gtk_alchera_map_undo( map );
	}

	gtk_widget_queue_draw( widget );
	return TRUE;
}

static gboolean gtk_alchera_map_key_press( GtkWidget * widget, GdkEventKey *event )
{
	AlcheraMap * map = GTK_ALCHERA_MAP(widget);

	return !!( map->selected );

}

/* Drag and Drop Event */
void gtk_alchera_map_drop_received( GtkWidget * widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint type, guint time )
{
	g_return_if_fail(widget != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(widget));

	AlcheraMap * map = GTK_ALCHERA_MAP(widget);

	gchar * ident = (gchar *)gtk_selection_data_get_text(data);
	gdouble event_x = x / map->scale;
	gdouble event_y = y / map->scale;


	DisplayObject * obj = Alchera_DisplayObject_Add( map->info, ident, event_x, event_y, -1.0, -1.0, gtk_alchera_map_get_layer(map) );
	if ( obj )
	{
		map->info->display_list = g_list_insert_sorted_with_data( map->info->display_list, (gpointer)obj, (GCompareDataFunc)Alchera_DisplayObject_Order, NULL);

		/* Remove Active Flag from prevuios selected object */
		if ( map->selected )
		{
			map->selected->active = FALSE;
		}

		map->map_mode = WIDGETMAP_NORMAL;
		map->selected = obj;
		if ( map->selection_region.ident )
		{
			g_free(map->selection_region.ident);
			map->selection_region.ident = NULL;
		}
		gtk_alchera_map_clear_selection_region( widget, map );
	}

	gtk_drag_finish(context, TRUE, FALSE, time);
}

gboolean gtk_alchera_map_drag_drop( GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time_)
{
	//Don't think this need at all?
	/*
	gboolean is_valid_drop_site = TRUE;
	GdkAtom target_type;
	if ( gdk_drag_context_list_targets(context) )
	{
		target_type = GDK_POINTER_TO_ATOM( g_list_nth_data( gdk_drag_context_list_targets(context), 0) );
		gtk_drag_get_data( widget, context, target_type, time_ );
		is_valid_drop_site = TRUE;
	}
	*/
	return  TRUE;
}

gboolean gtk_alchera_map_drag_failed (GtkWidget *widget, GdkDragContext *context, GtkDragResult result)
{
	return TRUE;
}


/* Widget Function */

/********************************
* gtk_alchera_map_undo_push
*
*/
void gtk_alchera_map_undo_push( AlcheraMap * wid, DisplayObject * moved )
{
	g_return_if_fail(wid != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(wid));
	g_return_if_fail(moved != NULL);
	g_return_if_fail(moved->type != DT_DELETE);

	UndoMapObject * q = g_new0(UndoMapObject, 1);
	q->x = moved->x;
	q->y = moved->y;
	q->w = moved->w;
	q->h = moved->h;
	q->id = moved->id;
	q->obj = moved;

	wid->undo = g_slist_prepend( wid->undo, q );
}

/********************************
* gtk_alchera_map_undo
*
*/
void gtk_alchera_map_undo( AlcheraMap * wid )
{
	g_return_if_fail(wid != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(wid));
	if (wid->undo == NULL)
		return;

	UndoMapObject * q = (UndoMapObject *)wid->undo->data;

	GList * list_scan = g_list_last(wid->info->display_list);
	while( list_scan )
	{
		DisplayObject * object = (DisplayObject *)list_scan->data;
		if ( object->id == q->id )
		{
			object->x = q->x;
			object->y = q->y;
			object->w = q->w;
			object->h = q->h;
			if ( object->path )
			{
				DisplayObject * objn = g_slist_nth_data( object->path, 0);
				if (objn)
				{
					objn->x = q->x;
					objn->y = q->y;
				}
			}
			list_scan = NULL;
		}
		else
		{
			list_scan = g_list_previous(list_scan);
		}
	}

	wid->undo = g_slist_remove( wid->undo, q );
}

/********************************
* gtk_alchera_map_set_scale
*
*/
void gtk_alchera_map_set_scale( AlcheraMap * wid, gdouble scale )
{
	g_return_if_fail(wid->info != NULL);

	wid->scale = scale;
	gtk_alchera_map_refresh(wid);
}

/********************************
* gtk_alchera_map_set_selected_object
*
*/
void gtk_alchera_map_set_selected_object( AlcheraMap * wid, DisplayObject * obj )
{
	wid->selected = obj;
}

/********************************
* gtk_alchera_map_set_support_widget
*
*/
void gtk_alchera_map_set_support_widget(AlcheraMap * wid, GtkListStore * store, GtkStatusbar * status , GtkWidget * object_menu)
{
	wid->objects_list = store;
	wid->status_widget = status;
	wid->menu_widget = object_menu;

}

/********************************
* gtk_alchera_map_change_scale
*
*/
void gtk_alchera_map_change_scale( AlcheraMap * wid, gdouble value )
{
	wid->scale += value;
	gtk_alchera_map_set_scale( wid, wid->scale );
}

/********************************
* gtk_alchera_map_set_align
*
*/
void gtk_alchera_map_set_align( AlcheraMap * wid, gboolean able )
{
	wid->align = able;

}

/********************************
* gtk_alchera_map_refresh
*
*/
void gtk_alchera_map_refresh( AlcheraMap * wid )
{
	g_return_if_fail(wid != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(wid));
	g_return_if_fail(wid->info != NULL);

	GtkWidget * widget  = GTK_WIDGET(wid);

	wid->info->display_list = g_list_sort_with_data(wid->info->display_list, (GCompareDataFunc)Alchera_DisplayObject_Order, NULL);
	wid->scale_width = ((gdouble)wid->info->width * wid->scale);
	wid->scale_height = ((gdouble)wid->info->height * wid->scale);

	gtk_widget_set_size_request( widget, (gint)wid->scale_width, (gint)wid->scale_height);

	#if GTK_MAJOR_VERSION > 2
	if ( gtk_widget_is_drawable( widget ) )
	{
		cairo_region_t * region = gdk_window_get_clip_region( gtk_widget_get_window(widget) );
		gdk_window_invalidate_region( gtk_widget_get_window(widget), region, TRUE );
		gdk_window_process_updates( gtk_widget_get_window(widget), TRUE );
		cairo_region_destroy( region );
	}
	#else
	if ( GDK_IS_DRAWABLE(gtk_widget_get_window(widget)) )
	{
		GdkRegion * region = gdk_drawable_get_clip_region( gtk_widget_get_window(widget) );
		gdk_window_invalidate_region( gtk_widget_get_window(widget), region, TRUE );
		gdk_window_process_updates( gtk_widget_get_window(widget), TRUE );
		gdk_region_destroy( region );
	}
	#endif
}

/********************************
* gtk_alchera_map_get_list
*
*/
GList * gtk_alchera_map_get_list( AlcheraMap * wid )
{
	g_return_val_if_fail(wid != NULL, NULL);
	g_return_val_if_fail(GTK_IS_ALCHERA_MAP(wid), NULL);
	g_return_val_if_fail(wid->info != NULL, NULL);
	return wid->info->display_list;
}

/********************************
* gtk_alchera_map_get_visibility
*
*/
gboolean gtk_alchera_map_get_visibility(  AlcheraMap * wid, guint8 value )
{
	g_return_val_if_fail(wid != NULL, TRUE);
	g_return_val_if_fail(GTK_IS_ALCHERA_MAP(wid), TRUE);

	if ( value < 30 )
	{
		guint32 layer = (1 << value);
		return !!(wid->visible&layer);
	}
	return TRUE;
}

/********************************
* gtk_alchera_map_get_visibility
*
*/
void gtk_alchera_map_set_visibility(  AlcheraMap * wid, guint8 layer, gboolean value )
{
	g_return_if_fail(wid != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(wid));

	if ( layer < 30 )
	{
		if ( value )
			wid->visible |= (1 << layer);
		else
			wid->visible &= ~(1 << layer);
	}
	gtk_alchera_map_refresh( wid );
}

/********************************
* gtk_alchera_map_get_layer
*
*/
guint8 gtk_alchera_map_get_layer(  AlcheraMap * wid )
{
	g_return_val_if_fail( wid != NULL, 0 );
	g_return_val_if_fail( GTK_IS_ALCHERA_MAP(wid), 0 );
	return wid->current_layer;
}

/********************************
* gtk_alchera_map_set_layer
*
*/
void gtk_alchera_map_set_layer( AlcheraMap * wid, guint8 value )
{
	g_return_if_fail(wid != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(wid));
	wid->current_layer = value;
}

/********************************
* gtk_alchera_map_get_selected
*
*/
DisplayObject * gtk_alchera_map_get_selected( AlcheraMap * wid )
{
	g_return_val_if_fail( wid != NULL, NULL );
	g_return_val_if_fail( GTK_IS_ALCHERA_MAP(wid), NULL );
	if ( wid->selected )
		if ( wid->selected->type == DT_DELETE )
			return NULL;
	return wid->selected;
}

/********************************
* gtk_alchera_map_get_info
*
*/
MapInfo * gtk_alchera_map_get_info( AlcheraMap * wid )
{
	g_return_val_if_fail( wid != NULL, NULL );
	g_return_val_if_fail( GTK_IS_ALCHERA_MAP(wid), NULL );
	return wid->info;
}

/********************************
* gtk_alchera_map_set_new_object
* Create a new obkect to be added to map via drop or click
-
-

*/
gboolean gtk_alchera_map_set_new_object( AlcheraMap * wid, gchar * ident, gint width, gint height, GdkPixbuf * object )
{
	g_return_val_if_fail( wid != NULL, TRUE );
	g_return_val_if_fail( GTK_IS_ALCHERA_MAP(wid), TRUE );

	guint max_width, max_height;
	GdkCursor * cursor = NULL;
	GdkPixbuf * cursor_pixbuf;

	gdk_display_get_maximal_cursor_size( gdk_display_get_default(), &max_width, &max_height );

	if ( object )
	{
		cursor_pixbuf = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, max_width, max_height );
		gdk_pixbuf_fill( cursor_pixbuf, 0x00000000 );

		if ( gdk_display_supports_cursor_alpha( gdk_display_get_default() ) )
		{
			max_width = (gdk_pixbuf_get_width(object) < (max_width-3) ? gdk_pixbuf_get_width(object) : (max_width-3) );
			max_height = (gdk_pixbuf_get_height(object) < (max_height-3) ? gdk_pixbuf_get_height(object) : (max_height-3) );
			gdk_pixbuf_copy_area( object, 0, 0, max_width, max_height, cursor_pixbuf, 3, 3 );
		}
		gdk_pixbuf_copy_area( alchera_map_cursor, 0, 0, 16, 16, cursor_pixbuf, 0, 0 );
		cursor = gdk_cursor_new_from_pixbuf( gdk_display_get_default(), cursor_pixbuf, 0, 0 );
	}
	else
	{
		cursor = gdk_cursor_new_from_pixbuf(gdk_display_get_default(), alchera_map_cursor,0,0);
	}

	gdk_window_set_cursor( gtk_widget_get_window(GTK_WIDGET(wid)), cursor);

	gtk_grab_add( gtk_widget_get_toplevel(GTK_WIDGET(wid)) );
	gtk_widget_grab_focus( GTK_WIDGET(wid) );


	if ( cursor )
	{
		#if GTK_MAJOR_VERSION > 2
		g_object_unref(cursor);
		#else
		gdk_cursor_unref(cursor);
		#endif
	}

	/* Set Drag Infomation */
	wid->selection_region.id = 0;
	wid->selection_region.mode = MAP_PLACEOBJECT;
	wid->selection_region.sw = width;
	wid->selection_region.w = (gdouble)width;
	wid->selection_region.h = (gdouble)height;
	wid->selection_region.sh = height;
	wid->selection_region.x = wid->selection_region.y = 0;
	wid->selection_region.active = FALSE;
	wid->selection_region.rot = FALSE;
	wid->selection_region.ident = g_strdup( ident );

	return TRUE;
}

/********************************
* gtk_alchera_map_get_menu_widget
*
*/
GtkWidget * gtk_alchera_map_get_menu_widget( AlcheraMap * wid )
{
	g_return_val_if_fail( wid != NULL, NULL );
	g_return_val_if_fail( GTK_IS_ALCHERA_MAP(wid), NULL );

	GtkWidget * widget = gtk_alchera_map_object_submenu( wid, wid->selected );

	return widget;

}


/********************************
* gtk_alchera_map_destroy
*
*/
#if GTK_MAJOR_VERSION > 2
void gtk_alchera_map_destroy( GtkWidget * object )
{
	AlcheraMap * wid;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(object));

	wid = GTK_ALCHERA_MAP(object);

	if ( wid->info )
	{
		if ( wid->info->display_list )
			g_list_free( wid->info->display_list );
		g_free( wid->info->name );
		wid->info = NULL;
	}
	if ( wid->menu_widget )
	{
		gtk_widget_destroy(wid->menu_widget);
	}
}
#else
void gtk_alchera_map_destroy( GtkObject * object )
{
	AlcheraMap * wid;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_ALCHERA_MAP(object));

	wid = GTK_ALCHERA_MAP(object);

	if ( wid->info )
	{
		if ( wid->info->display_list )
			g_list_free( wid->info->display_list );
		g_free( wid->info->name );
		wid->info = NULL;
	}
	if ( wid->menu_widget )
	{
		gtk_widget_destroy(wid->menu_widget);
	}
}
#endif



/********************************
* gtk_alchera_map_init
*
*/
static void gtk_alchera_map_init( AlcheraMap * wid )
{
	memset( &wid->selection_region, 0, sizeof(MapSelection) );

	#if GTK_MINOR_VERSION > 17 || GTK_MAJOR_VERSION > 2
	gtk_widget_set_can_focus( GTK_WIDGET(wid), TRUE );
	#else
	GTK_WIDGET_SET_FLAGS( GTK_WIDGET(wid), GTK_CAN_FOCUS );
	#endif

	gtk_widget_add_events( GTK_WIDGET(wid), (GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK| GDK_KEY_RELEASE_MASK ) );
	wid->grid_value = g_key_file_get_integer( meg_pref_storage, "numeric", "gridvalue", NULL );

	gtk_drag_dest_set( GTK_WIDGET(wid), GTK_DEST_DEFAULT_ALL, &alchera_map_drop_target,1, GDK_ACTION_COPY );


	/* Create Cursor */
	if ( alchera_map_cursor == NULL )
	{
		alchera_map_cursor = gdk_pixbuf_add_alpha( gdk_pixbuf_new_from_inline( -1, dnd_cursor_copy, FALSE, NULL ), FALSE, 0, 0, 0 );
	}

}

/********************************
* gtk_alchera_map_new
*
*/
GtkWidget * gtk_alchera_map_new( MapInfo * data )
{
	g_return_val_if_fail(data != NULL, NULL);

	AlcheraMap * wid = g_object_new( gtk_alchera_map_get_type(), NULL );
	wid->info = data;
	wid->scale = 1.0;
	wid->visible = 0xFFFFFFFF;
	wid->align = FALSE;
	wid->undo = NULL;
	wid->path = NULL;
	wid->status_widget = NULL;

	return GTK_WIDGET(wid);
}

/* Widget Creation */
static void gtk_alchera_map_class_init( AlcheraMapClass * klass )
{
	GtkWidgetClass * widget_class = GTK_WIDGET_CLASS(klass);

	/* GtkWidget signals */
	widget_class->button_press_event = gtk_alchera_map_button_press;
	widget_class->button_release_event = gtk_alchera_map_button_release;
	widget_class->key_release_event = gtk_alchera_map_key_release;
	widget_class->key_press_event = gtk_alchera_map_key_press;
	/*widget_class->grab_broken_event = gtk_alchera_map_grab_break;*/

	widget_class->motion_notify_event = gtk_alchera_map_motion_notify;

	#if GTK_MAJOR_VERSION == 2
	GtkObjectClass * object_class = GTK_OBJECT_CLASS(klass);
	widget_class->expose_event = gtk_alchera_map_expose;
	object_class->destroy = gtk_alchera_map_destroy;
	#else
	widget_class->draw = gtk_alchera_map_draw;
	widget_class->destroy = gtk_alchera_map_destroy;
	#endif

	widget_class->drag_data_received = gtk_alchera_map_drop_received;
	widget_class->drag_drop = gtk_alchera_map_drag_drop;

}

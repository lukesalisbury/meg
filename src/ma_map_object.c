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
#include <gdk/gdk.h>
#if (GTK_MAJOR_VERSION == 2)
#include "gdkkeysyms-backward.h"
#endif

/* Required Headers */
#include "ma_map.h"

/* Global Variables */
extern GtkListStore * mapFileStore;
extern GtkListStore * sectionFileStore;

/* Local Variables */

/* Functions */




/********************************
* Event:
* Result:
*/
gboolean Meg_MapObject_PushAnimation( DisplayObject * object )
{
	if ( object->data )
	{
		if ( gdk_pixbuf_animation_iter_advance((GdkPixbufAnimationIter *)object->data, NULL) )
		{
			/*
			if ( GTK_WIDGET_VISIBLE(widget_map_display) && GDK_IS_DRAWABLE(widget_map_display->window) )
			{
				gtk_widget_queue_draw(widget_map_display);
			}
			*/
				/*
	if ( GTK_WIDGET_VISIBLE(widget) && GDK_IS_DRAWABLE(widget->window) )
	{
		cairo_t * cr;

		cr = gdk_cairo_create( widget->window );

		if ( !cr )
		{
			Meg_Main_PrintStatus("gdk_cairo_create failed ");
			return TRUE;
		}

		cairo_scale( cr, mapScale, mapScale );
		cairo_set_source_rgba( cr, 1, 1, 1, 1 );

		if ( mapdisplay_list )
		{
			g_list_foreach(mapdisplay_list, (GFunc)Alchera_Map_AnimationIdle, cr);
		}
		cairo_destroy( cr );
	}
	*/
		}
	}
	else
	{
		object->timeout = FALSE;
	}
	return object->timeout;
}


/* Map Object Events */


/********************************
* Meg_MapObject_Advance
* Event:
* Result:
*/
void Meg_MapObject_Advance( GtkAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_Advance( gtk_alchera_map_get_info( map ), obj, window ) )
	{
		gtk_alchera_map_refresh( map );
	}
}

/********************************
* Meg_MapObject_LayerUp
* Event:
* Result:
*/
void Meg_MapObject_LayerUp( GtkAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_LayerUp( gtk_alchera_map_get_info( map ), obj, window ) )
	{
		gtk_alchera_map_refresh( map );
	}
}

/********************************
* Meg_MapObject_LayerDown
* Event:
* Result:
*/

void Meg_MapObject_LayerDown( GtkAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_LayerDown( gtk_alchera_map_get_info( map ), obj, window ) )
	{
		gtk_alchera_map_refresh( map );
	}
}



/********************************
* Meg_MapObject_Rotate
* Event:
* Result:
*/

void Meg_MapObject_Rotate( GtkAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_Rotate( gtk_alchera_map_get_info( map ), obj , window ) )
	{
		gtk_alchera_map_refresh( map );
	}
}

/********************************
* Meg_MapObject_Flip
* Event:
* Result:
*/

void Meg_MapObject_Flip( GtkToggleAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_Flip( gtk_alchera_map_get_info( map ), obj, window ) )
	{
		gtk_alchera_map_refresh( map );
	}

	if ( obj )
		gtk_toggle_action_set_active( action, obj->flip );

}


/********************************
* Meg_MapObject_WidthDecrease
* Event:
* Result:
*/

void Meg_MapObject_WidthDecrease( GtkAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_KeyInput( gtk_alchera_map_get_info( map ), obj, GDK_KEY_Left, TRUE, window ) )
	{
		gtk_alchera_map_refresh( map );
	}
}

/********************************
* Meg_MapObject_WidthIncrease
* Event:
* Result:
*/

void Meg_MapObject_WidthIncrease( GtkAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_KeyInput( gtk_alchera_map_get_info( map ), obj, GDK_KEY_Right, TRUE, window ) )
	{
		gtk_alchera_map_refresh( map );
	}
}


/********************************
* Meg_MapObject_HeightDecrease
* Event:
* Result:
*/

void Meg_MapObject_HeightDecrease( GtkAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_KeyInput( gtk_alchera_map_get_info( map ), obj, GDK_KEY_Up, TRUE, window ) )
	{
		gtk_alchera_map_refresh( map );
	}
}

/********************************
* Meg_MapObject_HeightIncrease
* Event:
* Result:
*/

void Meg_MapObject_HeightIncrease( GtkAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_KeyInput( gtk_alchera_map_get_info( map ), obj, GDK_KEY_Down, TRUE, window ) )
	{
		gtk_alchera_map_refresh( map );
	}
}


/********************************
* Meg_MapObject_KeyUp
* Event:
* Result:
*/

void Meg_MapObject_KeyUp( GtkAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_KeyInput( gtk_alchera_map_get_info( map ), obj, GDK_KEY_Up, FALSE, window ) )
	{
		gtk_alchera_map_refresh( map );
	}
}

/********************************
* Meg_MapObject_KeyDown
* Event:
* Result:
*/

void Meg_MapObject_KeyDown( GtkAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_KeyInput( gtk_alchera_map_get_info( map ), obj, GDK_KEY_Down, FALSE, window ) )
	{
		gtk_alchera_map_refresh( map );
	}
}

/********************************
* Meg_MapObject_KeyDown
* Event:
* Result:
*/

void Meg_MapObject_KeyLeft( GtkAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_KeyInput( gtk_alchera_map_get_info( map ), obj, GDK_KEY_Left, FALSE, window ) )
	{
		gtk_alchera_map_refresh( map );
	}
}

/********************************
* Meg_MapObject_KeyRight
* Event:
* Result:
*/

void Meg_MapObject_KeyRight( GtkAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_KeyInput( gtk_alchera_map_get_info( map ), obj, GDK_KEY_Right, FALSE, window ) )
	{
		gtk_alchera_map_refresh( map );
	}
}


/********************************
* Meg_MapObject_Remove
* Event:
* Result:
*/

void Meg_MapObject_Remove( GtkAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_Remove( gtk_alchera_map_get_info( map ), obj, window ) )
	{
		gtk_alchera_map_refresh( map );
	}
}

/********************************
* Meg_MapObject_Attach
* Event:
* Result:
*/

void Meg_MapObject_Attach( GtkAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_Attach( gtk_alchera_map_get_info( map ), obj, window ) )
	{
		gtk_alchera_map_refresh( map );
	}
}

/********************************
* Meg_MapObject_Path
* Event:
* Result:
*/

void Meg_MapObject_Path( GtkAction * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	GtkWindow * window = Meg_Misc_ParentWindow( GTK_WIDGET(map) );

	if ( Alchera_DisplayObject_Path( gtk_alchera_map_get_info( map ), obj, window ) )
	{
		gtk_alchera_map_refresh( map );
	}

}


/********************************
* Meg_MapObject_PathModify
* Event:
* Result:
*/

void Meg_MapObject_PathModify( GtkAction * action, AlcheraMap * map )
{
	gtk_alchera_map_edit_path( map );
	gtk_alchera_map_refresh( map );

}




/********************************
* Meg_MapObject_EditPathCall
* Event:
* Result:
*/
void Meg_MapObject_EditPathCall( GtkMenuItem * action, AlcheraMap * map )
{
	DisplayObject * obj = gtk_alchera_map_get_selected( map );
	if ( obj )
	{
		if ( obj->path )
		{
			if ( map->path == obj->path )
				map->path = NULL;
			else
				map->path = obj->path;
		}
		else
		{
			GtkWidget * delete_dialog = gtk_message_dialog_new( GTK_WINDOW(gtk_widget_get_toplevel( GTK_WIDGET(map) )),
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO,
				"Do wish to add a path to this object?"
			);
			gint result = gtk_dialog_run( GTK_DIALOG(delete_dialog) );
			switch (result)
			{
				case GTK_RESPONSE_YES:
				{
					DisplayObject * objn = g_new0(DisplayObject, 1);
					objn->x = obj->x;
					objn->y = obj->y;
					obj->path = g_slist_append(obj->path,obj);
					map->path = obj->path;
					map->selection_region.mode = 4;
				}
				break;
				default:
				break;
			}
			gtk_widget_destroy( delete_dialog );
		}
		gtk_widget_queue_draw( GTK_WIDGET(map) );
	}
}




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

/* Global Variables */
GdkRectangle sheetDragRect = {-1, -1, 0, 0};
guint32 last_click_time = 0;

GdkRGBA sheetBackColour;

gboolean sheetModeSelect = TRUE;
gboolean sheetModeAlign = TRUE;

static gdouble sheetItemActive[] = {25.0, 10.0, 25.0, 10.0 };
static gdouble sheetItemActiveColor[] = {1.0, 0.0, 0.0, 0.8 };
static gdouble sheetItemIdleColor[] = {0.5, 1.0, 0.0, 0.8 };
static gdouble sheetItemHoverColor[] = {0.5, 0.0, 1.0, 0.8 };

gdouble sheetScale = 1.0;
guint16 sheetGridValue = 16;
extern guint32 sheetAlignValue;
extern GtkTreeViewColumn * meg_sheet_deletecolumn;


/********************************
* PointCollide
*
*/
gboolean PointCollide( GdkRectangle a, gdouble x, gdouble y )
{
	gint intx = (gint)x;
	gint inty = (gint)y;
	if ( intx < a.x)
		return 0;
	if ( intx > (a.x + a.width))
		return 0;
	if ( inty < a.y)
		return 0;
	if ( inty > a.y + a.height)
		return 0;
	return 1;
}

/********************************
* RectCollide
*
*/
gboolean RectCollide( GdkRectangle a, GdkRectangle b )
{
	if ( b.x + b.width < a.x)
		return 0;
	if ( b.x > a.x + a.width)
		return 0;
	if ( b.y + b.height < a.y)
		return 0;
	if ( b.y > a.y + a.height)
		return 0;
	return 1;
}

/********************************
* meg_sheet_GridAlign
*
@ value:
- return rounded number.
*/
gint Meg_Spritesheet_GridAlign( gint value )
{
	if ( sheetModeAlign )
		return (value/sheetGridValue)*sheetGridValue;
	else
		return value;
}

/********************************
* Meg_Spritesheet_ButtonColourSet
*
*/
void Meg_Spritesheet_ButtonColourSet( GtkColorButton *widget, gpointer user_data )
{
	Meg_ColorButton_GetColor( widget, &sheetBackColour );
}

/********************************
* Meg_Spritesheet_ButtonColourSet
*
*/
void Meg_Spritesheet_DisplayScale( GtkWidget * display )
{
	gint width, height;
	Spritesheet * spritesheet = (Spritesheet*)g_object_get_data( G_OBJECT(display), "sheet-data" );


	if ( spritesheet )
	{
		width = gdk_pixbuf_get_width(spritesheet->image);
		height = gdk_pixbuf_get_height(spritesheet->image);

		width = (gint)((gdouble)width * sheetScale);
		height = (gint)((gdouble)height * sheetScale);
		gtk_widget_set_size_request( display, width, height );
		gtk_widget_queue_draw( display );
	}
}

/********************************
* Alchera_Sheet_ButtonZoomOut
*
*/
void Meg_Spritesheet_ButtonZoomOut( GtkButton * widget, gpointer user_data )
{
	GtkWidget * display = (GtkWidget*)user_data;

	sheetScale -= 0.25;

	Meg_Spritesheet_DisplayScale(display);
}

/********************************
* Alchera_Sheet_ButtonZoomNormal
*
*/
void Meg_Spritesheet_ButtonZoomNormal( GtkButton * widget, gpointer user_data )
{
	GtkWidget * display = (GtkWidget*)user_data;

	sheetScale = 1.0;

	Meg_Spritesheet_DisplayScale(display);

}

/********************************
* Alchera_Sheet_ButtonZoomIn
*
*/
void Meg_Spritesheet_ButtonZoomIn( GtkButton *widget, gpointer user_data )
{
	GtkWidget * display = (GtkWidget*)user_data;

	sheetScale += 0.25;

	Meg_Spritesheet_DisplayScale(display);
}

/********************************
* Event: Mode Buttons Toggled
* Result: Change Mouse Modes
*/
void Meg_Spritesheet_SetMode( GtkToggleButton * widget, gpointer user_data )
{
	GtkWidget * display = (GtkWidget*)user_data;
	sheetModeSelect = !gtk_toggle_button_get_active( widget );
	gtk_widget_queue_draw(display);
}

/********************************
* Event: Align Button Toggled
* Result: Ables Align mode
*/
void Meg_Spritesheet_SetAlign( GtkToggleButton * widget, gpointer user_data )
{
	GtkWidget * display = (GtkWidget*)user_data;
	sheetModeAlign = gtk_toggle_button_get_active( widget );
	sheetGridValue = g_key_file_get_integer( Meg_Preference_Storage(), "numeric", "gridvalue", NULL );
	sheetAlignValue = g_key_file_get_integer( Meg_Preference_Storage(), "numeric", "alignvalue", NULL );
	gtk_widget_queue_draw(display);
}

/********************************
* Event: Sheet Visible Button Toggled
* Result:
*/
void Meg_Spritesheet_SetVisible( GtkToggleButton * widget, gpointer user_data )
{
	GtkWidget * display = (GtkWidget*)user_data;
	Spritesheet * spritesheet = (Spritesheet*)g_object_get_data( G_OBJECT(display), "sheet-data" );
	if ( spritesheet )
	{
		spritesheet->visible = gtk_toggle_button_get_active( widget );
		//AL_Sheet_Save( spritesheet );
	}
}


/********************************
* Event: Map Overview Refresh
* Result:
*/
gboolean Meg_Spritesheet_ViewDraw( GtkWidget * window, cairo_t * cr, gpointer data )
{
	Spritesheet * spritesheet = (Spritesheet*)g_object_get_data( G_OBJECT(window), "sheet-data" );
    if ( !spritesheet || !spritesheet->image_loaded || !spritesheet->image)
		return FALSE;

	guint width, height;
	guint x = sheetGridValue, y = sheetGridValue;

	width = gdk_pixbuf_get_width( spritesheet->image );
	height = gdk_pixbuf_get_height( spritesheet->image );

	cairo_scale(cr, sheetScale, sheetScale);
	cairo_save( cr );
	gdk_cairo_set_source_rgba( cr, &sheetBackColour );
	cairo_rectangle( cr, 0, 0, width, height );
	cairo_fill( cr );
	cairo_restore( cr );

	if ( sheetModeAlign )
	{
		cairo_save(cr);
		cairo_set_source_rgba( cr, 0, 0, 0, 0.8 );
		cairo_set_line_width(cr, 1);
		while ( x < width )
		{
			cairo_move_to (cr, x, 0);
			cairo_line_to (cr, x, height);
			cairo_stroke (cr);
			x += sheetGridValue;
		}
		while ( y < height )
		{
			cairo_move_to( cr, 0, y );
			cairo_line_to( cr, width, y );
			cairo_stroke( cr );
			y += sheetGridValue;
		}
		cairo_restore( cr );
	}

	cairo_save( cr );
	cairo_rectangle( cr, 0, 0, width, height );
	cairo_pattern_set_filter( cairo_get_source(cr), CAIRO_FILTER_FAST );
	gdk_cairo_set_source_pixbuf( cr, spritesheet->image, 0, 0 );
	cairo_pattern_set_filter( cairo_get_source(cr), CAIRO_FILTER_FAST );
	cairo_fill( cr );
	cairo_restore( cr );

	if ( sheetModeSelect )
	{
		GSList * list_scan = spritesheet->children;
		while ( list_scan )
		{
			SheetObject * object = (SheetObject *)list_scan->data;
/*
			if ( RectCollide(object->position, event->area) )
			{
*/
				cairo_save (cr);
				cairo_set_dash (cr, sheetItemActive, 2, 0.0);
				cairo_rectangle(cr, object->position.x, object->position.y, object->position.width, object->position.height);
				if ( object == spritesheet->selected )
				{
					cairo_set_source_rgba(cr, sheetItemActiveColor[0], sheetItemActiveColor[1], sheetItemActiveColor[2], sheetItemActiveColor[3] - 0.4);
					cairo_fill_preserve (cr);
					cairo_set_source_rgba(cr, sheetItemActiveColor[0], sheetItemActiveColor[1], sheetItemActiveColor[2], sheetItemActiveColor[3]);
				}
				else
				{
					cairo_set_source_rgba(cr, sheetItemIdleColor[0], sheetItemIdleColor[1], sheetItemIdleColor[2], sheetItemIdleColor[3]);
				}
				cairo_stroke (cr);
				cairo_restore (cr);
/*			}*/
			list_scan = g_slist_next( list_scan );
		}
	}
	if ( sheetDragRect.x != -1 )
	{
		sheetDragRect.width = Meg_Spritesheet_GridAlign( sheetDragRect.width );
		sheetDragRect.height = Meg_Spritesheet_GridAlign( sheetDragRect.height );
		sheetDragRect.x = Meg_Spritesheet_GridAlign( sheetDragRect.x );
		sheetDragRect.y = Meg_Spritesheet_GridAlign( sheetDragRect.y );

		cairo_save( cr );
		cairo_rectangle( cr, sheetDragRect.x, sheetDragRect.y, sheetDragRect.width, sheetDragRect.height );
		cairo_set_source_rgba( cr, sheetItemHoverColor[0], sheetItemHoverColor[1], sheetItemHoverColor[2], sheetItemHoverColor[3] - 0.4 );
		cairo_fill_preserve( cr );
		cairo_set_source_rgba( cr, sheetItemHoverColor[0], sheetItemHoverColor[1], sheetItemHoverColor[2], sheetItemHoverColor[3] );
		cairo_stroke( cr );
		cairo_restore( cr );
	}
	return TRUE;
}

/********************************
* Event: Refreshing Whole Image
* Result: Draw grid, sprite outlines
*/
gboolean Meg_Spritesheet_ViewExpose( GtkWidget * widget, GdkEventExpose * event, gpointer data )
{
	cairo_t * cr = gdk_cairo_create( gtk_widget_get_window(widget) );

	if ( !cr )
	{
		Meg_Main_PrintStatus("gdk_cairo_create failed ");
		return FALSE;
	}

	return Meg_Spritesheet_ViewDraw( widget, cr, NULL );
}

/********************************
* Event: Mouse Click on Image
* Result:
*/
gboolean Meg_Spritesheet_ViewMouse(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	GtkWidget * display = (GtkWidget*)user_data;
	Spritesheet * spritesheet = (Spritesheet*)g_object_get_data( G_OBJECT(display), "sheet-data" );

	if ( !spritesheet ) // No Sheet Loaded
		return FALSE;


	gdouble x = (event->x / sheetScale);
	gdouble y = (event->y / sheetScale);

	if ( event->button == 1 )
	{
		if ( sheetModeSelect )
		{
			/* Selection Mode */

			if ( event->type == GDK_BUTTON_PRESS )
			{
				if ((last_click_time + 250) >= event->time)
				{
					return FALSE;
				}
				last_click_time = event->time;

				GSList * list_scan = spritesheet->children;
				while ( list_scan )
				{
					SheetObject * map_object = (SheetObject *)list_scan->data;
					if ( PointCollide(map_object->position, x, y) )
					{
						spritesheet->selected = map_object;
						Meg_Main_PrintStatus("Selected Sprite: %s", map_object->display_name);
						gtk_widget_queue_draw(widget);

						return TRUE;
					}
					else
					{
						list_scan = g_slist_next(list_scan);
					}
				}
			}
			else if ( event->type == GDK_2BUTTON_PRESS )
			{
				if ( spritesheet->selected )
				{
					AL_Sprite_Advance( spritesheet, spritesheet->selected->display_name );
					MegWidget_Spritesheet_SetFile(spritesheet);
					sheetDragRect.x = sheetDragRect.y = -1;
				}
			}
		}
		else
		{
			/* Creation Mode */
			if ( event->type == GDK_BUTTON_RELEASE )
			{
				Meg_Main_PrintStatus("Mouse released at X: %f, Y: %f", x, y);
				if ( sheetDragRect.width < 2 || sheetDragRect.height < 2 )
				{
					sheetDragRect.x = sheetDragRect.y = -1;
					return FALSE;
				}

				sheetDragRect.width = Meg_Spritesheet_GridAlign(sheetDragRect.width);
				sheetDragRect.height = Meg_Spritesheet_GridAlign(sheetDragRect.height);

				if ( sheetDragRect.width != 0 && sheetDragRect.height != 0 )
				{
					AL_Sprite_Add( spritesheet, &sheetDragRect);
					MegWidget_Spritesheet_SetFile(spritesheet);
				}

				sheetDragRect.x = sheetDragRect.y = -1;

				gtk_widget_queue_draw(widget);
			}
			else if ( event->type == GDK_BUTTON_PRESS )
			{
				Meg_Main_PrintStatus("Mouse pressed at X: %f, Y: %f", x, y);

				sheetDragRect.x = Meg_Spritesheet_GridAlign((gint)x);
				sheetDragRect.y = Meg_Spritesheet_GridAlign((gint)y);
			}
			sheetDragRect.width = sheetDragRect.height = 0;

		}
	}
	return FALSE;
}

/********************************
* Meg_Spritesheet_DrawMouse
* Event: Mouse over image
* Result: Draw Selection Rect
*/
gboolean Meg_Spritesheet_DrawMouse(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
	gdouble x = (event->x / sheetScale);
	gdouble y = (event->y / sheetScale);
	if (sheetDragRect.x != -1 )
	{
		sheetDragRect.width = (gint)x - sheetDragRect.x;
		sheetDragRect.height = (gint)y - sheetDragRect.y;
		gtk_widget_queue_draw(widget);
		return TRUE;
	}
	return FALSE;
}


/********************************
* Meg_Spritesheet_Chooser_Preview
* Event: File Selector Preview Widget
* Result:
*/
static void Meg_Spritesheet_Chooser_Preview( GtkFileChooser *file_chooser, gpointer data )
{
	GtkWidget * preview = GTK_WIDGET(data);
	gchar * filename;
	GdkPixbuf * pixbuf;
	gboolean have_preview = FALSE;

	filename = gtk_file_chooser_get_preview_filename( file_chooser );

	if (filename)
	{
		pixbuf = gdk_pixbuf_new_from_file_at_size( filename, 128, 128, NULL );
		have_preview = (pixbuf != NULL);
		gtk_image_set_from_pixbuf( GTK_IMAGE(preview), pixbuf );
		if ( pixbuf )
			g_object_unref( pixbuf );
		g_free( filename );
	}
	gtk_file_chooser_set_preview_widget_active( file_chooser, have_preview );
}

/********************************
* Event: Sprite list activated
* Result: Show Sprite Advance Info
*/
void Meg_Spritesheet_SpriteSelect( GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data )
{
	if ( !user_data )
		return;

	GtkWidget * display = (GtkWidget*)user_data;
	Spritesheet * spritesheet = (Spritesheet*)g_object_get_data( G_OBJECT(display), "sheet-data" );

	if ( !spritesheet )
		return;

	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection * select = gtk_tree_view_get_selection(tree_view);

	if ( gtk_tree_selection_get_selected(select, &model, &iter) )
	{
		gchar * text;
		gtk_tree_model_get (model, &iter, 0, &text, -1);
		if ( meg_sheet_deletecolumn == column )
		{
			GtkWidget * question = gtk_message_dialog_new( Meg_Main_GetWindow(), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Do you want to remove %s from %s", text, spritesheet->file);
			gint answer = gtk_dialog_run( GTK_DIALOG(question) );
			gtk_widget_destroy( question );

			if ( answer == GTK_RESPONSE_YES )
			{
				AL_Sprite_Remove( spritesheet, text );
			}
		}
		else
		{
			/* TODO
			Draw Selection on Image around sprite
			*/
			if ( AL_Sprite_Advance( spritesheet, text ) )
			{
				MegWidget_Spritesheet_RefreshSpriteList();
			}
		}
		MegWidget_Spritesheet_SetFile(spritesheet);

		g_free( text );
	}
}



\
/********************************
* Meg_Spritesheet_Changed
* Event: Sheet Combo Change
* Result: Loaded new sheet
*/
void Meg_Spritesheet_SheetChanged( GtkComboBox *widget, gpointer user_data )
{
	gchar * select_text = Meg_ComboText_GetText( widget );
	if ( select_text )
	{
		Spritesheet * spritesheet = AL_Sheet_Get( select_text );

		if ( spritesheet )
		{
			MegWidget_Spritesheet_SetFile( spritesheet );
		}
		else
		{
			Meg_Log_Print(LOG_WARNING, "Error loading spritesheet '%s'", select_text);
		}
		g_free (select_text);
	}
}

/********************************
* Meg_Spritesheet_Add
* Event: Add button is pressed
* Result: Brings up file selector, then runs
*/
void Meg_Spritesheet_SheetAdd( GtkButton * widget, gpointer user_data )
{
	gchar * filename = NULL;
	GtkWidget * dialog;
	GtkFileFilter * filter;
	GtkWidget * preview;

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name( filter, "Images" );
	gtk_file_filter_add_pixbuf_formats( filter );
	dialog = gtk_file_chooser_dialog_new( "Select Image File", Meg_Main_GetWindow(),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
		NULL);

	gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(dialog), filter );

	preview = gtk_image_new();
	gtk_file_chooser_set_preview_widget( GTK_FILE_CHOOSER(dialog), preview );
	g_signal_connect( dialog, "update-preview", G_CALLBACK(Meg_Spritesheet_Chooser_Preview), GTK_IMAGE(preview) );

	if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
	{
		filename = g_strdup( gtk_file_chooser_get_filename( GTK_FILE_CHOOSER (dialog) ) );
	}
	gtk_widget_destroy( dialog );

	if ( filename)
	{
		if ( AL_Sheet_Add( filename ) )
		{
			MegWidget_Spritesheet_SetFile( NULL );
			MegWidget_Spritesheet_SetSheetList( );
		}
		g_free( filename);
	}

}



/********************************
* Meg_Spritesheet_SheetRemove
* Event: remove button is pressed
* Result: removes selected sprite
@ widget
*/
void Meg_Spritesheet_SheetRemove( GtkButton *widget, GtkWidget *combo)
{
	gchar * parent_text = Meg_ComboText_GetText( GTK_COMBO_BOX(combo) );
	if ( parent_text )
	{
		GtkWidget * question = gtk_message_dialog_new( Meg_Main_GetWindow(), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Do you want to remove '%s'", parent_text);
		gint answer = gtk_dialog_run( GTK_DIALOG(question) );
		gtk_widget_destroy( question );
		if ( answer == GTK_RESPONSE_YES )
		{
			if ( AL_Sheet_Remove( parent_text ) )
			{
				MegWidget_Spritesheet_SetFile( NULL );
			}
			MegWidget_Spritesheet_SetSheetList();
		}
		g_free(parent_text);
	}
}

/********************************
* Meg_Spritesheet_Edit
* Event: remove button is pressed
* Result: removes selected sprite
@ widget
*/
void Meg_Spritesheet_SheetEdit( GtkButton * widget, GtkWidget * combo )
{
	gchar * parent_text = Meg_ComboText_GetText( GTK_COMBO_BOX(combo) );
	if ( parent_text )
	{
		gchar * file = AL_ResourcePath(parent_text, "sprites");
		Meg_RunProgram( "IMAGE_EDITOR", file );
		g_free( file );
	}
	g_free(parent_text);

}




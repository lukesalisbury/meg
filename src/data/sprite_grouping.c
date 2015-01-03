/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Standard Headers */
#include "loader_global.h"
#include "global.h"
/* External Functions */

/* Global Variables */

/* Local Variables */



/* UI */
const gchar * mokoiUI_SpriteGrouping = GUI_SPRITE_GROUPING;


/* Functions */

/********************************
* Meg_ComboText_AppendText
* Design to replace gtk_combo_box_append_text
*
*/
void SpriteGrouping_ComboAppend( GtkComboBox * widget, const gchar * text, GdkPixbuf * image )
{
	GtkTreeIter iter;
	GtkListStore *store = GTK_LIST_STORE( gtk_combo_box_get_model( widget ) );

	gtk_list_store_append( store, &iter );
	gtk_list_store_set( store, &iter, 0, text, 1, image, -1 );
}

/********************************
* SpriteGrouping_ShowDialog
*
*/
gboolean SpriteGrouping_ShowDialog( GtkWidget * parent_widget, SpriteChild * sprite_child )
{
	GtkWidget * dialog, * combo_sprite, * combo_repeat, * label_parentsprite;
	Spritesheet * sheet = NULL;

	sheet = (Spritesheet*)g_object_get_data( G_OBJECT(parent_widget), "Spritesheet" );

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(mokoiUI_SpriteGrouping, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );

	/* Widget */
	dialog = GET_WIDGET( ui, "dialog");
	combo_sprite = GET_WIDGET( ui, "combo_sprite" );
	combo_repeat = GET_WIDGET( ui, "combo_repeat" );
	label_parentsprite = GET_WIDGET( ui, "label_parentsprite" );

	/* Sprite Listing */
	SpriteGrouping_ComboAppend( GTK_COMBO_BOX(combo_sprite), MEG_COMBOFILE_NONE, NULL );

	if ( sheet )
	{
		GSList * scan = sheet->children;
		while ( scan )
		{
			SheetObject * sprite_object =  (SheetObject *)scan->data;
			SpriteGrouping_ComboAppend( GTK_COMBO_BOX(combo_sprite), sprite_object->display_name, SPRITE_DATA(sprite_object)->image );
			scan = g_slist_next( scan );
		}
	}

	/* Set Current Values */
	Meg_Misc_SetLabel_Print( label_parentsprite, "Sprite: %s ", "_" );
	Meg_ComboText_SetIndex( GTK_COMBO_BOX(combo_sprite), sprite_child->name );
	gtk_combo_box_set_active( GTK_COMBO_BOX(combo_repeat), sprite_child->repeat );
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Misc_ParentWindow(parent_widget) );

	if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == -1 )
	{
		/* Get Selected Sprite */
		gchar * selected_sprite = Meg_ComboText_GetText( GTK_COMBO_BOX(combo_sprite) );

		g_free(sprite_child->name); // Clear old one
		sprite_child->name = NULL;

		if ( selected_sprite )
		{
			sprite_child->name = g_strdup(selected_sprite);

			GdkPixbuf * sprite = Sheet_GetPixbuf( sprite_child->name, sheet);
			g_object_set_data( G_OBJECT(parent_widget), "Image", sprite );
		}
		else
		{
			g_object_set_data( G_OBJECT(parent_widget), "Image", NULL );
		}
		/* Repeat mode */
		sprite_child->repeat = gtk_combo_box_get_active( GTK_COMBO_BOX(combo_repeat) );
	}
	gtk_widget_destroy( dialog );
	gtk_widget_queue_draw( parent_widget );
	return TRUE;
}

/********************************
* SpriteGrouping_ButtonPressed
* Event:
* Result:
*/
gboolean SpriteGrouping_ButtonPressed( GtkWidget* widget, GdkEventButton * event, SpriteChild * sprite_child )
{
	if (event->type == GDK_BUTTON_PRESS)
	{
		SpriteGrouping_ShowDialog( widget, sprite_child );
	}
	return TRUE;
}


/********************************
* SpriteGrouping_ViewDraw
* Event:
* Result:
*/
gboolean SpriteGrouping_ViewDraw( GtkWidget * window, cairo_t * cr, SheetObject * sprite )
{
	gint width, height;

	Spritesheet * sheet = (Spritesheet*)g_object_get_data( G_OBJECT(window), "Spritesheet" );
	GdkPixbuf * selected_sprite = (GdkPixbuf*)g_object_get_data( G_OBJECT(window), "Image" );
	g_return_val_if_fail( sheet, FALSE );

	cairo_save( cr );
	if ( selected_sprite )
	{
		width = gdk_pixbuf_get_width( selected_sprite );
		height = gdk_pixbuf_get_height( selected_sprite );

		gtk_widget_set_size_request( window, width, height );
		cairo_rectangle( cr, 0, 0, width, height );
		gdk_cairo_set_source_pixbuf( cr, selected_sprite, 0, 0 );
		cairo_pattern_set_filter( cairo_get_source(cr), CAIRO_FILTER_FAST );
		cairo_fill( cr );
	}
	else
	{
		width = gdk_pixbuf_get_width( SPRITE_DATA(sprite)->image );
		height = gdk_pixbuf_get_height( SPRITE_DATA(sprite)->image );

		gtk_widget_set_size_request( window, width, height );
		cairo_rectangle( cr, 0, 0, width, height );
		cairo_fill( cr );
	}
	cairo_restore( cr );

	return TRUE;
}

/********************************
* Event: Refreshing Whole Image
* Result: Draw grid, sprite outlines
*/
gboolean SpriteGrouping_ViewExpose( GtkWidget * widget, GdkEventExpose * event, SheetObject * sprite )
{
	cairo_t * cr = gdk_cairo_create( gtk_widget_get_window(widget) );

	if ( !cr )
	{
		Meg_Main_PrintStatus("gdk_cairo_create failed ");
		return FALSE;
	}

	return SpriteGrouping_ViewDraw( widget, cr, sprite );
}

/********************************
* Sprite_AdvanceDialog
*
*/
GtkWidget * SpriteGrouping_GetWidget(GtkBuilder * ui, gchar * name, Spritesheet *sheet, SheetObject *sprite , gint position)
{
	GtkWidget * widget = GET_WIDGET( ui, name);
	g_object_set_data( G_OBJECT(widget), "Spritesheet", sheet );
	g_object_set_data( G_OBJECT(widget), "Position", GINT_TO_POINTER(position) );
	g_signal_connect( G_OBJECT(widget), "button-press-event", G_CALLBACK(SpriteGrouping_ButtonPressed), &SPRITE_DATA(sprite)->childrens[position] );
#if GTK_MAJOR_VERSION == 2
	g_signal_connect( G_OBJECT(widget), "expose-event", G_CALLBACK(SpriteGrouping_ViewExpose), sprite );
#else
	g_signal_connect( G_OBJECT(widget), "draw", G_CALLBACK(SpriteGrouping_ViewDraw), sprite );
#endif

	SPRITE_DATA(sprite)->childrens[position].position = position;

	GdkPixbuf * image = NULL;
	if ( SPRITE_DATA(sprite)->childrens[position].name )
	{
		image = Sheet_GetPixbuf( SPRITE_DATA(sprite)->childrens[position].name, sheet );
	}
	g_object_set_data( G_OBJECT(widget), "Image", image );
	gtk_widget_queue_draw( widget );

	return widget;
}

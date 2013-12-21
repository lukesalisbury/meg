/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Standard Headers */
#include "loader_global.h"

/* Required Headers */
#include "animation.h"
#include "animation_functions.h"

/* External Functions */

/* Local Type */


/* Global Variables */
extern GSList * mokoiSpritesheets;

/* Local Variables */

/* UI */

/* Events */

/********************************
* Animation_ChangePosition
*
*/
void Animation_ChangePosition( GtkCellRendererSpin *renderer, gchar * path, gchar * new_text, GtkListStore * store )
{
	GtkTreeIter iter;
	gtk_tree_model_get_iter_from_string( GTK_TREE_MODEL(store), &iter, path );
	gint column_number = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(renderer), "my_column_num"));
	gint column_value = g_ascii_strtoll(new_text, NULL, 10);
	gtk_list_store_set( store, &iter, column_number, column_value, -1 );
}

/********************************
* Animation_Preview_Get_Frame
*
*/
AnimationPreviewFrame * Animation_Preview_Get_Frame( AnimationPreview * anim, guint ms )
{
	AnimationPreviewFrame * current_frame = (AnimationPreviewFrame *)g_list_nth_data(anim->frames, anim->current);

	anim->timer += ms;

	if ( !current_frame )
		return NULL;

	if ( anim->timer > current_frame->length_ms  )
	{
		anim->current++;
		anim->timer -= current_frame->length_ms;
	}

	return (AnimationPreviewFrame *)g_list_nth_data(anim->frames, anim->current);

}

/********************************
* Animation_Preview_Timer
*
*/
gboolean Animation_Preview_Timer( AnimationPreview * anim )
{
	if ( anim->widget )
	{
		AnimationPreviewFrame * frame = Animation_Preview_Get_Frame( anim, 10 );

		if ( frame )
		{
			g_object_set_data( G_OBJECT(anim->widget), "AnimationFrame", frame );
			gtk_widget_queue_draw( anim->widget );
			return TRUE;
		}
	}
	anim->id = 0;

	return FALSE;
}

/********************************
* Animation_Preview_Play
*
*/
void Animation_Preview_Play( GtkButton * button, SheetObject * sprite )
{
	g_return_if_fail( button != NULL );
	g_return_if_fail( sprite != NULL );

	AnimationPreview * anim_data = (AnimationPreview *)g_object_get_data( G_OBJECT(button), "AnimationPreview" );

	g_return_if_fail( anim_data != NULL );

	if ( anim_data->id == 0 )
	{
		SpriteAnimation_Create( sprite, anim_data );

		anim_data->current = 0;
		anim_data->timer = 0;
		anim_data->id = g_timeout_add( 10, (GSourceFunc) Animation_Preview_Timer, anim_data );
	}
}

/********************************
* Animation_Preview_Pressed
*
*/
gboolean Animation_Preview_Pressed( GtkWidget * button, GdkEventButton * event, SheetObject * sprite )
{
	if ( event->type == GDK_BUTTON_PRESS && event->button == 1 )
	{
		Animation_Preview_Play( GTK_BUTTON(button), sprite);
	}
	return FALSE;
}

/********************************
* Animation_Preview_Draw
*
*/
gboolean Animation_Preview_Draw(GtkWidget * widget, cairo_t * cr, AnimationPreview *preview)
{
	guint center_x = 0, center_y = 0;
	GtkAllocation dim;
	AnimationPreviewFrame * anim_frame = NULL;

	gtk_widget_get_allocation( widget, &dim );

	anim_frame = (AnimationPreviewFrame *)g_object_get_data( G_OBJECT(widget), "AnimationFrame" );

	center_x = (dim.width / 2);
	center_y = (dim.height / 2);


	cairo_save( cr );
	cairo_set_line_width( cr, 1 );
	cairo_set_line_join( cr, CAIRO_LINE_JOIN_ROUND );
	cairo_save(cr);
	cairo_set_source_rgba( cr, 0, 0, 0, 0.6 );
	cairo_move_to( cr, center_x, 0 );
	cairo_line_to( cr, center_x, dim.height );
	cairo_stroke( cr );
	cairo_move_to( cr, 0, center_y );
	cairo_line_to( cr, dim.width, center_y );
	cairo_stroke( cr );
	cairo_restore( cr );

	if ( anim_frame )
	{
		cairo_save( cr );
		cairo_rectangle( cr, center_x+anim_frame->offset.x, center_y+anim_frame->offset.y, gdk_pixbuf_get_width(anim_frame->sprite), gdk_pixbuf_get_height(anim_frame->sprite) );
		cairo_set_source_rgba( cr, 1.0, 1.0, 1.0, 1.0 );
		gdk_cairo_set_source_pixbuf( cr, anim_frame->sprite, center_x+anim_frame->offset.x, center_y+anim_frame->offset.y );
		cairo_fill( cr );
		cairo_restore( cr );
	}

	gchar * text = g_strdup_printf("%d", preview->current+1);
	cairo_save( cr );
	cairo_select_font_face( cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL );
	cairo_set_font_size(cr, 13);
	cairo_set_source_rgba( cr, 0, 0, 0 ,1.0);
	cairo_move_to(cr, 0, 14);
	cairo_show_text(cr, text);
	cairo_restore( cr );
	g_free(text);

	return FALSE;

}

/********************************
* Animation_Preview_Update
*
*/
gboolean Animation_Preview_Update(GtkWidget * widget, GdkEventExpose *event, AnimationPreview * preview )
{
	cairo_t * cr;
	cr = gdk_cairo_create( gtk_widget_get_window(widget) );
	if ( !cr )
	{
		Meg_Main_PrintStatus("gdk_cairo_create failed ");
		return FALSE;
	}
	return Animation_Preview_Draw( widget, cr, preview );
}

/********************************
* Animation_AddPreview
*
*/
void Animation_AddPreview( GtkComboBox * combo, GtkWidget * image )
{
	GdkPixbuf * sprite_image = NULL;
	gchar * sprite_text = NULL;
	gchar * sheet_text = NULL;
	sheet_text = (gchar *)g_object_get_data( G_OBJECT(combo), "mokoisheet" );
	sprite_text = Meg_ComboText_GetText( combo );
	sprite_image = Sprite_GetPixbuf( sprite_text, sheet_text );
	gtk_image_set_from_pixbuf( GTK_IMAGE(image), sprite_image );

}

/********************************
* Animation_AddSelected
*
*/
void Animation_AddSelected( GtkMenuItem * menuitem, GtkWidget * widget )
{
	GtkTreeIter iter;
	GtkListStore * store = GTK_LIST_STORE(gtk_tree_view_get_model( GTK_TREE_VIEW(widget) ));
	GtkWidget * dialog = gtk_dialog_new_with_buttons( "Select sprite", NULL, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);
	GtkWidget * combo = gtk_combo_box_new();

	GtkWidget * sprite = gtk_image_new();

	Meg_ComboText_Setup( combo, FALSE );

	g_signal_connect( G_OBJECT(combo), "changed", G_CALLBACK(Animation_AddPreview), (gpointer) sprite );

	g_object_set_data( G_OBJECT(combo), "mokoisheet", g_object_get_data( G_OBJECT(widget), "mokoisheet" ) );


	GSList * scan = NULL;
	Spritesheet * sheet = Sheet_Get( (gchar *)g_object_get_data( G_OBJECT(widget), "mokoisheet" ) , FALSE );

	if ( sheet )
	{
		scan = sheet->children;
		while ( scan )
		{
			SheetObject * sprite =  (SheetObject *)scan->data;
			Meg_ComboText_AppendText( GTK_COMBO_BOX(combo), sprite->display_name );
			scan = g_slist_next( scan );
		}
	}
	gtk_container_add( GTK_CONTAINER(gtk_dialog_get_content_area( GTK_DIALOG(dialog) )), sprite );
	gtk_container_add( GTK_CONTAINER(gtk_dialog_get_content_area( GTK_DIALOG(dialog) )), combo );

	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ) );
	if ( GTK_RESPONSE_ACCEPT == gtk_dialog_run( GTK_DIALOG(dialog) ) )
	{
		gchar * text = Meg_ComboText_GetText( GTK_COMBO_BOX(combo) );
		if ( text )
		{
			gtk_list_store_append( store, &iter );
			gtk_list_store_set( store, &iter, 0, g_strdup(text), -1 );
			g_free( text );
		}
	}
	gtk_widget_destroy( dialog );
}

/********************************
* Animation_UpdatePreview
*
*/
void Animation_RemoveSelected( GtkMenuItem * menuitem, GtkWidget * widget )
{
	GtkTreeModel * model;
	GtkTreeIter iter;
	GtkTreeSelection * selection;

	selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(widget) );
	gtk_tree_selection_get_selected( selection, &model, &iter );
	gtk_list_store_remove( GTK_LIST_STORE(model), &iter );
}

/********************************
* Animation_PopupMenu
*
*/
gboolean Animation_PopupMenu( GtkWidget * widget, GtkListStore * store )
{
	GtkWidget * menu, * item1, * item2;

	menu = gtk_menu_new();
	item1 = gtk_menu_item_new_with_label( "Add" );
	item2 = gtk_menu_item_new_with_label( "Remove" );
	gtk_menu_shell_append( GTK_MENU_SHELL(menu), item1 );
	gtk_menu_shell_append( GTK_MENU_SHELL(menu), item2 );

	g_signal_connect( G_OBJECT(item1), "activate", G_CALLBACK(Animation_AddSelected), (gpointer) widget );
	g_signal_connect( G_OBJECT(item2), "activate", G_CALLBACK(Animation_RemoveSelected), (gpointer) widget );

	gtk_widget_show_all( menu );
	gtk_menu_popup( GTK_MENU(menu), NULL, NULL, NULL, NULL, 3, gtk_get_current_event_time() );
	return TRUE;
}

/********************************
* Animation_MouseMenu
*
*/
gboolean Animation_MouseMenu( GtkWidget * widget, GdkEventButton * event, GtkListStore * store )
{
	if ( event->type == GDK_BUTTON_PRESS )
	{
		if ( event->button == 3 )
			return Animation_PopupMenu( widget, store );
	}
	return FALSE;
}

/********************************
* Animation_Tooltip
*
*/
gboolean Animation_Tooltip( GtkWidget *widget, gint x, gint y, gboolean keyboard_tip, GtkTooltip * tooltip, gpointer data)
{
	GtkTreeIter iter;
	GtkTreeModel *model = NULL;
	GdkPixbuf * image = NULL;
	if ( !gtk_tree_view_get_tooltip_context( GTK_TREE_VIEW(widget), &x, &y, keyboard_tip, &model, NULL, &iter) )
		return FALSE;

	gtk_tree_model_get( model, &iter, 1, &image, -1);

	gtk_tooltip_set_icon( tooltip, image );

	return TRUE;
}





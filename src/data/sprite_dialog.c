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

#include "sheets_functions.h"

/* External Functions */
gboolean Sheet_SaveFile( Spritesheet * sheet );
gboolean SpriteCollision_Read( SheetObject * sprite, GtkListStore * list );
gboolean SpriteCollision_Write( SheetObject * sprite, GtkTreeModel * data );

void SpriteCollision_Add( GtkButton * widget, GtkComboBox * combobox );
void SpriteCollision_Update( GtkComboBox * combobox, GtkWidget * widget );

gboolean SpriteCollision_Draw( GtkWidget * window, cairo_t * cr, GtkComboBox * combobox );
gboolean SpriteCollision_Expose(  GtkWidget * widget, GdkEventExpose * event, GtkComboBox * combobox );

gboolean SpriteCollision_ButtonMoved( GtkWidget * widget , GdkEventMotion * event,  GtkComboBox * combobox );
gboolean SpriteCollision_ButtonPressed( GtkWidget * widget , GdkEventMotion * event,  GtkComboBox * combobox );

/* Global Variables */
extern GError * mokoiError;
extern GSList * mokoiSpritesheets;
extern GdkRectangle Mokoi_SpriteCollision;

#define CLEAR_COLLISION_SELECTION() Mokoi_SpriteCollision.x = Mokoi_SpriteCollision.y = Mokoi_SpriteCollision.width = Mokoi_SpriteCollision.height = 0

/* Local Variables */



/* UI */

const gchar * mokoiUI_SpriteAdvance = GUI_SPRITE_ADVANCE;


/********************************
* SpriteCollision_Draw
*
*/

gboolean Sprite_DialogPreview_Draw( GtkWidget * widget, cairo_t * cr, gpointer data )
{
	gint sheet_width, sheet_height;
	gdouble width = 5, height = 5, x = 0, y = 0;

	GtkSpinButton  * spin_x, * spin_y, * spin_w, * spin_h;
	Spritesheet * sheet = NULL;

	gdouble sprite_center_x = 5, sprite_center_y = 5;
	gint widget_center_x, widget_center_y;



	sheet = g_object_get_data( G_OBJECT(widget), "sheet" );

	spin_x = g_object_get_data( G_OBJECT(widget), "spin_x" );
	spin_y = g_object_get_data( G_OBJECT(widget), "spin_y" );
	spin_w = g_object_get_data( G_OBJECT(widget), "spin_w" );
	spin_h = g_object_get_data( G_OBJECT(widget), "spin_h" );

	gtk_widget_set_size_request( widget, gtk_spin_button_get_value_as_int( spin_w )*4, gtk_spin_button_get_value_as_int( spin_h )*4 );

	x = gtk_spin_button_get_value( spin_x );
	y = gtk_spin_button_get_value( spin_y );
	width = gtk_spin_button_get_value( spin_w );
	height = gtk_spin_button_get_value( spin_h );


	#if GTK_MAJOR_VERSION == 2
	GtkAllocation wigdet_al;
	gtk_widget_get_allocation( widget, &wigdet_al);
	widget_center_x = wigdet_al.width / 2;
	widget_center_y = wigdet_al.height / 2;
	#else
	widget_center_x = gtk_widget_get_allocated_width( widget ) / 2;
	widget_center_y = gtk_widget_get_allocated_height( widget ) / 2;
	#endif


	sprite_center_x = widget_center_x - (x + (width/2));
	sprite_center_y = widget_center_y - (y + (height/2));

	cairo_translate( cr, sprite_center_x, sprite_center_y );
	cairo_save( cr );



	if ( sheet->image )
	{

		sheet_width = gdk_pixbuf_get_width( sheet->image );
		sheet_height = gdk_pixbuf_get_height( sheet->image );

		//
		cairo_save( cr );
		cairo_rectangle( cr, 0, 0, sheet_width, sheet_height );
		gdk_cairo_set_source_pixbuf( cr, sheet->image, 0, 0 );
		cairo_fill( cr );
		cairo_restore( cr );

		cairo_save( cr );
		cairo_set_operator( cr, CAIRO_OPERATOR_OVER );
		cairo_rectangle( cr, 0, 0, sheet_width, sheet_height );
		cairo_set_source_rgba( cr, 0, 0.0, 0.0, 0.5);
		cairo_fill( cr );
		cairo_restore( cr );

		cairo_save( cr );
		cairo_rectangle( cr, x, y, width, height);
		cairo_set_source_rgba( cr, 1.0, 1.0, 1.0, 1.0 );
		cairo_fill(cr);
		cairo_restore( cr );

		cairo_save( cr );
		cairo_rectangle( cr, x, y, width, height);
		cairo_clip(cr);
		gdk_cairo_set_source_pixbuf( cr, sheet->image, 0, 0 );
		cairo_paint(cr);
		cairo_restore( cr );

		cairo_save( cr );
		cairo_rectangle( cr, x - 1.0, y - 1.0, width + 2.0, height + 2.0 );
		cairo_set_source_rgba( cr, .2, .6, 1, 0 );
		cairo_fill_preserve( cr );
		cairo_set_source_rgba( cr, .2, .6, 1, 1.0 );
		cairo_set_line_width( cr , 0.5);
		cairo_stroke( cr );
		cairo_restore( cr );

	}

	cairo_restore( cr );
	return TRUE;


}

/********************************
* Sprite_DialogPreview_Expose
*
*/
gboolean Sprite_DialogPreview_Expose(  GtkWidget * widget, GdkEventExpose * event, gpointer data )
{
	cairo_t * cr;

	cr = gdk_cairo_create( gtk_widget_get_window(widget) );

	if ( !cr )
	{
		Meg_Main_PrintStatus("gdk_cairo_create failed ");
		return FALSE;
	}


	return Sprite_DialogPreview_Draw( widget, cr, data );

}

/********************************
* Sprite_DialogPreview_SpinChange
*
*/
void Sprite_DialogPreview_SpinChange( GtkSpinButton *spinbutton, GtkWidget * area )
{
	gtk_widget_queue_draw( area );

}

/********************************
* Sprite_AdvanceDialog
*
*/
gboolean Sprite_AdvanceDialog( Spritesheet * sheet, SheetObject * sprite )
{
	GtkWidget * dialog, * entry_name, * spin_mask, *check_maskauto, * file_mask, * file_entity, * area_collision , *combo_collision, * image_preview, * check_visible, * button_entitysettings;
	GtkWidget * image_group, * area_preview;
	GtkSpinButton  * spin_x, * spin_y, * spin_w, * spin_h;
	GtkListStore * store_collision;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(mokoiUI_SpriteAdvance, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );

	/* Widget */
	dialog = GET_WIDGET( ui, "dialog");

	/* Detail Tab */
	entry_name = GET_WIDGET( ui, "entry_name");
	check_visible = GET_WIDGET( ui, "check_visible" );
	spin_x = GET_SPIN_WIDGET( ui, "spin_x");
	spin_y = GET_SPIN_WIDGET( ui, "spin_y");
	spin_w = GET_SPIN_WIDGET( ui, "spin_w");
	spin_h = GET_SPIN_WIDGET( ui, "spin_h");
	spin_mask = GET_WIDGET( ui, "spin_mask");
	file_mask = GET_WIDGET( ui, "combo_maskfile");
	area_preview = GET_WIDGET( ui, "area_preview");
	check_maskauto = GET_WIDGET( ui, "check_maskauto");

	/* Collision Tab */
	area_collision = GET_WIDGET( ui, "area_collision");
	combo_collision = GET_WIDGET( ui, "combo_collision");
	store_collision = GET_LISTSTORE( ui, "store_collision");

	/* Entity Tab */
	file_entity = GET_WIDGET( ui, "combo_entities");
	button_entitysettings = GET_WIDGET( ui, "button_entitysettings");

	/* Settings */
	g_object_set_data(G_OBJECT(area_collision), "sprite", sprite );
	CLEAR_COLLISION_SELECTION();
	SpriteCollision_Read( sprite, store_collision );

	/* Grouping Tab */
	image_group = GET_WIDGET( ui, "image_group");

	SpriteGrouping_GetWidget(  ui, "area_grouptopleft", sheet, sprite, 0 );
	SpriteGrouping_GetWidget(  ui, "area_grouptop", sheet, sprite, 1 );
	SpriteGrouping_GetWidget(  ui, "area_grouptopright", sheet, sprite, 2 );
	SpriteGrouping_GetWidget(  ui, "area_groupright", sheet, sprite, 3 );
	SpriteGrouping_GetWidget(  ui, "area_groupbottomright", sheet, sprite, 4 );
	SpriteGrouping_GetWidget(  ui, "area_groupbottom", sheet, sprite, 5 );
	SpriteGrouping_GetWidget(  ui, "area_groupbottomleft", sheet, sprite, 6 );
	SpriteGrouping_GetWidget(  ui, "area_groupleft", sheet, sprite, 7 );

	/* Signal */
	SET_OBJECT_SIGNAL( ui, "button_addcollision", "clicked", G_CALLBACK(SpriteCollision_Add), combo_collision );

	g_signal_connect( combo_collision, "changed", G_CALLBACK(SpriteCollision_Update), area_collision );
	g_signal_connect( area_collision, "button-press-event", G_CALLBACK(SpriteCollision_ButtonPressed), combo_collision );
	g_signal_connect( area_collision, "button-release-event", G_CALLBACK(SpriteCollision_ButtonPressed), combo_collision );
	g_signal_connect( area_collision, "motion-notify-event", G_CALLBACK(SpriteCollision_ButtonMoved), combo_collision );
	g_signal_connect( button_entitysettings, "button-press-event", G_CALLBACK(EntityCombo_Properties_Open), file_entity );
#if GTK_MAJOR_VERSION == 2
	g_signal_connect( area_collision, "expose-event", G_CALLBACK(SpriteCollision_Expose), combo_collision );
	g_signal_connect( area_preview, "expose-event", G_CALLBACK(Sprite_DialogPreview_Expose), NULL );
#else
	g_signal_connect( area_collision, "draw", G_CALLBACK(SpriteCollision_Draw), combo_collision );
	g_signal_connect( area_preview, "draw", G_CALLBACK(Sprite_DialogPreview_Draw), NULL );
#endif

	g_signal_connect( spin_x, "value-changed", G_CALLBACK(Sprite_DialogPreview_SpinChange), area_preview );
	g_signal_connect( spin_y, "value-changed", G_CALLBACK(Sprite_DialogPreview_SpinChange), area_preview );
	g_signal_connect( spin_w, "value-changed", G_CALLBACK(Sprite_DialogPreview_SpinChange), area_preview );
	g_signal_connect( spin_h, "value-changed", G_CALLBACK(Sprite_DialogPreview_SpinChange), area_preview );

	g_object_set_data(G_OBJECT(area_preview), "sheet", sheet );
	g_object_set_data(G_OBJECT(area_preview), "sprite", sprite );
	g_object_set_data(G_OBJECT(area_preview), "spin_x", spin_x );
	g_object_set_data(G_OBJECT(area_preview), "spin_y", spin_y );
	g_object_set_data(G_OBJECT(area_preview), "spin_w", spin_w );
	g_object_set_data(G_OBJECT(area_preview), "spin_h", spin_h );

	/* Set Default Values */
	gtk_entry_set_text( GTK_ENTRY(entry_name), sprite->display_name );

	Meg_ComboFile_Scan( file_mask, "masks",  ".pgm", TRUE, 0 );
	Meg_ComboFile_Scan( file_entity, "scripts",  ".mps", TRUE, 0 );
	gtk_widget_set_size_request( area_preview, sprite->position.width * 4, sprite->position.height * 4 );


	if ( SPRITE_DATA(sprite)->mask.name != NULL )
	{
		Meg_ComboText_SetIndex( GTK_COMBO_BOX(file_mask), SPRITE_DATA(sprite)->mask.name);
	}
	else
	{
		gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_mask), (gdouble) SPRITE_DATA(sprite)->mask.value );
		SPRITE_DATA(sprite)->mask.autogen = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_maskauto) );
	}

	if ( SPRITE_DATA(sprite)->entity )
	{
		Meg_ComboText_SetIndex( GTK_COMBO_BOX(file_entity), SPRITE_DATA(sprite)->entity );
	}

	if ( SPRITE_DATA(sprite)->image )
	{
		gtk_image_set_from_pixbuf( GTK_IMAGE(image_group), SPRITE_DATA(sprite)->image );
	}

	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(check_visible), sprite->visible );

	gtk_spin_button_set_value( spin_x, (gdouble) sprite->position.x );
	gtk_spin_button_set_value( spin_y, (gdouble) sprite->position.y );
	gtk_spin_button_set_value( spin_w, (gdouble) sprite->position.width );
	gtk_spin_button_set_value( spin_h, (gdouble) sprite->position.height );




	/* Run Dialog */
	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ) );
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Main_GetWindow() );

	gint result = gtk_dialog_run( GTK_DIALOG (dialog) );
	if ( result == GTK_RESPONSE_APPLY ) // -10
	{
		const gchar * sprite_name;
		gchar * mask_filename, * entity_filename;

		sprite_name = gtk_entry_get_text( GTK_ENTRY(entry_name) );
		mask_filename = Meg_ComboText_GetText( GTK_COMBO_BOX(file_mask) );
		entity_filename = Meg_ComboText_GetText( GTK_COMBO_BOX(file_entity) );

		if ( !g_utf8_strlen(sprite_name,-1) )
			Meg_Error_Print( __func__, __LINE__, "Mokoi_Sprite_AdvanceSprite: No name given");
		else
		{
			if ( sprite->display_name)
				g_free( sprite->display_name );

			sprite->display_name = g_strdup( sprite_name );
			sprite->ident_string = g_strdup_printf( "%s:%s", sprite->parent_sheet, sprite->display_name );

			if ( mask_filename )
				SPRITE_DATA(sprite)->mask.name = g_strdup( mask_filename );
			else
			{
				SPRITE_DATA(sprite)->mask.value = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_mask) );
				SPRITE_DATA(sprite)->mask.autogen = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_maskauto) );
			}
			if ( entity_filename )
				SPRITE_DATA(sprite)->entity = g_strdup( entity_filename );


			sprite->visible = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_visible) );


			sprite->position.x = gtk_spin_button_get_value_as_int( spin_x );
			sprite->position.y = gtk_spin_button_get_value_as_int( spin_y );
			sprite->position.width = gtk_spin_button_get_value_as_int( spin_w );
			sprite->position.height = gtk_spin_button_get_value_as_int( spin_h );

			sprite->visible = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_visible) );


			if ( SPRITE_DATA(sprite)->image )
				g_object_unref( SPRITE_DATA(sprite)->image );

			/* Create new sprite */
			GdkPixbuf * parent_image;
			parent_image = AL_GetImage( sprite->parent_sheet, NULL );
			SPRITE_DATA(sprite)->image = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, sprite->position.width, sprite->position.height );
			if ( parent_image )
			{
				gdk_pixbuf_copy_area( parent_image, sprite->position.x, sprite->position.y, sprite->position.width, sprite->position.height, SPRITE_DATA(sprite)->image, 0, 0 );
				SPRITE_DATA(sprite)->image_loaded = TRUE;
			}
			g_object_unref( parent_image );

			/* Children Tag */
			guint8 t = 0;
			for ( ; t < 8; t++ )
			{
				if ( SPRITE_DATA(sprite)->childrens[t].name && SPRITE_DATA(sprite)->childrens[t].position >= 0 && SPRITE_DATA(sprite)->childrens[t].position <= 7 )
				{
					g_print( "<child name=\"%s\" position=\"%d\" repeat=\"%d\"/>\n", SPRITE_DATA(sprite)->childrens[t].name, SPRITE_DATA(sprite)->childrens[t].position, SPRITE_DATA(sprite)->childrens[t].repeat );
				}
			}


			SpriteCollision_Write( sprite, GTK_TREE_MODEL(store_collision) );
			Sheet_SaveFile( sheet );

		}
	}
	else if ( result == 1 ) // Delete
	{
		GtkWidget * delete_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_YES_NO,
			"Are you sure you want to delete this sprite?"
		);
		gint result = gtk_dialog_run( GTK_DIALOG(delete_dialog) );

		if ( result == GTK_RESPONSE_YES )
		{
			Sheet_RemoveSprite( sheet, sprite );
		}
		gtk_widget_destroy( delete_dialog );
	}

	g_object_set_data(G_OBJECT(area_collision), "sprite", NULL );


	gtk_widget_destroy( dialog );
	return TRUE;
}






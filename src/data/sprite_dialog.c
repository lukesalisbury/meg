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

/* External Functions */
gboolean Sheet_SaveFile( MokoiSheet * sheet );
gboolean SpriteCollision_Read( MokoiSprite * sprite, GtkListStore * list );
gboolean SpriteCollision_Write( MokoiSprite * sprite, GtkTreeModel * data );

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
#include "ui/sprite_advance.gui.h"
const gchar * mokoiUI_SpriteAdvance = GUISPRITE_ADVANCE


/********************************
* Sprite_AdvanceDialog
*
*/
gboolean Sprite_AdvanceDialog( MokoiSheet * sheet, MokoiSprite * sprite )
{
	GtkWidget * dialog, * entry_name, * spin_mask, * file_mask, * file_entity, * spin_x, * spin_y, * spin_w, * spin_h, *area_collision , *combo_collision, * image_preview, * check_visible, * button_entitysettings;
	GtkWidget * image_group, * area_grouptopleft , * area_grouptop, * area_grouptopright, * area_groupright, * area_groupbottomright, * area_groupbottom, * area_groupbottomleft, * area_groupleft;
	GtkListStore * store_collision;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(mokoiUI_SpriteAdvance, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );

	/* Widget */
	dialog = GET_WIDGET( ui, "dialog");

	/* Detail Tab */
	entry_name = GET_WIDGET( ui, "entry_name");
	check_visible = GET_WIDGET( ui, "check_visible" );
	spin_x = GET_WIDGET( ui, "spin_x");
	spin_y = GET_WIDGET( ui, "spin_y");
	spin_w = GET_WIDGET( ui, "spin_w");
	spin_h = GET_WIDGET( ui, "spin_h");
	spin_mask = GET_WIDGET( ui, "spin_mask");
	file_mask = GET_WIDGET( ui, "combo_maskfile");
	image_preview = GET_WIDGET( ui, "image_preview");

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
/*
	area_grouptopleft = SpriteGrouping_GetWidget(  ui, "area_grouptopleft", sheet, sprite, 0 );
	area_grouptop = SpriteGrouping_GetWidget(  ui, "area_grouptop", sheet, sprite, 1 );
	area_grouptopright = SpriteGrouping_GetWidget(  ui, "area_grouptopright", sheet, sprite, 2 );
	area_groupright = SpriteGrouping_GetWidget(  ui, "area_groupright", sheet, sprite, 3 );
	area_groupbottomright = SpriteGrouping_GetWidget(  ui, "area_groupbottomright", sheet, sprite, 4 );
	area_groupbottom = SpriteGrouping_GetWidget(  ui, "area_groupbottom", sheet, sprite, 5 );
	area_groupbottomleft = SpriteGrouping_GetWidget(  ui, "area_groupbottomleft", sheet, sprite, 6 );
	area_groupleft = SpriteGrouping_GetWidget(  ui, "area_groupleft", sheet, sprite, 7 );
*/
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
#else
	g_signal_connect( area_collision, "draw", G_CALLBACK(SpriteCollision_Draw), combo_collision );
#endif

	/* Set Default Values */
	gtk_entry_set_text( GTK_ENTRY(entry_name), sprite->detail->name );

	Meg_ComboFile_Scan( file_mask, "masks",  ".pgm", TRUE, 0 );
	Meg_ComboFile_Scan( file_entity, "scripts",  ".mps", TRUE, 0 );

	if ( sprite->mask )
	{
		if ( sprite->mask->name != NULL )
		{
			Meg_ComboText_SetIndex( GTK_COMBO_BOX(file_mask), sprite->mask->name);
		}
		else
		{

			gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_mask), (gdouble) sprite->mask->value );
		}
	}

	if ( sprite->entity )
	{
		Meg_ComboText_SetIndex( GTK_COMBO_BOX(file_entity), sprite->entity );
	}


	if (sprite->image)
	{
		gtk_image_set_from_pixbuf( GTK_IMAGE(image_preview), sprite->image );
		gtk_image_set_from_pixbuf( GTK_IMAGE(image_group), sprite->image );


	}

	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(check_visible), sprite->visible );

	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_x), (gdouble) sprite->detail->position.x );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_y), (gdouble) sprite->detail->position.y );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_w), (gdouble) sprite->detail->position.width );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_h), (gdouble) sprite->detail->position.height );




	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ) );
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Main_GetWindow() );

	if ( gtk_dialog_run( GTK_DIALOG (dialog) ) == GTK_RESPONSE_APPLY )
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
			if ( sprite->detail->name)
				g_free( sprite->detail->name );

			sprite->detail->name = g_strdup( sprite_name );
			sprite->ident = g_strdup_printf( "%s:%s", sprite->parent, sprite->detail->name );

			if ( mask_filename )
				sprite->mask->name = g_strdup( mask_filename );
			else
				sprite->mask->value = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_mask) );

			if ( entity_filename )
				sprite->entity = g_strdup( entity_filename );


			sprite->visible = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_visible) );


			sprite->detail->position.x = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_x) );
			sprite->detail->position.y = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_y) );
			sprite->detail->position.width = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_w) );
			sprite->detail->position.height = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_h) );

			sprite->visible = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_visible) );


			if ( sprite->image )
				g_object_unref( sprite->image );

			/* Create new sprite */
			GdkPixbuf * parent_image;
			parent_image = AL_GetImage( sprite->parent, NULL );
			sprite->image = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, sprite->detail->position.width, sprite->detail->position.height );
			if ( parent_image )
			{
				gdk_pixbuf_copy_area( parent_image, sprite->detail->position.x, sprite->detail->position.y, sprite->detail->position.width, sprite->detail->position.height, sprite->image, 0, 0 );
				sprite->image_loaded = TRUE;
			}
			g_object_unref( parent_image );

			/* Children Tag */
			guint8 t = 0;
			for ( ; t < 8; t++ )
			{
				if ( sprite->childrens[t].name && sprite->childrens[t].position >= 0 && sprite->childrens[t].position <= 7 )
				{
					g_print( "<child name=\"%s\" position=\"%d\" repeat=\"%d\"/>\n", sprite->childrens[t].name, sprite->childrens[t].position, sprite->childrens[t].repeat );
				}
			}


			SpriteCollision_Write( sprite, GTK_TREE_MODEL(store_collision) );
			Sheet_SaveFile( sheet );

		}
	}
	g_object_set_data(G_OBJECT(area_collision), "sprite", NULL );


	gtk_widget_destroy( dialog );
	return TRUE;
}






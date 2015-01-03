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

/* Required Headers */
#include "animation.h"
#include "virtual_sprite.h"
#include "virtual_sprite_dialog.h"
#include "data_types.h"

/* External Functions */
gboolean Sprite_AdvanceDialog( Spritesheet * sheet, SheetObject * sprite );
void Sheet_Create( GdkPixbuf * image, gchar * filename );
gboolean Sheet_SaveFile( Spritesheet * sheet );

/* Local Type */

/* Global Variables */
extern gchar * mokoiBasePath;
extern GError * mokoiError;
extern GSList * mokoiSpritesheets;

/* Local Variables */



/* UI */


const gchar * mokoiUI_SpriteAdd = GUI_SPRITE_ADD;


/********************************
* AL_Sheet_Save
* Updates ComboBox'list' with list of sheets.
@ list:
*/
gboolean AL_Sheet_Save( Spritesheet * spritesheet )
{
	Spritesheet * sheet = Sheet_Get( spritesheet->file, FALSE );

	Sheet_SaveFile( sheet );

	return TRUE;
}


/********************************
* AL_Sheet_GetList
* Updates ComboBox'list' with list of sheets.
@ list:
*/
void AL_Sheet_List( GtkListStore * list, gboolean add_all )
{
	GtkTreeIter iter;

	g_slist_foreach( mokoiSpritesheets, (GFunc)Sheets_ListEach, list);

	gtk_list_store_append( list, &iter );
	gtk_list_store_set( list, &iter, 0, g_strdup("Virtual"), -1);

}

/********************************
* AL_Sheet_Add
* Add New sheet to project
@ filename: new image
*/
gboolean AL_Sheet_Add( gchar * filename )
{
	gboolean results = FALSE;
	gchar * file, * dest;
	GdkPixbuf * image = NULL;
	GdkPixbufFormat * f = gdk_pixbuf_get_file_info(filename, NULL, NULL);

	if ( g_strcmp0(gdk_pixbuf_format_get_name(f), "png") ) // Not a PNG image
	{
		file = g_strconcat( g_path_get_basename(filename), ".png", NULL);
		dest = g_build_filename( mokoiBasePath, "sprites", file, NULL );
		GdkPixbuf * src = gdk_pixbuf_new_from_file( filename, NULL );
		results = gdk_pixbuf_save(src, dest, "png", NULL, NULL);
		g_object_unref( src );
	}
	else
	{
		file = g_path_get_basename( filename );
		dest = g_build_filename( mokoiBasePath, "sprites", file, NULL );
		results = Meg_FileCopy( filename, dest );
	}

	if ( results )
	{

		image = gdk_pixbuf_new_from_file( dest, NULL );

		Sheet_Create( image, dest );
		Sheet_Get( file, TRUE );

		g_object_unref( image );
	}
	else
	{
		Meg_Error_Print( __func__, __LINE__, "AL_Sheet_Add: Can't create %s", dest );
	}

	g_free( dest );
	g_free( file );

	return results;
}

/********************************
* AL_Sheet_Remove
*
@ file: request file removing
*/
gboolean AL_Sheet_Remove( gchar * filename )
{
	/* Todo */
	Meg_Log_Print( LOG_WARNING, "AL_Sheet_Remove not enabled yet. Please close this program, and remove the file manually. " );
	return FALSE;
}

/********************************
* AL_Sheet_Get
*
@ file:

*/
Spritesheet * AL_Sheet_Get( gchar * file )
{
	if ( !file )
		return NULL;

	GSList * scan = NULL;
	Spritesheet * sheet = NULL;



	if ( !g_ascii_strcasecmp(file, "Virtual") )
	{
		return VirtualSpriteSheet_Get( FALSE );
	}
	else
	{
		sheet = Sheet_Get( file, FALSE );
	}

	if ( sheet )
	{
		/* update if need */
		if ( !sheet->image_loaded )
		{
			sheet->image = AL_GetImage( file, &sheet->file_size );
			sheet->image_loaded = TRUE;
		}

		/* Get Sprite List */
		/*
		if ( !sheet->children )
		{
			scan = sheet->children;
			while ( scan )
			{
				SheetObject * sprite = (SheetObject *)scan->data;
				sheet->children = g_slist_append(sheet->children, (gpointer)sprite->detail);
				scan = g_slist_next(scan);
			}
		}
		*/
	}
	else
	{
		Meg_Error_Print( __func__, __LINE__, "Sheet %s not found.", file);
	}

	return sheet;

}

/********************************
* AL_Sprite_Advance
*
@ parent:
@ id:
*/
gboolean AL_Sprite_Advance( Spritesheet * spritesheet, gchar * id )
{
	if ( !id || !spritesheet )
	{
		Meg_Error_Print( __func__, __LINE__, "Invalid sprite");
		return FALSE;
	}

	if ( !g_ascii_strcasecmp( spritesheet->file, "Virtual" ) )
	{
		return VirtualSpriteDialog_Display( id );
	}
	else
	{
		SheetObject * sprite = Sprite_Get( id, spritesheet->file, TRUE );

		if ( sprite )
		{

			if ( SPRITE_DATA(sprite)->animation )
			{
				return Animation_AdvanceDialog( spritesheet, sprite );
			}
			else
			{
				return Sprite_AdvanceDialog( spritesheet, sprite );
			}
		}
		else
		{
			Meg_Error_Print( __func__, __LINE__, "Invalid sprite %s:%s", spritesheet->file, id );

		}
	}
	return FALSE;
}


/********************************
* AL_Sprite_Add
*
@ sprite_rect: rectangle draw in editor
@ parent: name of sheet.
*/
void AL_Sprite_Add( Spritesheet * spritesheet, GdkRectangle * sprite_rect )
{
	if ( !spritesheet || !mokoiBasePath )
	{
		Meg_Error_Print( __func__, __LINE__, "AL_Sheet_AddSprite: Invalid parent" );
		return;
	}

	GtkWidget * dialog, * text_name, * file_mask, * file_entity, * radio_align;
	GtkSpinButton * spin_mask, * spin_x, * spin_y, * spin_w, * spin_h, * spin_frames;

	/* UI */
	GError * error = NULL;
	GtkBuilder * ui = gtk_builder_new();

	if ( !gtk_builder_add_from_string( ui, mokoiUI_SpriteAdd, -1, &error ) )
	{
		Meg_Error_Print( __func__, __LINE__, "UI creation error '%s'.", error->message );
		return;
	}


	dialog = GET_WIDGET( ui, "mokoi_sheet_addchild");
	text_name = GET_WIDGET( ui, "text_name");

	spin_x = GET_SPIN_WIDGET( ui, "spin_x");
	spin_y = GET_SPIN_WIDGET( ui, "spin_y");
	spin_w = GET_SPIN_WIDGET( ui, "spin_w");
	spin_h = GET_SPIN_WIDGET( ui, "spin_h");
	spin_mask = GET_SPIN_WIDGET( ui, "spin_mask");
	spin_frames = GET_SPIN_WIDGET( ui, "spin_frames");

	file_mask = GET_WIDGET( ui, "file_mask");
	file_entity = GET_WIDGET( ui, "file_entity");

	radio_align = GET_WIDGET( ui, "radio_align");

	/* Set Default Values */
	gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER(file_mask), mokoiBasePath );
	gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER(file_entity), mokoiBasePath );

	gtk_spin_button_set_value( spin_x, (gdouble)sprite_rect->x );
	gtk_spin_button_set_value( spin_y, (gdouble)sprite_rect->y );
	gtk_spin_button_set_value( spin_w, (gdouble)sprite_rect->width );
	gtk_spin_button_set_value( spin_h, (gdouble)sprite_rect->height );

	/* Show Dialog */
	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ) );
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Main_GetWindow());
	if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_APPLY )
	{
		gboolean bool_horizontal = FALSE;
		const gchar * sprite_name;
		gchar * mask_file, * entity_file;
		gint frame_count = 0;

		sprite_name = gtk_entry_get_text( GTK_ENTRY(text_name) );
		mask_file = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(file_mask) );
		entity_file = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(file_entity) );
		frame_count = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_frames) );
		bool_horizontal = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(radio_align) );

		if ( !g_utf8_strlen(sprite_name, -1) )
		{
			Meg_Error_Print( __func__, __LINE__, "AL_Sheet_AddSprite: No name given" );
		}
		else
		{
			SheetObject * sprite = NULL;
			SpriteData * sprite_data = g_new0(SpriteData, 1);

			sprite = SheetObject_New( (gpointer)sprite_data, &SpriteData_FreePointer );

			sprite->parent_sheet = g_strdup( spritesheet->file );
			sprite->display_name = g_strdup( sprite_name );

			sprite->ident_string = g_strdup_printf( "%s:%s", sprite->parent_sheet, sprite->display_name );

			if ( entity_file )
				sprite_data->entity = g_path_get_basename( entity_file );
			else
				sprite_data->entity = NULL;

			sprite->visible = TRUE;
			sprite_data->image_loaded = FALSE;

			sprite->position.x = gtk_spin_button_get_value_as_int( spin_x );
			sprite->position.y = gtk_spin_button_get_value_as_int( spin_y );
			sprite->position.width = gtk_spin_button_get_value_as_int( spin_w );
			sprite->position.height = gtk_spin_button_get_value_as_int( spin_h );

			if ( mask_file && g_utf8_strlen( mask_file, -1 ) )
				sprite_data->mask.name = g_strdup(mask_file);
			else
				sprite_data->mask.value = gtk_spin_button_get_value_as_int( spin_mask );

			GdkPixbuf * parent_image = AL_GetImage( sprite->parent_sheet, NULL );
			if ( frame_count > 0 )
			{
				/* Creating Animation */
				gint frame_length = 0;
				if ( bool_horizontal )
				{
					frame_length = sprite->position.width / frame_count;
					sprite->position.width = frame_length;
				}
				else
				{
					frame_length = sprite->position.height / frame_count;
					sprite->position.height = frame_length;
				}
				sprite_data->animation = g_new0(AnimationDetail, 1);


				gint q = 0;
				while ( q < frame_count )
				{
					/* Create the sprite for the frame */
					SheetObject * frame_sprite = NULL;
					SpriteData * frame_sprite_data = g_new0(SpriteData, 1);

					frame_sprite = SheetObject_New( (gpointer)frame_sprite_data, &SpriteData_FreePointer );

					frame_sprite->display_name = g_strdup_printf("%s_%d", sprite->display_name, q);
					frame_sprite->parent_sheet = g_strdup( spritesheet->file );
					frame_sprite->ident_string = g_strdup_printf( "%s:%s", frame_sprite->parent_sheet, frame_sprite->display_name );
					frame_sprite->visible = FALSE;

					frame_sprite_data->mask = sprite_data->mask;
					frame_sprite_data->image_loaded = FALSE;

					if ( bool_horizontal )
					{
						frame_sprite->position.x = sprite_rect->x + (q * frame_length);
						frame_sprite->position.y = sprite_rect->y;
						frame_sprite->position.width = frame_length;
						frame_sprite->position.height = sprite_rect->height;
					}
					else
					{
						frame_sprite->position.x = sprite_rect->x;
						frame_sprite->position.y = sprite_rect->y + (q * frame_length);
						frame_sprite->position.width = sprite_rect->width;
						frame_sprite->position.height = frame_length;
					}

					frame_sprite_data->image = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, frame_sprite->position.width, frame_sprite->position.height );
					gdk_pixbuf_copy_area( parent_image, frame_sprite->position.x, frame_sprite->position.y, frame_sprite->position.width, frame_sprite->position.height, frame_sprite_data->image, 0, 0);
					frame_sprite_data->image_loaded = TRUE;

					spritesheet->children = g_slist_append( spritesheet->children, frame_sprite);

					/* Create the Frame and append to list */
					AnimationFrame * frame = g_new0( AnimationFrame, 1 );
					frame->sprite = g_strdup( frame_sprite->display_name );
					frame->offset.x = 0;
					frame->offset.y = 0;
					frame->length_ms = 1000;

					sprite_data->animation->frames = g_slist_append( sprite_data->animation->frames, frame);

					q++;
				}
			}
			else
			{
				/* Create Sprite */
				sprite_data->image = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, sprite->position.width, sprite->position.height );
				gdk_pixbuf_copy_area( parent_image, sprite->position.x, sprite->position.y, sprite->position.width, sprite->position.height, sprite_data->image, 0, 0 );
				sprite_data->image_loaded = TRUE;
			}
			g_object_unref( parent_image );

			spritesheet->children = g_slist_append( spritesheet->children, sprite );
			Sheet_SaveFile( spritesheet );
		}
	}
	gtk_widget_destroy( dialog );
}


/********************************
* AL_Sprite_Remove
*
@
@
*/
void AL_Sprite_Remove( Spritesheet * spritesheet, gchar * id )
{
	SheetObject * sprite = Sprite_Get( id, spritesheet->file, FALSE );

	if ( SPRITE_DATA(sprite)->image )
	{
		g_object_unref( SPRITE_DATA(sprite)->image );
		SPRITE_DATA(sprite)->image = NULL;
	}

	spritesheet->children = g_slist_remove( spritesheet->children, sprite );
	Sheet_SaveFile( spritesheet );
}



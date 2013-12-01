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
#include "virtual_sprite.h"

/* External Functions */
gboolean Sprite_AdvanceDialog( MokoiSheet * sheet, MokoiSprite * sprite );
void Sheet_Create( GdkPixbuf * image, gchar * filename );
gboolean Sheet_SaveFile( MokoiSheet * sheet );

/* Local Type */

/* Global Variables */
extern gchar * mokoiBasePath;
extern GError * mokoiError;
extern GSList * mokoiSpritesheets;

/* Local Variables */



/* UI */

#include "ui/sprite_add.gui.h"
const gchar * mokoiUI_SpriteAdd = GUISPRITE_ADD;


/********************************
* AL_Sheet_Save
* Updates ComboBox'list' with list of sheets.
@ list:
*/
gboolean AL_Sheet_Save( Spritesheet * spritesheet )
{
	MokoiSheet * sheet = Sheet_Get( spritesheet->file, FALSE );

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
	Spritesheet * spritesheet = NULL;
	MokoiSheet * sheet = NULL;



	if ( !g_ascii_strcasecmp(file, "Virtual") )
	{
		return VirtualSprite_GetSpritesheet( );
	}
	else
	{
		sheet = Sheet_Get( file, FALSE );
	}

	if ( sheet )
	{
		spritesheet = sheet->detail;

		/* update if need */
		if ( !spritesheet->image_loaded )
		{
			spritesheet->image = AL_GetImage( file, &spritesheet->file_size );
			spritesheet->image_loaded = TRUE;
		}

		/* Get Sprite List */
		if ( !spritesheet->children )
		{
			scan = sheet->children;
			while ( scan )
			{
				MokoiSprite * sprite = (MokoiSprite *)scan->data;
				spritesheet->children = g_slist_append(spritesheet->children, (gpointer)sprite->detail);
				scan = g_slist_next(scan);
			}
		}
	}
	else
	{
		Meg_Error_Print( __func__, __LINE__, "Sheet %s not found.", file);
	}

	return spritesheet;

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
        return VirtualSprite_DialogDisplay( id );
	}
	else
	{
		MokoiSheet * sheet = Sheet_Get( spritesheet->file, FALSE );
		MokoiSprite * sprite = Sprite_Get( id, spritesheet->file, TRUE );

		if ( sprite )
		{
			if ( sprite->animation )
			{
				return Animation_AdvanceDialog( sheet, sprite );
			}
			else
			{
				return Sprite_AdvanceDialog( sheet, sprite );
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

	MokoiSheet * sheet = NULL;
	GtkWidget * dialog, * text_name, * image_preview, * spin_x, * spin_y, * spin_w, * spin_h;
	GtkWidget * spin_mask, * file_mask, * file_entity, * check_anim, * spin_frames, * radio_align;

	sheet = Sheet_Get( spritesheet->file, FALSE );
	if ( !sheet )
	{
		Meg_Error_Print( __func__, __LINE__, "AL_Sheet_AddSprite: Invalid parent %s", sheet->detail->file);
		return;
	}

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
	image_preview = GET_WIDGET( ui, "image_preview");
	spin_x = GET_WIDGET( ui, "spin_x");
	spin_y = GET_WIDGET( ui, "spin_y");
	spin_w = GET_WIDGET( ui, "spin_w");
	spin_h = GET_WIDGET( ui, "spin_h");
	spin_mask = GET_WIDGET( ui, "spin_mask");
	file_mask = GET_WIDGET( ui, "file_mask");
	file_entity = GET_WIDGET( ui, "file_entity");
	check_anim = GET_WIDGET( ui, "check_anim");
	spin_frames = GET_WIDGET( ui, "spin_frames");
	radio_align = GET_WIDGET( ui, "radio_align");

	/* Set Default Values */
	gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER(file_mask), mokoiBasePath);
	gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER(file_entity), mokoiBasePath);
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_x), (gdouble)sprite_rect->x );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_y), (gdouble)sprite_rect->y );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_w), (gdouble)sprite_rect->width );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_h), (gdouble)sprite_rect->height );

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
			MokoiSprite * sprite = g_new0(MokoiSprite, 1);

			sprite->detail = g_new0(SheetObject, 1);
			sprite->mask = g_new0(MokoiMask, 1);


			sprite->detail->name = g_strdup(sprite_name);
			sprite->parent = g_strdup(sheet->detail->file);
			sprite->ident = g_strdup_printf( "%s:%s", sprite->parent, sprite->detail->name );
			if ( entity_file )
				sprite->entity = g_path_get_basename( entity_file );
			else
				sprite->entity = NULL;
			sprite->visible = TRUE;
			sprite->image_loaded = FALSE;

			sprite->detail->position.x = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_x) );
			sprite->detail->position.y = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_y) );
			sprite->detail->position.width = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_w) );
			sprite->detail->position.height = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_h) );

			if ( mask_file && g_utf8_strlen( mask_file, -1 ) )
				sprite->mask->name = g_strdup(mask_file);
			else
				sprite->mask->value = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_mask) );

			GdkPixbuf * parent_image = AL_GetImage( sprite->parent, NULL );
			if ( frame_count > 0 )
			{
				/* Creating Animation */
				gint frame_length = 0;
				if ( bool_horizontal )
				{
					frame_length = sprite->detail->position.width / frame_count;
					sprite->detail->position.width = frame_length;
				}
				else
				{
					frame_length = sprite->detail->position.height / frame_count;
					sprite->detail->position.height = frame_length;
				}
				sprite->animation = g_new0(MokoiAnimation, 1);
				sprite->animation->frames = NULL;
				sprite->animation->ms_length = 0;

				gint q = 0;
				while ( q < frame_count )
				{
					/* Create the sprite for the frame */
					MokoiSprite * frame_sprite = g_new0( MokoiSprite, 1 );
					frame_sprite->detail = g_new0(SheetObject, 1);
					frame_sprite->mask = g_new0(MokoiMask, 1);
					frame_sprite->mask = sprite->mask;

					frame_sprite->detail->name = g_strdup_printf("%s_%d", sprite->detail->name, q);
					frame_sprite->parent = g_strdup(sheet->detail->file);
					frame_sprite->ident = g_strdup_printf( "%s:%s", frame_sprite->parent, frame_sprite->detail->name );


					frame_sprite->visible = FALSE;
					frame_sprite->image_loaded = FALSE;

					if ( bool_horizontal )
					{
						frame_sprite->detail->position.x = sprite_rect->x + (q * frame_length);
						frame_sprite->detail->position.y = sprite_rect->y;
						frame_sprite->detail->position.width = frame_length;
						frame_sprite->detail->position.height = sprite_rect->height;
					}
					else
					{
						frame_sprite->detail->position.x = sprite_rect->x;
						frame_sprite->detail->position.y = sprite_rect->y + (q * frame_length);
						frame_sprite->detail->position.width = sprite_rect->width;
						frame_sprite->detail->position.height = frame_length;
					}
					frame_sprite->image = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, frame_sprite->detail->position.width, frame_sprite->detail->position.height );
					gdk_pixbuf_copy_area( parent_image, frame_sprite->detail->position.x, frame_sprite->detail->position.y, frame_sprite->detail->position.width, frame_sprite->detail->position.height, frame_sprite->image, 0, 0);
					frame_sprite->image_loaded = TRUE;

					sheet->children = g_slist_append( sheet->children, frame_sprite);

					/* Create the Frame and append to list */
					MokoiAnimationFrame * frame = g_new0( MokoiAnimationFrame, 1 );
					frame->sprite = g_strdup( frame_sprite->detail->name );
					frame->offset.x = 0;
					frame->offset.y = 0;
					frame->length_ms = 33;

					sprite->animation->frames = g_slist_append(sprite->animation->frames, frame);


					q++;
				}
			}
			else
			{
				/* Create Sprite */
				sprite->image = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, sprite->detail->position.width, sprite->detail->position.height );
				gdk_pixbuf_copy_area( parent_image, sprite->detail->position.x, sprite->detail->position.y, sprite->detail->position.width, sprite->detail->position.height, sprite->image, 0, 0 );
				sprite->image_loaded = TRUE;
			}
			g_object_unref( parent_image );

			sheet->children = g_slist_append( sheet->children, sprite );
			spritesheet->children = g_slist_append( spritesheet->children, sprite->detail );
			Sheet_SaveFile( sheet );
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
void AL_Sprite_Remove(Spritesheet *spritesheet, gchar * id )
{
	MokoiSheet * sheet = Sheet_Get( spritesheet->file, FALSE );
	MokoiSprite * sprite = Sprite_Get( id, spritesheet->file, FALSE );

	if ( sprite->image )
	{
		g_object_unref( sprite->image );
		sprite->image = NULL;
	}
	sheet->children = g_slist_remove( sheet->children, sprite );
	spritesheet->children = g_slist_remove( spritesheet->children, sprite->detail );
	Sheet_SaveFile( sheet );
}



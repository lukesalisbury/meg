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

/* External Functions */

/* Local Type */

/* Global Variables */
extern GSList * mokoiSpritesheets;

/* Local Variables */

#ifdef __SUNPRO_C
#pragma align 4 (anim_overlay)
#endif
#ifdef __GNUC__
static const guint8 anim_overlay[] __attribute__ ((__aligned__ (4))) =
#else
static const guint8 anim_overlay[] =
#endif
{ ""
  /* Pixbuf magic (0x47646b50) */
  "GdkP"
  /* length: header (24) + pixel_data (324) */
  "\0\0\1\\"
  /* pixdata_type (0x1010002) */
  "\1\1\0\2"
  /* rowstride (36) */
  "\0\0\0$"
  /* width (9) */
  "\0\0\0\11"
  /* height (9) */
  "\0\0\0\11"
  /* pixel_data: */
  "\355\355\355\355\355\355\355\355\355\355\355\0\355\355\355\0\355\355"
  "\355\0\355\355\355\0\355\355\355\355\355\355\355\355\0\0\0\0\355\355"
  "\355\355\0\0\0\355\0\0\0\355\355\355\355\355\355\355\355\355\355\355"
  "\355\0\355\355\355\0\355\355\355\355\0\0\0\355\355\355\355\0\0\0\0\355"
  "\355\355\355\355\355\355\355\0\0\0\0\355\355\355\355\355\355\355\355"
  "\0\355\355\355\0\0\0\0\355\355\355\355\0\355\355\355\0\355\355\355\355"
  "\0\0\0\355\355\355\355\2\355\355\355\355\0\0\0\355\355\355\355\0\0\0"
  "\0\0\355\355\355\0\355\355\355\0\355\355\355\355\355\355\355\355\355"
  "\355\355\355\355\355\355\355\0\0\0\355\355\355\355\0\0\0\0\0\355\355"
  "\355\0\355\355\355\0\355\355\355\355\0\0\0\355\0\0\0\355\355\355\355"
  "\355\0\0\0\355\355\355\355\0\0\0\0\0\355\355\355\355\355\355\355\0\355"
  "\355\355\0\0\0\0\355\355\355\355\0\355\355\355\0\0\0\0\355\355\355\355"
  "\355\0\0\0\0\355\355\355\355\355\355\355\355\355\355\355\0\355\355\355"
  "\0\355\355\355\0\355\355\355\0\355\355\355\355\355\355\355\355\0\0\0"
  "\355\0\0\0\0\0\0\0\355\0\0\0\355\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\355\0\0\0\355"};

/* UI */


/********************************
* SpriteAnimation_Preview_ClearItem
*
*/
void SpriteAnimation_Preview_ClearItem( AnimationPreviewFrame * frame )
{
	g_object_unref(frame->sprite);
}

/********************************
* SpriteAnimation_Preview_Foreach
*
*/
gboolean SpriteAnimation_Preview_Foreach( GtkTreeModel * model, GtkTreePath *path, GtkTreeIter *iter, AnimationPreview * anim )
{
	gchar * sprite_name;
	AnimationPreviewFrame * frame = g_new0( AnimationPreviewFrame, 1 );

	gtk_tree_model_get( model, iter, 0, &sprite_name, 2, &frame->offset.x, 3, &frame->offset.y, 4, &frame->length_ms, -1 );
	frame->sprite = Sprite_GetPixbuf( sprite_name, anim->parent );
	if ( frame->sprite  )
	{
		anim->frames = g_list_append( anim->frames, frame );
	}
	return FALSE;
}

/********************************
* SpriteAnimation_Build
*
*/
void SpriteAnimation_Build( SheetObject * sprite )
{
	if ( SPRITE_DATA(sprite)->animation )
	{
		Spritesheet * parent = Sheet_Get(sprite->parent_sheet, FALSE);

		if ( !parent )
		{
			Meg_Log_Print( LOG_WARNING, "Parent Sheet not found '%s'.", sprite->parent_sheet );
			return;
		}
		if ( SPRITE_DATA(sprite)->image )
		{
			g_object_unref(SPRITE_DATA(sprite)->image);
			SPRITE_DATA(sprite)->image = NULL;
		}
		SPRITE_DATA(sprite)->animation->image_loaded = TRUE;


		if ( SPRITE_DATA(sprite)->animation->frames )
		{
			AnimationFrame * first_frame = (AnimationFrame *)g_slist_nth_data(SPRITE_DATA(sprite)->animation->frames, 0);

			GdkPixbuf * src = Sprite_GetPixbuf( first_frame->sprite, sprite->parent_sheet );
			GdkPixbuf * image = gdk_pixbuf_copy( src );
			GdkPixbuf * overlay = gdk_pixbuf_new_from_inline( -1, anim_overlay, FALSE, NULL );

			gdk_pixbuf_copy_area( overlay, 0,0, 9,9, image, 2, 2 );

			SPRITE_DATA(sprite)->image = image;
			SPRITE_DATA(sprite)->animation->w = gdk_pixbuf_get_width(image);
			SPRITE_DATA(sprite)->animation->h = gdk_pixbuf_get_height(image);

			SPRITE_DATA(sprite)->image = image;
			SPRITE_DATA(sprite)->image_loaded = TRUE;

			g_object_unref(src);

		}

	}
}

/********************************
* SpriteAnimation_Clear
*
*/
void SpriteAnimation_Clear(  AnimationPreview * preview )
{
	preview->id = 0;
	preview->timer = 0;
	if ( preview->frames )
	{
		g_list_free_full( preview->frames, (GDestroyNotify)SpriteAnimation_Preview_ClearItem);
	}
	preview->frames = NULL;
}

/********************************
* SpriteAnimation_Create
*
*/
void SpriteAnimation_Create( SheetObject * sprite, AnimationPreview * preview )
{
	g_return_if_fail( sprite != NULL );
	g_return_if_fail( SPRITE_DATA(sprite)->animation != NULL );
	g_return_if_fail( preview != NULL );

	SpriteAnimation_Clear(preview);

	gtk_tree_model_foreach( SPRITE_DATA(sprite)->animation->model, (GtkTreeModelForeachFunc)SpriteAnimation_Preview_Foreach, preview);
}




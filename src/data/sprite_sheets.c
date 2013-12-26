/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/*-------------------------------
	Sprite Sheets Handling Code
-------------------------------*/

/* Standard Headers */
#include "loader_global.h"
#include "resources_functions.h"
#include "entity_functions.h"
#include "animation_functions.h"
#include "runtime_parser.h"
#include "virtual_sprite.h"

/* Global Functions */

gboolean SpriteCollision_Load( SheetObject * sprite, gchar * file );
void Sheet_RemoveSprite( Spritesheet * sheet, SheetObject * sprite );

/* Local Functions */
void sheet_xml_end_element(GMarkupParseContext *context, const gchar*element_name, gpointer user_data, GError **error);
void sheet_xml_start_element(GMarkupParseContext * context, const gchar * element_name, const gchar ** attribute_names, const gchar ** attribute_values, gpointer user_data, GError ** error);

/* Global Variables */
extern GError * mokoiError;
extern GSList * mokoiSpritesheets;

/* Local Variables */

static GMarkupParser sheet_xml_parser = {
		sheet_xml_start_element,
		sheet_xml_end_element,
		NULL,
		NULL,
		NULL
};
static gchar * mokoiChildPositionStrings[] = {
	"top-left",
	"top",
	"top-right",
	"right",
	"bottom-right",
	"bottom",
	"bottom-left",
	"left",
	"unknown"
};


/********************************
* Sheets_ListEachVisible
*
*/
void Sheets_ListEachVisible( Spritesheet * sheet, GtkListStore * list )
{
	if ( sheet->visible )
	{
		GtkTreeIter iter;
		gtk_list_store_append( list, &iter );
		gtk_list_store_set( list, &iter, 0, g_strdup(sheet->file), -1);
	}
}

/********************************
* Sheets_ListEach
*
*/
void Sheets_ListEach( Spritesheet * sheet, GtkListStore * list )
{
	if ( sheet )
	{
		GtkTreeIter iter;
		gtk_list_store_append( list, &iter );
		gtk_list_store_set( list, &iter, 0, g_strdup(sheet->file), -1);
	}
}




/********************************
* Sheet_Find
*
*/
GSList * Sheet_Find( gchar * file_name )
{
	GSList * list_scan = mokoiSpritesheets;
	while ( list_scan )
	{
		Spritesheet * sheet =  ((Spritesheet *)list_scan->data);
		if ( file_name && sheet->file )
		{
			if ( !g_ascii_strcasecmp( sheet->file, file_name ) )
			{
				return list_scan;
			}
		}
		list_scan = g_slist_next( list_scan );
	}
	return NULL;
}




/********************************
* Sheet_Get
*
*/
Spritesheet * Sheet_Get( gchar * file_name, gboolean create )
{
	if ( !g_ascii_strcasecmp(file_name, "Virtual") )
	{
		return VirtualSpriteSheet_Get( TRUE );
	}


	GSList * wanted_sheet = Sheet_Find( file_name );
	if ( wanted_sheet )
	{
		return (Spritesheet*)wanted_sheet->data;
	}
	if ( create )
	{
		if ( Sheet_Parse(file_name) )
		{
			wanted_sheet = Sheet_Find(file_name);
			if ( wanted_sheet )
			{
				return (Spritesheet*)wanted_sheet->data;
			}
		}
	}
	return NULL;
}

/********************************
* Sheet_Load
* Loads PNG File
@ current: Sheet to load
*/
void Sheet_Load( Spritesheet * current )
{
	GdkPixbuf * parent_image = NULL;
	GSList * list_scan;

	if ( !current->ref )
	{
		parent_image = AL_GetImage( current->file, NULL );
		list_scan = current->children;

		while ( list_scan )
		{
			SheetObject * sprite = (SheetObject *)list_scan->data;
			if ( !SPRITE_DATA(sprite)->image_loaded )
			{
				if ( SPRITE_DATA(sprite)->animation )
				{
					//SpriteAnimation_Build( sprite );
					SPRITE_DATA(sprite)->image_loaded = FALSE;
				}
				else
				{
					SPRITE_DATA(sprite)->image = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, sprite->position.width, sprite->position.height);
					if ( parent_image )
					{
						gdk_pixbuf_copy_area( parent_image, sprite->position.x, sprite->position.y, sprite->position.width, sprite->position.height, SPRITE_DATA(sprite)->image, 0, 0 );
						SPRITE_DATA(sprite)->image_loaded = TRUE;
					}
				}
			}
			list_scan = g_slist_next(list_scan);
		}
		g_object_unref(parent_image);
		current->ref++;
	}
}

/********************************
* Sheet_SpriteExists
*
@ parent:
@ sprite:
*/
gboolean Sheet_SpriteExists( Spritesheet * parent, gchar * sprite )
{
	GSList * child = Sprite_Find(parent->children, sprite);
	if ( child != NULL )
		return TRUE;
	else
		return FALSE;
}

/********************************
* Sheet_SaveEachFrame
*
@ data: AnimationFrame
@ user_data: content
*/

void Sheet_SaveEachFrame( gpointer data, gpointer user_data )
{
	AnimationFrame * value = (AnimationFrame *)data;
	GString * content = (GString *)user_data;

	/* <frame sprite="process_0" x="0" y="0" ms="100"/> */
	g_string_append_printf( content, "\t\t<frame sprite=\"%s\" x=\"%d\" y=\"%d\" ms=\"%d\"/>\n", value->sprite, value->offset.x, value->offset.y, value->length_ms );

}

/********************************
* Sheet_SaveEachCollision
*
@ data: SpriteChild
@ user_data: content
*/

void Sheet_SaveEachCollision( SheetObject * sprite, GString * content )
{
	guint8 rect = 0;
	for ( rect = 0; rect < 7; rect++ )
	{
		/* <collision id="0" x="0" y="0" w="16" h="16"/> */
		if ( SPRITE_DATA(sprite)->collisions[rect].width != 0 && SPRITE_DATA(sprite)->collisions[rect].height != 0 )
		{
			g_string_append_printf( content, "\t\t<collision id=\"%d\" x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\" />\n", rect, SPRITE_DATA(sprite)->collisions[rect].x, SPRITE_DATA(sprite)->collisions[rect].y, SPRITE_DATA(sprite)->collisions[rect].width, SPRITE_DATA(sprite)->collisions[rect].height );
		}
	}
}


/********************************
* Sheet_SaveFile
* Save 'parent' sheets.
@ parent: Sheet to save
*/
gboolean Sheet_SaveFile( Spritesheet * sheet )
{
	if ( sheet == NULL || sheet->data == NULL )
		return FALSE;

	GString * content = g_string_new("");

	g_string_append_printf( content, "<sheet xmlns=\"http://mokoi.info/format/sheet\"%s>\n", (sheet->visible ? "" : " hidden=\"hidden\"") );

	GSList * scan = sheet->children;
	while ( scan )
	{
		SheetObject * sprite = (SheetObject *)scan->data;

		if ( SPRITE_DATA(sprite)->animation != NULL )
		{
			/* Start Tag */
			g_string_append_printf( content, "\t<animation name=\"%s\"", sprite->display_name );

			if ( SPRITE_DATA(sprite)->mask.name != NULL )
				g_string_append_printf( content, " mask=\"%s\"", SPRITE_DATA(sprite)->mask.name );
			else
				g_string_append_printf( content, " mask=\"%d\"", SPRITE_DATA(sprite)->mask.value );


			if ( SPRITE_DATA(sprite)->entity )
			{
				g_string_append_printf( content, " entity=\"%s\"", SPRITE_DATA(sprite)->entity );
			}

			if ( !sprite->visible )
			{
				content = g_string_append( content, " hidden=\"hidden\"" );
			}
			content = g_string_append( content, ">\n" );

			/* Children Tag */
			g_slist_foreach( SPRITE_DATA(sprite)->animation->frames, Sheet_SaveEachFrame, content ); /* Frames */

			/* End Tag */
			content = g_string_append( content, "\t</animation>\n" );
		}
		else
		{
			/* Start Tag */
			g_string_append_printf( content, "\t<sprite name=\"%s\"", sprite->display_name );

			if ( SPRITE_DATA(sprite)->mask.name != NULL )
				g_string_append_printf( content, " mask=\"%s\"", SPRITE_DATA(sprite)->mask.name );
			else
				g_string_append_printf( content, " mask=\"%d\"", SPRITE_DATA(sprite)->mask.value );


			if ( SPRITE_DATA(sprite)->entity )
			{
				g_string_append_printf( content, " entity=\"%s\"", SPRITE_DATA(sprite)->entity );
			}

			if ( !sprite->visible )
			{
				content = g_string_append( content, " hidden=\"hidden\"" );
			}
			content = g_string_append( content, ">\n" );

			g_string_append_printf( content, "\t\t<position x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\" />\n", sprite->position.x, sprite->position.y, sprite->position.width, sprite->position.height ); /* Position */

			Sheet_SaveEachCollision( sprite, content ); /* Collision */

			/* Children Tag */
			guint8 t = 0;
			for ( ; t < 8; t++ )
			{
				if ( SPRITE_DATA(sprite)->childrens[t].name && SPRITE_DATA(sprite)->childrens[t].position >= 0 && SPRITE_DATA(sprite)->childrens[t].position <= 7 )
				{
					g_string_append_printf( content, "\t\t<child name=\"%s\" position=\"%d\" repeat=\"%d\"/>\n", SPRITE_DATA(sprite)->childrens[t].name, SPRITE_DATA(sprite)->childrens[t].position, SPRITE_DATA(sprite)->childrens[t].repeat );
				}
			}

			/* End Tag */
			content = g_string_append( content, "\t</sprite>\n" );

		}

		scan = g_slist_next( scan );
	}
	content = g_string_append( content, "</sheet>" );

	/* Write the content out to file */
	gchar * sheet_file = g_strdup_printf("/sprites/%s.xml", sheet->file);
	Meg_file_set_contents( sheet_file, content->str, -1, &mokoiError );
	g_free( sheet_file );


	g_string_free( content, TRUE );

	if ( mokoiError )
	{
		Meg_Error_Print( __func__, __LINE__, "Spritesheet could not be saved. Reason: %s", mokoiError->message );
		g_clear_error( &mokoiError );
		return FALSE;
	}
	return TRUE;
}

void SpritesheetList_DestroyNotify(gpointer data)
{
	Spritesheet_Free( (Spritesheet*)data );
}

/********************************
* Sheet_Unload
*
-
*/
gboolean Sheet_Unload( )
{
	g_slist_free_full( mokoiSpritesheets, SpritesheetList_DestroyNotify );
	mokoiSpritesheets = NULL;
	return TRUE;
}

/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/*-------------------------------
	Sprite Sheets Parser
-------------------------------*/

/* Standard Headers */
#include "loader_global.h"
#include "sheets_functions.h"

/* Global Functions */
gboolean SpriteCollision_Load( SheetObject * sprite, gchar * file );
GSList * Path_FileLoad( gchar * file );

/* Local Functions */
void sheet_xml_end_element(GMarkupParseContext *context, const gchar*element_name, gpointer user_data, GError **error);
void sheet_xml_start_element(GMarkupParseContext * context, const gchar * element_name, const gchar ** attribute_names, const gchar ** attribute_values, gpointer user_data, GError ** error);

/* Global Variables */
extern GError * mokoiError;

/* Local Variables */
GSList * mokoiSpritesheets = NULL; /* <Spritesheet*> */
SheetObject * sheet_xml_sprite = NULL;
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

/* UI */
#include "ui/sheet_create.gui.h"
const gchar * mokoiUI_SheetCreate = GUISHEET_CREATE



/********************************
* Sheet_Create
* Dialog to create quick sheets.
@ image:
@ filename:
*/
void Sheet_Create( GdkPixbuf * image, gchar * imagefile )
{
	GString * content = g_string_new("<sheet xmlns=\"http://mokoi.info/format/sheet\" visible=\"TRUE\">");
	GtkWidget * dialog, * spin_top, * spin_left, * spin_bottom, * spin_right, * spin_width, * spin_height;

	/* UI */
	GError * error = NULL;
	GtkBuilder * ui = gtk_builder_new();
	if ( !gtk_builder_add_from_string(ui, mokoiUI_SheetCreate, -1, &error) )
	{
		Meg_Error_Print( __func__, __LINE__, "UI creation error '%s'.", error->message );
		return;
	}

	dialog = GET_WIDGET( ui, "sheet_create");
	spin_top = GET_WIDGET( ui, "spin_top");
	spin_left = GET_WIDGET( ui, "spin_left");
	spin_bottom = GET_WIDGET( ui, "spin_bottom");
	spin_right = GET_WIDGET( ui, "spin_right");
	spin_width = GET_WIDGET( ui, "spin_width");
	spin_height = GET_WIDGET( ui, "spin_height");

	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ) );
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Main_GetWindow() );

	if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_APPLY )
	{
		guint width = 16, height = 16, c = 0, xc, yc, section_width, section_height;

		width = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_width) );
		height = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_height) );

		yc = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_top) );

		section_width = gdk_pixbuf_get_width( image ) - gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_right) );
		section_height = gdk_pixbuf_get_height( image ) - gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_bottom) );

		for (; yc < section_height; yc += height )
		{
			xc = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_left) );
			for (; xc < section_width; xc += width )
			{

				g_string_append_printf( content, "\t<sprite name=\"%d\">\n", ++c );
				g_string_append_printf( content, "\t\t<position x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\" />\n", xc, yc, width, height ); /* Position */
				content = g_string_append( content, "\t</sprite>\n" );
			}
		}
	}

	content = g_string_append( content, "\n</sheet>" );

	gtk_widget_destroy( dialog );

	/* Save Content to file */
	gchar * sheet_file = g_strconcat( imagefile, ".xml", NULL);
	g_file_set_contents( sheet_file, content->str, -1, NULL );
	g_free( sheet_file );
	g_string_free( content, TRUE );

	/* Load Sprite sheet */
	Sheet_Parse(imagefile);

}


/********************************
* xml parser
*/
void sheet_xml_end_element(GMarkupParseContext *context, const gchar*element_name, gpointer user_data, GError **error)
{
	Spritesheet * sheet = (Spritesheet*)user_data;

	if ( !g_ascii_strcasecmp(element_name, "sprite")  )
	{
		if ( sheet_xml_sprite )
		{
			if ( !Sheet_SpriteExists( sheet, sheet_xml_sprite->display_name ) ) /* Don't re-add a sprite with the same name */
			{
				sheet->children = g_slist_prepend( sheet->children, sheet_xml_sprite );
			}
			else
			{
				/* Todo: Delete sheet_xml_sprite */
				SheetObject_Free(sheet_xml_sprite);
			}
		}
		sheet_xml_sprite = NULL;
	}
	else if ( !g_ascii_strcasecmp(element_name, "animation") )
	{
		if ( SPRITE_DATA(sheet_xml_sprite)->animation )
		{
			sheet->children = g_slist_append( sheet->children, sheet_xml_sprite );
		}
		else
		{
			/* Todo: Delete sheet_xml_sprite */
			SheetObject_Free(sheet_xml_sprite);
		}
		sheet_xml_sprite = NULL;
	}
}

/********************************
* xml parser
*/
void sheet_xml_start_element(GMarkupParseContext * context, const gchar * element_name, const gchar ** attribute_names, const gchar ** attribute_values, gpointer user_data, GError ** error)
{
	Spritesheet * sheet = (Spritesheet*)user_data;
	const gchar **name_cursor = attribute_names;
	const gchar **value_cursor = attribute_values;

	if ( !g_ascii_strcasecmp(element_name, "sheet") )
	{
		sheet->visible = TRUE;
		while (*name_cursor)
		{
			if ( !g_ascii_strcasecmp(*name_cursor, "hidden") )
			{
				sheet->visible = FALSE;
			}

			name_cursor++;
			value_cursor++;
		}
	}
	else if ( !g_ascii_strcasecmp(element_name, "virtualsprite") )
	{
		SpriteData * sprite_data = g_new0(SpriteData, 1);

		sheet_xml_sprite = SheetObject_New( (gpointer)sprite_data, &SpriteData_FreePointer );

		sheet_xml_sprite->visible = TRUE;
		sheet_xml_sprite->parent_sheet = g_strdup(sheet->file);

	}
	else if ( !g_ascii_strcasecmp(element_name, "sprite") || !g_ascii_strcasecmp(element_name, "animation") ) /* Create New Sprite */
	{
		SpriteData * sprite_data = g_new0(SpriteData, 1);

		sheet_xml_sprite = SheetObject_New( (gpointer)sprite_data, &SpriteData_FreePointer );

		sheet_xml_sprite->visible = TRUE;
		sheet_xml_sprite->parent_sheet = g_strdup(sheet->file);

		while (*name_cursor)
		{
			if ( !g_ascii_strcasecmp(*name_cursor, "name") )
			{
				sheet_xml_sprite->display_name = g_strdup( *value_cursor );
			}
			else if ( !g_ascii_strcasecmp(*name_cursor, "hidden") )
			{
				sheet_xml_sprite->visible = FALSE;
			}
			else if ( !g_ascii_strcasecmp(*name_cursor, "mask") )
			{
				if ( g_ascii_isdigit( *value_cursor[0]) )
					sprite_data->mask.value = strtoul( *value_cursor, NULL, 0 );
				else
					sprite_data->mask.name = g_strdup( *value_cursor );
			}
			else if ( !g_ascii_strcasecmp(*name_cursor, "entity") )
			{
				sprite_data->entity = g_strdup( *value_cursor );
			}

			name_cursor++;
			value_cursor++;
		}

		if ( !g_ascii_strcasecmp(element_name, "animation") )
		{
			sprite_data->animation = g_new0(AnimationDetail, 1);
		}
		sheet_xml_sprite->ident_string = g_strdup_printf( "%s:%s", sheet->file, sheet_xml_sprite->display_name );

		/*
		gchar * collision_file = g_strdup_printf( "%s-%s.collision", sheet->file, sheet_xml_sprite->display_name );
		SpriteCollision_Load( sheet_xml_sprite, collision_file );
		g_free(collision_file);
		*/

	}
	else if ( !g_ascii_strcasecmp(element_name, "position") && sheet_xml_sprite != NULL) /* Update Position */
	{
		for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
		{
			if ( *attribute_names[0] =='x' )
				sheet_xml_sprite->position.x = g_ascii_strtod(*attribute_values, NULL);
			else if ( *attribute_names[0] =='y' )
				sheet_xml_sprite->position.y = g_ascii_strtod(*attribute_values, NULL);
			else if ( *attribute_names[0] =='w' )
				sheet_xml_sprite->position.width = g_ascii_strtod(*attribute_values, NULL);
			else if ( *attribute_names[0] =='h' )
				sheet_xml_sprite->position.height = g_ascii_strtod(*attribute_values, NULL);
		}
	}
	else if ( !g_ascii_strcasecmp(element_name, "collision") && sheet_xml_sprite != NULL) /* Add collision */
	{
		guint8 rect = 0;

		while ( SPRITE_DATA(sheet_xml_sprite)->collisions[rect].width )
		{
			rect++;
		}

		if ( rect <= 6 )
		{
			for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
			{
				if ( *attribute_names[0] =='x' )
					SPRITE_DATA(sheet_xml_sprite)->collisions[rect].x = g_ascii_strtod(*attribute_values, NULL);
				else if ( *attribute_names[0] =='y' )
					SPRITE_DATA(sheet_xml_sprite)->collisions[rect].y = g_ascii_strtod(*attribute_values, NULL);
				else if ( *attribute_names[0] =='w' )
					SPRITE_DATA(sheet_xml_sprite)->collisions[rect].width = g_ascii_strtod(*attribute_values, NULL);
				else if ( *attribute_names[0] =='h' )
					SPRITE_DATA(sheet_xml_sprite)->collisions[rect].height = g_ascii_strtod(*attribute_values, NULL);
			}
		}
	}
	else if ( !g_ascii_strcasecmp(element_name, "child") && sheet_xml_sprite != NULL) /* Add child sprites */
	{
		/* <child name="process_0" position="top-left" repeat="0"/> */
		SpriteChild child;
		child.position = -1;
		child.name = NULL;
		child.repeat = 0;

		for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
		{
			if ( !g_ascii_strcasecmp(*attribute_names, "name") )
				child.name = g_strdup(*attribute_values);
			else if ( !g_ascii_strcasecmp(*attribute_names, "position") )
			{
				if ( g_ascii_isdigit( *attribute_values[0]) )
				{
					child.position = strtoul( *attribute_values, NULL, 0 );
				}
				else
				{
					if ( !g_ascii_strcasecmp(*attribute_values, "top-left") )
					{
						child.position = 0;
					}
					else if ( !g_ascii_strcasecmp(*attribute_values, "top") )
					{
						child.position = 1;
					}
					else if ( !g_ascii_strcasecmp(*attribute_values, "top-right") )
					{
						child.position = 2;
					}
					else if ( !g_ascii_strcasecmp(*attribute_values, "right") )
					{
						child.position = 3;
					}
					else if ( !g_ascii_strcasecmp(*attribute_values, "bottom-right") )
					{
						child.position = 4;
					}
					else if ( !g_ascii_strcasecmp(*attribute_values, "bottom") )
					{
						child.position = 5;
					}
					else if ( !g_ascii_strcasecmp(*attribute_values, "bottom-left") )
					{
						child.position = 6;
					}
					else if ( !g_ascii_strcasecmp(*attribute_values, "left") )
					{
						child.position = 7;
					}
				}
			}
			else if ( !g_ascii_strcasecmp(*attribute_names, "repeat") )
			{
				if ( g_ascii_isdigit(*attribute_values[0]) )
					child.repeat = *attribute_values[0] - '0';
			}
		}
		if ( child.position >= 0 && child.position <= 7)
			SPRITE_DATA(sheet_xml_sprite)->childrens[child.position] = child;

	}
	else if ( !g_ascii_strcasecmp(element_name, "frame") && sheet_xml_sprite != NULL) /* Animation Frames */
	{
		if ( SPRITE_DATA(sheet_xml_sprite)->animation )
		{
			/* <frame sprite="process_0" x="0" y="0" ms="100"/> */
			AnimationFrame * frame = g_new0( AnimationFrame, 1 );
			for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
			{
				if ( !g_ascii_strcasecmp(*attribute_names, "sprite") )
					frame->sprite = g_strdup(*attribute_values);
				else if ( *attribute_names[0] =='x' )
					frame->offset.x = g_ascii_strtod(*attribute_values, NULL);
				else if ( *attribute_names[0] =='y' )
					frame->offset.y = g_ascii_strtod(*attribute_values, NULL);
				else if ( !g_ascii_strcasecmp(*attribute_names, "ms") )
					frame->length_ms = g_ascii_strtod(*attribute_values, NULL);
			}
			SPRITE_DATA(sheet_xml_sprite)->animation->frames = g_slist_append(SPRITE_DATA(sheet_xml_sprite)->animation->frames, frame);
		}
	}
}



/********************************
* Sheet_ParseXMLFormat
*
@ filename:
- return true if successful
*/
gboolean Sheet_ParseXMLFormat( gchar * filename )
{

	gboolean function_success = FALSE;
	gchar * sheet_file = g_strdup_printf("/sprites/%s.xml", filename);

	if ( Meg_file_test( sheet_file, G_FILE_TEST_IS_REGULAR ) )
	{
		Spritesheet * sheet = NULL;
		GMarkupParseContext * ctx;
		gchar * sheet_contents = NULL;

		sheet = g_new0(Spritesheet, 1);
		sheet->file = g_path_get_basename(filename);
		sheet->children = NULL;
		sheet->image_loaded = FALSE;
		sheet->visible = TRUE;

		mokoiSpritesheets = g_slist_prepend( mokoiSpritesheets, sheet );
		Meg_file_get_contents( sheet_file, &sheet_contents, NULL, &mokoiError );

		if ( !Meg_Error_Check( mokoiError, FALSE, "Sheet_ParseXMLFormat" ) )
		{
			/* XML parser */
			GError  * err = NULL;
			ctx = g_markup_parse_context_new( &sheet_xml_parser, (GMarkupParseFlags)0, sheet, NULL );
			g_markup_parse_context_parse( ctx, sheet_contents, -1, &err );
			g_markup_parse_context_end_parse( ctx, &err );
			g_markup_parse_context_free( ctx );
			if ( err != NULL )
			{
				Meg_Log_Print( LOG_ERROR, "Spritesheet Parse Error: %s", err->message);
				g_clear_error(&err);
				function_success = FALSE;
			}
			else
				function_success =  TRUE;
		}
		g_free( sheet_contents );
	}

	g_free( sheet_file );

	return function_success;
}

gboolean Sheet_SaveFile( Spritesheet * sheet );
/********************************
* Sheet_ParseTextFormat
* Old Format
*/
gboolean Sheet_ParseTextFormat( gchar * filename )
{
	GError * local_error = NULL;
	gboolean function_success = FALSE;
	gchar * sheet_file = g_strdup_printf("/sprites/%s.txt", filename);

	if ( Meg_file_test( sheet_file, G_FILE_TEST_IS_REGULAR ) )
	{
		gchar * sheet_contents = NULL;
		Spritesheet * sheet = g_new0(Spritesheet, 1);
		sheet->file = g_strdup(filename);
		sheet->image_loaded = FALSE;
		sheet->visible = TRUE;
		sheet->children = NULL;
		sheet->ref = 0;



		mokoiSpritesheets = g_slist_prepend( mokoiSpritesheets, sheet );
		Meg_file_get_contents( sheet_file, &sheet_contents, NULL, &local_error );

		if ( !Meg_Error_Check( local_error, FALSE, "Sheet_Parse" ) )
		{
			gchar ** lines = g_strsplit_set( sheet_contents, "\n", -1);
			gint lc = 0;

			while ( lines[lc] != NULL )
			{
				if( !g_ascii_iscntrl( lines[lc][0] ) )
				{
					gchar ** array;
					guint length = 0;

					/* Create Value Array */
					lines[lc] = g_strstrip( lines[lc] );
					array = g_strsplit( lines[lc], "\t", -1 );
					length = g_strv_length( array );
					if ( length > 7 )
					{
						if ( !g_ascii_strcasecmp( array[1], "ANIMATION" ) )
						{
							g_warning("Animation '%s' in old format, skipping", array[0]);
							continue;
						}

						/* Create New Sprite */
						SpriteData * sprite_data = g_new0(SpriteData, 1);
						SheetObject * sprite = SheetObject_New( (gpointer)sprite_data, &SpriteData_FreePointer );

						sprite->display_name = g_strdup( array[0] );
						sprite->parent_sheet = g_path_get_basename(filename);
						sprite->ident_string = g_strdup_printf( "%s:%s", sprite->parent_sheet, sprite->display_name );
						sprite->visible = TRUE;

						sprite_data->animation = NULL;
						sprite_data->image = NULL;
						SPRITE_DATA(sprite)->image_loaded = 0;

						SPRITE_DATA(sprite)->flag = strtoul( array[1], NULL, 0 );

						SPRITE_DATA(sprite)->mask.name = NULL;
						SPRITE_DATA(sprite)->mask.value = 0;

						if ( g_ascii_isdigit( array[2][0]) )
							SPRITE_DATA(sprite)->mask.value = strtoul( array[2], NULL, 0 );
						else
							SPRITE_DATA(sprite)->mask.name = g_strdup( array[2] );

						if ( array[3][0] == 0 || array[3][0] == '0' )
							SPRITE_DATA(sprite)->entity = NULL;
						else
							SPRITE_DATA(sprite)->entity = g_strdup( array[3] );

						if ( !g_ascii_strcasecmp( array[4], "ANIMATION" ) )
						{
							sprite->position.x = sprite->position.y = sprite->position.width = sprite->position.height = 0;

							SPRITE_DATA(sprite)->animation = g_new0(AnimationDetail, 1);
							SPRITE_DATA(sprite)->animation->ms_length = strtoul( array[5], NULL, 0 );
							SPRITE_DATA(sprite)->animation->frames = NULL;
							SPRITE_DATA(sprite)->animation->w = 0;
							SPRITE_DATA(sprite)->animation->h = 0;

							/* Load Offsets file */
							gchar * offset_file = g_strdup_printf( "/paths/%s-%s.animation", sprite->parent_sheet, sprite->display_name );
							GSList * offset_path = Path_FileLoad( offset_file );
							g_free(offset_file);

							guint n = 6;
							while ( n < length )
							{
								AnimationFrame * frame = g_new0( AnimationFrame, 1 );
								PathPoint * path = (PathPoint *)g_slist_nth_data( offset_path, n - 6 );

								frame->sprite = g_strdup( array[n] );
								frame->length_ms = SPRITE_DATA(sprite)->animation->ms_length;

								if ( path )
								{
									frame->offset.x = path->x;
									frame->offset.y = path->y;
								}
								else
								{
									frame->offset.x = frame->offset.y = 0;
								}
								SPRITE_DATA(sprite)->animation->frames = g_slist_append( SPRITE_DATA(sprite)->animation->frames, frame );
								n++;
							}

							g_slist_free(offset_path); /* Free Offsets file */

						}
						else
						{
							sprite->position.x = strtoul( array[4], NULL, 0 );
							sprite->position.y = strtoul( array[5], NULL, 0 );
							sprite->position.width = strtoul( array[6], NULL, 0 );
							sprite->position.height = strtoul( array[7], NULL, 0 );
						}

						/* Load Collision file */
						gchar * collision_file = g_strdup_printf( "%s-%s.collision", sprite->parent_sheet, sprite->display_name );
						SpriteCollision_Load( sprite, collision_file );
						g_free(collision_file);

						/* Added Animation to the End */
						if ( SPRITE_DATA(sprite)->animation )
							sheet->children = g_slist_append( sheet->children, sprite );
						else if ( !Sheet_SpriteExists( sheet, sprite->display_name ) )
							sheet->children = g_slist_prepend( sheet->children, sprite );
						else
							Meg_Error_Print( __func__, __LINE__, "Sprite '%s' already exist on sheet.", sprite->display_name );
					}
					g_strfreev( array );
				}
				lc++;
			}
			g_strfreev( lines );
			function_success =  TRUE;
		}
		g_free( sheet_contents );

		Sheet_SaveFile( sheet );
	}
	g_free( sheet_file );


	return function_success;
}

/********************************
* Sheet_Parse
*
@ filename:
-
*/
gboolean Sheet_Parse( gchar * filename )
{
	if ( Sheet_ParseXMLFormat(filename) )
	{
		return TRUE;
	}
	else
	{
		return Sheet_ParseTextFormat(filename);
	}

	return FALSE;

}




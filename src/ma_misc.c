/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Global Header */
#include <zlib.h>
#include "global.h"
#include <glib/gstdio.h>
#include "physfs/physfs.h"

/* Global Variables */
extern GKeyFile * meg_pref_storage;

/* UI */



/********************************
* Meg_Builder_Create
*
*
*/
GtkBuilder * Meg_Builder_Create( const gchar * text, const char * funcname, gint line )
{
	GError * error = NULL;
	GtkBuilder * ui = gtk_builder_new();
	if ( !gtk_builder_add_from_string(ui, text, -1, &error) )
	{
		Meg_Error_Print( funcname, line, "UI creation error '%s'.", error->message );
		g_object_unref(ui);
		return NULL;
	}
	return ui;
}





/********************************
* Meg_ColorButton_SetColor
*
*
*/
void Meg_ColorButton_SetColor( GtkColorButton * button, GdkRGBA * rgbacolour )
{
#if GTK_CHECK_VERSION(3,4,0)
	gtk_color_chooser_set_rgba( GTK_COLOR_CHOOSER(button), rgbacolour );
#elif GTK_CHECK_VERSION(3,0,0)
	gtk_color_button_set_rgba( button, rgbacolour );
#else
	GdkColor c;
	guint16 a;

	c.red = (guint16)(rgbacolour->red * 65535.0);
	c.blue = (guint16)(rgbacolour->blue * 65535.0);
	c.green = (guint16)(rgbacolour->green * 65535.0);
	c.pixel = 0;
	a = (guint16)(rgbacolour->alpha * 65535.0);

	gtk_color_button_set_color( button, &c);
	gtk_color_button_set_alpha( button, a);

#endif

}

/********************************
* Meg_ColorButton_SetColor
*
*
*/
void Meg_ColorButton_GetColor( GtkColorButton * button, GdkRGBA * rgbacolour )
{
#if GTK_CHECK_VERSION(3,4,0)
	gtk_color_chooser_get_rgba( GTK_COLOR_CHOOSER(button), rgbacolour );
#elif GTK_CHECK_VERSION(3,0,0)
	gtk_color_button_get_rgba( button, rgbacolour );
#else
	GdkColor c;
	guint16 a;

	gtk_color_button_get_color( button, &c );
	a = gtk_color_button_get_alpha( button );

	rgbacolour->red = (double)c.red / 65535.0;
	rgbacolour->blue = (double)c.blue / 65535.0;
	rgbacolour->green = (double)c.green / 65535.0;
	rgbacolour->alpha = (double)a / 65535.0;

#endif

}


/********************************
* Meg_ComboText_Setup
* Design to replace gtk_combo_box_new_text
* You still have to create a combo box first
*/
void Meg_ComboFile_Setup( GtkWidget * combo_box, gchar * path, gboolean force_model )
{
	Meg_ComboText_Setup( combo_box, force_model );
	Meg_ComboFile_Scan( combo_box, path, NULL, FALSE, 0 );

}

/********************************
* Meg_ComboText_Setup
* Design to replace gtk_combo_box_new_text
* You still have to create a combo box first
*/
void Meg_ComboText_Setup( GtkWidget * combo_box, gboolean force_model )
{
	GtkCellRenderer * cell;

	if ( force_model || gtk_combo_box_get_model(GTK_COMBO_BOX(combo_box) ) == NULL )
	{
		GtkListStore *store = gtk_list_store_new( 1, G_TYPE_STRING );
		gtk_combo_box_set_model( GTK_COMBO_BOX(combo_box), GTK_TREE_MODEL(store) );
		g_object_unref(store);
	}

	cell = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start( GTK_CELL_LAYOUT(combo_box), cell, TRUE );
	gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT (combo_box), cell, "text", 0, NULL );
}

/********************************
* Meg_ComboText_GetText
* Design to replace gtk_combo_box_get_active_text
*
*/
gchar * Meg_ComboText_GetText( GtkComboBox * widget )
{
	GtkTreeIter iter;
	gchar * text = NULL;
	if ( gtk_combo_box_get_active_iter( widget, &iter) )
		gtk_tree_model_get( gtk_combo_box_get_model( widget ), &iter, 0, &text, -1 );
	if ( !text )
		return NULL;
	if ( g_ascii_strcasecmp("(None)", text) == 0 )
		return NULL;
	return text;
}

/********************************
* Meg_ComboText_SetIndex
* Looks for search in column 0
*
*/

void Meg_ComboText_SetIndex( GtkComboBox * widget, gchar * search )
{
	GtkTreeModel * model = gtk_combo_box_get_model(widget);
	GtkTreeIter iter;

	if ( gtk_tree_model_get_iter_first(model, &iter) )
	{
		do {
			gchar * key = NULL;
			gtk_tree_model_get(model, &iter, 0, &key, -1);
			if ( key && search )
			{
				if ( !g_ascii_strcasecmp(key, search) )
				{
					gtk_combo_box_set_active_iter( widget, &iter );
				}
			}
		} while ( gtk_tree_model_iter_next( model, &iter ) );
	}
}

/********************************
* Meg_ComboText_Clear
* Design to replace gtk_combo_box_append_text
*
*/
void Meg_ComboText_Clear( GtkComboBox * widget )
{
	GtkListStore * store = GTK_LIST_STORE( gtk_combo_box_get_model( widget ) );
	gtk_list_store_clear(store);
}

/********************************
* Meg_ComboText_AppendText
* Design to replace gtk_combo_box_append_text
*
*/
void Meg_ComboText_AppendText( GtkComboBox * widget, const gchar * text )
{
	GtkTreeIter iter;
	GtkListStore *store = GTK_LIST_STORE( gtk_combo_box_get_model( widget ) );

	gtk_list_store_append( store, &iter );
	gtk_list_store_set( store, &iter, 0, text, -1 );
}

/********************************
* Meg_Misc_TreeIterCompare
* Event:
* Result:
*/
gint Meg_Misc_TreeIterCompare( GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data )
{
	gint sortcol = GPOINTER_TO_INT(user_data);
	gint ret = 0;

	switch (sortcol)
	{
		case 2:
		{
			gchar *name1, *name2;

			gtk_tree_model_get(model, a, 0, &name1, -1);
			gtk_tree_model_get(model, b, 0, &name2, -1);

			if (name1 == NULL || name2 == NULL)
			{
				if (name1 == NULL && name2 == NULL)
					break; /* both equal => ret = 0 */

				ret = (name1 == NULL) ? -1 : 1;
			}
			else
			{
			ret = g_utf8_collate(name1,name2);
			}

			g_free(name1);
			g_free(name2);
			break;
		}
		case 1:
		{
			guint num1, num2;

			gtk_tree_model_get(model, a, 0, &num1, -1);
			gtk_tree_model_get(model, b, 0, &num2, -1);

			if (num1 != num2)
			{
				ret = (num1 > num2) ? 1 : -1;
			}
			break;
		}
		default:
			g_return_val_if_reached(0);
	}
	return ret;
}

/********************************
* Meg_Misc_ParentWindow
* Event:
* Result:
*/
GtkWindow * Meg_Misc_ParentWindow( GtkWidget * widget )
{
	return GTK_WINDOW( gtk_widget_get_toplevel( widget ));
}


/********************************
* Meg_Misc_SetLabel
*
*/
void Meg_Misc_SetLabel( GtkWidget *label, gchar * section, gchar * text, gchar sep )
{
	gchar * markup = g_markup_printf_escaped( "<b>%s</b>%c%s", section, sep, text );
	gtk_label_set_markup( GTK_LABEL(label), markup );
	g_free( markup );
}


/********************************
* Meg_Misc_SetLabel_Print
*
*/
void Meg_Misc_SetLabel_Print( GtkWidget * label, const gchar * format, ... )
{
	gchar * markup;
	va_list args;

	va_start(args, format);
	markup = g_strdup_vprintf( format, args );

	gtk_label_set_markup( GTK_LABEL(label), markup );

	g_free( markup );
	va_end (args);

}



/********************************
* Meg_FileCopy
* File Coping with error handling
*/
gboolean Meg_FileCopy( const gchar * source, const  gchar * destination )
{
	GError * error = NULL;
	gboolean successful = FALSE;
	GFile * source_file = g_file_new_for_path(source);
	GFile * destination_file = g_file_new_for_path(destination);

	g_print("Meg_FileCopy: %s to %s\n", source, destination );
	successful = g_file_copy( source_file, destination_file, G_FILE_COPY_TARGET_DEFAULT_PERMS, NULL, NULL, NULL, &error );

	Meg_Error_Check(error, FALSE, __func__);


	g_object_unref(source_file);
	g_object_unref(destination_file);

	return successful;

}

/********************************
* Meg_DirectoryCopy
* File Coping with error handling
*/
gboolean Meg_DirectoryCopy( const gchar * source, const  gchar * destination )
{
	GError * error = NULL;
	gboolean successful = FALSE;
	GFile * source_file = g_file_new_for_path(source);
	GFile * destination_file = g_file_new_for_path(destination);

	GFileInfo * file_info;
	GFileEnumerator * files = g_file_enumerate_children( source_file, "*", G_FILE_QUERY_INFO_NONE, NULL, NULL);

	gchar * next_source_path = NULL;
	gchar * next_destination_path = NULL;

	g_print("Meg_DirectoryCopy: %s to %s\n", source, destination );

	for (file_info = g_file_enumerator_next_file(files, NULL, NULL); file_info != NULL; file_info = g_file_enumerator_next_file(files, NULL, NULL) )
	{
		GFileType type = g_file_info_get_file_type(file_info);

		if ( type == G_FILE_TYPE_DIRECTORY)
		{
			next_source_path = g_build_path( G_DIR_SEPARATOR_S, source, g_file_info_get_name(file_info), NULL );
			next_destination_path = g_build_path( G_DIR_SEPARATOR_S, destination, g_file_info_get_name(file_info), NULL );

			successful = g_mkdir_with_parents( next_destination_path, 0755 );
			Meg_DirectoryCopy( next_source_path, next_destination_path );

			g_free(next_source_path);
			g_free(next_destination_path);

		}
		else if ( type == G_FILE_TYPE_REGULAR )
		{
			next_source_path = g_build_path( G_DIR_SEPARATOR_S, source, g_file_info_get_name(file_info), NULL );
			next_destination_path = g_build_path( G_DIR_SEPARATOR_S, destination, g_file_info_get_name(file_info), NULL );

			successful = Meg_FileCopy( next_source_path, next_destination_path );
			//

			g_free(next_source_path);
			g_free(next_destination_path);

		}


	}

	Meg_Error_Check(error, FALSE, __func__);


	g_object_unref(source_file);
	g_object_unref(destination_file);

	return successful;
}

/********************************
* Meg_Preference_ClearFile
* Event: Clear button pressed on preference dialog
* Result: clears the file chooser.
*/
void Meg_Preference_ClearFile( GtkButton * button, GtkFileChooser * widget )
{
	gtk_file_chooser_set_filename( widget, NULL );
}

/********************************
* Meg_Preference_ClearFile
*/
GKeyFile * Meg_Preference_Storage()
{
	return meg_pref_storage;
}

/********************************
* Meg_RunProgram
* Event:
* Result:
*/
gboolean Meg_RunProgram( gchar * program, ...)
{
	va_list argptr;
	va_start( argptr, program );

	GError * error = NULL;
	/*gchar * output = NULL, * errors = NULL;*/
	gchar ** argv = g_new0(char* , 3);
	argv[0] = NULL;
	if ( !g_ascii_strcasecmp(program, "TEXT_EDITOR") )
	{
		argv[0] = g_key_file_get_value( meg_pref_storage, "path", "texteditor", NULL );
		argv[1] = va_arg(argptr, char*);
		argv[2] = NULL;
	}
	else if ( !g_ascii_strcasecmp(program, "IMAGE_EDITOR") )
	{
		argv[0] = g_key_file_get_value( meg_pref_storage, "path", "imageeditor", NULL );
		argv[1] = va_arg(argptr, char*);
		argv[2] = NULL;
	}

	va_end( argptr );

	if ( argv[0] )
	{
		if ( g_file_test(argv[0], G_FILE_TEST_IS_EXECUTABLE) )
		{
			g_spawn_async(Meg_Directory(), argv, NULL, (GSpawnFlags)0, NULL, NULL, NULL, &error);

			if ( error )
			{
				Meg_Error_Print(  (char*)__func__, __LINE__, "Can't spawn program: %s",  error->message);
				g_clear_error(&error);
			}
			return TRUE;
		}
	}
	return FALSE;
}

/********************************
* Meg_Dialog_ButtonClose
*
*/
void Meg_Dialog_ButtonClose( GtkButton * button, gpointer user_data )
{
	gtk_widget_destroy( gtk_widget_get_toplevel( GTK_WIDGET(button) ) );

}


/********************************
* Meg_String_ReplaceFileExtension
*
*/
gchar * Meg_String_ReplaceFileExtension( gchar * string, gchar * orignal_ext, const gchar * new_ext  )
{
	gchar * new_string = NULL;
	GRegex * regex = g_regex_new( orignal_ext, 0, 0, NULL );

	new_string = g_regex_replace_literal( regex, string, -1, 0, new_ext, 0, NULL );

	g_regex_unref( regex );

	return new_string;
}

/********************************
* CellRender_FixToggledEvent
* Fixes toggle button on download list
@ cell:
@ path_str:
@ store: ListStore to update
*/
void CellRender_Event_FixToggled( GtkCellRendererToggle *cell, gchar * path_str, GtkListStore * store )
{
	gboolean value;
	GtkTreeIter iter;
	GtkTreePath * path = gtk_tree_path_new_from_string( path_str );

	gtk_tree_model_get_iter( GTK_TREE_MODEL(store), &iter, path );
	gtk_tree_model_get( GTK_TREE_MODEL(store), &iter, 0, &value, -1 );
	value ^= 1;

	gtk_list_store_set( store, &iter, 0, value, -1); /* set new value */
	gtk_tree_path_free( path ); /* clean up */
}

/********************************
* ToggleButton_Event_WidgetSensitive
*
@ button:
@ widget:
*/
void ToggleButton_Event_WidgetSensitive( GtkToggleButton * button, GtkWidget * widget )
{
	if ( gtk_toggle_button_get_active(button) )
	{
		gtk_widget_set_sensitive(widget, TRUE);
	}
	else
	{
		gtk_widget_set_sensitive(widget, FALSE);
	}
}


/********************************
* TextView_GetText
*
@ text_view:
*/
gchar * TextView_GetText( const GtkWidget * text_view )
{
	GtkTextIter start, end;
	GtkTextBuffer * buffer = NULL;

	buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(text_view) );
	gtk_text_buffer_get_bounds( buffer, &start, &end );
	return gtk_text_buffer_get_text( buffer, &start, &end, FALSE );
}


/********************************
* Meg_FileCRC
*
@ filename:
*/
gulong Meg_FileCRC( const gchar * filename )
{
	guchar buffer[1024];
	gulong crc = crc32(0L, Z_NULL, 0);

	FILE * file = g_fopen( filename, "rb" );
	if ( file )
	{
		while ( fread( buffer, 1, 1024, file) != EOF )
		{
			crc = crc32(crc, buffer, 1024);
		}
	}

	fclose(file);

	g_print("Meg_FileCRC: %s %lu\n", filename, crc);

	return crc;
}



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
#include <glib/gstdio.h>
#include "language.h"

/* Required Headers */

/* External Functions */

/* Local Type */

/* Global Variables */
extern gchar * mokoiBasePath;

/* Local Variables */
MokoiLanguage mokoiCurrentLanguage = {NULL, NULL, NULL};
MokoiLanguage mokoiDefaultString = {NULL, NULL, NULL};
MokoiLanguage mokoiDefaultDialog = {NULL, NULL, NULL};

/* UI */
#include "ui/language_add.gui.h"
const gchar * mokoiUI_LanguageAdd = GUILANGUAGE_ADD

/* Events */

/********************************
* AL_Language_Load
*
*/
void AL_Language_Load( )
{
	Language_LoadFile( &mokoiDefaultString, "lang", "00.txt", TRUE );
	Language_LoadFile( &mokoiDefaultDialog, "dialog", "00.txt", TRUE );
}


/********************************
* AL_Language_Clear
*
*/
void AL_Language_Clear( )
{
	Language_Clear( &mokoiCurrentLanguage );
	Language_Clear( &mokoiDefaultString );
	Language_Clear( &mokoiDefaultDialog );
}


/********************************
* AL_Language_Types
*
@ list:
*/
void AL_Language_Types( GtkListStore * list )
{
	g_return_if_fail( mokoiBasePath != NULL );

	GtkTreeIter iter;
	char ** i;

	gtk_list_store_clear( list );

	char ** lang_files = PHYSFS_enumerateFiles("lang");
	for (i = lang_files; *i != NULL; i++)
	{
		if ( g_str_has_suffix( *i, ".txt" ) )
		{
			gtk_list_store_append( list, &iter );
			gtk_list_store_set( list, &iter, 0, g_build_filename("lang", *i, NULL), -1);
		}
	}
	PHYSFS_freeList(lang_files);

	char ** dialog_files = PHYSFS_enumerateFiles("dialog");
	for (i = dialog_files; *i != NULL; i++)
	{
		if ( g_str_has_suffix( *i, ".txt" ) )
		{
			gtk_list_store_append( list, &iter );
			gtk_list_store_set( list, &iter, 0, g_build_filename("dialog", *i, NULL), -1);
		}
	}
	PHYSFS_freeList(dialog_files);

}


/********************************
* AL_Language_Add
*
@ list:
*/
gboolean AL_Language_Add( gchar * file )
{
	g_return_if_fail( mokoiBasePath != NULL );

	GtkWidget * dialog, * entry;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create( mokoiUI_LanguageAdd, __func__, __LINE__ );
	g_return_if_fail( ui );


	dialog = GET_WIDGET( ui, "mokoi_language_add" );
	entry = GET_WIDGET( ui, "entry1" );

	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ) );
	gtk_window_set_transient_for( GTK_WINDOW(dialog), Meg_Main_GetWindow() );

	switch ( gtk_dialog_run( GTK_DIALOG(dialog) ) )
	{
		case 1:
		{
			const gchar * entity = gtk_entry_get_text( GTK_ENTRY(entry) );
			gchar * lang = g_strdup_printf( "%s.txt", entity );
			guint length = g_utf8_strlen( entity, -1);

			if ( length == 2 && g_ascii_strcasecmp( lang, "00.txt" )  )
			{
				gchar * file1 = NULL, * file2 = NULL;
				gchar * dialog = NULL, * string = NULL;

				file1 = g_build_filename( mokoiBasePath, "dialog", "00.txt", NULL );
				file2 = g_build_filename( mokoiBasePath, "dialog", lang, NULL );

				if ( g_file_get_contents( file1, &dialog, NULL, NULL ) )
					g_file_set_contents( file2, dialog, -1, NULL );
				else
					g_file_set_contents( file2, "Edit Me", -1, NULL);

				g_free( file1 );
				g_free( file2 );

				file1 = g_build_filename( mokoiBasePath, "lang", "00.txt", NULL );
				file2 = g_build_filename( mokoiBasePath, "lang", lang, NULL );

				if ( g_file_get_contents( file1, &string, NULL, NULL ) )
					g_file_set_contents( file2, string, -1, NULL );
				else
					g_file_set_contents( file2, "Edit Me", -1, NULL);

				g_free( dialog );
				g_free( string );
				g_free( file1 );
				g_free( file2 );
			}
			g_free( lang );
			break;
		}
		default:
			break;
	}
	gtk_widget_destroy(dialog);
	return FALSE;
}

/********************************
* AL_Language_RemoveFile
*
*
*/
gboolean AL_Language_Remove(gchar * file)
{
	gchar * lang = g_path_get_basename( file );

	if ( !g_str_has_suffix( lang, "/00.txt" ) )
	{
		gchar * file1 = NULL, * file2 = NULL;
		file1 = g_build_filename( mokoiBasePath, file, NULL );
		file2 = g_strconcat(file1, ".remove", NULL);

		g_print("renaming '%s' as '%s'\n ",file1, file2);
		g_rename(file1, file2);
		free(file1);
		free(file2);
		return TRUE;
	}
	return FALSE;
}

/********************************
* AL_String_List
*
@ file:
@ list:
*/
void AL_String_List( gchar * file, GtkListStore * list )
{
	gchar * lang = g_path_get_basename( file );
	gchar * type = g_path_get_dirname( file );


	gtk_list_store_clear(list);


	Language_LoadFile( &mokoiCurrentLanguage, type, lang, FALSE );

	if ( g_ascii_strcasecmp(type, "lang") == 0 )
	{
		if ( g_hash_table_size( mokoiDefaultString.values ) )
		{
			g_hash_table_foreach( mokoiDefaultString.values, (GHFunc)Language_TableList, list );
		}
	}
	else
	{
		if ( g_hash_table_size( mokoiDefaultDialog.values ) )
		{
			g_hash_table_foreach( mokoiDefaultDialog.values, (GHFunc)Language_TableList, list );
		}
	}
}


/********************************
* AL_String_Get
*
*
*/
gchar * AL_String_Get( gchar * file, guint id )
{
	gchar * text = NULL;
	text = g_hash_table_lookup( mokoiDefaultString.values, GINT_TO_POINTER(id) );
	return text;
}

/********************************
* AL_String_Set
*
*
*/
gboolean AL_String_Set( gchar * file, guint id, gchar * string )
{
	g_print( "language_file = %s; id: ", file, id );

	Language_Set_String( mokoiCurrentLanguage.values, id, string );

	/* Check if it's a default File */
	if ( g_str_has_suffix( mokoiCurrentLanguage.file, "00.txt" ) )
	{
		if ( g_ascii_strcasecmp(mokoiCurrentLanguage.type, "lang") == 0 )
		{
			Language_Set_String( mokoiDefaultString.values, id, string );
			g_hash_table_replace( mokoiDefaultString.values, GUINT_TO_POINTER(id), g_strdup(string) );
		}
		else
		{
			Language_Set_String( mokoiDefaultDialog.values, id, string );
		}
	}

	Language_SaveFile( &mokoiCurrentLanguage, mokoiCurrentLanguage.file );
	return FALSE;
}

/********************************
* AL_String_Add
*
@ list:
*/
gboolean AL_String_Add( GtkListStore * list )
{
	GtkTreeIter iter;
	gchar * default_string = NULL;
	gchar * language_string = "New String";
	guint key = 0;

	if ( !mokoiCurrentLanguage.file || !mokoiCurrentLanguage.type )
	{
		Meg_Error_Print( __func__, __LINE__, "Language file not loaded" );
		return FALSE;
	}

	if ( g_ascii_strcasecmp(mokoiCurrentLanguage.type, "lang") == 0 )
	{
		key = g_hash_table_size( mokoiDefaultString.values );
		default_string = g_strdup_printf("%s-%d", g_ascii_strup(mokoiDefaultString.type, -1), key);
		g_hash_table_insert( mokoiDefaultString.values, GUINT_TO_POINTER(key), default_string );
		Language_SaveFile( &mokoiDefaultString, mokoiDefaultString.file );
	}
	else
	{
		key = g_hash_table_size( mokoiDefaultDialog.values );
		default_string = g_strdup_printf("%s-%d", g_ascii_strup(mokoiDefaultDialog.type, -1), key);
		g_hash_table_insert( mokoiDefaultDialog.values, GUINT_TO_POINTER(key), default_string);
		Language_SaveFile( &mokoiDefaultDialog, mokoiDefaultDialog.file );
	}

	/* Make sure we aren't add it the default langauge again */
	if ( !g_str_has_suffix( mokoiCurrentLanguage.file, "00.txt" ) )
	{
		g_hash_table_insert( mokoiCurrentLanguage.values, GUINT_TO_POINTER(key), g_strdup(language_string) );
		Language_SaveFile( &mokoiCurrentLanguage, mokoiCurrentLanguage.file );
	}



	Language_ExportRoutines( );

	if ( list )
	{
		gtk_list_store_append( list, &iter );
		gtk_list_store_set( list, &iter, 0, key, 1, default_string, 2, language_string, -1 );
	}


	return TRUE;
}



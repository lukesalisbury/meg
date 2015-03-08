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
#include "language.h"


/* Required Headers */
#include <string.h>

/* Global Variables */
extern gchar * mokoiBasePath;
extern GError * mokoiError;
extern MokoiLanguage mokoiDefaultString;
extern MokoiLanguage mokoiDefaultDialog;
extern MokoiLanguage mokoiCurrentLanguage;

/* Local Variables */


gchar * escape_newline( gchar * haystack )
{
	GRegex * regex = g_regex_new( "\n", 0, 0, NULL );
	gchar * new_string = g_regex_replace_literal( regex, haystack, -1, 0, "\\n", 0, NULL );

	g_regex_unref( regex );

	return new_string;
}

/********************************
* Language_Set_String
*
*
*/
void Language_Set_String( GHashTable * table, guint id, const gchar * string )
{
	g_hash_table_replace( table, GUINT_TO_POINTER(id), g_strdup(string) );
}

/********************************
* Language_LoadFile
*
*
*/
guint Language_Size()
{
	if ( mokoiDefaultString.values )
	{
		return g_hash_table_size(mokoiDefaultString.values);
	}
	return 0;
}

/********************************
* Language_Clear
*
*
*/
void Language_Clear( MokoiLanguage  * details )
{
	if ( details->file )
	{
		g_free(details->file);
		details->file = NULL;
	}
	if ( details->type )
	{
		g_free(details->type);
		details->type = NULL;
	}
	if ( details->values )
	{
		if ( g_hash_table_size(details->values) )
		{
			g_hash_table_destroy(details->values);
			details->values = NULL;
		}
	}
}

/********************************
* Language_LoadFile
*
*
*/
void Language_LoadFile( MokoiLanguage * file, gchar  * type, gchar  * lang, gboolean is_default )
{
	gchar * content;
	gchar ** content_lines;
	guint lc = 0;

	Language_Clear(file);
	file->type  = g_strdup(type);
	file->file  = g_build_filename(mokoiBasePath, type, lang, NULL);
	file->values = g_hash_table_new( NULL, NULL );

	if ( !g_file_get_contents(file->file, &content, NULL, NULL ) )
	{
		if ( is_default )
		{
			/* Not Default Found, Create New One */
			content = "Edit Me";
		}
		else
		{
			/* Save Default as a new One */
			gchar * default_file  = g_build_filename(mokoiBasePath, type, "00.txt", NULL);
			g_file_get_contents(default_file, &content, NULL, NULL );
			g_free(default_file);
		}
		g_file_set_contents(file->file, content, -1, NULL);
	}

	content_lines = g_strsplit_set(content, "\n", -1);
	if ( g_strv_length(content_lines) )
	{
		while(content_lines[lc] != NULL)
		{
			g_hash_table_insert(file->values, GINT_TO_POINTER(lc), g_strdup( content_lines[lc]) );
			lc++;
		}
	}
	g_strfreev(content_lines);
	g_free(content);
}


/********************************
* Language_TableList
*
*
*/
void Language_TableList( gpointer key_p, gchar * value, GtkListStore * list )
{
	GtkTreeIter iter;
	gchar * translated_string;
	guint key = GPOINTER_TO_UINT(key_p);
	translated_string = (gchar*)g_hash_table_lookup(mokoiCurrentLanguage.values, key_p);
	gtk_list_store_append (list, &iter);

	if ( translated_string )
	{
		gtk_list_store_set (list, &iter, 0, key, 1, value, 2, translated_string, -1);
	}
	else
	{
		gtk_list_store_set (list, &iter, 0, key, 1, value, 2, value, -1);
	}
}

/********************************
* Language_StringForeachList
*
*
*/
void Language_StringForeachList( gpointer key, gchar* value, GString * string )
{
	g_string_append_printf( string, "%s\n", value);
}

#define allowchars "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_"
/********************************
* Language_ExportRoutines
*
*
*/
void Language_ExportRoutines( )
{
	gchar * content = NULL;
	GString * str_content = g_string_new("");
	gint lc = 0;

	/* Scans dialog */
	if ( Meg_file_get_contents("/dialog/00.txt", &content, NULL, NULL ) )
	{
		lc = 0;
		gchar ** content_lines;
		content_lines = g_strsplit_set(content, "\n", -1);
		if ( g_strv_length(content_lines) )
		{
			while(content_lines[lc] != NULL)
			{
				gchar * string_name = g_ascii_strup( g_strcanon( g_strstrip(content_lines[lc]), allowchars,'_' ), -1);

				g_string_append_printf(str_content, "#define DIALOG_%s %d\n", string_name, lc);

				g_free(string_name);
				lc++;
			}
		}
		g_strfreev(content_lines);
	}
	g_free(content);

	/* Scans strings */
	if ( Meg_file_get_contents("/lang/00.txt", &content, NULL, NULL ) )
	{
		lc = 0;
		gchar ** content_lines;
		content_lines = g_strsplit_set(content, "\n", -1);
		if ( g_strv_length(content_lines) )
		{
			while(content_lines[lc] != NULL)
			{
				gchar * string_name = g_ascii_strup( g_strcanon( g_strstrip(content_lines[lc]), allowchars,'_' ), -1);
				g_string_append_printf(str_content, "#define LANG_%s %d\n", string_name, lc);
				g_free(string_name);
				lc++;
			}
		}
		g_strfreev(content_lines);
	}
	g_free(content);


	Meg_file_set_contents("/scripts/routines/private_language.inc", str_content->str, -1, NULL );
	g_string_free( str_content, TRUE );

}



/********************************
* Language_SaveFile
*
*
*/
void Language_SaveFile( MokoiLanguage * details, gchar * filename )
{
	GString * content = g_string_new("");
	guint size = 0;
	guint c;
	gchar * value = NULL;

	if ( g_ascii_strcasecmp(details->type, "lang") == 0 )
	{
		size = g_hash_table_size( mokoiDefaultString.values );
	}
	else
	{
		size = g_hash_table_size( mokoiDefaultDialog.values );
	}

	for (c = 0; c < size; c++)
	{
		value = g_hash_table_lookup( details->values, GINT_TO_POINTER(c) );
		if ( !value )
		{
			if ( g_ascii_strcasecmp(details->type, "lang") == 0 )
			{
				value = g_hash_table_lookup( mokoiDefaultString.values, GINT_TO_POINTER(c) );
			}
			else
			{
				value = g_hash_table_lookup( mokoiDefaultDialog.values, GINT_TO_POINTER(c) );
			}
		}

		gchar * compressed = escape_newline( value );
		g_string_append_printf( content, "%s\n", compressed );
		g_free( compressed );
	}


	g_file_set_contents( filename, g_strstrip(content->str), -1, &mokoiError);
	if ( mokoiError )
	{
		Meg_Error_Print( __func__, __LINE__, "File could not be saved. Reason: %s", mokoiError->message );
		g_clear_error( &mokoiError );
	}
	g_string_free( content, TRUE );
}

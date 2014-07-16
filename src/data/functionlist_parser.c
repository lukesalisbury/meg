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


/* Functions local */
void funclist_start_updates(GMarkupParseContext *context, const gchar *funclist_name, const gchar **attribute_names, const gchar **attribute_values, gpointer data, GError **error);
void funclist_end_updates( GMarkupParseContext *context, const gchar *funclist_name, gpointer data, GError **error);
void funclist_text_updates( GMarkupParseContext *context, const gchar *text, gsize text_len, gpointer data, GError **error);

/* Global Variables */
GSList * mokoiFunctionDatabase = NULL; //<EditorDatabaseListing *>
GHashTable * mokoiFunctionFiles = NULL; //< EditorDatabaseListing *>

/* Local Variables */
static GMarkupParser funclist_parser = { funclist_start_updates, funclist_end_updates, funclist_text_updates, NULL, NULL};
gint funclist_mode = 0;

/*
<function name="SetBit">
	<summary></summary>
	<param name="&v">Value to change.</param>
	<param name="n">Bit position to change.</param>
</function>
typedef struct
{
	gchar * name;
	gchar * arguments;
	gchar * info;
	gpointer user_data;
} EditorDatabaseListing;
*/

/* Functions */

void funclist_start_updates(GMarkupParseContext * context, const gchar *funclist_name, const gchar **attribute_names, const gchar **attribute_values, gpointer data, GError ** error)
{
	GSList ** local_listing = data;
	if ( g_ascii_strcasecmp( funclist_name, "function" ) == 0 )
	{


		EditorDatabaseListing * function_listing = g_new0(EditorDatabaseListing, 1);

		mokoiFunctionDatabase = g_slist_prepend( mokoiFunctionDatabase, function_listing );
		*local_listing = g_slist_prepend( *local_listing, function_listing );

		for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
		{
			if ( g_ascii_strcasecmp( *attribute_names, "name" ) == 0 )
			{
				function_listing->name = g_strdup(*attribute_values);
			}
		}

		funclist_mode = 1;
	}
	else if ( g_ascii_strcasecmp( funclist_name, "summary" ) == 0 )
	{
		funclist_mode = 2;
	}
	else if ( g_ascii_strcasecmp( funclist_name, "param" ) == 0 )
	{
		/* Add New Argument */

		if ( !mokoiFunctionDatabase )
			return;

		EditorDatabaseListing * function_listing = (EditorDatabaseListing * )mokoiFunctionDatabase->data;
		EditorDatabaseListing * argument = g_new0( EditorDatabaseListing, 1);

		function_listing->arguments = g_list_append(function_listing->arguments, argument);

		for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
		{
			if ( g_ascii_strcasecmp( *attribute_names, "name" ) == 0 )
			{
				argument->name = g_strdup(*attribute_values);
			}
			else if ( g_ascii_strcasecmp( *attribute_names, "type" ) == 0 )
			{
				argument->arguments_string = g_strdup(*attribute_values);
			}
			else if ( g_ascii_strcasecmp( *attribute_names, "default" ) == 0 )
			{
				argument->user_data = (gpointer)g_strdup(*attribute_values);
			}
		}
		funclist_mode = 3;
	}

}

void funclist_end_updates( GMarkupParseContext *context, const gchar *funclist_name, gpointer data, GError **error )
{
	funclist_mode = 0;
	if ( g_ascii_strcasecmp( funclist_name, "function" ) == 0 )
	{
		/* Write Arguments to string */
		EditorDatabaseListing * listing = (EditorDatabaseListing *)(mokoiFunctionDatabase->data);
		GList * scan = (GList *)listing->user_data;
		GString * buff_string = g_string_new("(");

		while ( scan )
		{
			EditorDatabaseListing * argument = (EditorDatabaseListing *)scan->data;
			g_string_append( buff_string, argument->name );
			scan = g_list_next(scan);
			if ( scan )
			{
				g_string_append( buff_string, ", " );
			}
		}
		g_string_append( buff_string, ")" );
		listing->arguments_string = g_strdup(buff_string->str);
		g_string_free( buff_string, TRUE );
	}
}

void funclist_text_updates( GMarkupParseContext *context, const gchar *text, gsize text_len, gpointer data, GError **error )
{
	if ( mokoiFunctionDatabase )
	{
		EditorDatabaseListing * function_listing = (EditorDatabaseListing *)(mokoiFunctionDatabase->data);
		if ( function_listing )
		{
			if ( text_len )
			{
				if ( funclist_mode == 1)
				{
					function_listing->info = g_strdup(text);
				}
				else if ( funclist_mode == 2)
				{
					GList * arr = g_list_last( function_listing->arguments );
					if ( arr )
					{
						EditorDatabaseListing * argument = (EditorDatabaseListing *)arr->data;
						argument->info = g_strstrip(g_strdup(text));
					}
				}
			}
		}
	}
}

/********************************
* Funclist_Scan
*
*/
GString * Funclist_GetFile( const gchar * file )
{
	/* Read the file */
	gchar * path = Meg_Directory_Share("include");
	gchar * header_path = g_build_filename( path, file, NULL );
	gboolean validtext = FALSE;
	gchar * content = NULL;
	guint lc = 0;
	GString * function_xml = g_string_new("");

	if ( g_file_get_contents(header_path, &content, NULL, NULL ) )
	{
		gchar ** content_lines = g_strsplit_set(content, "\n", -1);
		if ( g_strv_length(content_lines) )
		{
			while(content_lines[lc] != NULL)
			{
				if ( !g_ascii_strcasecmp( content_lines[lc], "/***" ) )
				{
					validtext = TRUE;
				}
				else if ( !g_ascii_strcasecmp( content_lines[lc], "***/" ) )
				{
					validtext = FALSE;
				}
				else
				{
					if ( validtext )
					{
						function_xml = g_string_append( function_xml, content_lines[lc] );
						function_xml = g_string_append( function_xml, "\n" );
					}
				}
				lc++;
			}
		}
		g_strfreev(content_lines);
	}
	g_free(content);

	g_free(header_path);
	g_free(path);

	return function_xml;
}
/********************************
* Funclist_Scan
*
*/
void Funclist_ScanFile( const gchar * name )
{
	/* Scan XML */
	GSList * local_functions = NULL;
	GString * function_xml = Funclist_GetFile( name );
	GError * error = NULL;
	GMarkupParseContext * ctx;
	if ( function_xml->len > 0 )
	{
		ctx = g_markup_parse_context_new( &funclist_parser, (GMarkupParseFlags)0, &local_functions, NULL );
		g_markup_parse_context_parse( ctx, function_xml->str, -1, &error );
		if ( error )
		{
			g_warning("XML Parse Error: %s", error->message);
			g_clear_error(&error);
		}
		g_markup_parse_context_end_parse( ctx, &error );
		if ( error )
		{
			g_warning("XML Parse Error: %s", error->message);
			g_clear_error(&error);
		}
		g_markup_parse_context_free( ctx );

		/* */
		if ( g_slist_length(local_functions) )
			g_hash_table_insert( mokoiFunctionFiles, g_strdup(name), local_functions );
	}

}

/**
 * @brief Funclist_GetHashTable
 * @return
 */
GHashTable * Funclist_GetHashTable()
{
	return mokoiFunctionFiles;
}

/********************************
* Funclist_Scan
*
*/
gboolean Funclist_Scan( )
{
	GDir * dir;
	gchar * path;
	const gchar * current;


	mokoiFunctionFiles = g_hash_table_new_full( g_str_hash, g_str_equal, g_free, NULL );

	path = Meg_Directory_Share("include");
	dir = g_dir_open(path, 0, NULL);
	current = g_dir_read_name(dir);
	while ( current != NULL )
	{
		if ( g_str_has_suffix(current, ".inc") )
		{
			Funclist_ScanFile( current );

		}
		current = g_dir_read_name(dir);
	}
	g_dir_close( dir );
	g_free( path );

	return FALSE;
}




/**
 * @brief Funclist_FreeArgument
 * @param data
 */
void Funclist_FreeArgument( gpointer data )
{
	if ( data )
	{
		EditorDatabaseListing * argument = (EditorDatabaseListing *)data;
		CLEAR_STRING( argument->arguments_string );
		CLEAR_STRING( argument->info );
		CLEAR_STRING( argument->name );
		CLEAR_STRING( argument->user_data );
		g_list_free_full( argument->arguments, Funclist_FreeArgument );

		g_free( argument );
	}
}


/**
 * @brief Funclist_Free
 */
void Funclist_Free( )
{
	g_slist_free_full( mokoiFunctionDatabase, Funclist_FreeArgument );

	g_hash_table_destroy( mokoiFunctionFiles );

	mokoiFunctionFiles = NULL;
	mokoiFunctionDatabase = NULL;

}

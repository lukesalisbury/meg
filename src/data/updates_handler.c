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
#include "import_functions.h"

/* Required Headers */

/* Functions local */
void element_start_updates( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, gpointer data, GError **error);
void element_end_updates( GMarkupParseContext *context, const gchar *element_name, gpointer data, GError **error);
void element_text_updates( GMarkupParseContext *context, const gchar *text, gsize text_len, gpointer data, GError **error);


/* Global Variables */

/* Local Variables */
gint updates_parse_mode = 0;
gchar * info[5];

/* Functions */
void element_start_updates( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, gpointer data, GError **error)
{
	/* GtkListStore * store = (GtkListStore *)data; */
	if ( updates_parse_mode == 1 )
	{
		if ( g_ascii_strcasecmp( element_name, "title" ) == 0 )
		{
			updates_parse_mode = 2;
		}
		else if ( g_ascii_strcasecmp( element_name, "summary" ) == 0 )
		{
			updates_parse_mode = 4;
		}
		else if ( g_ascii_strcasecmp( element_name, "version" ) == 0 )
		{
			updates_parse_mode = 3;
		}
		else if ( g_ascii_strcasecmp( element_name, "link" ) == 0 )
		{
			gboolean is_link = FALSE;
			for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
			{
				if ( g_ascii_strcasecmp( *attribute_names, "rel" ) == 0 )
				{
					if ( g_ascii_strcasecmp( *attribute_values, "download" ) == 0 )
					{
						is_link = TRUE;
					}
				}
				if ( is_link )
				{
					if ( g_ascii_strcasecmp( *attribute_names, "href" ) == 0 )
					{
						info[1] = g_strdup(*attribute_values);
					}
				}
			}
		}
	}
	else
	{
		if ( g_ascii_strcasecmp( element_name, "entry" ) == 0 )
		{
			updates_parse_mode = 1;
		}
	}

}

void element_end_updates( GMarkupParseContext *context, const gchar *element_name, gpointer data, GError **error )
{
	GtkListStore * store = (GtkListStore *)data;

	if ( g_ascii_strcasecmp( element_name, "entry" ) == 0 )
	{
		Import_AddListing( store, info[0], info[1], "", info[2], info[3], 0, FALSE);
		g_free(info[0]);
		g_free(info[1]);
		g_free(info[2]);
		g_free(info[3]);
		updates_parse_mode = 0;
	}
	else if ( updates_parse_mode > 1 )
	{
		updates_parse_mode = 1;
	}
}

void element_text_updates( GMarkupParseContext *context, const gchar *text, gsize text_len, gpointer data, GError **error )
{
	if ( updates_parse_mode == 2 ) /* Title */
	{
		info[0] = g_strdup(text);
	}
	if ( updates_parse_mode == 3 ) /* version */
	{
		info[2] = g_strdup(text);
	}
	if ( updates_parse_mode == 4 ) /* summary */
	{
		info[3] = g_strdup(text);
	}
}






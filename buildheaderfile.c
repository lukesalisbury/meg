/****************************
Copyright © 2013-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/


#include <glib.h>
#include <glib/gprintf.h>


gchar * title_replace = NULL;
gchar * descript_replace = NULL;
gchar * icon_replace = NULL;
gint header_count = 0;

gchar * replace_string( gchar * needle, gchar * haystack, gchar * value )
{
	GRegex * regex = g_regex_new( needle, 0, 0, NULL );
	gchar * new_string = g_regex_replace_literal( regex, haystack, -1, 0, value, 0, NULL );

	g_regex_unref( regex );

	return new_string;
}


gchar * read_file( const gchar * file_name )
{
	gchar * new_content = NULL;
	gchar * content = NULL;
	gchar * new_define = NULL;

	gchar ** f = g_strsplit( file_name, ".", 2 );

	new_define = g_utf8_strup( g_path_get_basename( f[0] ), -1);

	g_file_get_contents( file_name, &content, NULL, NULL);

	if ( title_replace )
	{
		gchar * temp = replace_string( "%BUILDHEADER_GUI_TITLE%", content, title_replace );
		g_free(content);
		content = temp;
	}

	if ( descript_replace )
	{
		gchar * temp = replace_string( "%BUILDHEADER_GUI_DESCRIPT%", content, descript_replace );
		g_free(content);
		content = temp;
	}

	if ( icon_replace )
	{
		gchar * temp = replace_string( "%BUILDHEADER_GUI_ICON%", content, icon_replace );
		g_free(content);
		content = temp;
	}

	new_content = g_strconcat( "#define GUI_", new_define, " \"", g_strescape(content, NULL), "\" \n", NULL );

	g_strfreev(f);
	g_free(content);
	g_free(new_define);

	return new_content;
}




GString * scan_files( gchar * dir )
{
	GString * header_content = g_string_new("");
	gchar * full_path = NULL;
	GDir * directory = g_dir_open( dir, 0, NULL );
	if ( directory )
	{
		const gchar * file = g_dir_read_name( directory );
		while ( file != NULL )
		{
			if ( file[0] != '.')
			{
				full_path = g_build_filename( dir, file, NULL);
				if ( g_str_has_suffix(file, ".gui") )
				{
					gchar * content;
					content = read_file( full_path );
					g_string_append( header_content, content );
					g_free( content );

					header_count++;
				}
				g_free( full_path );
			}
			file = g_dir_read_name( directory );
		}
		g_dir_close( directory );
	}
	return header_content;
}


int main(int argc, char *argv[])
{
	if ( argc < 2 )
	{
		g_print("buildheaderfile.exe - missing argument");
		return 0;
	}

	GTimer * yimer = g_timer_new();

	if (argc >= 3)
	{
		title_replace = argv[3];
		//g_print("title_replace = \"%s\"\n", title_replace);
		if (argc >= 4)
		{
			descript_replace = argv[4];
			//g_print("descript_replace = \"%s\"\n", descript_replace);
		}
		if (argc >= 5)
		{
			icon_replace = argv[5];
			//g_print("icon_replace = \"%s\"\n", icon_replace);
		}

	}

	GString * content = NULL;
	gchar * output_file = g_strdup( argv[2] );

	content = scan_files( argv[1] );

	if ( content )
	{
		g_print("Writing %d header to %s. Time Taken:  %f\n", header_count, output_file, g_timer_elapsed(yimer, NULL) );
		g_file_set_contents( output_file, content->str, -1, NULL);
	}

	g_free( output_file );
	g_string_free( content, TRUE );

	return 0;
}

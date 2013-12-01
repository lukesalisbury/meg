/****************************
Copyright © 2013 Luke Salisbury
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

gchar * replace_string( gchar * needle, gchar * haystack, gchar * value )
{
	GRegex * regex = g_regex_new( needle, 0, 0, NULL );
	gchar * new_string = g_regex_replace_literal( regex, haystack, -1, 0, value, 0, NULL );

	g_regex_unref( regex );

	return new_string;
}


int main(int argc, char *argv[])
{
	if ( argc < 2 )
	{
		g_print("buildheader.exe - missing argument");
		return 1;
	}

	//g_print("buildheader.exe - Reading %s\n", argv[1]);
	if ( !g_str_has_suffix( argv[1], ".gui") )
	{
		g_print("buildheader.exe - not a GtkBuilder file");
		return 1;
	}

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




	gchar * outfile = NULL;
	gchar * content = NULL;
	gchar * new_content = NULL;
	gchar * new_define = NULL;


	if ( argc > 2)
	{
		outfile = g_strdup( argv[2] );
	}
	else
	{
		outfile = g_strconcat( argv[1], ".h", NULL );
	}
	//g_print("buildheader.exe - Saving '%s' to '%s'\n", argv[1], outfile);

	gchar** f = g_strsplit( argv[1], ".", 2 );
	new_define = g_utf8_strup( g_path_get_basename( f[0] ), -1);
	g_strfreev(f);

	g_file_get_contents( argv[1], &content, NULL, NULL);



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

	new_content = g_strconcat( "#define GUI", new_define, " \"", g_strescape(content, NULL), "\";\n", NULL );

	g_file_set_contents( outfile, new_content, -1, NULL);


	g_free(outfile);
	g_free(content);
	g_free(new_content);
	g_free(new_define);

	return 0;
}

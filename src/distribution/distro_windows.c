/****************************
Copyright © 2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Standard Headers */
#include "../data/loader_global.h"
#include <glib/gstdio.h>
#include <zlib.h>

/* Required Headers */

/* External Functions */

/* Local Type */

/* Global Variables */


/* Local Variables */


/* UI */




/* Functions */



gboolean Distro_WindowsCreate( const gchar * project_title, const gchar * game_path )
{
	gchar * blank_buffer = g_new(gchar, 512);
	gchar * game_buffer = NULL;
	gsize game_buffer_length = 0;
	gsize bytes_read = 0;



	gzFile binary_original;
	FILE * binary_temporary;

	gchar * shared_directory = Meg_Directory_Share("binaries");
	gchar * binary_original_path;
	gchar * binary_temporary_path;

	if ( g_file_get_contents( game_path, &game_buffer, &game_buffer_length, NULL ) )
	{
		binary_original_path = g_build_filename(shared_directory, "windows-exe.gz", NULL);
		binary_temporary_path = g_build_filename( g_get_tmp_dir(), project_title, NULL);

		binary_original = gzopen( binary_original_path, "rb" );
		binary_temporary = g_fopen( binary_temporary_path, "wb" );

		g_print( "binary_original: %p\n", binary_original );
		g_print( "binary_temporary: %p\n", binary_temporary );
		g_print("binary_original_path: %s\n", binary_original_path );
		g_print("binary_temporary_path: %s\n", binary_temporary_path );

		if ( binary_original && binary_temporary )
		{
			while ( TRUE ) // Read gzip
			{
				bytes_read = gzread( binary_original, blank_buffer, 512);
				if ( bytes_read > 0 )
				{
					fwrite( blank_buffer, bytes_read, 1, binary_temporary );
				}
				else
				{
					break;
				}

			}
			fwrite( game_buffer, game_buffer_length, 1, binary_temporary );
		}




		gzclose(binary_original);
		fclose(binary_temporary);


		g_free(binary_temporary_path);
		g_free(binary_original_path);

		return TRUE;
	}

	return FALSE;
}

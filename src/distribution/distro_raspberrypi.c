/****************************
Copyright © 2013-2014 Luke Salisbury
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
#define MINIZ_HEADER_FILE_ONLY
#include "miniz.c"

/* Required Headers */

/* External Functions */

/* Local Type */

/* Global Variables */


/* Local Variables */


/* UI */




/* Functions */



gboolean Distro_RaspberryPICreate( const gchar * project_title, const gchar * game_path )
{
	gchar * blank_buffer = g_new(gchar, 512);
	gchar * game_buffer = NULL;
	gchar * binary_buffer = NULL;
	gsize game_buffer_length = 0;
	gsize binary_buffer_length = 0;
	gsize bytes_read = 0;

	gzFile binary_original;
	FILE * binary_temporary;

	gchar * shared_directory = Meg_Directory_Share("binaries");
	gchar * binary_original_path;
	gchar * binary_temporary_path;
	gchar * target_name;
	gchar * zip_target_path;


	if ( g_file_get_contents( game_path, &game_buffer, &game_buffer_length, NULL ) )
	{
		target_name = g_strdup_printf("%s", project_title );

		binary_original_path = g_build_filename( shared_directory, "rasp-binary.gz", NULL);
		binary_temporary_path = g_build_filename( g_get_tmp_dir(), target_name, NULL);

		zip_target_path = g_strdup_printf( "%s"G_DIR_SEPARATOR_S"%s-raspberrypi.zip", Meg_Directory_Document(), project_title );


		binary_original = gzopen( binary_original_path, "rb" );
		binary_temporary = g_fopen( binary_temporary_path, "wb" );

		g_print( "binary_original: %p\n", binary_original );
		g_print( "binary_temporary: %p\n", binary_temporary );
		g_print( "binary_original_path: %s\n", binary_original_path );
		g_print( "binary_temporary_path: %s\n", binary_temporary_path );
		g_print( "zip_target_path: %s\n", zip_target_path );


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

			if ( g_file_get_contents( binary_temporary_path, &binary_buffer, &binary_buffer_length, NULL ) )
			{
				mz_zip_add_mem_to_archive_file_in_place( zip_target_path, target_name, binary_buffer, binary_buffer_length, NULL, 0, 0 );
			}
			g_free(binary_buffer);
		}
		else
		{
			g_warning("Distro_RaspberryPICreate Failed");
		}




		gzclose(binary_original);
		fclose(binary_temporary);

		g_free(game_buffer);

		g_free(zip_target_path);

		g_free(binary_temporary_path);
		g_free(binary_original_path);

		return TRUE;
	}

	return FALSE;
}

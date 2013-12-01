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
#include <gio/gio.h>

/* Required Headers */

/* External Functions */

/* Local Type */

/* Global Variables */


/* Local Variables */


/* UI */




/* Functions */



gboolean Distro_WindowsCreate( const gchar * file_name, const gchar * game_path )
{
	gchar * blank_buffer = g_new(512);
	gchar * game_buffer = NULL;
	gsize game_buffer_length = 0;
	GFile * source_file, * dest_file;
	GZlibDecompressor * file_decompressor;
	GFileOutputStream * binary_output;
	GOutputStream * decompress_output;


	gchar * shared_directory;
	gchar * binary_original_path;
	gchar * output_file_path;


	if ( g_file_get_contents( game_path, &game_buffer, &game_buffer_length, NULL ) )
	{

		output_file_path = Meg_String_ReplaceFileExtension( file_name, "."GAME_EXTENSION, ".exe" );
		shared_directory = Meg_Directory_Share("binaries");
		binary_original_path = g_build_filename(shared_directory, "windows.exe.gz");


		source_file = g_file_new_for_path( binary_original_path );
		dest_file = g_file_new_for_path( output_file_path );

		/* Extract */
		file_decompressor = g_zlib_decompressor_new( G_ZLIB_COMPRESSOR_FORMAT_GZIP );

		binary_output = g_file_replace( dest_file, NULL, FALSE, 0, NULL, NULL); // Replace Dest file

		decompress_output = g_converter_output_stream_new( file_output, G_CONVERTER(file_decompressor) );
		g_output_stream_splice( decompress_output, source_file, 0, NULL, NULL); // Write decompressed output.
		g_output_stream_close( decompress_output, NULL, NULL );

		/* Append Game File to binary. */
		g_output_stream_write( binary_output, blank_buffer, 512, NULL, NULL);
		g_output_stream_write( binary_output, game_buffer, game_buffer_length, NULL, NULL);

	}

}

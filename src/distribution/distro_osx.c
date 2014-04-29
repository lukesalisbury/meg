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
#define MINIZ_HEADER_FILE_ONLY
#include "miniz.c"

/* Required Headers */

/* External Functions */

/* Local Type */

/* Global Variables */


/* Local Variables */


/* UI */




/* Functions */



gboolean Distro_OSXCreate( const gchar * project_title, const gchar * game_path )
{
	gchar * shared_directory = Meg_Directory_Share("binaries");
	gchar * target_name;
	gchar * zip_original_path;
	gchar * zip_target_path;
	gchar * game_target_path = "MokoiGaming.app/Contents/Resources/game.data";

	target_name = g_strdup_printf("%s", project_title );
	zip_original_path = g_build_filename( shared_directory, "osx-app.zip", NULL);
	zip_target_path = g_strdup_printf( "%s"G_DIR_SEPARATOR_S"%s-osx.zip", Meg_Directory_Document(), project_title );

	g_print( "zip_original_path: %s\n", zip_original_path );
	g_print( "zip_target_path: %s\n", zip_target_path );


	Meg_FileCopy( zip_original_path, zip_target_path );

	mz_zip_archive zip;
	mz_zip_writer_init_file( &zip, zip_target_path, 0 );
	mz_zip_writer_add_file( &zip, game_target_path, game_path, NULL, 0, 9 );
	mz_zip_writer_end( &zip );


	g_free(zip_original_path);
	g_free(zip_target_path);




	return TRUE;
}

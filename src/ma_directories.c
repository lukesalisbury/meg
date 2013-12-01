/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/



/* Global Header */
#include "global.h"

/* Required Headers */


/* GtkWidget */

/* Global Variables */
gchar * program_directory = NULL;


/* External Functions */

/* UI */

/* Functions */


/********************
* Meg_Directory_Set
* Set up program directory
*/
void Meg_Directory_Set( gchar * excutable_path )
{
	program_directory = g_path_get_dirname( excutable_path );
	#ifdef G_OS_WIN32
	const gchar * paths;
	paths = g_getenv("PATH");
	g_setenv("PATH", g_strdup_printf("%s;%s", paths, program_directory), TRUE);
	#endif
}

/********************
* Meg_Directory
* Returns the directory Alchera is in.
*/
gchar * Meg_Directory()
{
	return program_directory;
}

/********************
* Meg_Directory_Data
*
*/
gchar * Meg_Directory_Data( const gchar * folder )
{
	gchar * path = g_build_path( G_DIR_SEPARATOR_S, g_get_user_data_dir(), MEGDATADIRECTORY, folder, NULL );
	g_mkdir_with_parents(path, 0755);
	return path;
}

/********************
* Meg_Directory_DataFile
*
*/
gchar * Meg_Directory_DataFile( const gchar * folder, const gchar * file )
{
	gchar * file_path = NULL;
	gchar * directory = Meg_Directory_Data( folder );

	if ( directory )
	{
		file_path = g_build_filename( directory, file, NULL );
		g_free(directory);
	}
	else
	{
		file_path = g_strdup(file);
	}

	return file_path;
}

/********************
* Meg_Directory_Document
* Return the directory that is use to stored games. It Will also create the folder if needed
*/
gchar * Meg_Directory_Document()
{
	gchar const * home = g_get_home_dir();
	gchar * document_dir = g_build_path( G_DIR_SEPARATOR_S, home, PROGRAM_DOCUMENTS, NULL );
	if ( g_mkdir_with_parents( document_dir, 0755 ) )
	{
		gchar * user_data = Meg_Directory_Data(NULL);
		Meg_Error_Print( __func__, __LINE__, "Error creating default game directory '%s'. Will use '%s' instead.", document_dir, user_data );
		g_free(document_dir);
		document_dir = user_data;
	}

	return document_dir;
}

/********************
* Meg_Directory_Global_Share
* Return the global share directory
*/
gchar * Meg_Directory_ProgramShare( gchar * path )
{
	gchar * directory = NULL;
	guint lc = 0;
	const gchar * const * data_dirs;

	data_dirs = g_get_system_data_dirs();
	while ( data_dirs[lc] != NULL && directory == NULL )
	{
		if ( g_str_has_suffix(data_dirs[lc], "share") || g_str_has_suffix(data_dirs[lc], "share/"))
		{
			gchar * check_path = g_build_path(G_DIR_SEPARATOR_S, data_dirs[lc], MEGSHAREDIRECTORY, NULL );
			if ( g_file_test(check_path, G_FILE_TEST_IS_DIR ))
			{
				gchar * include_path = g_build_path(G_DIR_SEPARATOR_S, data_dirs[lc], path, NULL );
				if ( g_file_test(include_path, G_FILE_TEST_IS_DIR ))
				{
					directory = g_strdup(include_path);
				}
				g_free(include_path);
			}
			g_free(check_path);
		}
		lc++;
	}
	if ( !directory )
	{
		gchar * include_path = g_build_path(G_DIR_SEPARATOR_S, program_directory, "..", "share", path, NULL );
		if ( g_file_test(include_path, G_FILE_TEST_IS_DIR ))
		{
			directory = g_strdup(include_path);
		}
		g_free(include_path);

	}


	return directory;

}

/********************
* Meg_Directory_Global_Share
* Return the global share directory
*/
gchar * Meg_Directory_GlobalShare( gchar * path )
{
	gchar * directory = NULL;
	guint lc = 0;
	const gchar * const * data_dirs;

	data_dirs = g_get_system_data_dirs();
	while ( data_dirs[lc] != NULL && directory == NULL )
	{
		if ( g_str_has_suffix(data_dirs[lc], "share") || g_str_has_suffix(data_dirs[lc], "share/"))
		{
			gchar * include_path = g_build_path(G_DIR_SEPARATOR_S, data_dirs[lc], path, NULL );
			if ( g_file_test(include_path, G_FILE_TEST_IS_DIR ))
			{
				directory = g_strdup(include_path);
			}
			g_free(include_path);
		}
		lc++;
	}
	if ( !directory )
	{
		gchar * include_path = g_build_path(G_DIR_SEPARATOR_S, program_directory, "..", "share", path, NULL );
		if ( g_file_test(include_path, G_FILE_TEST_IS_DIR ) )
		{
			directory = g_strdup(include_path);
		}
		g_free(include_path);

	}


	return directory;
}


/********************
* Meg_Directory_Share
* Returns the Meg's Share Directory.
*/
gchar * Meg_Directory_Share( gchar * folder )
{
	gchar * directory = NULL;
	guint lc = 0;
	const gchar * const * data_dirs;

	data_dirs = g_get_system_data_dirs();
	while ( data_dirs[lc] != NULL && directory == NULL )
	{
		if ( g_str_has_suffix(data_dirs[lc], "share") || g_str_has_suffix(data_dirs[lc], "share/") )
		{
			gchar * include_path = g_build_path(G_DIR_SEPARATOR_S, data_dirs[lc], MEGSHAREDIRECTORY, NULL );
			if ( g_file_test(include_path, G_FILE_TEST_IS_DIR ))
			{
				directory = g_build_path(G_DIR_SEPARATOR_S, data_dirs[lc], MEGSHAREDIRECTORY, folder, NULL );
			}
			g_free(include_path);
		}
		lc++;
	}
	if ( !directory )
	{
		gchar * include_path = g_build_path(G_DIR_SEPARATOR_S, program_directory,"..", "share", MEGSHAREDIRECTORY, NULL );
		if ( g_file_test(include_path, G_FILE_TEST_IS_DIR ))
		{
			directory = g_build_path(G_DIR_SEPARATOR_S, program_directory, "..", "share", MEGSHAREDIRECTORY, folder, NULL );
		}
		g_free(include_path);

	}

	return directory;
}

/********************
* Meg_Directory_Config
*
*/
gchar * Meg_Directory_Config( )
{
	gchar * path = g_build_path( G_DIR_SEPARATOR_S, g_get_user_config_dir(), MEGCONFIGDIRECTORY, NULL );
	g_mkdir_with_parents(path, 0755);
	return path;
}

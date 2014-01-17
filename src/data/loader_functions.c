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

/* Required Headers */
#include <glib/gstdio.h>
#include "section_functions.h"
#include "sheets_functions.h"

/* Global Variables */
extern gchar * mokoiBasePath;
extern GKeyFile * mokoiConfigTable;


/********************************
* Project_CleanTitle
*
@ title: string to clean up, if NULL will get it from settings
* Returns clean up project title, may return NULL
*/
gchar * Project_CleanTitle( const gchar * title )
{
	gchar * title_clean = NULL;

	if ( title )
		title_clean = g_strdup(title);
	else
		title_clean = AL_Setting_GetString( "project.title" );

	if ( title_clean )
	{
		title_clean = g_strstrip( title_clean ); //Trims Project Title
		title_clean = g_strdelimit( title_clean, "_-|> <.\\/?*", (gchar)'_' );
	}
	return title_clean;
}


/********************************
* Project_ValidDir
*
* Returns TRUE is project is loaded
*/
gboolean Project_ValidDir()
{
	return TRUE;
}

/********************************
* Project_Directory
* Return the project directory with folder added
*/
gchar * Project_Directory( gchar * path )
{
	if ( Project_ValidDir() )
	{
		return g_build_path(G_DIR_SEPARATOR_S, mokoiBasePath, path, NULL );
	}
	return NULL;
}
/********************************
* Project_Directory2
* Return the project directory with 2 folder added
*/
gchar * Project_Directory2( gchar * path,  gchar * path2 )
{
	if ( Project_ValidDir() )
	{
		return g_build_path(G_DIR_SEPARATOR_S, mokoiBasePath, path, path2, NULL );
	}
	return NULL;
}


/********************************
* Project_File
@ directory
@ file
* Return the path to file
*/
gchar * Project_File( gchar * directory, gchar * file )
{
	if ( Project_ValidDir() )
	{
		return g_build_filename(G_DIR_SEPARATOR_S, mokoiBasePath, directory, file, NULL );
	}
	return NULL;
}

/********************************
* Project_LoadSpritesheet
* Load spritesheets
*/
void Project_LoadSpritesheet()
{
	/* Spritesheet loading */
	char ** files = PHYSFS_enumerateFiles("/sprites/");
	char ** current;

	for (current = files; *current != NULL; current++)
	{
		if ( g_str_has_suffix( *current, ".png" ) )
		{
			Sheet_Parse( *current );
		}
	}
	PHYSFS_freeList(files);

}

/********************************
* Project_UnloadSpritesheet
* Load spritesheets
*/
void Project_UnloadSpritesheet()
{

	/* Spritesheet loading */
	char ** files = PHYSFS_enumerateFiles("/sprites/");
	char ** current;

	for (current = files; *current != NULL; current++)
	{
		if ( g_str_has_suffix( *current, ".png" ) )
		{
			Sheet_Unload( *current );
		}
	}
	PHYSFS_freeList(files);

}



/*-------------------------------
	Misc Functions
-------------------------------*/






/********************************
* rgba2gdkcolor
*
*/
void rgba2gdkcolor(rgbaColour * rgba, GdkRGBA * gdk)
{
	gdk->red = (double)rgba->red / 255.0;
	gdk->blue = (double)rgba->blue / 255.0;
	gdk->green = (double)rgba->green / 255.0;
	gdk->alpha = (double)rgba->alpha / 255.0;
}

/********************************
* gdk2rgbacolor
*
*/
void gdk2rgbacolor(GdkRGBA * gdk, rgbaColour * rgba)
{
	rgba->red = (guint8)(gdk->red * 255);
	rgba->blue = (guint8)(gdk->blue * 255);
	rgba->green = (guint8)(gdk->green * 255);
	rgba->alpha = (guint8)(gdk->alpha * 255);
}


/*-------------------------------
	File Pointer Helper Functions
-------------------------------*/

/********************************
* file_write_byte
* Helper function to write long int to file in BE mode;
@
*/
void file_write_byte( FILE * fd, guint8 value )
{
	fwrite(&value, sizeof(guint8), 1, fd);
}


/********************************
* file_write_short
* Helper function to write short int to file in BE mode;
@
*/
void file_write_short(FILE * fd, guint16 value )
{
	gushort v = GINT16_TO_BE(value);
	fwrite(&v, sizeof(guint16), 1, fd);
}

/********************************
* file_write_long
* Helper function to write long int to file in BE mode;
@
*/
void file_write_long( FILE * fd, guint32 value )
{
	guint32 v = GINT32_TO_BE(value);
	fwrite(&v, sizeof(guint32), 1, fd);
}

/********************************
* file_read_long
* Helper function to write long int to file in BE mode;
@
*/
guint32 file_read_long( FILE * fd )
{
	guint32 v = 0;
	if ( fread(&v, sizeof(guint32), 1, fd)  )
	{
		return GINT32_FROM_BE(v);
	}
	return 0;
}

/********************************
* file_read_short
* Helper function to write long int to file in BE mode;
@
*/
guint16 file_read_short( FILE * fd )
{
	guint16 v = 0;
	if ( fread(&v, sizeof(guint16), 1, fd) )
	{
		return GINT16_FROM_BE(v);
	}
	return 0;
}

/********************************
* file_read_byte
* Helper function to write long int to file in BE mode;
@
*/
guint8 file_read_byte( FILE * fd )
{
	guint8 v = 0;
	if ( fread(&v, sizeof(guint8), 1, fd) )
	{
		return v;
	}
	return 0;
}

/********************************
*
*
*/
guint file_get_size( gchar * file )
{
	struct stat buf;
	if ( !g_stat( file, &buf ) )
	{
		return buf.st_size;
	}
	return 0;
}

/********************************
*
*
*/
time_t file_get_date( gchar * file )
{
	struct stat buf;
	if ( !g_stat( file, &buf ) )
	{
		return buf.st_mtime;
	}
	return 0;
}

/********************************
*
*
*/
time_t physfs_file_get_date( gchar * file )
{
	return (time_t)PHYSFS_getLastModTime(file);
	/*
	PHYSFS_Stat buf;
	if ( !PHYSFS_stat( file, &buf ) )
	{
		return (time_t)buf.modtime;
	}
	return 0;
	*/
}

/********************************
*
*
*/
guint physfs_file_get_size( gchar * file )
{
	PHYSFS_sint64 file_size = 0;
	PHYSFS_File * src_file = PHYSFS_openRead(file);
	if ( src_file )
	{
		file_size = PHYSFS_fileLength(src_file);
	}
	PHYSFS_close(src_file);
	return (guint)file_size;
	/*
	PHYSFS_Stat buf;
	if ( !PHYSFS_stat( file, &buf ) )
	{
		return (time_t)buf.modtime;
	}
	return 0;
	*/
}


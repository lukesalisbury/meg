/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Global Header */
#include "global.h"
#include <glib/gstdio.h>
#include "physfs/physfs.h"
#include <string.h>

/* Global Variables */


/* UI */



/********************************
* PHYSFS_writeString
*
*/
void PHYSFS_writeString( PHYSFS_File * file,  const gchar * format, ... )
{
	gchar * log;
	gint length = 0;
	va_list argptr;

	va_start( argptr, format );
	length =  g_vasprintf ( &log, format, argptr );
	va_end( argptr );

	if ( length )
		PHYSFS_write(file, log, 1, length );
	g_free(log);
}

/********************************
* PHYSFS_rename
* NO ERROR CHECKING
*/
void PHYSFS_rename(  const gchar * original_path, const gchar * new_path )
{
	gchar * contents;
	gsize length;

	Meg_file_get_contents(original_path, &contents, &length, NULL);
	Meg_file_set_contents(new_path, contents, length, NULL);
}


/********************************
* PHYSFS_buildLocalFilename
*
*/
gchar * PHYSFS_buildLocalFilename(  const gchar * internel_filename )
{
	const gchar * write_path = NULL;
	gchar * file_path = NULL;

	write_path = PHYSFS_getWriteDir();

	if ( write_path )
	{
		file_path = g_build_filename(write_path, internel_filename, NULL );
	}

	return file_path;
}

/********************************
* Meg_ComboFile_Scan
*
@
@
*/
void Meg_ComboFile_Scan(GtkWidget * widget, gchar * path, gchar * suffix, gboolean none, guint extension_length )
{
	/* Scan Directory */
	char ** directory_listing = PHYSFS_enumerateFiles(path);
	char ** current_file;

	if ( none )
	{
		Meg_ComboText_AppendText( GTK_COMBO_BOX(widget), g_strdup(MEG_COMBOFILE_NONE) );
	}

	for (current_file = directory_listing; *current_file != NULL; current_file++)
	{
		char * file_name = g_strndup(*current_file, g_utf8_strlen(*current_file, -1) - extension_length );

		if ( suffix == NULL )
			Meg_ComboText_AppendText( GTK_COMBO_BOX(widget), file_name );
		else if ( g_str_has_suffix(*current_file, suffix) )
			Meg_ComboText_AppendText( GTK_COMBO_BOX(widget), file_name );
		else
			g_free(file_name);

	}
	PHYSFS_freeList(directory_listing);
	gtk_combo_box_set_active( GTK_COMBO_BOX(widget), 0);
}


/********************************
* error domain
*/
GQuark meg_error_quark(void)
{
	static GQuark q = 0;

	if (q == 0)
		q = g_quark_from_static_string("meg-error-quark");

	return q;
}


/********************************
* Wrapper Functions
*/
gint64 Meg_file_size( const gchar  *filename )
{
	gint64 size = -1;
	PHYSFS_File * src_file = PHYSFS_openRead(filename);
	if ( src_file )
	{
		size = PHYSFS_fileLength(src_file);
	}
	PHYSFS_close(src_file);
	return size;
}

/********************************
* Physfs version of g_file_test
* sort of
*/
gboolean Meg_file_test( const gchar  *filename, GFileTest test )
{
	if ( (test & G_FILE_TEST_EXISTS) != 0)
	{
		return PHYSFS_exists(filename);
	}

	if ( (test & G_FILE_TEST_IS_REGULAR) != 0 )
	{
		if ( PHYSFS_isDirectory(filename) || PHYSFS_isSymbolicLink(filename ))
			return FALSE;
		return PHYSFS_exists(filename);
	}
	if ( (test & G_FILE_TEST_IS_DIR) != 0 )
	{
		return PHYSFS_isDirectory(filename);
	}

	return PHYSFS_exists(filename);
}


/********************************
* Meg_FileImport
* File Coping with error handling
*/
gboolean Meg_FileImport(gchar * source, gchar * dest)
{

	GFile * dest_file, * source_file;
	const gchar * write_directory = NULL;
	gchar * dest_path = NULL;



	write_directory = PHYSFS_getWriteDir();

	if ( write_directory == NULL )
	{
		Meg_Error_Print( __func__, __LINE__, "Can't write file.");
		return FALSE;
	}

	dest_path = g_build_filename(write_directory, dest, NULL );

	source_file =  g_file_new_for_path(source);
	dest_file = g_file_new_for_path(dest_path);




	if ( !g_file_copy( source_file, dest_file, 0, NULL, NULL, NULL, NULL) )
	{
		Meg_Error_Print( __func__, __LINE__, "File '%s' seem to already exist", dest_path);
		return FALSE;
	}

	return FALSE;
}



/********************************
* Wrapper Functions
*/
gchar * Meg_file_get_path( const gchar * filename )
{
	const char * directory = PHYSFS_getRealDir(filename);
	if ( directory && g_file_test(directory, G_FILE_TEST_IS_DIR) )
		return g_strdup_printf("%s%s%s", directory, G_DIR_SEPARATOR_S, filename );
	return NULL;
}

/********************************
* Physfs version of g_file_get_contents
*
*/
gboolean Meg_file_get_contents(const gchar *filename, gchar ** contents, gsize * length, GError ** error)
{
	g_return_val_if_fail(contents, FALSE);

	gboolean results = FALSE;
	PHYSFS_File * src_file = NULL;
	PHYSFS_sint64 file_size = 0;
	gsize length_read = 0;

	*contents = NULL;

	src_file = PHYSFS_openRead(filename);
	if ( !src_file )
	{
		g_set_error( error, meg_error_quark(), G_FILE_ERROR_FAILED,  "Failed to open file: %s", filename );
		return results;
	}

	file_size = PHYSFS_fileLength(src_file);

	if ( file_size <= 0 )
	{
		g_set_error( error, meg_error_quark(), G_FILE_ERROR_FAILED,  "Failed to read file: %s", filename );
		if ( length )
			*length = 0;
	}
	else
	{
		*contents = g_new0(gchar, file_size+1);

		length_read = PHYSFS_read(src_file, *contents, 1, file_size);

		if ( length_read != file_size )
			g_set_error( error, meg_error_quark(), G_FILE_ERROR_FAILED,  "Failed to read complete file: %s", filename);
		else
		{
			if ( length )
			{
				*length = length_read;
			}
		}
		results = TRUE;
	}
	PHYSFS_close( src_file );
	return results;
}

/********************************
* Physfs version of g_file_set_contents
*
*/
gboolean Meg_file_set_contents(const gchar *filename, const gchar *contents, gssize length, GError ** error)
{
	gboolean result = FALSE;
	PHYSFS_File * dest_file = PHYSFS_openWrite( filename );
	if ( dest_file == NULL )
	{
		g_set_error( error, meg_error_quark(), G_FILE_ERROR_FAILED,  "Failed to open file: %s\n Reason %s", filename, PHYSFS_getLastError() );
		return FALSE;
	}

	if ( length == -1 )
		length = strlen( contents );

	if ( PHYSFS_write( dest_file, contents, 1, length ) != length )
	{
		g_set_error( error, meg_error_quark(), G_FILE_ERROR_FAILED,  "Failed to open file: %s\n Reason %s", filename, PHYSFS_getLastError() );
	}
	else
	{
		result = TRUE;
	}

	PHYSFS_close( dest_file );
	return result;
}

/********************************
* Physfs version of gdk_pixbuf_new_from_file
*
*/
GdkPixbuf * Megdk_pixbuf_new_from_file(const char *filename, GError **error)
{
	GdkPixbufLoader * pix_loader = NULL;
	GdkPixbuf * image = NULL;
	guint8 * contents = NULL;
	gsize data_length = 0;
	gsize length_read = 0;
	PHYSFS_sint64 file_size = 0;
	PHYSFS_File * src_file = PHYSFS_openRead(filename);

	if ( !src_file )
	{
		g_set_error( error, meg_error_quark(), G_FILE_ERROR_FAILED,  "Failed to open image: %s\n Reason %s", filename, PHYSFS_getLastError() );
		return image;
	}


	file_size = PHYSFS_fileLength(src_file);

	if ( file_size <= 0 )
	{
		g_set_error( error, meg_error_quark(), G_FILE_ERROR_FAILED,  "Failed to open image: %s\n Reason %s", filename, PHYSFS_getLastError() );
		PHYSFS_close( src_file );
		return image;
	}

	contents = g_new0(guint8, file_size);
	length_read = PHYSFS_read(src_file, contents, 1, file_size);

	PHYSFS_close( src_file );

	if ( length_read != file_size )
	{
		g_set_error( error, meg_error_quark(), G_FILE_ERROR_FAILED,  "Failed to open image: %s\n Reason %s", filename, PHYSFS_getLastError() );
		return image;
	}

	data_length = file_size;

	pix_loader = gdk_pixbuf_loader_new_with_type( "png", error );
	if ( gdk_pixbuf_loader_write( pix_loader, contents, data_length, error) )
	{
		image = gdk_pixbuf_loader_get_pixbuf(pix_loader);
	}

	gdk_pixbuf_loader_close(pix_loader, NULL);
	g_free(contents);


	return image;
}

/********************************
* Modified version of PHYSFS_enumerateFilesCallback
* as it doesn't care about dupes
*/
void Meg_enumerateFilesCallback( const char * dir, PHYSFS_EnumFilesCallback c, void * data )
{
	if ( c )
	{
		char **rc = PHYSFS_enumerateFiles(dir);
		char **i;

		for (i = rc; *i != NULL; i++)
			c(data, dir, *i);

		PHYSFS_freeList(rc);
	}

}

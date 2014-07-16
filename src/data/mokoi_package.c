/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
/* Packages Format */
/* Patch File Header
 * uint8_t[8] magic = {139, 'M', 'o', 'k', 'o', 'i', '1', 10}; // - magic[6] stores the file version
 * uint32_t game_id;
 * uint32_t game_crc;
 * uint32_t crc;
*/
/* Resource File Header
 * uint8_t[8] magic = {138, 'M', 'o', 'k', 'o', 'i', '1', 10}; // - magic[6] stores the file version
 * uint8_t[128] name;
 * uint8_t[128] author;
 * uint8_t category;
 * uint32_t crc;
*/
/* Game File Header
 * uint8_t[8] magic = {137, 'M', 'o', 'k', 'o', 'i', '1', 10}; // - magic[6] stores the file version
 * int8_t[256] name;
 * uint32_t id;
 * uint32_t logo_length; //png logo length
 * uint8_t * logo; // png logo data
 * uint32_t crc;
*/
/* Files
 * uint16_t filename_length;
 * uint8_t * filename;
 * uint32_t file_size_length;
 * uint32_t compressed_size_length;
 * uint8_t * data;
 */


/* Standard Headers */
#include "loader_global.h"

/* Required Headers */
#include <zlib.h>
#include <string.h>
#include <glib/gstdio.h>
#include "mokoi_package.h"
#include "compression_functions.h"
#include "loader_functions.h"
#include "logger_functions.h"

/* Global Variables */

guchar mokoi_game_magic[6] = {137, 'M', 'o', 'k', 'o', 'i' };
guchar mokoi_patch_magic[6] = {139, 'M', 'o', 'k', 'o', 'i' };
guchar mokoi_resource_magic[6] = {138, 'M', 'o', 'k', 'o', 'i' };

/* Local Variables */


/********************************
* MokoiPackage_Scan
*
@ src:
@ files:
*/
GSList * MokoiPackage_Scan( FILE * source, GSList * files )
{
	GSList * list = files;
	while ( !feof(source) )
	{
		StoredFileInfo * info = g_new0(StoredFileInfo, 1);
		info->name_length = file_read_short( source ) - 2;

		if ( info->name_length < 1 )
		{
			g_free(info);
			break;
		}

		//Name includes ./ so skip them
		fseek(source, 2, SEEK_CUR);
		info->name = g_new0(gchar, info->name_length+1);
		if ( fread( info->name, 1, info->name_length, source ) != info->name_length )
		{
			g_warning("MokoiPackage_Scan read error.");
			break;
		}

		info->length = file_read_long( source );
		info->compress = file_read_long( source );
		info->offset = ftell(source);

		fseek(source, info->compress, SEEK_CUR);

		//g_print("\t'%s' offset:%d\n", info->name, info->offset );
		list = g_slist_append(list, info);
	}

	return list;

}

/********************************
* MokoiPackage_WriteFile
*
@ data: StoredFileInfo
@ user_data: FILE*
*/
void MokoiPackage_WriteFile(gpointer data, gpointer user_data)
{
	FILE *fd = (FILE*)user_data;
	StoredFileInfo * info = (StoredFileInfo*)data;

	file_write_short( fd, info->name_length + 2 ); /* name length */
	fwrite( "./", 2, 1, fd ); /* filename prefix */
	fwrite( info->name, info->name_length, 1, fd ); /* filename */
	file_write_long( fd, info->length );/* file size */
	file_write_long( fd, info->compress );/* compress length */
	fwrite( info->compress_data, info->compress, 1, fd ); /* data */

}

/********************************
* MokoiPackage_OpenGame
*
@ src:
@ files:
*/
void MokoiPackage_OpenGame(FILE * src_fd, GSList ** files )
{
	*files = MokoiPackage_Scan( src_fd, NULL );
}


/********************************
* MokoiPackage_SaveGame
*
@ src:
@ files:
*/
void MokoiPackage_SaveGame( const gchar * dest, GSList * files, gchar * title, guint32 id, gchar * png , guint32 length)
{
	FILE * fd = fopen(dest, "wb");
	fwrite( &mokoi_game_magic, 6, 1, fd ); // Magic
	file_write_byte( fd, '1' ); // version
	file_write_byte( fd, '\n' ); // buffer
	fwrite( title, 255, 1, fd ); // title
	file_write_long( fd, id );

	file_write_long( fd, length );
	if ( length > 0 )
	{
		fwrite( png, length, 1, fd ); // PNG
	}
	file_write_long( fd, 0 ); // CRC

	// Pack up files
	g_slist_foreach( files, (GFunc) MokoiPackage_WriteFile, fd );

	fclose(fd);
}

/********************************
* MokoiPackage_OpenPatch
*
@ src:
@ files:
*/
void MokoiPackage_OpenPatch(FILE * src_fd, GSList ** files )
{
	*files = MokoiPackage_Scan( src_fd, NULL );
}

/********************************
* MokoiPackage_SavePatch
*
@ src:
@ files:
*/
void MokoiPackage_SavePatch( const gchar * dest, GSList * files, guint32 id, guint32 checksum )
{
	FILE * fd = fopen(dest, "wb");
	fwrite( &mokoi_patch_magic, 6, 1, fd ); // Magic
	file_write_byte( fd, '1' ); // version
	file_write_byte( fd, '\n' ); // buffer
	file_write_long( fd, id ); // game_id
	file_write_long( fd, checksum ); // game_crc
	file_write_long( fd, 0 ); // CRC

	// Pack up files
	g_slist_foreach( files, (GFunc) MokoiPackage_WriteFile, fd );

	fclose(fd);

}

/********************************
* MokoiPackage_OpenResource
*
@ src:
@ files:
*/
gboolean MokoiPackage_OpenResource(FILE * src_fd, GSList ** files, gchar ** title, gchar ** author, guint8 * category)
{
	gboolean successful = TRUE;
	guint8 * magic = g_new0(guint8, 6);
	gchar * safe_title = g_new0(gchar, 128);
	gchar * safe_author = g_new0(gchar, 128);
	guint8 version;
	guint8 safe_category;
	guint32 crc;

	if ( files != NULL )
		*files = NULL;

	fseek( src_fd, 0, SEEK_SET);
	if ( fread( magic, 1, 6, src_fd ) != 6 ) // Magic
	{
		successful = FALSE;
		goto function_exit;
	}
	if ( memcmp(magic, mokoi_resource_magic, 6) != 0 )
	{
		successful = FALSE; // Not a resource file
		goto function_exit;
	}

	version = file_read_byte( src_fd );
	fseek( src_fd, 1, SEEK_CUR); // skip buffer

	if ( fread( safe_title, 1, 128, src_fd ) != 128 )  // title
	{
		g_warning("end of file to early..");
		successful = FALSE;// Not a resource file
		goto function_exit;
	}

	if ( fread( safe_author, 1, 128, src_fd ) != 128 )  // author
	{
		g_warning("end of file to early...." );
		successful = FALSE; // Not a resource file
		goto function_exit;
	}

	//g_print("Resource: '%s' '%s'\n", safe_title, safe_author );

	safe_category = file_read_byte( src_fd ); // buffer
	crc = file_read_long( src_fd ); // CRC

	if ( files != NULL )
		*files = MokoiPackage_Scan( src_fd, NULL );

	if ( title != NULL )
		*title = g_strdup(safe_title);

	if ( author != NULL )
		*author = g_strdup(safe_author);

	if ( category != NULL )
		*category = safe_category;


	function_exit:


	g_free(magic);
	g_free(safe_title);
	g_free(safe_author);

	return successful;
}


/********************************
* MokoiPackage_SaveResource
*
@ src:
@ files:
*/
void MokoiPackage_SaveResource( const  gchar * dest, GSList * files, const gchar * title, const gchar * author, guint8 category )
{
	gchar * safe_title = g_new0(gchar, 128);
	gchar * safe_author = g_new0(gchar, 128);

	g_snprintf( safe_title, 127, "%s", title );
	g_snprintf( safe_author, 127, "%s", author );

	/* */
	FILE * fd = fopen(dest, "wb");
	fwrite( &mokoi_resource_magic, 6, 1, fd ); // Magic
	file_write_byte( fd, '1' ); // version
	file_write_byte( fd, '\n' ); // buffer
	fwrite( safe_title, 128, 1, fd ); // title
	fwrite( safe_author, 128, 1, fd ); // author
	file_write_byte( fd, category ); // buffer
	file_write_long( fd, 0 ); // CRC

	// Pack up files
	g_slist_foreach( files, (GFunc) MokoiPackage_WriteFile, fd );

	fclose(fd);
}

/********************************
* MokoiPackage_Extract
*
@ file_info:
@ dest:
*/
void MokoiPackage_Extract( FILE * fd, StoredFileInfo *fi, const gchar *dest )
{
	g_return_if_fail(fi);
	g_return_if_fail(fi->offset > 256);
	g_return_if_fail(fi->compress);
	g_return_if_fail(fi->length);

	gchar * dest_directory = NULL;
	guchar * data = NULL;
	guchar * buffer = NULL;
	gulong uncompress_length = fi->length;

	buffer = g_new0( guchar, fi->compress );
	data = g_new0( guchar, fi->length );


	if ( buffer && data )
	{
		fseek( fd, fi->offset, SEEK_SET);
		if ( fread( buffer, 1, fi->compress, fd ) == fi->compress );
		{
			uncompress(data, &uncompress_length, buffer, fi->compress );
		}
	}
	g_print("MokoiPackage_Extract: '%s'\n", dest);


	dest_directory = g_path_get_dirname(dest);
	PHYSFS_mkdir(dest_directory);
	Meg_file_set_contents(dest, (gchar*)data, uncompress_length, NULL);

	g_free(dest_directory);
	g_free(buffer);
	g_free(data);
}


/********************************
* MokoiPackage_Extract
*
@ file_info:
@ dest:
*/
gchar * MokoiPackage_ExtractContent( FILE * fd, StoredFileInfo *fi )
{
	if ( !fi )
		return NULL;
	g_return_val_if_fail(fi->offset > 256, NULL);
	g_return_val_if_fail(fi->compress, NULL);
	g_return_val_if_fail(fi->length, NULL);

	guchar * data = NULL;
	guchar * buffer = NULL;
	gulong uncompress_length = fi->length;

	buffer = g_new0( guchar, fi->compress );
	data = g_new0( guchar, fi->length+1 );

	if ( buffer && data )
	{
		fseek( fd, fi->offset, SEEK_SET);
		if ( fread( buffer, 1, fi->compress, fd ) == fi->compress )
		{
			uncompress(data, &uncompress_length, buffer, fi->compress );
		}
	}

	return (gchar *)data;
}


/********************************
* MokoiPackage_Add
*
@ files:
@ dest:
*/
void MokoiPackage_Add( GSList ** files, gchar * name, guint8 * data, guint32 length )
{
	gulong temp_length = 0;
	StoredFileInfo * info = g_new0(StoredFileInfo, 1);

	info->name_length = strlen(name);
	info->name = g_strdup(name);

	info->length = length;

	temp_length = (length + (length / 10) + 12);
	info->compress_data = g_new0(unsigned char, temp_length);

	int error = compress2( info->compress_data, &temp_length, data, length, Z_BEST_COMPRESSION);
	if ( !error )
	{
		info->compress = temp_length;

		*files = g_slist_append(*files, info);

		Logger_FormattedLog( NULL, LOG_FINE, "'%s' added ( %d bytes / %d bytes )\n", name, length, info->compress);
	}
	else
	{
		Logger_FormattedLog( NULL, LOG_ERROR, "'%s' Compesssion Error: %s\n", name, zError(error) );
	}

}

/********************************
* MokoiPackage_Remove
*
@ files:
@ dest:
*/
void MokoiPackage_Remove(GSList **files, gchar * name )
{


}

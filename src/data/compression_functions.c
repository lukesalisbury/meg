/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
/* Compression Format */
/*
 * Header (Note: Patch files only have int8_t[7] magic; )
 * int8_t[8] magic; // - magic[6] stores the file version
 * int8_t[256] name;
 * uint32_t id;
 * uint32_t logo_length; //png logo length
 * uint8_t * logo; // png logo data
 * uint32_t crc;
 * Files
 * uint16_t filename_length;
 * int8_t * filename;
 * uint32_t file_size_length;
 * uint32_t compressed_size_length;
 * int8_t * data;
 *
 *
 */


/* Standard Headers */
#include "loader_global.h"

/* Required Headers */
#include <string.h>
#include <glib/gstdio.h>
#include "logger_functions.h"
#include "game_compiler.h"
#include "loader_functions.h"

#include "mokoi_package.h"
#include "compression_functions.h"


/* Global Variables */
extern gchar * mokoiBasePath;


/* Local Variables */


/* Private functions */
/*
mz_bool mz_zip_reader_init_pfile(mz_zip_archive *pZip, FILE * fd, mz_uint32 flags)
{
	mz_uint64 file_size;
	MZ_FILE *pFile = fd;
	if (!pFile)
		return MZ_FALSE;
	if (MZ_FSEEK64(pFile, 0, SEEK_END))
		return MZ_FALSE;
	file_size = MZ_FTELL64(pFile);
	if (!mz_zip_reader_init_internal(pZip, flags))
	{
		MZ_FCLOSE(pFile);
		return MZ_FALSE;
	}
	pZip->m_pRead = mz_zip_file_read_func;
	pZip->m_pIO_opaque = pZip;
	pZip->m_pState->m_pFile = pFile;
	pZip->m_archive_size = file_size;
	if (!mz_zip_reader_read_central_dir(pZip, flags))
	{
		mz_zip_reader_end(pZip);
		return MZ_FALSE;
	}
	return MZ_TRUE;
}
*/
/*
size_t mz_file_write_function(void *pOpaque, mz_uint64 file_ofs, const void *pBuf, size_t n)
{
	gchar * filename = (gchar*)pOpaque;
	Meg_file_set_contents(filename,pBuf,n, NULL);
	return n;
}
*/

/********************************
* CompressionZip_FileList
*
@ fd:
@ files:
*/
void CompressionZip_FileList( FILE * fd, GSList ** files )
{
	/* Should be a zip */
	/*
	mz_zip_archive zip_archive;
	memset(&zip_archive, 0, sizeof(zip_archive));
	if (mz_zip_reader_init_pfile(&zip_archive, fd, 0))
	{
		guint32 i = 0;
		for (i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++)
		{
			mz_zip_archive_file_stat file_stat;
			if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
			{
				g_print("mz_zip_reader_file_stat() failed!\n");
				break;
			}
			else
			{
				StoredFileInfo * info = g_new0(StoredFileInfo, 1);
				info->name = g_strdup(file_stat.m_filename);
				info->name_length = strlen(info->name);
				info->length = file_stat.m_uncomp_size;
				info->compress = file_stat.m_comp_size;
				info->offset = i;
				*files = g_slist_append(*files, info);
			}

		}
	}
	mz_zip_reader_end(&zip_archive);
	*/
}

/********************************
* CompressionZip_Extract
*
@ compress_file:
*/
void CompressionZip_Extract( FILE * fd, StoredFileInfo *fi, gchar * dest  )
{
	/*
	mz_zip_archive zip_archive;
	memset(&zip_archive, 0, sizeof(zip_archive));
	if ( mz_zip_reader_init_pfile(&zip_archive, fd, 0) )
	{
		if ( mz_zip_reader_extract_to_callback(&zip_archive, fi->offset, mz_file_write_function, dest, 0) )
		{
			g_print("Error Extracting File.");
		}
	}
	*/
}

/********************************
* Compression_PackFile
*
@ fd: file
@ filename: filename to write to file
@ filepath: path to file
@ local_only:
*/
void Compression_PackFile(GSList ** files, gchar * internal_path, gchar * file_name, gboolean local_only )
{
	unsigned char * data = NULL;
	gsize file_size = 0;
	gboolean read_results = FALSE;


	if ( local_only )
	{
		gchar * local_path = g_build_filename(mokoiBasePath, file_name, NULL );
		read_results = g_file_get_contents( local_path, (gchar**)&data, &file_size, NULL );
		g_free( local_path );
	}
	else
	{
		read_results = Meg_file_get_contents( file_name, (gchar**)&data, &file_size, NULL );
	}

	if ( file_size > 0 && read_results )
	{
		guint32 data_size = file_size;
		MokoiPackage_Add(files, internal_path, data, data_size);
	}
	g_free(data);

}


/********************************
* Compression_PackText
*/
void Compression_PackText(GSList ** files, gchar * internal_path, const gchar *text)
{
	Logger_Append(NULL, LOG_NONE, internal_path);

	guint32 length = strlen(text);


	MokoiPackage_Add(files, internal_path, (guchar*)text, length);

}


/********************************
* Compression_PackFolder
*
@ fd:
@ directory:
@ ext:
*/
void Compression_PackFolder(GSList **files, gchar *directory, gchar *ext, gboolean local_only)
{


	char ** directory_listing = PHYSFS_enumerateFiles(directory);
	char ** current_file;

	for (current_file = directory_listing; *current_file != NULL; current_file++)
	{
		gchar * file_path = g_strdup_printf( "%s/%s", directory, *current_file );
		if ( Meg_file_test(file_path, G_FILE_TEST_IS_REGULAR) )
		{

			if (ext == NULL)
			{
				Compression_PackFile(files, file_path, file_path, local_only );
			}
			else
			{
				if ( g_str_has_suffix(file_path, ext) )
				{
					Compression_PackFile(files, file_path, file_path, local_only );
				}
			}
		}
		g_free(file_path);

	}
	PHYSFS_freeList(directory_listing);

}



/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/*
 * Mokoi Resource File support routines for PhysicsFS.
 *
 */


#if (defined PHYSFS_SUPPORTS_MOKOIRESOURCE)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "physfs.h"

#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"

typedef unsigned long PHYSFS_unsignedlong;
typedef struct _MokoiResourceEntry MokoiResourceEntry;

struct _MokoiResourceEntry
{
	char * name;
	char * path;
	size_t path_len;
	PHYSFS_uint32 pos;
	PHYSFS_unsignedlong size;
	PHYSFS_unsignedlong compress_size;
	MokoiResourceEntry * next;
};

typedef struct
{
	char * filename;
	PHYSFS_sint64 last_mod_time;
	PHYSFS_uint32 offset;
	MokoiResourceEntry * files;
} MokoiResourceInfo;

typedef struct
{
	void * handle;
	MokoiResourceEntry *entry;
	PHYSFS_uint32 pos;
	PHYSFS_uint8 * buffer;
} MokoiResourceFileinfo;


int dotgame_uncompress(void ** fh, PHYSFS_uint8 ** buffer, MokoiResourceEntry * file)
{
	PHYSFS_uint8 * zbuffer;
	int err = 0;

	__PHYSFS_platformSeek(*fh, file->pos);

	zbuffer = (PHYSFS_uint8 *) allocator.Malloc( file->compress_size );
	BAIL_IF_MACRO(zbuffer == NULL, ERR_OUT_OF_MEMORY, 1);

	__PHYSFS_platformRead(*fh, zbuffer, 1, file->compress_size);

	*buffer = (PHYSFS_uint8 *) allocator.Malloc( file->size );
	BAIL_IF_MACRO(*buffer == NULL, ERR_OUT_OF_MEMORY, 1);

	err = uncompress(*buffer, &file->size, zbuffer, file->compress_size);
	if ( err )
		BAIL_MACRO(ERR_COMPRESSION, 1);
	allocator.Free(zbuffer);
	return 0;
}

static int dotgame_open(const char *filename, int forWriting, void **fh)
{
	*fh = NULL;
	BAIL_IF_MACRO(forWriting, ERR_ARC_IS_READ_ONLY, 0);

	PHYSFS_uint8 magic[6] = {138, 'M', 'o', 'k', 'o', 'i'};
	PHYSFS_uint8 buf[8];


	*fh = __PHYSFS_platformOpenRead(filename);
	BAIL_IF_MACRO(*fh == NULL, NULL, 0);

	if ( __PHYSFS_platformRead(*fh, buf, 1,8) == 8 )
	{
		if ( memcmp(buf, magic, 6) == 0 && buf[6] == '1' && buf[7] == 10 )
		{
			// Header's magic is correct
			PHYSFS_uint8 name[128];
			PHYSFS_uint8 author[128];
			PHYSFS_uint8 category;
			PHYSFS_uint32 crc;
			/* title */
			if ( __PHYSFS_platformRead(*fh, name, 1, 128) != 128 )
			{
				__PHYSFS_setError(ERR_UNSUPPORTED_ARCHIVE);
				return 0;
			}

			/* author */
			if ( __PHYSFS_platformRead(*fh, author, 1, 128) != 128 )
			{
				__PHYSFS_setError(ERR_UNSUPPORTED_ARCHIVE);
				return 0;
			}

			/* category */
			if ( __PHYSFS_platformRead(*fh, &category, 1, 1) != 1 )
			{
				__PHYSFS_setError(ERR_UNSUPPORTED_ARCHIVE);
				return 0;
			}
			/* Checksum */
			if ( __PHYSFS_platformRead(*fh, &crc, 1, 4) != 4 )
			{
				__PHYSFS_setError(ERR_UNSUPPORTED_ARCHIVE);
				return 0;
			}


			return 1;
		}
	}

	/* Invalid File */
	__PHYSFS_setError(ERR_UNSUPPORTED_ARCHIVE);

	if (*fh != NULL)
		__PHYSFS_platformClose(*fh);

	*fh = NULL;
	return 0;
} /* dotgame_open */

static int dotgame_load_entries(const char *name, int forWriting, MokoiResourceInfo *info)
{
	void *fh = NULL;
	PHYSFS_sint16 name_length = 0;
	PHYSFS_uint32 file_size = 0;
	PHYSFS_uint32 compress_size = 0;
	MokoiResourceEntry * first_file = NULL;
	MokoiResourceEntry * prev_file = NULL;

	BAIL_IF_MACRO(!dotgame_open(name, forWriting, &fh), NULL, 0);
	info->offset = __PHYSFS_platformTell(fh);

	if ( __PHYSFS_platformEOF(fh) )
	{
		__PHYSFS_setError(ERR_UNSUPPORTED_ARCHIVE);
		return 0;
	}



	while ( !__PHYSFS_platformEOF(fh) )
	{
		__PHYSFS_platformRead(fh, &name_length, 2, 1);
		name_length = PHYSFS_swapSBE16(name_length);
		if ( name_length < 3 )
		{
			break;
		}

		MokoiResourceEntry * current_file = (MokoiResourceEntry*)allocator.Malloc( sizeof(MokoiResourceEntry));
		memset( current_file, '\0', sizeof(MokoiResourceEntry) );

		__PHYSFS_platformRead(fh, NULL, name_length, 1);


		current_file->path = (char *) allocator.Malloc(name_length+1);
		memset( current_file->path, '\0', name_length+1 );
		__PHYSFS_platformRead(fh, current_file->path, 1, name_length);

		size_t file_len = name_length;
		size_t path_len = file_len - 1;
		while (path_len >= 0 && current_file->path[path_len] != '/')
			path_len--;
		path_len++;
		file_len -= path_len;

		current_file->path_len = path_len;
		current_file->name = (char *) allocator.Malloc(file_len+1);
		memset( current_file->name, '\0', file_len+1 );
		memcpy( current_file->name, current_file->path + path_len, file_len);

		/*printf("Scan: [%s] %s\n", current_file->path, current_file->name);*/


		__PHYSFS_platformRead(fh, &file_size, 4, 1);
		__PHYSFS_platformRead(fh, &compress_size, 4, 1);
		current_file->size = PHYSFS_swapUBE32(file_size);
		current_file->compress_size = PHYSFS_swapUBE32(compress_size);
		current_file->pos = __PHYSFS_platformTell(fh);
		__PHYSFS_platformSeek(fh, current_file->pos + current_file->compress_size);

		if ( prev_file )
			prev_file->next = current_file;
		if ( !prev_file )
			first_file = current_file;

		prev_file = current_file;
	}
	__PHYSFS_platformClose(fh);

	info->files = first_file;


	return (info->files != NULL ? 1 : 0);
} /* dotgame_load_entries */



static void MokoiResource_dirClose(dvoid *opaque)
{
	MokoiResourceInfo * info = (MokoiResourceInfo *) opaque;
	MokoiResourceEntry * current_file = info->files;
	MokoiResourceEntry * next_file = NULL;

	while ( current_file != NULL )
	{
		next_file = current_file->next;

		allocator.Free(current_file->name);
		allocator.Free(current_file->path);
		allocator.Free(current_file);

		current_file = next_file;
	}

} /* MokoiResource_dirClose */


static PHYSFS_sint64 MokoiResource_read(fvoid *opaque, void *buffer, PHYSFS_uint32 objSize, PHYSFS_uint32 objCount)
{
	PHYSFS_sint64 rc = -1;
	MokoiResourceFileinfo *finfo = (MokoiResourceFileinfo *) opaque;
	if ( !finfo->buffer || finfo->pos >= finfo->entry->size )
	{
		 return rc;
	}

	PHYSFS_uint32 bytesLeft = finfo->entry->size - finfo->pos;
	if ( bytesLeft > 0 )
	{
		PHYSFS_uint32 objsLeft = (bytesLeft / objSize);

		if (objsLeft < objCount)
			objCount = objsLeft;

		rc = objSize * objCount;
		finfo->buffer += finfo->pos;
		memcpy( buffer, finfo->buffer, rc );
		finfo->buffer -= finfo->pos;
		finfo->pos += rc;

		return rc;
	}
	return 0;
} /* MokoiResource_read */


static PHYSFS_sint64 MokoiResource_write(fvoid *opaque, const void *buffer, PHYSFS_uint32 objSize, PHYSFS_uint32 objCount)
{
	BAIL_MACRO(ERR_NOT_SUPPORTED, -1);
} /* MokoiResource_write */

static int MokoiResource_eof(fvoid *opaque)
{
	MokoiResourceFileinfo *finfo = (MokoiResourceFileinfo *) opaque;
	BAIL_IF_MACRO(finfo->pos < 0, ERR_INVALID_ARGUMENT, 1);
	BAIL_IF_MACRO(!finfo->entry, ERR_PAST_EOF, 1);
	BAIL_IF_MACRO(finfo->pos >= finfo->entry->size, ERR_INVALID_ARGUMENT, 1);
	return 0;
} /* MokoiResource_eof */


static PHYSFS_sint64 MokoiResource_tell(fvoid *opaque)
{
	if ( opaque )
	{
		return ((MokoiResourceFileinfo *) opaque)->pos;;
	}
	return 0;
} /* MokoiResource_tell */


static int MokoiResource_seek(fvoid *opaque, PHYSFS_uint64 offset)
{
	if ( opaque )
	{
		MokoiResourceFileinfo *finfo = (MokoiResourceFileinfo *) opaque;
		BAIL_IF_MACRO(offset < 0, ERR_INVALID_ARGUMENT, 0);
		BAIL_IF_MACRO(offset >= finfo->entry->size, ERR_PAST_EOF, 0);

		finfo->pos = (PHYSFS_uint32)offset;
		return 1;
	}
	return 0;
} /* MokoiResource_seek */


static PHYSFS_sint64 MokoiResource_fileLength(fvoid *opaque)
{
	MokoiResourceFileinfo * finfo = (MokoiResourceFileinfo *) opaque;
	if ( finfo && finfo->entry )
		return (PHYSFS_sint64)finfo->entry->size;
	else
		return 0;
} /* MokoiResource_fileLength */


static int MokoiResource_fileClose(fvoid *opaque)
{
	if ( opaque )
	{
		MokoiResourceFileinfo *finfo = (MokoiResourceFileinfo *) opaque;

		/*if ( finfo->buffer )
		{
			allocator.Free(finfo->buffer);
		}*/
		if ( finfo->handle )
		{
			BAIL_IF_MACRO(!__PHYSFS_platformClose(finfo->handle), NULL, 0);
		}
		allocator.Free(finfo);
	}
	return 1;
} /* MokoiResource_fileClose */


static int MokoiResource_isArchive(const char *filename, int forWriting)
{
	void *fh;
	int retval = dotgame_open(filename, forWriting, &fh);

	if ( fh != NULL )
		__PHYSFS_platformClose(fh);

	return retval ;
} /* MokoiResource_isArchive */





static void * MokoiResource_openArchive(const char *name, int forWriting)
{

	PHYSFS_sint64 modtime = __PHYSFS_platformGetLastModTime(name);
	MokoiResourceInfo * info = (MokoiResourceInfo *) allocator.Malloc(sizeof (MokoiResourceInfo));

	BAIL_IF_MACRO(info == NULL, ERR_OUT_OF_MEMORY, NULL);
	memset(info, '\0', sizeof (MokoiResourceInfo));

	info->filename = (char *) allocator.Malloc(strlen(name) + 1);
	BAIL_IF_MACRO(!info->filename, ERR_OUT_OF_MEMORY, NULL);

	info->files = NULL;

	if ( dotgame_load_entries(name, forWriting, info) )
	{
		strcpy(info->filename, name);
		info->last_mod_time = modtime;
		return info;
	}
	return NULL;
} /* MokoiResource_openArchive */


static void MokoiResource_enumerateFiles(dvoid *opaque, const char *dname, int omitSymLinks, PHYSFS_EnumFilesCallback cb, const char *origdir, void *callbackdata)
{
	char path[60];
	size_t path_len = 0;

	MokoiResourceInfo *info = ((MokoiResourceInfo *) opaque);
	MokoiResourceEntry * current_file = info->files;

	if ( *dname == '\0' )
		snprintf(path, 59, "./");
	else
		snprintf(path, 59, "./%s/", dname );
	path_len = strlen(path);

	while ( current_file != NULL )
	{
		if ( current_file->path && path_len == current_file->path_len )
		{
			if ( strncmp(current_file->path, path, current_file->path_len ) == 0 )
			{
				cb(callbackdata, origdir, current_file->name);
			}
		}
		current_file = (MokoiResourceEntry*)current_file->next;
	}
} /* MokoiResource_enumerateFiles */


static int MokoiResource_exists(dvoid *opaque, const char *name)
{
	MokoiResourceInfo *info = ((MokoiResourceInfo *) opaque);
	MokoiResourceEntry * scan = info->files;

	char path[256];

	if ( name[0] == '\0')
		return 0;
	else if ( name[0] == '.' && name[1] == '/' )
		snprintf(path, 255, "%s", name );
	else
		snprintf(path, 255, "./%s", name );

	//printf("MokoiResource_exists '%s'\n", path);
	while ( scan != NULL )
	{
		if ( scan->name )
		{
			if ( strcmp(scan->path, path ) == 0 )
			{
				return 1;
			}
		}
		scan = scan->next;
	}
	return 0;
} /* MokoiResource_exists */


static int MokoiResource_isDirectory(dvoid *opaque, const char *name, int *fileExists)
{
	return 0;
} /* MokoiResource_isDirectory */


static int MokoiResource_isSymLink(dvoid *opaque, const char *name, int *fileExists)
{
	return 0;
} /* MokoiResource_isSymLink */


static PHYSFS_sint64 MokoiResource_getLastModTime(dvoid *opaque, const char *name, int *fileExists)
{
	return 0;
} /* MokoiResource_getLastModTime */




static fvoid * MokoiResource_openRead(dvoid *opaque, const char *fnm, int *fileExists)
{

	MokoiResourceFileinfo * finfo = NULL;
	finfo = (MokoiResourceFileinfo *) allocator.Malloc(sizeof (MokoiResourceFileinfo));
	finfo->handle = __PHYSFS_platformOpenRead( ((MokoiResourceInfo *) opaque)->filename );
	finfo->entry = NULL;
	finfo->buffer = NULL;
	finfo->pos = 0;

	MokoiResourceEntry * scan = ((MokoiResourceInfo *) opaque)->files;
	char path[256];
//	int path_len = 0;

	memset( path, 0, 256);
	snprintf(path, 254, "./%s", fnm );
//	path_len = strlen(path);
	*fileExists = 0;

	while ( scan != NULL )
	{
		if ( scan->path )
		{
			if ( strcmp(scan->path, path) == 0 )
			{
				finfo->entry = scan;
				finfo->pos = 0;
				dotgame_uncompress(&finfo->handle, &finfo->buffer, scan);
				*fileExists = 1;
				break;
			}
		}
		scan = scan->next;
	}
	return finfo;
} /* MokoiResource_openRead */


static fvoid *MokoiResource_openWrite(dvoid *opaque, const char *name)
{
	BAIL_MACRO(ERR_NOT_SUPPORTED, NULL);
} /* MokoiResource_openWrite */


static fvoid *MokoiResource_openAppend(dvoid *opaque, const char *name)
{
	BAIL_MACRO(ERR_NOT_SUPPORTED, NULL);
} /* MokoiResource_openAppend */


static int MokoiResource_remove(dvoid *opaque, const char *name)
{
	BAIL_MACRO(ERR_NOT_SUPPORTED, 0);
} /* MokoiResource_remove */


static int MokoiResource_mkdir(dvoid *opaque, const char *name)
{
	BAIL_MACRO(ERR_NOT_SUPPORTED, 0);
} /* MokoiResource_mkdir */


const PHYSFS_ArchiveInfo __PHYSFS_ArchiveInfo_MokoiResource =
{
	"Mokoi Resource",
	"Mokoi Resource Files",
	"Luke Salisbury <luke@mokoi.info>",
	"http://mokoi.info",
};


const PHYSFS_Archiver __PHYSFS_Archiver_MokoiResource =
{
	&__PHYSFS_ArchiveInfo_MokoiResource,
	MokoiResource_isArchive,          /* isArchive() method      */
	MokoiResource_openArchive,        /* openArchive() method    */
	MokoiResource_enumerateFiles,     /* enumerateFiles() method */
	MokoiResource_exists,             /* exists() method         */
	MokoiResource_isDirectory,        /* isDirectory() method    */
	MokoiResource_isSymLink,          /* isSymLink() method      */
	MokoiResource_getLastModTime,     /* getLastModTime() method */
	MokoiResource_openRead,           /* openRead() method       */
	MokoiResource_openWrite,          /* openWrite() method      */
	MokoiResource_openAppend,         /* openAppend() method     */
	MokoiResource_remove,             /* remove() method         */
	MokoiResource_mkdir,              /* mkdir() method          */
	MokoiResource_dirClose,           /* dirClose() method       */
	MokoiResource_read,               /* read() method           */
	MokoiResource_write,              /* write() method          */
	MokoiResource_eof,                /* eof() method            */
	MokoiResource_tell,               /* tell() method           */
	MokoiResource_seek,               /* seek() method           */
	MokoiResource_fileLength,         /* fileLength() method     */
	MokoiResource_fileClose           /* fileClose() method      */
};

#endif  /* defined PHYSFS_SUPPORTS_MOKOIRESOURCE */


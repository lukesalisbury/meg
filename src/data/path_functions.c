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
#include <glib/gstdio.h>

void file_write_short( FILE * fd, guint16 value );
void file_write_long( FILE * fd, guint32 value );
guint32 file_read_long( FILE * fd );
guint16 file_read_short( FILE * fd );

/* Global Variables */

/********************************
* Path_FileLoad
* OLD Method
*/
GSList * Path_FileLoad( gchar * file )
{
	GSList * current = NULL;

	PHYSFS_File * handle = PHYSFS_openRead( file );

	if ( handle )
	{
		while ( !PHYSFS_eof( handle )  )
		{
			PathPoint * path = g_new0( PathPoint, 1 );
			PHYSFS_readSBE16( handle, &path->x );
			PHYSFS_readSBE16( handle, &path->y );
			PHYSFS_readUBE32( handle, &path->ms_length );
			current = g_slist_append( current, path );
		}
	}
	PHYSFS_close(handle);
	return current;
}


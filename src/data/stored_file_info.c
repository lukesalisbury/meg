/****************************
Copyright © 2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Standard Headers */
#include "loader_global.h"

/* Required Headers */
#include "stored_file_info.h"

/* Global Variables */


/* Local Variables */


/********************************
* StoredFileInfo_Free
*
@ data:
@ files:
*/
void StoredFileInfo_Free(StoredFileInfo * info )
{
	if ( info->name )
		g_free( info->name );
	if ( info->path )
		g_free( info->path );
	if ( info->compress_data )
		g_free( info->compress_data );

}


/********************************
* StoredFileInfo_Find
*
@ list:
@ file_name:
*/
StoredFileInfo * StoredFileInfo_Find( GSList * list, gchar * file_name )
{
	GSList * list_scan = list;
	while ( list_scan )
	{
		if ( file_name && ((StoredFileInfo *)list_scan->data)->name )
		{
			if ( !g_ascii_strcasecmp(((StoredFileInfo *)list_scan->data)->name, file_name) )
			{
				return (StoredFileInfo *)list_scan->data;
			}
		}
		list_scan = g_slist_next( list_scan );
	}
	return NULL;
}



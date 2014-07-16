/****************************
Copyright © 2013-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef STORED_FILE_INFO
#define STORED_FILE_INFO

#include <gtk/gtk.h>

typedef struct {
	gchar * name, * path;
	guint16 name_length;
	guint32 length, compress;
	guint8 * compress_data;
	guint32 offset;
	gint64 time;
	gboolean from_zip;
} StoredFileInfo;

void StoredFileInfo_Free( StoredFileInfo * info );
StoredFileInfo * StoredFileInfo_Find( GSList * list, gchar * file_name );
gchar * StoredFileInfo_ReadContent( GSList * list, gchar * file_name );



#endif

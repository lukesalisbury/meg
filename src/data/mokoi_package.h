/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef MOKOI_PACKAGE
#define MOKOI_PACKAGE


#include "stored_file_info.h"

void MokoiPackage_SaveGame( const gchar * dest, GSList * files, gchar * title, guint32 id, gchar * png, guint32 length );
void MokoiPackage_SavePatch( const gchar * dest, GSList * files, guint32 id, guint32 checksum );
void MokoiPackage_SaveResource( const gchar * dest, GSList * files, const gchar *title, const gchar *author, guint8 category );

void MokoiPackage_Extract(FILE * fd, StoredFileInfo * fi, const gchar *dest );
gchar * MokoiPackage_ExtractContent(FILE * fd, StoredFileInfo *fi);
void MokoiPackage_Add( GSList ** files, gchar * name, guchar * data, guint32 length );
void MokoiPackage_Remove( GSList ** files, gchar * name );


void MokoiPackage_OpenGame( FILE * src_fd, GSList ** files );
void MokoiPackage_OpenPatch( FILE * src_fd, GSList ** files );
gboolean MokoiPackage_OpenResource( FILE * src_fd, GSList ** files, gchar ** title,  gchar ** author, guint8 * category );


#endif

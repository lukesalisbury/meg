/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

gboolean Map_Copy( gchar * file, gchar * content );
gboolean Map_Open( gchar * file, MapInfo * map_info );
gboolean Map_New( gchar * file, guint width, guint height);
gboolean Map_Save( MapInfo * map_info );
void Map_Free( MapInfo * map_info, gboolean save );
gboolean Map_GetDimensions( gchar * file, gint * width, gint * height );

gboolean Map_SetStartingPoint( );
void Map_GetOptions( MapInfo * map_info );
GHashTable * Map_GetReplacableSheets( MapInfo * map_info );

void Map_ReplacableSheets_Update_ForEach( gchar * key, GtkWidget * text_sheet, MapInfo * map_info );
void Map_ReplacableSheets_Widget_ForEach( gchar * key, GtkWidget * text_sheet, GtkWidget * list );

gchar MapObject_Type( const gchar * type_name );
DisplayObjectTypes MapObject_Internal2DisplayObjectType( char internal_type );


gchar * MapChooser_Dialog( gchar * title, gchar * selected_item, gchar * additional_option );


/****************************
Copyright © 2013-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "data_types.h"

// SpriteData
SpriteData * SpriteData_New( )
{
	SpriteData * sprite_data = g_new0(SpriteData, 1);

	return sprite_data;
}



gboolean SpriteData_Free( SpriteData * sprite_data )
{
	CLEAR_STRING( sprite_data->mask.name );
	CLEAR_STRING( sprite_data->entity );

	return TRUE;
}

gboolean SpriteData_FreePointer( gpointer sprite_data )
{
	return SpriteData_Free( (SpriteData *)sprite_data );
}



// MapObjectData
MapObjectData * MapObjectData_New( const gchar * name )
{
	MapObjectData * object_data = g_new0(MapObjectData, 1);
	object_data->name = g_strdup(name);
	object_data->type = 'u';
	object_data->settings = g_hash_table_new(g_str_hash, g_str_equal);
	object_data->colour8.red = object_data->colour8.blue \
							 = object_data->colour8.green \
							 = object_data->colour8.alpha \
							 = 255;
	return object_data;
}
gboolean MapObjectData_Free( MapObjectData * object_data )
{
	CLEAR_STRING( object_data->name );
	CLEAR_STRING( object_data->type_name );

	CLEAR_STRING( object_data->entity_file );
	CLEAR_STRING( object_data->entity_language );
	CLEAR_STRING( object_data->object_name );

	g_hash_table_destroy( object_data->settings );

	return TRUE;
}

gboolean MapObjectData_FreePointer( gpointer object_data )
{
	return MapObjectData_Free( (MapObjectData *)object_data );
}

// VirtualObjectData
gboolean VirtualObjectData_FreePointer( gpointer data )
{
	VirtualObjectData * object_data = (VirtualObjectData *)data;

	CLEAR_STRING( object_data->name );
	CLEAR_STRING( object_data->ident );


	return TRUE;
}


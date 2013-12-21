/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef STRUCTS
	#define STRUCTS

#include "data_types.h"





typedef struct {
	gchar * xml_filename;
	gchar * thumb_filename;
	gchar * entity_filename;
	GdkRectangle position;
	GList * objects; /* MokoiMapObject */
	rgbaColour colour8;
	guint id_count;
	/*guint parse_mode;*/
} MokoiMap;

typedef struct {
	gchar * name;
	gchar * type_name;
	gchar type;
	guint id;
	GHashTable * settings;
	DisplayObject * object;
	MapInfo * parent;
	rgbaColour colour8;

	gchar * entity_file;
	gchar * object_id;
	gchar * entity_language;
	gboolean entity_global;

} MokoiMapObject;

typedef struct {
	gchar * name;
	gchar * ident;
	gint type;
	DisplayObject * object;
} VirtualObject;

typedef struct {
	GList * objects; /* MokoiMapObject */
	guint id_count;
} VirtualObjectList;



typedef struct {
	gchar * value;
	gchar * type;
	guint internal_type;
	gboolean removable;
	gboolean deleted;
	GtkWidget * widget;
} RuntimeSettingsStruct;


typedef struct {
	gchar * data;
	guint32 size;
} BinData;

#endif

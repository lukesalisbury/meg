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

#include "widgets/display_object.h"

typedef struct {
	gint16 x;
	gint16 y;
	guint32 ms_length;
} MokoiPath;

typedef struct {
	guint length_ms;
	GdkPoint offset;
	gchar * sprite;
} MokoiAnimationFrame;

typedef struct {
	guint ms_length;
	GtkTreeModel * model;
	GSList * frames; /* MokoiAnimationFrame */
	gboolean image_loaded;
	guint w, h;
} MokoiAnimation;

typedef struct {
	gchar * name;
	gint value;
} MokoiMask;

typedef struct {
	gchar * name;
	guint position;
	gint repeat; /* Single, Scaled Single, Repeat, Repeat Horizontal, Repeat Vertically */
} MokoiSpriteChild;

typedef struct {
/*	guint id;
	gchar * name;
	GdkRectangle position;
	gboolean show_rect;
*/
	SheetObject * detail;

	gchar * parent;
	gchar * ident;
	gchar * entity;

	gchar flag;
	gboolean visible;

	gboolean image_loaded;
	GdkPixbuf * image;

	MokoiAnimation * animation;
	MokoiMask * mask;
	GdkRectangle collisions[7];

	MokoiSpriteChild childrens[8]; /* <MokoiSpriteChild> */
	guint ref;
} MokoiSprite;

typedef struct {
/*	GdkPixbuf * image;
	gboolean image_loaded;
	gchar * file;
	gint64 file_size;
	GSList * children;
	gboolean visible;
*/
	Spritesheet * detail;
	GSList * children;  /* <MokoiSprite> */
	guint ref;
} MokoiSheet;

typedef struct {
	guint8 red;
	guint8 green;
	guint8 blue;
	guint8 alpha;
} rgbaColour;

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

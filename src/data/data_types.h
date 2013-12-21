/****************************
Copyright © 2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef DATA_TYPES_H
	#define DATA_TYPES_H

	#include "widgets/display_object.h"
	#include "ma_types.h"

	typedef struct {
		guint8 red;
		guint8 green;
		guint8 blue;
		guint8 alpha;
	} rgbaColour;


	typedef struct {
		gint16 x;
		gint16 y;
		guint32 ms_length;
	} PathPoint;

	typedef struct {
		guint length_ms;
		GdkPoint offset;
		gchar * sprite;
	} AnimationFrame;

	typedef struct {
		guint ms_length;
		GtkTreeModel * model;
		GSList * frames; /* AnimationFrame */
		gboolean image_loaded;
		guint w, h;
	} AnimationDetail;

	typedef struct {
		gchar * name;
		gint value;
	} ObjectMask;

	typedef struct {
		gchar * name;
		guint position;
		gint repeat; /* Single, Scaled Single, Repeat, Repeat Horizontal, Repeat Vertically */
	} SpriteChild;

	typedef struct {
		gchar * entity;
		gchar flag;

		GdkPixbuf * image;
		gboolean image_loaded;

		AnimationDetail * animation;
		ObjectMask mask;
		GdkRectangle collisions[7];

		SpriteChild childrens[8]; /* <SpriteChild> */

	} SpriteData;

	#define SPRITE_DATA(obj) ((SpriteData *)(obj)->data)

	SpriteData * SpriteData_New( );
	gboolean SpriteData_Free( SpriteData * sheet );
	gboolean SpriteData_FreePointer( gpointer sheet );
#endif // DATA_TYPES_H

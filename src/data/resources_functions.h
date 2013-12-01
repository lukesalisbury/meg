/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef RESOURCES_FUNCTIONS
	#define RESOURCES_FUNCTIONS
	#include <gtk/gtk.h>
	#include "loader_structs.h"

	/* Sheets */
	void Sheets_ListEach( MokoiSheet * sheet, GtkListStore * list );
	GSList * Sheet_Find( gchar * file_name );
	MokoiSheet * Sheet_Get( gchar * file_name, gboolean create );
	void Sheet_Load( MokoiSheet * current );
	gboolean Sheet_Parse( gchar * filename );

	/* Sprite */
	void Sprite_Clear( MokoiSprite * sprite );
	GSList * Sprite_Find( GSList * first, gchar * file_name );
	MokoiSprite * Sprite_Get( gchar * name, gchar * sheet, gboolean load);
	MokoiSprite * Sprite_GetFull( gchar * full_ident, gboolean load);
	GdkPixbuf * Sprite_GetPixbuf( gchar * name, gchar * sheet );
	GdkPixbuf * SpriteSheet_GetPixbuf(gchar * name, MokoiSheet * sheet );

	void VirtualObject_UpdateSprite( VirtualObject * object );
	VirtualObject * VirtualObject_New(gchar * name, gdouble area_width, gdouble area_height );



	/* Animation */
	MokoiAnimation * Animation_Get( gchar * name );


	/* Map Object */
	MokoiMapObject * MapObject_New(gchar * name , gdouble area_width, gdouble area_height);
	MokoiSprite * MapObject_UpdateSprite( MokoiMapObject * object );
	VirtualObject * VirtualObject_New(gchar * name, gdouble area_width, gdouble area_height );

	/* Sheets */
	MokoiSheet * MokoiSheet_New();
	gboolean MokoiSheet_Free( MokoiSheet * sheet );

#endif

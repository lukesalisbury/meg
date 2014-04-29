/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/


#ifndef MAP_INFO_H
#define MAP_INFO_H

typedef struct _MapInfo MapInfo;

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "gtk_compat.h"
#include "display_object.h"


struct _MapInfo {
	DisplayObject * selected;
	GList * display_list;
	GHashTable * settings;
	gchar * name;

	guint visible;
	guint id_counter;
	guint width, height;
	GdkRGBA colour;

	guint8 parse_mode;
	guint8 file_type;

	gpointer data;
	gboolean (*free) ( gpointer data );

};


void MapInfo_Destroy( MapInfo * data );

#endif // MAP_INFO_H

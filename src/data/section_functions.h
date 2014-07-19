/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef SECTION_FUNCTIONS
	#define SECTION_FUNCTIONS
	typedef struct {
		gchar * name;
		gint id;
		GdkRectangle position;
		GdkPixbuf * image;
	} MokoiWorldMap;

	typedef struct {
		gchar * name;
		GSList * maps;
		guint width;
		guint height;
		guint16 grid[64][64];
		guint8 type;
	} MokoiWorldFile;

	gchar * Section_NewMap( MokoiWorldFile * file, guint8 x, guint8 y, GtkWindow * window );
	void Section_RemoveMap( MokoiWorldFile * file, guint8 x, guint8 y, GtkWindow * window );
	gchar * Section_MapName( MokoiWorldFile * file, guint8 x, guint8 y, GtkWindow * window );
	gboolean Section_Save( MokoiWorldFile * file );
	MokoiWorldFile * Section_Load( gchar * file );
	void Section_Free( MokoiWorldFile * file );

	GdkPixbuf * Section_GetMapThumb( gchar * map );
	gboolean Section_Available( MokoiWorldFile * file, gchar * name, GdkRectangle * area );
	MokoiWorldFile * Section_Get( gchar * section );
#endif

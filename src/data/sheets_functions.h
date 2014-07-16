/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef SHEETS_FUNCTIONS
	#define SHEETS_FUNCTIONS
	#include <gtk/gtk.h>

	/* Sheets */
	void Sheets_ListEachVisible( Spritesheet * sheet, GtkListStore * list );
	void Sheets_ListEach( Spritesheet * sheet, GtkListStore * list );

	void Sheet_RemoveSprite( Spritesheet * sheet, SheetObject * sprite );

	gboolean Sheet_SpriteExists( Spritesheet * parent, gchar * sprite );
	GSList * Sheet_Find( gchar * file_name );
	Spritesheet * Sheet_Get( gchar * file_name, gboolean create );
	void Sheet_Load( Spritesheet * current );

	gboolean Sheet_Unload( );
	gboolean Sheet_Parse( gchar * filename );
	gboolean Sheet_ParseXMLFormat( gchar * filename );
	gboolean Sheet_ParseTextFormat( gchar * filename );

#endif



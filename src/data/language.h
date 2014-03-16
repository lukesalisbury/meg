/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef LANGUAGE_H
	#define LANGUAGE_H

typedef struct {
	gchar * type;
	gchar * file;
	GHashTable * values;
} MokoiLanguage;


void Language_SaveFile( MokoiLanguage * details, gchar * filename );
void Language_LoadFile( MokoiLanguage * file, gchar * type, gchar * lang, gboolean is_default );
void Language_TableList( gpointer key_p, gchar * value, GtkListStore * list );

void Language_Set_String( GHashTable * table, guint id, const gchar * string );
void Language_Clear( MokoiLanguage  * details );
void Language_ExportRoutines(  );


#endif

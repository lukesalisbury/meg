/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef ENTITY_OPTIONS_H
#define ENTITY_OPTIONS_H

#include "loader_global.h"

#define ENTITYOPTION_NONE 0
#define ENTITYOPTION_BOOLEAN 1
#define ENTITYOPTION_MUSIC 2
#define	ENTITYOPTION_SOUNDFX 3
#define	ENTITYOPTION_HIDDEN 4
#define	ENTITYOPTION_ENTITY 5
#define	ENTITYOPTION_SECTION 6
#define	ENTITYOPTION_SECTIONMAP 7
#define	ENTITYOPTION_MAP 8
#define	ENTITYOPTION_MAPENTITY 9
#define	ENTITYOPTION_NUMBER 10


EntityOptionStruct * EntityOption_Lookup( GHashTable * hash_table, const gchar * key);
guint EntityOption_Type( const gchar *type );
gboolean EntityOption_BooleanCheck( GHashTable * settings_table, gchar * value);

void EntityOption_AddOption( GtkButton * button, GtkWidget * table );
void EntityOption_CreateWidget( const gchar * name, EntityOptionStruct * options, GtkWidget * list );
void EntityOption_CreateWidgetWithSignal( const gchar * name, EntityOptionStruct * options, GtkWidget * list );
void EntityOption_AttachWidget( const gchar * name, EntityOptionStruct * options, GtkWidget * list );
void EntityOption_SaveWidget_Foreach( const gchar * name, EntityOptionStruct * options, gpointer data );

EntityOptionStruct * EntityOption_New(const gchar *value, const gchar *type );
void EntityOption_FreePointer( gpointer data );

EntityOptionStruct * EntityOption_Copy( EntityOptionStruct * value );
void EntityOption_InsertNew( GHashTable * settings_table, const gchar * key, const gchar * value, const gchar * type );
void EntityOption_Update( GHashTable * settings_table, const gchar *key, const gchar *value, const gchar  *type);
void EntityOption_UpdateValue(GHashTable * settings_table, const gchar *key, const gint value , const gchar * type);
void EntityOption_UpdateBoolean( GHashTable * settings_table, const gchar * key, const gboolean value, const gchar *type );
void EntityOption_Delete( EntityOptionStruct * data );
void EntityOption_Append(const gchar *key, EntityOptionStruct * value, GHashTable * table );
gint EntityOption_GetValue( GHashTable * settings_table, gchar * key );
void EntityOption_SetDefaultValues( DisplayObject * object );

#endif

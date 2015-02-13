/****************************
Copyright © 2007-2014 Luke Salisbury
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
#define	ENTITYOPTION_TARGET 11

EntitySettingsStruct * EntitySettings_Lookup( GHashTable * hash_table, const gchar * key);
guint EntitySettings_Type( const gchar *type );
gboolean EntitySettings_BooleanCheck( GHashTable * settings_table, gchar * value);

void EntitySettings_AddOption( GtkButton * button, GtkWidget * table );
void EntitySettings_CreateWidget( const gchar * name, EntitySettingsStruct * options, GtkWidget * list );
void EntitySettings_CreateWidgetWithSignal( const gchar * name, EntitySettingsStruct * options, GtkWidget * list );
void EntitySettings_AttachWidget( const gchar * name, EntitySettingsStruct * options, GtkWidget * list );
void EntitySettings_SaveWidget_Foreach( const gchar * name, EntitySettingsStruct * options, gpointer data );

EntitySettingsStruct * EntitySettings_New(const gchar *value, const gchar *type );
void EntitySettings_FreePointer( gpointer data );

EntitySettingsStruct * EntitySettings_Copy( EntitySettingsStruct * value );
EntitySettingsStruct * EntitySettings_InsertNew( GHashTable * settings_table, const gchar * key, const gchar * value, const gchar * type );
void EntitySettings_Update( GHashTable * settings_table, const gchar *key, const gchar *value, const gchar  *type);
void EntitySettings_UpdateValue(GHashTable * settings_table, const gchar *key, const gint value , const gchar * type);
void EntitySettings_UpdateBoolean( GHashTable * settings_table, const gchar * key, const gboolean value, const gchar *type );
void EntitySettings_Delete( EntitySettingsStruct * data );
void EntitySettings_Append(const gchar *key, EntitySettingsStruct * value, GHashTable * table );
gint EntitySettings_GetValue( GHashTable * settings_table, gchar * key );
void EntitySettings_SetDefaultValues( MapObjectData * object_data );

#endif

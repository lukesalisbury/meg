/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef RUNTIME_OPTIONS_H
#define RUNTIME_OPTIONS_H

#include "loader_global.h"

#define RUNTIMEOPTION_NONE 0
#define RUNTIMEOPTION_BOOLEAN 1
#define RUNTIMEOPTION_MUSIC 2
#define	RUNTIMEOPTION_SOUNDFX 3
#define	RUNTIMEOPTION_HIDDEN 4
#define	RUNTIMEOPTION_ENTITY 5
#define	RUNTIMEOPTION_SECTION 6
#define	RUNTIMEOPTION_MAP 7
#define	RUNTIMEOPTION_MAPENTITY 8

guint RuntimeSetting_Type( const gchar *type );
gboolean RuntimeSetting_BooleanCheck( GHashTable * settings, gchar * value);

void RuntimeSetting_AddOption( GtkButton * button, GtkWidget * table );
void RuntimeSetting_CreateWidget( const gchar * name, RuntimeSettingsStruct * options, GtkWidget * list );
void RuntimeSetting_CreateWidgetWithSignal( const gchar * name, RuntimeSettingsStruct * options, GtkWidget * list );
void RuntimeSetting_AttachWidget( const gchar * name, RuntimeSettingsStruct * options, GtkWidget * list );
void RuntimeSetting_SaveWidget_Foreach( const gchar * name, RuntimeSettingsStruct * options, gpointer data );

RuntimeSettingsStruct * RuntimeSetting_New(const gchar *value, const gchar *type );
RuntimeSettingsStruct * RuntimeSetting_Copy( RuntimeSettingsStruct * value );
void RuntimeSetting_Update( GHashTable * settings, gchar * key, gchar * value );
void RuntimeSetting_UpdateValue( GHashTable * settings, gchar * key, gint value );
void RuntimeSetting_UpdateBoolean( GHashTable * settings, gchar * key, gboolean value );
void RuntimeSetting_Delete( RuntimeSettingsStruct * data );
void RuntimeSetting_Append( gchar * key, RuntimeSettingsStruct * value, GHashTable * table );
gint RuntimeSetting_GetValue( GHashTable * settings, gchar * key );
void RuntimeSetting_SetDefaultValues( MokoiMapObject * object );

#endif

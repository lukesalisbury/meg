/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef ENTITY_OPTIONS_PARSER_H
#define ENTITY_OPTIONS_PARSER_H

#include "loader_global.h"

GHashTable * RuntimeParser_Load( gchar * entity_name );
gboolean RuntimeParser_Save( gchar * entity_name, GHashTable * options );

void RuntimeParser_CreateStore( gchar * key, EntityOptionStruct * value, GtkListStore * liststore );
void RuntimeParser_SaveString( gchar * key, EntityOptionStruct * value, GString * file_content );

#endif






/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef LOADER_GLOBAL
#define LOADER_GLOBAL
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>

#include "ma_types.h"
#include "alchera_loader.h"
#include "alchera_functions.h"
#include "loader_structs.h"

#include "data_types.h"
#include "resources_functions.h"
#include "sprite_grouping.h"
#include "meg_setting.h"
#include "physfs/physfs.h"


gboolean AL_SettingBoolean( gchar * key );
gchar * AL_Setting_GetString( gchar * key );
gint AL_Setting_GetNumber( gchar * key );

gboolean EntityCombo_Properties_Open( GtkWidget * button, GdkEventButton * event, GtkComboBox * combo );


#endif

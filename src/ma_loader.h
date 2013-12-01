/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _AL_H_
#define _AL_H_

#include <gtk/gtk.h>
#include <glib.h>
#include "alchera_types.h"
#include "alchera_loader.h"

gboolean Alchera_Loaders_CreateNew(const gchar * title, gchar * base_project);
gboolean Alchera_Loaders_Init(gchar * project_path);
gboolean Alchera_Loaders_Close( );

#endif

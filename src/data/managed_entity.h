/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <gtk/gtk.h>

typedef struct
{
	gchar * name;
	gchar * predefined;
	gchar * custom;
} EntityFunction;

typedef struct
{
	gchar * name;
	GList * causes;
	GList * actions;
	guint directions;
	gchar * base_sprite;
	GList * collisions;
	gboolean locked;
} EntityState;

enum TemplateParserModes {
	TPM_NEW,
	TPM_ROOT,
	TPM_STATE,
	TPM_SETTING,
	TPM_FUNCTION,
	TPM_COLLSION
};

typedef struct
{
	gchar * file;
	GList * subroutines;
	GHashTable * options;
	GList * states;
	GtkTreeStore * store;
	guint mode;
	GtkTreeIter * parent_iter;
	GtkTreeIter * current_iter;
} MokoiManagedEntity;

gboolean ManagedEntity_Save(gchar * entity_name, MokoiManagedEntity * managed_entity);
MokoiManagedEntity * ManagedEntity_Load(gchar * entity_name);




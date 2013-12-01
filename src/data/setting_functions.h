/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef SETTING_FUNCTIONS
	#define SETTING_FUNCTIONS
	gboolean Setting_Unlocked( char * key );
	gboolean Setting_Update( GtkListStore * store, GtkTreePath * path, GtkTreeIter * iter, gboolean * result );
	gboolean Setting_Save();
	void Setting_WidgetWrite(gchar * name, GObject * wid );

	void Setting_Package( GtkComboBox * combo, gchar * prefix );

	void Setting_Event_ButtonSave(GtkButton* button, GHashTable * table );
	void Setting_Event_SetImage(GtkButton* button, gchar * type );
	void Setting_Dialog_CustomController(GtkButton* button, GHashTable * table );

	void Setting_SaveForeach(gpointer key, gpointer value, gpointer user_data);
	void Setting_LoadForeach(gpointer key, gpointer value, gpointer user_data);
	void Setting_ClearForeach(gpointer key, gpointer value, gpointer user_data);

	void Setting_ListAdvance();
	void Setting_ClearAdvance();

#endif

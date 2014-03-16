/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

void Meg_Language_Realize(GtkWidget * widget, gpointer user_data);
void Meg_Language_TypeChanged(GtkComboBox * widget, gpointer user_data);
void Meg_Language_Save(GtkToolButton * widget, gpointer user_data);
void Meg_Language_StringEdit( GtkCellRendererText *cellrenderertext, gchar *path_string, gchar *new_text );
void Meg_Language_AddLanguage( GtkToolButton * widget, gpointer user_data );
void Meg_Language_RemoveLanguage(GtkToolButton * widget);
void Meg_Language_AddString(GtkToolButton *widget, gpointer user_data);
void Meg_Language_RemoveString( GtkToolButton *widget, GtkTreeView *tree );


void Meg_Language_EditDialog( GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data );

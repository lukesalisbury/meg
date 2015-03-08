/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/



void Entity_Load( gchar * name, GtkTextBuffer * buffer );
void Entity_Save( gchar * name, GtkTextBuffer * buffer );

gboolean Entity_New( gchar * filename, gchar * script );
gboolean Entity_Add();
gboolean Entity_Remove( gchar * filename );
gboolean Meg_EntityList_Settings(const gchar *filename );

void Entity_NewDialog(GtkButton * button , GtkTreeView * tree);
void EntityList_UpdateMain();
void EntityList_Menu(GtkWidget * treeview, gpointer data);
gboolean EntityList_MenuClick( GtkWidget *treeview, GdkEventButton *event, gpointer data);
void EntityList_Selection( GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data );
void Entity_RebuildAll();

void EntityListMenu_OpenAction( GtkMenuItem * menuitem, GtkTreeView * tree_view  );
void EntityListMenu_CompileAction( GtkMenuItem * menuitem, GtkTreeView * tree_view );
void EntityListMenu_SettingsAction( GtkMenuItem * menuitem, GtkTreeView * tree_view );
void EntityListMenu_RemoveAction( GtkMenuItem * menuitem, GtkTreeView * tree_view );



// Entity Functions
gboolean Entity_New( gchar * filename, gchar * script );
void Entity_Edit( gchar * file );
gboolean EntityCompiler_FileWithRountines(gchar * string_inputfile, GtkWidget * logwidget, GtkWidget * textwidget );
gboolean EntityCompiler_File( gchar * inputfile, GtkWidget * logwidget, GtkWidget * textwidget );

// Entity List Functions
void EntityList_CreatePage();
void EntityList_UpdatePage( GtkTreeModel * list );

// Editor
GtkWidget * EntityEditor_New( gchar * file );

// Runtime Option Functions


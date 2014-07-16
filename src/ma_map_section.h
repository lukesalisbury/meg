/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef MA_MAP_SECTION
#define MA_MAP_SECTION

GtkWidget * Meg_MapSection_Open( gchar * file );

gboolean Meg_MapSection_LayoutPressed( GtkWidget * widget, GdkEventButton * event, GtkWidget * window );
void Meg_MapSection_ButtonSettings( gpointer user_data, GtkToolButton *toolbutton );
void Meg_MapSection_KeyPressed( GtkWidget * widget, GdkEventKey * event, GtkWidget * window );
gboolean Meg_MapSection_LayoutDraw( GtkWidget * widget, cairo_t * cr, GtkWidget * overview );
gboolean Meg_MapSection_LayoutExpose( GtkWidget * widget, GdkEventExpose *event, GtkWidget * overview );

void Meg_MapSection_AddFile( GtkAction * action, GtkWidget * treeview );
void Meg_MapSection_RemoveFile( GtkAction * action, GtkWidget * treeview );
void Meg_MapSection_OpenFile(GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn *column, gpointer user_data  );

void Meg_MapSection_ListMenu( GtkTreeView * treeview, gpointer user_data );

void Meg_MapSection_ButtonSave( GtkButton *button, GtkWidget *window );
void Meg_MapSection_ButtonZoomOut( GtkButton *button, GtkWidget *window );
void Meg_MapSection_ButtonZoomNormal( GtkButton *button, GtkWidget *window );
void Meg_MapSection_ButtonZoomIn( GtkButton *button, GtkWidget *window );

void Meg_MapSection_ComboType( GtkComboBox * combo, GtkWidget * window );

#endif

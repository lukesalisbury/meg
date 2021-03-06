/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

void Meg_Audio_Save( gboolean force );
gboolean MegWidget_Audio_Destroy(GtkWidget * widget, GdkEvent * event, gpointer user_data );

void Audio_Listing_Refresh(  GtkButton * button, GtkTreeView * treeview );
void Audio_Listing_Update( GtkTreeView * widget );

void Audio_File_Selected( GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data );
void Audio_File_Add( GtkButton * button, GtkTreeView * treeview );
void Audio_File_Remove( GtkButton * button, GtkTreeView * treeview );
gboolean Audio_File_Import( gchar * source_file );

void Audio_Chooser_Play( GtkButton * button, GtkFileChooser * chooser );
void Audio_Chooser_AddFilter( GtkWidget * file_chooser, const gchar * title, const gchar * pattern );

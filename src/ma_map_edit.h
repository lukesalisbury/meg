/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef MA_MAP_EDIT
#define MA_MAP_EDIT

gboolean Meg_MapEdit_CloseTab( GtkWidget * event_box, GdkEvent * event, GtkWidget * widget );
gboolean Meg_MapEdit_KeyPress( GtkWidget * widget, GdkEventKey * event, GtkComboBox * layers );
GtkWidget * Meg_MapEdit_GetMapWidget( GtkWidget * widget );
void Meg_MapEdit_OpenFilename( gchar * file );

void Alchera_Map_AnimationIdle(DisplayObject* object, cairo_t *cr);
void Alchera_Map_DisplayObjectAdvance(GtkTreeView * tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);


MapInfo * Meg_MapEdit_GetMapInfo( GtkWidget * widget );
void Meg_MapEdit_RefreshSettings( GtkWidget * map_widget );


GtkWidget * Meg_MapEdit_Open( gchar * file );
void Meg_MapEdit_OpenFile( GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn *column, gpointer user_data );
void Meg_MapEdit_AddFile( GtkAction * action, GtkTreeView * treeview );
void Meg_MapEdit_RemoveFile( GtkAction * action, GtkWidget * treeview );
void Meg_MapEdit_ListMenu( GtkTreeView * treeview, gpointer user_data );
void Meg_MapEdit_OpenFilename( gchar * file );

void Meg_MapEdit_Resort( GtkAction * action, AlcheraMap * map );
void Meg_MapEdit_Undo( GtkAction * action, AlcheraMap * map );
void Meg_MapEdit_GroupChanged( GtkComboBox * widget, GtkWidget * iconview );
void Meg_MapEdit_LayerChanged( GtkComboBox * layers, GtkWidget * layout );
void Meg_MapEdit_ToggleLayer(GtkToolButton * toolbutton , GtkWidget * layers);
void Meg_MapEdit_UpdateLayers( GtkToolButton * toolbutton , GtkWidget * map_widget );
void Meg_MapEdit_ButtonAlign( GtkToggleToolButton * widget, GtkWidget * map_widget );
void Meg_MapEdit_ButtonSave( GtkButton *widget, GtkWidget * map_widget );
void Meg_MapEdit_ButtonOptions(GtkButton *widget, GtkWidget * map_widget );
void Meg_MapEdit_ButtonZoomOut(GtkButton *widget, GtkWidget * map_widget );
void Meg_MapEdit_ButtonZoomNormal( GtkButton *widget, GtkWidget * map_widget );
void Meg_MapEdit_ButtonZoomIn( GtkButton *widget, GtkWidget * map_widget );
void Meg_MapEdit_ButtonKeys( GtkButton *widget, GtkWidget * box );

gboolean Meg_MapObject_PushAnimation( DisplayObject * object );
void Meg_MapEdit_ObjectSelected( GtkIconView * icon_view, GtkTreePath *path, GtkWidget * map_widget );
void Meg_MapEdit_ObjectDrop( GtkWidget * widget, GdkDragContext * drag_context, GtkSelectionData * data, guint info, guint time, gpointer user_data );
gboolean Meg_MapEdit_ObjectSelectedKey( GtkWidget * layout, GdkEventKey * event, GtkIconView * icon_view );
void Meg_MapEdit_ColorUpdate( GtkColorButton * widget, GtkWidget * map_widget );
void Meg_MapEdit_DisplayObjectAdvance( GtkTreeView * tree_view, GtkTreeViewColumn *column, GtkTreePath *path, GtkWidget * map_widget );
void Meg_MapEdit_EditorMenu( GtkMenuToolButton * button, GtkWidget * map_widget );

void Meg_MapEdit_GroupChangedNext( GtkWidget * widget, GtkComboBox * combo_group );
void Meg_MapEdit_GroupChangedPrev( GtkWidget * widget, GtkComboBox * combo_group );

void Meg_MapObject_Advance( GtkAction * action, AlcheraMap * map );
void Meg_MapObject_LayerUp( GtkAction * action, AlcheraMap * map );
void Meg_MapObject_LayerDown( GtkAction * action, AlcheraMap * map );
void Meg_MapObject_Remove( GtkAction * action, AlcheraMap * map );
void Meg_MapObject_Rotate( GtkAction * action, AlcheraMap * map );
void Meg_MapObject_Attach( GtkAction * action, AlcheraMap * map );
void Meg_MapObject_Path( GtkAction * action, AlcheraMap * map );
void Meg_MapObject_Flip( GtkToggleAction * action, AlcheraMap * map );
void Meg_MapObject_PathModify( GtkAction * action, AlcheraMap * map );


void Meg_MapObject_KeyUp( GtkAction * action, AlcheraMap * map );
void Meg_MapObject_KeyDown( GtkAction * action, AlcheraMap * map );
void Meg_MapObject_KeyLeft( GtkAction * action, AlcheraMap * map );
void Meg_MapObject_KeyRight( GtkAction * action, AlcheraMap * map );

void Meg_MapObject_WidthDecrease( GtkAction * action, AlcheraMap * map );
void Meg_MapObject_WidthIncrease( GtkAction * action, AlcheraMap * map );
void Meg_MapObject_HeightDecrease( GtkAction * action, AlcheraMap * map );
void Meg_MapObject_HeightIncrease( GtkAction * action, AlcheraMap * map );


#endif

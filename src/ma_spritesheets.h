/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* */
void MegWidget_Spritesheet_SetFile( Spritesheet * spritesheet );
void MegWidget_Spritesheet_SetSheetList( );
void MegWidget_Spritesheet_RefreshSpriteList( );


void Meg_Spritesheet_ButtonColourSet( GtkColorButton *widget, gpointer user_data );

void Meg_Spritesheet_ButtonZoomOut( GtkButton * widget, gpointer user_data );
void Meg_Spritesheet_ButtonZoomNormal( GtkButton * widget, gpointer user_data );
void Meg_Spritesheet_ButtonZoomIn( GtkButton * widget, gpointer user_data );
void Meg_Spritesheet_SetMode( GtkToggleButton * widget, gpointer user_data );
void Meg_Spritesheet_SetAlign( GtkToggleButton * widget, gpointer user_data );
void Meg_Spritesheet_SetVisible( GtkToggleButton * widget, gpointer user_data );

gboolean Meg_Spritesheet_ViewDraw( GtkWidget * window, cairo_t * cr, gpointer data );
gboolean Meg_Spritesheet_ViewExpose( GtkWidget * widget, GdkEventExpose * event, gpointer data );
gboolean Meg_Spritesheet_ViewMouse( GtkWidget *widget, GdkEventButton *event, gpointer user_data );
gboolean Meg_Spritesheet_DrawMouse( GtkWidget *widget, GdkEventMotion *event, gpointer user_data );

void Meg_Spritesheet_SpriteSelect( GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data );

void Meg_Spritesheet_SheetChanged( GtkComboBox *widget, gpointer user_data );
void Meg_Spritesheet_SheetAdd( GtkButton * widget, gpointer user_data );
void Meg_Spritesheet_SheetRemove( GtkButton * widget,GtkWidget * combo );
void Meg_Spritesheet_SheetEdit( GtkButton *widget, GtkWidget *combo );


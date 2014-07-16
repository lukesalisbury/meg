/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Standard Headers */
#include "loader_global.h"
#include "virtual_sprite.h"
#include "widgets/widget_map.h"
#include "gtk_compat.h"
#include "maps.h"

/* External Functions */
void Meg_MapEdit_GroupChangedNext( GtkWidget * widget, GtkComboBox * combo_group );
void Meg_MapEdit_GroupChangedPrev( GtkWidget * widget, GtkComboBox * combo_group );
void Meg_MapEdit_GroupChanged( GtkComboBox * widget, GtkWidget * iconview );
void Meg_MapEdit_ObjectSelected( GtkIconView * icon_view, GtkTreePath *path, GtkWidget * map_widget );
void Meg_MapEdit_ObjectDrop( GtkWidget * widget, GdkDragContext * drag_context, GtkSelectionData * data, guint info, guint time, gpointer user_data );
gboolean Meg_MapEdit_ObjectSelectedKey( GtkWidget * layout, GdkEventKey * event, GtkIconView * icon_view );


/* Global Variables */
extern const GtkTargetEntry alchera_map_drop_target;

/* Local Variables */


/* UI */
#include "ui/virtual_sprite_dialog.gui.h"
const gchar * ui_data_virtual_sprite_dialog = GUIVIRTUAL_SPRITE_DIALOG;


/********************************
* VirtualSpriteDialog_UpdateName
*
*/
void VirtualSpriteDialog_UpdateName(GtkLabel *label, gchar * uri,  MapInfo * map_info)
{
	g_return_if_fail( map_info );

	GtkWidget * dialog = gtk_dialog_new_with_buttons( "Rename Virtual Sprite", Meg_Misc_ParentWindow(GTK_WIDGET(label)), GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL );
	GtkWidget * entry = gtk_entry_new();

	gtk_entry_set_max_length( GTK_ENTRY(entry), 64 );

	gtk_box_pack_start( GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), entry, FALSE, FALSE, 1);
	gtk_widget_show_all( gtk_dialog_get_content_area(GTK_DIALOG(dialog)) );

	gint result = gtk_dialog_run( GTK_DIALOG (dialog) );
	switch (result)
	{
		case GTK_RESPONSE_ACCEPT:
		{
			REPLACE_STRING_DUPE( map_info->name, gtk_entry_get_text( GTK_ENTRY(entry) ) );
			VirtualSprite_UpdateName(label, map_info);
			break;
		}
		case GTK_RESPONSE_REJECT:
		{

			break;
		}
		default:

			break;
	}
	gtk_widget_destroy( dialog );

}

/********************************
* VirtualSprite_DialogDisplay
*
*/
gboolean VirtualSpriteDialog_Display( gchar * id )
{
	MapInfo * map_info = VirtualSprite_GetInfo( id );

	g_return_val_if_fail( map_info, FALSE );

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create( ui_data_virtual_sprite_dialog, __func__, __LINE__ );
	g_return_val_if_fail( ui, FALSE );

	/* Widgets */
	GtkWidget * window, * editor_viewport, * widget_map, * label_name, * iconview_objects, * combo_groups;
	GtkListStore * store_current_objects, * store_groups;

	window = GET_WIDGET( ui, "window" );
	editor_viewport = GET_WIDGET( ui, "editor_viewport");
	label_name = GET_WIDGET( ui, "label_name");

	iconview_objects = GET_WIDGET( ui, "iconview_objects" );
	combo_groups = GET_WIDGET( ui, "combo_groups" );
	store_current_objects = GET_LISTSTORE( ui, "store_current_objects" );
	store_groups = GET_LISTSTORE( ui, "store_groups" );

	if ( store_groups  )
	{
		/* Spritesheet loading */
		char ** files = PHYSFS_enumerateFiles("/sprites/virtual/");
		char ** current;

		for (current = files; *current != NULL; current++)
		{
			if ( g_str_has_suffix( *current, ".xml" ) )
			{
				gchar * file_name = g_strndup(*current, g_utf8_strlen(*current, -1) - 4 ); // Strip .xml
				VirtualSpriteSheet_Insert( file_name, TRUE );
				g_free(file_name);
			}
		}
		PHYSFS_freeList(files);

		gtk_list_store_clear( store_groups );
		AL_Object_Groups( store_groups, FALSE );
	}

	/* Settings */
	VirtualSprite_UpdateName( GTK_LABEL(label_name), map_info );
	gtk_icon_view_enable_model_drag_source( GTK_ICON_VIEW(iconview_objects), GDK_BUTTON1_MASK, &alchera_map_drop_target, 1, GDK_ACTION_COPY );


	/* Alchera Map widget */
	widget_map = gtk_alchera_map_new( map_info );
	if ( GTK_IS_ALCHERA_MAP(widget_map) )
	{
		gtk_alchera_map_set_support_widget( GTK_ALCHERA_MAP(widget_map), store_current_objects, NULL, NULL );
		gtk_widget_set_size_request( GTK_WIDGET(editor_viewport), map_info->width, map_info->height );
		gtk_alchera_map_set_align( GTK_ALCHERA_MAP(widget_map), TRUE );
		gtk_widget_show_all( widget_map );
	}
	gtk_container_add( GTK_CONTAINER(editor_viewport), widget_map );

	/* Signals */
	SET_OBJECT_SIGNAL( ui, "button_save", "clicked", G_CALLBACK(VirtualSprite_Save), map_info );
	SET_OBJECT_SIGNAL_SWAP( ui, "button_close", "clicked", G_CALLBACK(gtk_widget_destroy), window );
	SET_OBJECT_SIGNAL_SWAP( ui, "window", "close", G_CALLBACK(gtk_widget_destroy), window );

	SET_OBJECT_SIGNAL( ui, "label_name", "activate-link", G_CALLBACK(VirtualSpriteDialog_UpdateName), map_info );

	g_signal_connect( G_OBJECT(combo_groups), "changed", G_CALLBACK(Meg_MapEdit_GroupChanged), iconview_objects );

	SET_OBJECT_SIGNAL( ui, "button_nextgroup", "clicked", G_CALLBACK(Meg_MapEdit_GroupChangedNext), combo_groups );
	SET_OBJECT_SIGNAL( ui, "button_prevgroup", "clicked", G_CALLBACK(Meg_MapEdit_GroupChangedPrev), combo_groups );

	g_signal_connect( G_OBJECT(iconview_objects), "item-activated", G_CALLBACK(Meg_MapEdit_ObjectSelected), widget_map );
	g_signal_connect( G_OBJECT(iconview_objects), "drag-data-get", G_CALLBACK(Meg_MapEdit_ObjectDrop), widget_map );

	gtk_dialog_run( GTK_DIALOG(window) );


	return TRUE;
}

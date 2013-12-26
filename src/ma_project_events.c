/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Global Header */
#include "global.h"

/* Global Variables */


/********************************
* MegWidget_Project_AddSetting
*
*/
void MegWidget_Project_AddSetting(GtkButton * widget, GtkListStore * config_store )
{
	GtkWidget * dialog = gtk_dialog_new_with_buttons( "Add Settings to project", Meg_Main_GetWindow(), GTK_DIALOG_MODAL, GTK_STOCK_OK,  GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL );
	GtkWidget * entry = gtk_entry_new();
	gtk_entry_set_max_length( GTK_ENTRY(entry), 64);

	gtk_box_pack_start( GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), entry, FALSE, FALSE, 1);
	gtk_widget_show_all( gtk_dialog_get_content_area(GTK_DIALOG(dialog)) );

	gint result = gtk_dialog_run (GTK_DIALOG (dialog));
	switch (result)
	{
		case GTK_RESPONSE_ACCEPT:
		{
			const gchar * title = gtk_entry_get_text( GTK_ENTRY(entry) );
			GtkTreeIter iter;
			gtk_list_store_append(config_store, &iter);
			gtk_list_store_set(config_store, &iter, 0, title, 1, "0", 2, TRUE, -1);
			break;
		}
		case GTK_RESPONSE_REJECT:
		{

			break;
		}
		default:

			break;
	}
	gtk_widget_destroy(dialog);
	//AL_Settings_Set(config_store, TRUE);
}

/********************************
* MegWidget_Project_RemoveSetting
*
*/
void MegWidget_Project_RemoveSetting(GtkButton *widget, GtkWidget * treeview )
{
	gchar * key = NULL;
	GtkTreeSelection * select;
	GtkTreeIter iter;
	GtkTreeModel * model;

	select = gtk_tree_view_get_selection( GTK_TREE_VIEW(treeview) );
	if ( gtk_tree_selection_get_selected( select, &model, &iter ) )
	{
		gtk_tree_model_get( model, &iter, 0, &key, -1 );
		if ( AL_Setting_Remove(key) )
		{
			gtk_list_store_remove( GTK_LIST_STORE(model), &iter);
		}
	}
}

/********************************
* MegWidget_Project_EditSetting
*
*/
void MegWidget_Project_EditSetting(GtkCellRendererText * cellrenderertext, gchar * path_string, gchar * new_text, GtkListStore * config_store )
{
	gchar * key = NULL;
	GtkTreeIter iter;
	gtk_tree_model_get_iter_from_string( GTK_TREE_MODEL(config_store), &iter, path_string );
	gtk_tree_model_get( GTK_TREE_MODEL(config_store), &iter, 0, &key, -1);
	gtk_list_store_set(config_store, &iter, 1, new_text, -1);
	if ( key )
	{
		AL_Setting_SetString( key, new_text );
	}
	else
	{
		g_warning("Project setting have not changed");
	}
}


/********************************
* MegWidget_Project_Menu_Display
* Event:
*/
void MegWidget_Project_Menu_Display( GtkWidget * treeview, GdkEventButton *event, GtkListStore * config_store )
{
	GtkWidget * menu = gtk_menu_new();
	GtkWidget * item_add, * item_remove;

	item_add = gtk_image_menu_item_new_from_stock(GTK_STOCK_ADD, NULL);
	g_signal_connect(item_add, "activate", G_CALLBACK(MegWidget_Project_AddSetting), config_store);
	gtk_menu_shell_append( GTK_MENU_SHELL(menu), item_add );

	item_remove = gtk_image_menu_item_new_from_stock(GTK_STOCK_REMOVE, NULL);
	g_signal_connect(item_remove, "activate", G_CALLBACK(MegWidget_Project_RemoveSetting), treeview);
	gtk_menu_shell_append( GTK_MENU_SHELL(menu), item_remove );


	gtk_widget_show_all(menu);

	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, (event != NULL ? event->button : 0), gdk_event_get_time((GdkEvent*)event) );

}

/********************************
* MegWidget_Project_Menu_Click
* Event:
*/
gboolean MegWidget_Project_Menu_Click( GtkWidget *treeview, GdkEventButton *event, GtkListStore * config_store )
{
	if ( event->type == GDK_BUTTON_PRESS && event->button == 3 )
	{
		MegWidget_Project_Menu_Display( treeview, event, config_store );
		return TRUE;
	}
	return FALSE;
}

/********************************
* MegWidget_Project_Menu_Button
* Event:
*/
gboolean MegWidget_Project_Menu_Button( GtkWidget *treeview, GtkListStore * config_store )
{
	MegWidget_Project_Menu_Display( treeview, NULL, config_store );
	return TRUE;
}


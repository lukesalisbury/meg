/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Global Header */
#include "global.h"
#include "ma_language.h"
#include "ma_misc.h"

void AL_Language_Load( );

/* Functions local */

/* Global Variables */
GtkWidget * language_type_combo;
GtkListStore * language_string_liststore, * language_type_liststore, * language_lang_liststore;
gchar * language_file = NULL;

/* UI */
#include "ui/page_language.gui.h"
const gchar * meg_language_ui = GUIPAGE_LANGUAGE;

/* Functions */

/********************************
* MegWidget_Language_Create
*
*/
void MegWidget_Language_Create()
{
	GtkWidget * widget = NULL;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create( meg_language_ui, __func__, __LINE__ );
	g_return_if_fail( ui );

	/* Widget */
	widget = GET_WIDGET( ui, "language_widget" );
	language_type_combo = GET_WIDGET( ui, "language_type_combo" );

	language_type_liststore = GET_LISTSTORE( ui, "store_language_type" ); /* File Name  */
	language_string_liststore = GET_LISTSTORE( ui, "store_language_list" ); /* ID, Default String */

	/* Widget Settings */
	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(language_string_liststore), 0, Meg_Misc_TreeIterCompare, GINT_TO_POINTER(1), NULL );
	gtk_tree_sortable_set_sort_column_id( GTK_TREE_SORTABLE(language_string_liststore), 0, GTK_SORT_ASCENDING );

	gtk_combo_box_set_model( GTK_COMBO_BOX(language_type_combo), GTK_TREE_MODEL(language_type_liststore) );


	/*  Signal */
	//g_signal_connect( widget, "realize", G_CALLBACK(Meg_Language_Realize), NULL );
	g_signal_connect( language_type_combo, "changed", G_CALLBACK(Meg_Language_TypeChanged), NULL );


	SET_OBJECT_SIGNAL( ui, "language_button_add", "clicked", G_CALLBACK(Meg_Language_AddLanguage), language_type_combo );
	SET_OBJECT_SIGNAL( ui, "language_button_remove", "clicked", G_CALLBACK(Meg_Language_RemoveLanguage), language_type_combo );
	SET_OBJECT_SIGNAL( ui, "language_button_save", "clicked", G_CALLBACK(Meg_Language_Save), language_type_combo );
	SET_OBJECT_SIGNAL( ui, "string_button_add", "clicked", G_CALLBACK(Meg_Language_AddString), language_type_combo );

	SET_OBJECT_SIGNAL( ui, "language_string_cell", "edited", G_CALLBACK(Meg_Language_StringEdit), NULL );
	SET_OBJECT_SIGNAL( ui, "language_list", "row-activated", G_CALLBACK(Meg_Language_EditDialog), NULL );

	Meg_Main_AddSection( widget, "Language", PAGE_ICON_LANGUAGE );

}


/********************************
* MegWidget_Language_Init()
*
*/
void MegWidget_Language_Init()
{
	AL_Language_Load( );

	AL_Language_Types(language_type_liststore);

	gtk_combo_box_set_active( GTK_COMBO_BOX(language_type_combo), 0 );

}

/********************************
* Meg_Language_Close()
*
*/
void MegWidget_Language_Close()
{
	gtk_list_store_clear(language_string_liststore);
}



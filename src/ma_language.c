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
#include "ma_language.h"
#include "ma_misc.h"

void AL_Language_Load( );

/* Functions local */

/* Global Variables */
GtkWidget * alchera_language_default, * alchera_language_typecombo, * alchera_language_chooser;
GtkListStore * language_string_liststore, * language_type_liststore, * language_lang_liststore;
gchar * language_file = NULL;

/* UI */
#include "ui/language_page.gui.h"
const gchar * alchera_language_ui = GUILANGUAGE_PAGE


/* Functions */

/********************************
* MegWidget_Language_Create
*
*/
void MegWidget_Language_Create()
{
	GtkWidget * alchera_language_widget;
	GError * error = NULL;
	GtkBuilder * ui = gtk_builder_new();
	if ( !gtk_builder_add_from_string( ui, alchera_language_ui, -1, &error ) )
	{
		Meg_Error_Print( __func__, __LINE__, "UI creation error: %s", error->message );
		return;
	}

	alchera_language_widget = GET_WIDGET( ui, "alchera_language_widget" );
	alchera_language_typecombo = GET_WIDGET( ui, "alchera_language_typecombo" );

	language_type_liststore = gtk_list_store_new (1, G_TYPE_STRING); /* File Name  */
	language_string_liststore = GET_LISTSTORE( ui, "store_language_list" ); /* ID, Default String */

	Meg_Main_AddSection(alchera_language_widget, "Language", PAGE_ICON_LANGUAGE);

	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(language_string_liststore), 0, Meg_Misc_TreeIterCompare, GINT_TO_POINTER(1), NULL);
	gtk_tree_sortable_set_sort_column_id( GTK_TREE_SORTABLE(language_string_liststore), 0, GTK_SORT_ASCENDING );

	gtk_combo_box_set_model( GTK_COMBO_BOX(alchera_language_typecombo), GTK_TREE_MODEL(language_type_liststore) );

	/*  Signal */
	g_signal_connect( alchera_language_widget, "realize", G_CALLBACK(Alchera_Language_Realize), NULL);
	g_signal_connect( alchera_language_typecombo, "changed", G_CALLBACK(Alchera_Language_TypeChanged), NULL);


	SET_OBJECT_SIGNAL( ui, "language_button_add", "clicked", G_CALLBACK(Alchera_Language_AddLanguage), alchera_language_typecombo );
	SET_OBJECT_SIGNAL( ui, "language_button_remove", "clicked", G_CALLBACK(Alchera_Language_RemoveLanguage), alchera_language_typecombo );
	SET_OBJECT_SIGNAL( ui, "language_button_save", "clicked", G_CALLBACK(Alchera_Language_Save), alchera_language_typecombo );
	SET_OBJECT_SIGNAL( ui, "string_button_add", "clicked", G_CALLBACK(Alchera_Language_AddString), alchera_language_typecombo );

	SET_OBJECT_SIGNAL( ui, "language_string_cell", "edited", G_CALLBACK(Alchera_Language_StringEdit), NULL );
	SET_OBJECT_SIGNAL( ui, "meg_language_list", "row-activated", G_CALLBACK(Alchera_Language_EditDialog), NULL );


}


/********************************
* MegWidget_Language_Init()
*
*/
void MegWidget_Language_Init()
{
	AL_Language_Load( );
}

/********************************
* Alchera_Language_Close()
*
*/
void MegWidget_Language_Close()
{

}



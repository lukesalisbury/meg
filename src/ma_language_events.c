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

/* Functions local */


void AL_Language_Save( );

/* Global Variables */
extern GtkWidget * alchera_language_default, * alchera_language_typecombo, * alchera_language_chooser;
extern GtkListStore * language_string_liststore, * language_type_liststore, * language_lang_liststore;
extern gchar * language_file;

/* UI */
#include <ui/language_edit.gui.h>
const gchar * meg_language_edit_ui = GUILANGUAGE_EDIT

/* Functions */

/********************************
* Alchera_Language_Realize
* Event:
* Result:
*/
void Alchera_Language_Realize(GtkWidget * widget, gpointer user_data)
{
	AL_Language_Types(language_type_liststore);
	gtk_combo_box_set_active( GTK_COMBO_BOX(alchera_language_typecombo), 0 );
}

/********************************
* Alchera_Language_TypeChanged
* Event:
* Result:
*/
void Alchera_Language_TypeChanged(GtkComboBox * widget, gpointer user_data)
{
	GtkTreeIter iter;
	if ( gtk_combo_box_get_active_iter( widget, &iter ) )
	{
		if ( language_file )
			g_free(language_file);
		language_file = NULL;
		gtk_tree_model_get( GTK_TREE_MODEL(language_type_liststore), &iter, 0, &language_file, -1 );
		AL_String_List( language_file, language_string_liststore );
	}
}

/********************************
* Alchera_Language_SaveTranslation
* Event:
* Result:
*/
void Alchera_Language_Save(GtkToolButton * widget, gpointer user_data)
{
	AL_Language_Save();
}

/********************************
* Alchera_Language_EditDialog

* Event:
* Result:
*/

void Alchera_Language_EditDialog( GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data )
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection * select = gtk_tree_view_get_selection(tree_view);

	if ( gtk_tree_selection_get_selected(select, &model, &iter) )
	{
		GtkWidget * dialog, * label_string, *text_string;
		GError * error = NULL;
		GtkBuilder * ui = gtk_builder_new();
		if ( !gtk_builder_add_from_string( ui, meg_language_edit_ui, -1, &error ) )
		{
			Meg_Error_Print( __func__, __LINE__, "UI creation error: %s", error->message );
			return;
		}

		dialog = GET_WIDGET( ui, "dialog1" );
		label_string = GET_WIDGET( ui, "label_string" );
		text_string = GET_WIDGET( ui, "text_string" );


		guint id;
		GtkTextBuffer * buffer;
		gchar * orginal_text = NULL, *translated_text = NULL;
		gtk_tree_model_get(model, &iter, 0, &id, 1, &orginal_text, 2, &translated_text, -1);

		translated_text = g_strcompress( translated_text );

		buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_string));
		gtk_label_set_text( GTK_LABEL(label_string), orginal_text);
		gtk_text_buffer_set_text(buffer, translated_text, -1);

		if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == 1 )
		{
			GtkTextIter start, end;
			gtk_text_buffer_get_bounds( buffer, &start, &end );
			translated_text = gtk_text_buffer_get_text( buffer, &start, &end, FALSE );

			translated_text = g_strescape( translated_text, "❶❷❸❹❺❻❼❽❾❿➀➁➂➃➄➅➆➇➈▲△▼▽◀◁▶▷◐◒◑◓");

			gtk_list_store_set(GTK_LIST_STORE(model), &iter, 2, translated_text, -1);
			AL_String_Set(language_file, id, translated_text);
		}

		gtk_widget_destroy( dialog );


	}
}



/********************************
* Alchera_Language_StringEdit
* Event:
* Result:
*/
void Alchera_Language_StringEdit( GtkCellRendererText *cellrenderertext, gchar *path_string, gchar *new_text )
{
	GtkTreeIter iter;
	guint id;
	gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(language_string_liststore), &iter, path_string);
	gtk_tree_model_get(GTK_TREE_MODEL(language_string_liststore), &iter, 0, &id, -1);
	gtk_list_store_set(language_string_liststore, &iter, 2, new_text, -1);

	g_print("language_file = %s", language_file);
	AL_String_Set(language_file, id, new_text);
}

/********************************
* Alchera_Language_AddLanguage
* Event:
* Result:
*/
void Alchera_Language_AddLanguage( GtkToolButton * widget, gpointer user_data )
{
	AL_Language_Add(NULL);
	gtk_list_store_clear( language_type_liststore );
	AL_Language_Types( language_type_liststore );
	gtk_combo_box_set_active( GTK_COMBO_BOX(alchera_language_typecombo), 0 );

}

/********************************
* Alchera_Language_RemoveLanguage
* Event:
* Result:
*/
void Alchera_Language_RemoveLanguage(GtkToolButton * widget)
{

	GtkWidget * delete_dialog = gtk_message_dialog_new(GTK_WINDOW(Meg_Main_GetWidget()),
		GTK_DIALOG_MODAL,
		GTK_MESSAGE_QUESTION,
		GTK_BUTTONS_YES_NO,
		"Are you sure you want to delete this '%s'?",
		language_file
	);
	gint result = gtk_dialog_run( GTK_DIALOG(delete_dialog) );
	gtk_widget_destroy( delete_dialog );
	if ( result == GTK_RESPONSE_YES )
	{
		if ( language_file )
		{
			if ( AL_Language_Remove(language_file) )
			{
				gtk_list_store_clear(language_type_liststore);
				AL_Language_Types(language_type_liststore);
				gtk_combo_box_set_active( GTK_COMBO_BOX(alchera_language_typecombo), 0 );
			}
		}
	}
}

/********************************
* Alchera_Language_AddString
* Event:
* Result:
*/
void Alchera_Language_AddString(GtkToolButton *widget, gpointer user_data)
{
	AL_String_Add( language_string_liststore );
}

/********************************
* Alchera_Language_RemoveString
* Event:
* Result:
*/
void Alchera_Language_RemoveString( GtkToolButton * widget, GtkTreeView *tree  )
{
	GtkTreeIter iter;
	GtkTreeModel * model;
	GtkTreeSelection * selection = gtk_tree_view_get_selection( tree );
	if ( gtk_tree_selection_get_selected( selection, &model, &iter ) )
	{
		guint id;
		gtk_tree_model_get( GTK_TREE_MODEL(language_string_liststore), &iter, 0, &id, -1);
		/* AL_String_Remove( id, language_string_liststore ); */
	}
}

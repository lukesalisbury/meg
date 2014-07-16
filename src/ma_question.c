/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Standard Headers */
#include "global.h"

/* Required Headers */
#include "ma_question.h"

/* External Functions */
void help_parser_apply_tags( GtkTextBuffer * buffer );

/* Local Type */

/* Global Variables */
extern GError * mokoiError;

/* Local Variables */
GtkWidget * mokoi_questions_treeview = NULL;
GtkWidget * mokoi_questions_text = NULL;

/* UI */
#include "ui/question_page.gui.h"
const gchar * mokoiUI_Questions = GUIQUESTION_PAGE

#if defined(USE_SOUP) && defined(Q2A_URL)

/********************************
* MegWidget_Questions_Create
* Create Audio page.
*/
void MegWidget_Questions_Create()
{

	GtkWidget * widget;
	GtkListStore * store_questions;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create( mokoiUI_Questions, __func__, __LINE__ );

	widget = GET_WIDGET( ui, "questions_widget" );
	mokoi_questions_treeview = GET_WIDGET( ui, "questions_main_treeview" );
	mokoi_questions_text = GET_WIDGET( ui, "questions_text_answers" );
	store_questions = GET_LISTSTORE( ui, "store_questions" );

	/* Signals */
	//SET_OBJECT_SIGNAL( ui, "button1", "clicked", G_CALLBACK(Meg_Questions_Search), NULL );
	SET_OBJECT_SIGNAL( ui, "questions_widget", "delete-event", G_CALLBACK(MegWidget_Questions_Destroy), NULL );
	SET_OBJECT_SIGNAL( ui, "questions_widget", "realize", G_CALLBACK(MegWidget_Questions_Refresh), store_questions );
	SET_OBJECT_SIGNAL( ui, "questions_main_treeview", "row-activated", G_CALLBACK(Meg_Questions_DisplayItem), NULL );

	help_parser_apply_tags( gtk_text_view_get_buffer( GTK_TEXT_VIEW(mokoi_questions_text) ) );

	Meg_Main_AddSection( widget, "Question", PAGE_ICON_QUESTION );


}

/********************************
* MegWidget_Questions_Refresh
*
*/
void MegWidget_Questions_Refresh(GtkWidget * widget, gpointer user_data)
{
	if ( user_data )
	{
		Meg_Questions_GetAll( GTK_LIST_STORE(user_data) );
	}
}
#else
void MegWidget_Questions_Create()
{
}

void MegWidget_Questions_Refresh(GtkWidget * widget, gpointer user_data)
{
}
#endif

/********************************
* MegWidget_Questions_Destroy
*
*/
gboolean MegWidget_Questions_Destroy(GtkWidget * widget, GdkEvent * event, gpointer user_data )
{
	return TRUE;
}



/********************************
* MegWidget_Questions_Init
*
*/
void MegWidget_Questions_Init()
{

}

/********************************
* MegWidget_Questions_Close
*
*/
void MegWidget_Questions_Close()
{


}


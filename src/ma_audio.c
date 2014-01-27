/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Standard Headers */
#include "global.h"

/* Required Headers */
#include "data/audio_functions.h"
#include "data/audio_playback.h"

/* External Functions */


/* Local Type */

/* Global Variables */
extern GError * mokoiError;

/* Local Variables */
GtkWidget * alchera_audio_treeview = NULL;


/* UI */
#include "ui/audio_page.gui.h"
const gchar * mokoiUI_Audio = GUIAUDIO_PAGE


/********************************
* MegWidget_Audio_Create
* Create Audio page.
*/
void MegWidget_Audio_Create()
{
	GtkWidget * widget;

	/* UI */
	GError * error = NULL;
	GtkBuilder * ui = gtk_builder_new();
	if ( !gtk_builder_add_from_string(ui, mokoiUI_Audio, -1, &error) )
	{
		Meg_Error_Print( __func__, __LINE__, "UI creation error '%s'.", error->message );
		return;
	}

	widget = GET_WIDGET( ui, "meg_audio_widget" );
	alchera_audio_treeview = GET_WIDGET( ui, "meg_audio_main_treeview" );

	/* Signals */
	SET_OBJECT_SIGNAL( ui, "button_refresh", "clicked", G_CALLBACK(Audio_Listing_Refresh), (gpointer) alchera_audio_treeview);
	SET_OBJECT_SIGNAL( ui, "button_add", "clicked", G_CALLBACK(Audio_File_Add), (gpointer) alchera_audio_treeview );
	SET_OBJECT_SIGNAL( ui, "button_remove", "clicked", G_CALLBACK(Audio_File_Remove), (gpointer) alchera_audio_treeview );

	SET_OBJECT_SIGNAL( ui, "meg_audio_widget", "delete-event", G_CALLBACK(MegWidget_Audio_Destroy), NULL );
	SET_OBJECT_SIGNAL( ui, "meg_audio_main_treeview", "row-activated", G_CALLBACK(Audio_File_Selected), NULL );

	Audio_Payback_Register();

	Meg_Main_AddSection( widget, "Audio", PAGE_ICON_AUDIO );

}
/********************************
* MegWidget_Audio_Destroy
*
*/
gboolean MegWidget_Audio_Destroy(GtkWidget * widget, GdkEvent * event, gpointer user_data )
{
	Audio_Payback_Unregister();
	return TRUE;
}

/********************************
* MegWidget_Audio_Refresh
*
*/
void MegWidget_Audio_Refresh(GtkWidget * widget, gpointer user_data)
{
	Audio_Listing_Update( GTK_TREE_VIEW(alchera_audio_treeview) );
}

/********************************
* MegWidget_Audio_Init
*
*/
void MegWidget_Audio_Init()
{
	MegWidget_Audio_Refresh( NULL, NULL );
}

/********************************
* MegWidget_Audio_Close
*
*/
void MegWidget_Audio_Close()
{


}

/********************************
* MegWidget_Audio_Close
*
*/
void MegWidget_Audio_Import( gchar * filename )
{
	g_return_if_fail(filename);



	GtkWidget * dialog = gtk_message_dialog_new(
				Meg_Main_GetWindow(),
				GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_INFO,
				GTK_BUTTONS_NONE,
				"Do you wish to import %s",
				filename
	);

	gtk_dialog_add_buttons( GTK_DIALOG(dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);

	if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
	{
		Audio_File_Import( filename );
	}
	gtk_widget_destroy( dialog );
}

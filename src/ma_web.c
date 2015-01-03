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
#include "ma_web.h"

/* UI */
const gchar * UIRetrieveDialog = GUI_RETRIEVE_DIALOG;


/********************************
* Meg_Web_RetrieveThread
*
*/
gpointer Meg_Web_RetrieveThread( gpointer data )
{
	WebRetrieve * download = (WebRetrieve *)data;

	if ( download->filename )
	{
		Meg_Web_RetrieveFileOverwrite( download, NULL, NULL);
	}
	else
	{
		download->content = Meg_Web_RetrieveText( download, NULL, NULL );
	}

	g_async_queue_push( download->queue, download );

	return NULL;
}

/********************************
* Meg_Web_RetrieveWatch
*
*/
gboolean Meg_Web_RetrieveWatch( WebRetrieve * download )
{
	gpointer q = g_async_queue_try_pop( download->queue );

	if ( q == download )
	{
		gtk_widget_destroy(download->dialog);

		if ( download->filename )
		{
			if ( !g_file_test(download->filename, G_FILE_TEST_IS_REGULAR) )
			{
				GtkWidget * message = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Error Downloading File '%s'", download->url );
				gtk_dialog_run( GTK_DIALOG(message) );
				gtk_widget_destroy( message );
			}
			if ( download->return_function )
			{
				download->return_function( download->filename, download->return_function_data );
			}
		}
		else if ( download->return_function )
		{
			download->return_function( download->content, download->return_function_data );
			g_free( download->content );
		}

		g_async_queue_push( download->global_queue, download->global_queue );

		g_async_queue_unref( download->queue );

		g_free(download->url);
		g_free(download->filename);

		g_free(download);

		return FALSE;
	}
	return TRUE;
}


/********************************
* Meg_WebQueue_Retrieve
*
*/
GAsyncQueue * Meg_WebQueue_Retrieve(GtkWidget * parent, const gchar * url, gchar * user, gchar * pass, const gchar * file_name, gint (*return_function)(gchar *,gpointer), gpointer data )
{
	GtkWidget * label_text;
	WebRetrieve * download = NULL;

	/* Confirm File overwrite */
	if ( g_file_test( file_name, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) )
	{
		GtkWidget * question = gtk_message_dialog_new( Meg_Misc_ParentWindow(parent), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Do you want to overwrite '%s'", file_name);
		gint answer = gtk_dialog_run( GTK_DIALOG(question) );
		gtk_widget_destroy( question );

		if ( answer != GTK_RESPONSE_YES )
		{
			return NULL;
		}
	}

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(UIRetrieveDialog, __func__, __LINE__);
	g_return_val_if_fail( ui, NULL );

	/* Widgets */
	label_text = GET_WIDGET( ui, "label_text" );

	download = g_new0( WebRetrieve, 1 );
	download->dialog = GET_WIDGET( ui, "window");
	download->label = GET_WIDGET( ui, "label_progress");

	download->queue = g_async_queue_new();
	download->global_queue = g_async_queue_new();
	download->url = g_strdup(url);
	download->filename = g_strdup(file_name);
	download->return_function = return_function;
	download->return_function_data = data;

	gtk_label_set_text( GTK_LABEL(label_text), url );

	gtk_window_set_transient_for( GTK_WINDOW(download->dialog), GTK_WINDOW(parent) );
	gtk_widget_show_all( download->dialog );

	Meg_Log_SetBuffer( NULL );
	if ( g_thread_try_new( "WebRetrieve", (GThreadFunc)Meg_Web_RetrieveThread, (gpointer)download, NULL ) )
	{
		g_timeout_add( 20, (GSourceFunc)Meg_Web_RetrieveWatch, (gpointer)download );
	}

	return download->global_queue;

}

/********************************
* Meg_WebQueue_RetrieveText
*
*/
GAsyncQueue * Meg_WebQueue_RetrieveText(GtkWidget *parent, const gchar * url, gchar * user, gchar * pass, gint (*return_text)(gchar *, gpointer), gpointer data )
{
	GtkWidget * label_text;
	WebRetrieve * download = g_new0( WebRetrieve, 1 );

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(UIRetrieveDialog, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );

	/* Widgets */
	label_text = GET_WIDGET( ui, "label_text" );
	download->dialog = GET_WIDGET( ui, "window");

	download->queue = g_async_queue_new();
	download->global_queue = g_async_queue_new();
	download->url = g_strdup(url);
	download->filename = NULL;
	download->return_function = return_text;
	download->return_function_data = data;

	gtk_label_set_text( GTK_LABEL(label_text), url );

	gtk_window_set_transient_for( GTK_WINDOW(download->dialog), GTK_WINDOW(parent) );
	gtk_widget_show_all( download->dialog );

	Meg_Log_SetBuffer( NULL );
	if ( g_thread_try_new( "WebRetrieveT", (GThreadFunc)Meg_Web_RetrieveThread, (gpointer)download, NULL ) )
	{
		g_timeout_add( 20, (GSourceFunc)Meg_Web_RetrieveWatch, (gpointer)download );
	}

	return download->global_queue;

}

/*****************************
* Meg_Web_RetrieveFile
*
*/
gboolean Meg_Web_RetrieveFile( WebRetrieve * request, gchar * user, gchar * pass )
{
	if ( g_file_test( request->filename, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) )
	{
		GtkWidget * question = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Do you want to overwrite '%s'", request->filename );
		gint answer = gtk_dialog_run( GTK_DIALOG(question) );
		gtk_widget_destroy( question );

		if ( answer != GTK_RESPONSE_YES )
		{
			Meg_Log_Print(LOG_ERROR, "'%s' already exists", request->filename );
			return FALSE;
		}
	}

	return Meg_Web_RetrieveFileOverwrite( request, user, pass );
}

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
#include "ui/retrieve_dialog.gui.h"
const gchar * UIRetrieveDialog = GUIRETRIEVE_DIALOG;


#ifdef USE_SOUP
#include <libsoup/soup.h>
#include <libsoup/soup-method.h>

SoupSession * session = NULL;
SoupLogger * logger = NULL;

/*****************************
* Meg_Web_Enable
*
*/
G_MODULE_EXPORT gboolean Meg_Web_Enable( )
{
	return TRUE;
}


/*****************************
* Meg_Web_RetrieveFileOverwrite
*
*/
gboolean Meg_Web_RetrieveFileOverwrite( GString * address, gchar * user, gchar * pass, gchar * file_name, guint directory )
{
	guint status;
	SoupMessage * msg;

	if ( !session )
	{
		session = soup_session_sync_new();
		logger = soup_logger_new(SOUP_LOGGER_LOG_MINIMAL, -1);
		soup_session_add_feature(session, SOUP_SESSION_FEATURE(logger) );
	}

	msg = soup_message_new( SOUP_METHOD_GET, address->str );
	soup_message_headers_append( msg->request_headers, "User-Agent", "Mozilla/5.0 (MokoiEditor)" );
	Meg_Log_Print(LOG_NONE, "Downloading '%s' to '%s'", address->str, file_name );
	status = soup_session_send_message( session, msg );
	if ( status == 200 )
	{
		if ( g_file_set_contents( file_name, msg->response_body->data, msg->response_body->length, NULL) )
		{
			return TRUE;
		}
		else
		{
			Meg_Log_Print(LOG_ERROR, "File write error" );
		}
	}
	else
	{
		Meg_Log_Print(LOG_NONE, "Downloading Error: Status Code: %"G_GUINT32_FORMAT" %s", msg->status_code, msg->reason_phrase);
	}
	return FALSE;
}

/*****************************
* Meg_Web_RetrieveFile
*
*/
gboolean Meg_Web_RetrieveFile( GString * address, gchar * user, gchar * pass, gchar * file_name, guint directory )
{

	if ( g_file_test( file_name, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) )
	{
		GtkWidget * question = gtk_message_dialog_new( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Do you want to overwrite '%s'", file_name);
		gint answer = gtk_dialog_run( GTK_DIALOG(question) );
		gtk_widget_destroy( question );

		if ( answer != GTK_RESPONSE_YES )
		{
			Meg_Log_Print(LOG_ERROR, "'%s' already exists", file_name );
			return FALSE;
		}
	}

	return Meg_Web_RetrieveFileOverwrite( address, user, pass, file_name, directory );
}
/*****************************
* Meg_Web_RetrieveText
*
*/
gchar * Meg_Web_RetrieveText( GString * address, gchar * user, gchar * pass )
{
	gchar * text = NULL;
	guint status;
	SoupMessage * msg;

	if ( !session )
	{
		session = soup_session_sync_new();
		logger = soup_logger_new(SOUP_LOGGER_LOG_MINIMAL, -1);
		soup_session_add_feature(session, SOUP_SESSION_FEATURE(logger) );
	}

	msg = soup_message_new( SOUP_METHOD_GET, address->str );
	soup_message_headers_append( msg->request_headers, "User-Agent", "Mozilla/5.0 (MokoiEditor)" );
	status = soup_session_send_message( session, msg );
	if ( status == 200 )
	{
		if ( g_utf8_validate( msg->response_body->data,-1,NULL) )
		{
			text = g_strdup( msg->response_body->data );
		}
	}
	return text;
}

/*****************************
* Meg_Web_SendFile
*
*/
gboolean Meg_Web_SendFile( GString * address, gchar * user, gchar * pass, gchar * file_name, guint directory )
{
	SoupMessage *msg;
	gchar * data = NULL;
	gsize length = 0;
	//gchar * file = g_build_filename( Meg_Directory_Data(), file_name, NULL );
	if ( !g_file_test( file_name, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) )
	{
		Meg_Log_Print(LOG_ERROR, "SendFile failed. '%s' missing", file_name );
		return FALSE;
	}

	if ( !g_file_get_contents( file_name, &data, &length, NULL) )
	{
		Meg_Log_Print(LOG_ERROR, "File read error:" );
		return FALSE;
	}

	if ( !session )
	{
		session = soup_session_sync_new();
		logger = soup_logger_new(SOUP_LOGGER_LOG_BODY, -1);
		soup_session_add_feature(session, SOUP_SESSION_FEATURE(logger) );
	}

	SoupMultipart * multipart = soup_multipart_new(SOUP_FORM_MIME_TYPE_MULTIPART);
	SoupBuffer * buffer = soup_buffer_new( SOUP_MEMORY_TAKE, data, length);

	soup_multipart_append_form_file( multipart, "file", "index.txt", "image/png", buffer);
	soup_buffer_free(buffer);

	msg = soup_form_request_new_from_multipart( address->str, multipart );
	soup_message_headers_append( msg->request_headers, "User-Agent", "Mozilla/5.0 (MokoiEditor)" );
	soup_session_send_message( session, msg );

	soup_multipart_free( multipart );

	if ( !SOUP_STATUS_IS_SUCCESSFUL(msg->status_code) )
		g_print("ERROR: Unexpected status %d %s\n", msg->status_code, msg->reason_phrase );

	return SOUP_STATUS_IS_SUCCESSFUL(msg->status_code);
}

/*****************************
* Meg_Web_SendText
*
*/
gboolean Meg_Web_SendText( GString * address, gchar * user, gchar * pass, gchar * text )
{
	SoupMessage *msg;

	if ( !session )
	{
		session = soup_session_sync_new();
		logger = soup_logger_new(SOUP_LOGGER_LOG_BODY, -1);
		soup_session_add_feature(session, SOUP_SESSION_FEATURE(logger) );
	}

	msg = soup_form_request_new( SOUP_METHOD_POST, address->str, "data", text, NULL );
	soup_message_headers_append( msg->request_headers, "User-Agent", "Mozilla/5.0 (MokoiEditor)" );
	soup_session_send_message( session, msg );

	if ( !SOUP_STATUS_IS_SUCCESSFUL(msg->status_code) )
		g_print("ERROR: Unexpected status %d %s\n", msg->status_code, msg->reason_phrase );

	return SOUP_STATUS_IS_SUCCESSFUL(msg->status_code);
}


#else
/*****************************
* Meg_Web_RetrieveFile
*
*/
gboolean Meg_Web_Enable( )
{
	return FALSE;
}

/*****************************
* Meg_Web_RetrieveFile
*
*/
gboolean Meg_Web_RetrieveFile( GString * address, gchar * user, gchar * pass, gchar * file_name, guint directory )
{
	return FALSE;
}

/*****************************
* Meg_Web_RetrieveFileOverwrite
*
*/
gboolean Meg_Web_RetrieveFileOverwrite( GString * address, gchar * user, gchar * pass, gchar * file_name, guint directory )
{
	return FALSE;
}

/*****************************
* Meg_Web_RetrieveText
*
*/
gchar * Meg_Web_RetrieveText( GString * address, gchar * user, gchar * pass )
{
	return NULL;
}

/*****************************
* Meg_Web_SendFile
*
*/
gboolean Meg_Web_SendFile( GString * address, gchar * user, gchar * pass, gchar * file_name, guint directory )
{
	return FALSE;
}

/*****************************
* Meg_Web_RetrieveText
*
*/
gboolean Meg_Web_SendText( GString * address, gchar * user, gchar * pass, gchar * text )
{
	return FALSE;
}


#endif


/********************************
* Meg_Web_RetrieveThread
*
*/
gpointer Meg_Web_RetrieveThread( gpointer data )
{
	WebRetrieve * download = (WebRetrieve *)data;

	GString * url = g_string_new(download->url);

	if ( download->filename )
	{
		Meg_Web_RetrieveFileOverwrite( url, NULL, NULL, download->filename, 1);
	}
	else
	{
		download->content = Meg_Web_RetrieveText( url, NULL, NULL );
	}
	g_string_free(url, FALSE);

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
* Meg_Web_RetrieveQueue
*
*/
GAsyncQueue * Meg_Web_RetrieveQueue(GtkWidget * parent, const gchar * url, gchar * user, gchar * pass, const gchar * file_name, gint (*return_function)(gchar *,gpointer), gpointer data )
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
	g_return_val_if_fail( ui, FALSE );

	/* Widgets */
	label_text = GET_WIDGET( ui, "label_text" );

	download = g_new0( WebRetrieve, 1 );
	download->dialog = GET_WIDGET( ui, "window");

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
* Meg_Web_RetrieveTextQueue
*
*/
GAsyncQueue * Meg_Web_RetrieveTextQueue(GtkWidget *parent, const gchar * url, gchar * user, gchar * pass, gint (*return_text)(gchar *, gpointer), gpointer data )
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



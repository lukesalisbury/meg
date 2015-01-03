/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifdef USE_SOUP

/* Global Header */
#include "global.h"
#include "ma_web.h"

#include <libsoup/soup.h>
#include <libsoup/soup-method.h>

/* Variables */
SoupSession * session = NULL;
SoupLogger * logger = NULL;

/*****************************
* Meg_Web_Enable
*
*/
gboolean Meg_Web_Enable( )
{
	return TRUE;
}


/*****************************
* Meg_Web_RetrieveFileOverwrite
*
*/
gboolean Meg_Web_RetrieveFileOverwrite( WebRetrieve * request, gchar * user, gchar * pass )
{
	guint status;
	SoupMessage * msg;

	if ( !session )
	{
		session = soup_session_sync_new();
		logger = soup_logger_new(SOUP_LOGGER_LOG_MINIMAL, -1);
		soup_session_add_feature(session, SOUP_SESSION_FEATURE(logger) );
	}

	msg = soup_message_new( SOUP_METHOD_GET, request->url );
	soup_message_headers_append( msg->request_headers, "User-Agent", "Mozilla/5.0 (Meg)" );
	Meg_Log_Print(LOG_NONE, "Downloading '%s' to '%s'", request->url, request->filename );
	status = soup_session_send_message( session, msg );

	if ( status == 200 )
	{
		if ( g_file_set_contents( request->filename, msg->response_body->data, msg->response_body->length, NULL) )
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
* Meg_Web_RetrieveText
*
*/
gchar * Meg_Web_RetrieveText( WebRetrieve * request, gchar * user, gchar * pass )
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

	msg = soup_message_new( SOUP_METHOD_GET, request->url );
	soup_message_headers_append( msg->request_headers, "User-Agent", "Mozilla/5.0 (Meg)" );
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
gboolean Meg_Web_SendFile( WebRetrieve * request, gchar * user, gchar * pass, gchar * file_name )
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

	msg = soup_form_request_new_from_multipart( request->url, multipart );
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
gboolean Meg_Web_SendText( WebRetrieve * request, gchar * user, gchar * pass, gchar * text )
{
	SoupMessage *msg;

	if ( !session )
	{
		session = soup_session_sync_new();
		logger = soup_logger_new(SOUP_LOGGER_LOG_BODY, -1);
		soup_session_add_feature(session, SOUP_SESSION_FEATURE(logger) );
	}

	msg = soup_form_request_new( SOUP_METHOD_POST, request->url, "data", text, NULL );
	soup_message_headers_append( msg->request_headers, "User-Agent", "Mozilla/5.0 (MokoiEditor)" );
	soup_session_send_message( session, msg );

	if ( !SOUP_STATUS_IS_SUCCESSFUL(msg->status_code) )
		g_print("ERROR: Unexpected status %d %s\n", msg->status_code, msg->reason_phrase );

	return SOUP_STATUS_IS_SUCCESSFUL(msg->status_code);
}

#endif



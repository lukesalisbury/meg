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
#include <glib/gstdio.h>

/* Local Variables */
GtkTextTag * log_bold, * log_error, * log_fine;
GtkTextBuffer * log_buffer = NULL;

/********************************
* Meg_Log_SetBuffer
*
*/
void Meg_Log_SetBuffer( GtkTextBuffer * buffer )
{
	log_buffer = buffer;
	if ( log_buffer )
	{
		log_bold = gtk_text_buffer_create_tag( log_buffer, NULL, "weight", PANGO_WEIGHT_BOLD, NULL );
		log_error = gtk_text_buffer_create_tag( log_buffer, NULL, "foreground", "red", NULL );
		log_fine = gtk_text_buffer_create_tag( log_buffer, NULL, "foreground", "green", NULL );
	}
}



/********************************
* Meg_Log_Append
*
*/
void Meg_Log_Append( LogStyle style, gchar * text )
{
	#ifdef DEBUG
	g_print("%s\n", text);
	#endif
	if ( !log_buffer )
	{
		if ( style == LOG_ERROR )
		{
			g_printerr("%s\n", text);
		}
		else
		{
			g_print("%s\n", text);
		}
	}
	else
	{
		GtkTextIter iter;
		gtk_text_buffer_get_end_iter(log_buffer, &iter);

		switch (style)
		{
			case LOG_NONE:
				gtk_text_buffer_insert( log_buffer, &iter, text, -1);
			break;
			case LOG_BOLD:
				gtk_text_buffer_insert_with_tags( log_buffer, &iter, text, -1, log_bold, NULL);
			break;
			case LOG_ERROR:
				gtk_text_buffer_insert_with_tags( log_buffer, &iter, text, -1, log_bold, log_error, NULL);
			break;
			case LOG_WARNING:
				gtk_text_buffer_insert_with_tags( log_buffer, &iter, text, -1, log_error, NULL);
			break;
			case LOG_FINE:
				gtk_text_buffer_insert_with_tags( log_buffer, &iter, text, -1, log_fine, NULL);
			break;
		}
		gtk_text_buffer_insert( log_buffer, &iter, "\n", -1);
	}
}

/********************************
* Meg_Log_Print
*
*/
void Meg_Log_Print( LogStyle style, const gchar * format, ... )
{
	gchar * log;
	va_list argptr;

	va_start( argptr, format );
	log = g_strdup_vprintf( format, argptr );
	Meg_Log_Append( style, log );

	va_end( argptr );
	g_free(log);
}

/********************************
* Meg_Error_Print
*
*/
void Meg_Error_Print( const gchar * function, gint line, const gchar * format, ... )
{
	GtkWidget * dialog = NULL;
	gchar * log = NULL;
	va_list argptr;

	va_start( argptr, format );
	log = g_strdup_vprintf( format, argptr );
	va_end( argptr );

	Meg_Log_Append( LOG_ERROR, log );

	dialog = gtk_message_dialog_new( Meg_Main_GetWindow(), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", log );
	gtk_window_set_transient_for(GTK_WINDOW(dialog), Meg_Main_GetWindow());
	gtk_dialog_run( GTK_DIALOG(dialog) );

	gtk_widget_destroy(dialog);
	g_free(log);
}

/********************************
* Meg_Error_Check
*
*/
gboolean Meg_Error_Check( GError * error, gboolean fatal, const gchar * function_name )
{
	if ( error != NULL )
	{
		if ( fatal )
		{
			Meg_Error_Print(function_name, -1, "Error: %s", error->message);
		}
		else
		{
			Meg_Log_Print(LOG_WARNING, "Function: '%s' - Warning: %s", function_name, error->message);
		}

		g_clear_error(&error);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Standard Headers */
#include "loader_global.h"

/* Required Headers */
#include "game_compiler.h"

/* Global Variables */


/* Local Variables */
GAsyncQueue * mokoiLoggerQueue = NULL;

/********************************
* Logger_SetBufferTag
*
@ buffer - Text buffer
*/
void Logger_SetBufferTag( GtkTextBuffer * buffer )
{
	GtkTextTagTable * table = gtk_text_buffer_get_tag_table(buffer);
	if ( !gtk_text_tag_table_get_size(table) )
	{
		gtk_text_buffer_create_tag( buffer, "Bold", "weight", PANGO_WEIGHT_BOLD, NULL );
		gtk_text_buffer_create_tag( buffer, "Error", "foreground", "white", "paragraph-background", "red", "weight", PANGO_WEIGHT_BOLD, "background-full-height", TRUE, NULL );
		gtk_text_buffer_create_tag( buffer, "Warning", "paragraph-background", "yellow", "foreground", "black", "background-full-height", TRUE,  NULL );
	}
}

/********************************
* Logger_SetQueue
*
@ queue
*/
void Logger_SetQueue( GAsyncQueue * queue )
{
	mokoiLoggerQueue = queue;
}

/********************************
* Logger_Append
* Appends Text to GtkTextView or Progress Dialog
*/
void Logger_Append( GtkWidget * widget, LogStyle style, gchar * text )
{
	g_return_if_fail( text != NULL );

	if ( mokoiLoggerQueue && !widget )
	{
		ProgressDialogLog * clog = g_new0(ProgressDialogLog, 1);
		clog->text = g_strdup(text);
		clog->style = style;
		g_async_queue_push( mokoiLoggerQueue, clog );
	}
	else if ( !widget )
	{
		Meg_Log_Append( style, text );
	}
	else
	{
		GtkTextBuffer * buffer = NULL;
		GtkTextIter iter;
		buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(widget) );
		if ( buffer )
		{
			gtk_text_buffer_get_end_iter(buffer, &iter);
			switch (style)
			{
				case LOG_NONE:
					gtk_text_buffer_insert( buffer, &iter, text, -1);
				break;
				case LOG_BOLD:
					gtk_text_buffer_insert_with_tags_by_name( buffer, &iter, text, -1, "Bold", NULL);
				break;
				case LOG_ERROR:
					gtk_text_buffer_insert_with_tags_by_name( buffer, &iter, text, -1, "Error", NULL);
				break;
				case LOG_WARNING:
					gtk_text_buffer_insert_with_tags_by_name( buffer, &iter, text, -1, "Warning", NULL);
				break;
				case LOG_FINE:
					gtk_text_buffer_insert( buffer, &iter, text, -1);
				break;
			}
		}
	}
	//g_free(text);
}

/********************************
* Logger_FormattedLog
*
*/
void Logger_FormattedLog( GtkWidget * log_widget, LogStyle style, const gchar * format, ...)
{
	gchar * log = g_new0( gchar, 512 );
	va_list argptr;
	va_start( argptr, format );
	g_vsnprintf( log, 511, format, argptr );

	Logger_Append( log_widget, style, log );

	va_end( argptr );

}

/********************************
* Logger_FormattedLogLine
*
*/
void Logger_FormattedLogLine( GtkWidget * log_widget, GtkWidget * editor_widget, char * file, gint line, LogStyle style, gchar * message )
{


	Logger_Append( log_widget, style, g_strdup(message) );

}

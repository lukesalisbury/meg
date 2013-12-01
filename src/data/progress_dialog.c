/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Standard Headers */
#include "loader_global.h"

/* Required Headers */
#include <glib/gstdio.h>
#include "logger_functions.h"
#include "loader_functions.h"
#include "compression_functions.h"
#include "progress_dialog.h"

/* Global Variables */
extern GError * mokoiError;
GtkWidget * mokoiCompileProgressBar;

/* Local Variables */

/* UI */
#include "ui/progress_dialog.gui.h"
const gchar * mokoiUI_ProgessDialog = GUIPROGRESS_DIALOG

/********************************
* ProgressDialog_Watch
* Function that watches the thread.
* Note: LOG_ERROR adds to error count
* Note: LOG_BOLD adds to done count
*/
void ProgressDialog_Clear( ProgressDialogWidgets * wids )
{
	g_slist_free( wids->file_list );
	g_free( wids->filename );


	wids->log = NULL;
	wids->progress = NULL;
	wids->dialog = NULL;
	wids->list = NULL;
	wids->queue = NULL;
	wids->thread = NULL;
	wids->id = 0;
	wids->file_list = NULL;
	wids->filename = NULL;
	wids->items = 0;
	wids->count = 0;
	wids->errors = 0;
}

/********************************
* ProgressDialog_Watch
* Function that watches the thread.
* Note: LOG_ERROR adds to error count
* Note: LOG_BOLD adds to done count
*/
gboolean ProgressDialog_Watch( ProgressDialogWidgets * wids )
{
	if ( !wids->items )
	{
		gtk_progress_bar_pulse( GTK_PROGRESS_BAR(wids->progress) );
	}
	else
	{
		gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR(wids->progress), 0.0 );
	}

	while( TRUE )
	{
		gpointer pop = g_async_queue_try_pop( wids->queue );

		ProgressDialogLog * clog = NULL;
		if ( wids->thread != pop )
		{
			clog = (ProgressDialogLog *)pop;
		}

		if ( wids->thread == pop )
		{
			gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR(wids->progress), 1.0 );
			if (wids->errors)
			{
				gchar * name = g_new0(gchar, 128);
				g_snprintf( name, 128, "%d Errors", wids->errors );
				gtk_progress_bar_set_text( GTK_PROGRESS_BAR(wids->progress), name );
				g_free(name);
			}
			else
				gtk_progress_bar_set_text( GTK_PROGRESS_BAR(wids->progress), "Finished" );

			ProgressDialog_AllowCloseEvent(wids);
			return FALSE;
		}
		else if ( pop )
		{

			Logger_Append( wids->log, clog->style, clog->text );
			if (clog->style == LOG_ERROR)
			{
				wids->errors++;
			}
			else if (clog->style == LOG_BOLD)
			{
				wids->count++;
			}
			if ( wids->items && wids->count )
			{
				gchar * name = g_new0(gchar, 128);
				gdouble f = (gdouble)wids->count / (gdouble)wids->items;
				g_snprintf( name, 128, "%d out of %d items", wids->count, wids->items);
				gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR(wids->progress), f );
				gtk_progress_bar_set_text( GTK_PROGRESS_BAR(wids->progress), name );
				g_free(name);
			}
		}
		break;
	}
	return TRUE;
}

/********************************
* ProgressDialog_CloseEvent
*
*/
gboolean ProgressDialog_CloseEvent( GtkWidget * window, GdkEvent  *event, gpointer data )
{
	return TRUE;
}


/********************************
* ProgressDialog_Create
*
*/
gboolean ProgressDialog_Create( ProgressDialogWidgets * wid, gchar * title, const gchar * file_name )
{
	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(mokoiUI_ProgessDialog, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );

	g_object_ref( ui );

	/* Widget */
	wid->dialog = GET_WIDGET( ui, "window");
	wid->progress = GET_WIDGET( ui, "progress_output");
	wid->log = GET_WIDGET( ui, "text_output");

	/* Setup */
	gtk_window_set_title( GTK_WINDOW(wid->dialog), title );
	gtk_window_set_transient_for( GTK_WINDOW(wid->dialog), Meg_Main_GetWindow() );
	Logger_SetBufferTag( gtk_text_view_get_buffer( GTK_TEXT_VIEW(wid->log) ) );

	/* Set Compile Widget */
	wid->queue = g_async_queue_new();
	wid->items = 0;
	wid->count = 0;
	wid->errors = 0;
	wid->list = NULL;
	if ( file_name )
		wid->filename = g_strdup(file_name);
	wid->id = g_signal_connect( wid->dialog, "delete-event", (GCallback)ProgressDialog_CloseEvent, NULL );


	return TRUE;
}
/********************************
* ProgressDialog_Activate
*
*/
gboolean ProgressDialog_AllowCloseEvent( ProgressDialogWidgets * wid )
{
	gulong handler_id = g_signal_handler_find( G_OBJECT(wid->dialog), G_SIGNAL_MATCH_FUNC, 0, 0, NULL, G_CALLBACK(ProgressDialog_CloseEvent), NULL );
	g_signal_handler_disconnect(wid->dialog, handler_id);
	return TRUE;
}

/********************************
* ProgressDialog_Activate
*
*/
gboolean ProgressDialog_Activate( ProgressDialogWidgets * wid, GThreadFunc thread_function, GSourceFunc watcher_function )
{
	if ( wid->dialog )
	{
#if GLIB_CHECK_VERSION(2,32,0)
		wid->thread = g_thread_new("ProgressDialog", thread_function, (gpointer)wid );
#else
		wid->thread = g_thread_create( thread_function, (gpointer)wid, FALSE, NULL );
#endif

		g_timeout_add( 50, watcher_function, (gpointer)wid );

		gtk_widget_show_all( wid->dialog );
	}
	else
	{
		g_print("ProgressDialogWidgets not correctly set up.");
	}
	return TRUE;
}

/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef PROGRESS_DIALOG
#define PROGRESS_DIALOG
#include "loader_global.h"

typedef struct {
	GtkWidget * log, * progress, * dialog;
	GtkListStore * list;
	GSList * file_list;
	GAsyncQueue * queue;
	GThread * thread;
	guint32 id;
	gchar * filename;
	gint items, count, errors;
} ProgressDialogWidgets;

typedef struct {
	LogStyle style;
	gchar * text;
} ProgressDialogLog;

gboolean ProgressDialog_AllowCloseEvent( ProgressDialogWidgets * wid );
void ProgressDialog_Clear( ProgressDialogWidgets * wids );
gboolean ProgressDialog_CloseEvent( GtkWidget * window, GdkEvent  *event, gpointer data );
gboolean ProgressDialog_Watch( ProgressDialogWidgets * wids );
gboolean ProgressDialog_Create(ProgressDialogWidgets * wid, gchar * title, const gchar *file_name);
gboolean ProgressDialog_Activate( ProgressDialogWidgets * wid, GThreadFunc thread_function, GSourceFunc watcher_function );
#endif

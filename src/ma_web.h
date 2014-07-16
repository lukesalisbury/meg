/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

typedef struct {
	GAsyncQueue * queue;
	GAsyncQueue * global_queue;
	gchar * url;
	gchar * filename;
	gchar * content;
	GtkWidget * dialog;
	gint (*return_function)( gchar * content, gpointer data );
	gpointer return_function_data;
} WebRetrieve;

typedef struct {
	GAsyncQueue * queue;
	GtkWidget * dialog;
	void (*download_function)(gpointer);
	void (*callback_function)(gpointer);
} WebRetrieveCallback;

	gboolean Meg_Web_Enable( );
	gboolean Meg_Web_RetrieveFile( GString * address, gchar * user, gchar * pass, gchar * file_name, guint directory );
	gchar * Meg_Web_RetrieveText( GString * address, gchar * user, gchar * pass );
	gboolean Meg_Web_SendFile( GString * address, gchar * user, gchar * pass, gchar * file_name, guint directory );
	gboolean Meg_Web_SendText( GString * address, gchar * user, gchar * pass, gchar * text );



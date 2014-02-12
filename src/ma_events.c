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

/* Required Headers */
#include <gio/gio.h>
#include "ma_widgets.h"

/* GtkWidget */
extern GtkWidget * alchera_init_window;
extern GtkWidget * meg_main_empty;
extern GtkWidget * alchera_main_frame;

/* Global Variables */
extern gchar * project_file_path;

/* External Functions */
void Meg_Project_Open();
gboolean Package_ImportDrop(const gchar * file );
/* UI */

/* Functions */



/********************************
* Meg_Event_AboutProgram
*
*/
gboolean Meg_Event_AboutProgram( )
{
	return Meg_Dialog_About( );
}


/********************************
* Meg_Event_Play
* Event:
* Result:
*/
gboolean Meg_Event_Play()
{
	if ( project_file_path )
		MegProject_Play();
	return TRUE;
}

/********************************
* Meg_Event_Compile
* Event:
* Result:
*/
gboolean Meg_Event_Compile()
{
	if ( project_file_path )
		MegProject_Compile();
	return TRUE;
}

/********************************
* Meg_Event_CreatePatch
* Event:
* Result:
*/
gboolean Meg_Event_CreatePatch()
{
	if ( project_file_path )
		return Meg_Dialog_CreatePatch();
	return FALSE;
}

/********************************
* Meg_Event_Export
* Event:
* Result:
*/
gboolean Meg_Event_Export()
{
	if ( project_file_path )
		Meg_Dialog_Export();
	return TRUE;
}

/********************************
* Meg_Event_Import
* Event:
* Result:
*/
gboolean Meg_Event_Import()
{
	if ( project_file_path )
		Meg_Dialog_Import();
	return TRUE;
}

/********************************
* Meg_Event_Save
* Event:
* Result:
*/
gboolean Meg_Event_Save()
{
	if ( project_file_path )
		MegProject_Save();
	return TRUE;
}


/********************************
* Meg_Event_OpenProjectDialog
* Event:
* Result:
*/
gboolean Meg_Event_OpenProjectDialog()
{
	gboolean success = Meg_Dialog_OpenProject();

	if ( success )
	{
		gtk_widget_destroy( alchera_init_window );
		alchera_init_window = NULL;
	}
	return success;
}
/********************************
* Meg_Event_NewProject
* Event:
* Result:
*/
gboolean Meg_Event_NewProjectDialog()
{
	gboolean success = Meg_Dialog_NewProject();

	if ( success )
	{
		gtk_widget_destroy( alchera_init_window );
		alchera_init_window = NULL;

		Meg_Help_Open( "welcome.xml" );
	}

	return success;
}

/********************************
* Meg_Event_CloseProject
* Event:
* Result:
*/
gboolean Meg_Event_CloseProject()
{
	gboolean success = FALSE;

	Meg_Loaders_Close();

	project_file_path = NULL;

	/* Show Empty Widget */
	GList * child_list = gtk_container_get_children( GTK_CONTAINER(alchera_main_frame) );
	GtkWidget * child = (GtkWidget *)g_list_nth_data( child_list, 0 );
	if ( child )
	{
		gtk_container_remove( GTK_CONTAINER(alchera_main_frame), child );
	}
	gtk_container_add( GTK_CONTAINER(alchera_main_frame), meg_main_empty );
	gtk_widget_show_all( alchera_main_frame );

	return success;
}

/********************************
* Meg_Event_OpenProject
* Event:
* Result:
*/
gboolean Meg_Event_OpenProject()
{
	Meg_Project_Open();
	return TRUE;
}

/********************************
* Meg_Event_CloseProgram
* Event:
* Result:
*/
gboolean Meg_Event_CloseProgram( void )
{
	GtkWidget * dialog, * label;
	GtkWindow * parent = Meg_Main_GetWindow();

	dialog = gtk_dialog_new_with_buttons( "Exit?", parent, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	label = gtk_label_new( "Are you sure you wish to exit?" );
	gtk_container_add( GTK_CONTAINER(gtk_dialog_get_content_area( GTK_DIALOG(dialog) )), label );
	gtk_widget_show( label );


	if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT )
	{
		Meg_Event_CloseProject();
		gtk_widget_destroy( dialog );
		gtk_main_quit();
		return FALSE;
	}
	else
	{
		gtk_widget_destroy( dialog );
		return TRUE;
	}
}

/********************************
* Meg_Event_Drop
* Event:
* Result:
*/
gboolean Meg_Event_Drop( GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer user_data )
{
	if ( project_file_path )
		return TRUE;
	else
		return FALSE;
}

/********************************
*
* Event:
* Result:
*/
void Meg_Event_DropReceived( GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint ttype, guint time, gpointer user_data )
{
	gchar * filename = NULL;
	gchar * filetype = NULL;
	gchar * mimetype = NULL;

	guint c = 0;
	gchar ** files = gtk_selection_data_get_uris(data);

	if ( files )
	{
		if ( g_strv_length( files ) )
		{
			while ( files[c] != NULL )
			{
				filename = g_filename_from_uri( files[c], NULL, NULL );
				filetype = g_content_type_guess(filename, NULL, 0, NULL );
				mimetype = g_content_type_get_mime_type(filetype);

				if ( project_file_path )
				{
					if ( g_str_has_prefix(mimetype, "image/") )
					{
						MegWidget_Spritesheet_Import(filename);
					}
					else if ( g_str_has_prefix(mimetype, "audio/") )
					{
						MegWidget_Audio_Import(filename);
					}
					else if ( g_str_has_prefix(mimetype, "application/x-ext-package") )
					{
						Package_ImportDrop(filename);
					}
					else if ( g_str_has_prefix(mimetype, "application/zip") )
					{
						Package_ImportDrop(filename);
					}

				}
				else
				{
					if (  g_strcmp0( ROOT_MIMETYPE, mimetype) == 0 || g_str_has_suffix(filename, ROOT_FILENAME) )
					{
						Meg_Loaders_Init( filename );
					}
				}

				// g_print( "Meg_Event_DropReceived '%s' %s\n", filename, mimetype );

				g_free(filetype);
				g_free(filename);

				c++;
			}
		}
		g_strfreev(files);
	}

}


/********************************
* Meg_Queue_FreeFinished
*
*/
gboolean Meg_Queue_FreeFinished( GAsyncQueue * queue )
{
	gpointer q = g_async_queue_try_pop( queue );
	if ( q == queue )
	{
		g_async_queue_unref( queue );
		return FALSE;
	}
	return TRUE;
}


/********************************
* Meg_Event_PackageCheck
* Event:
* Result:
*/
gboolean Meg_Event_PackageCheck()
{
	GAsyncQueue * queue = AL_PrecheckFiles();
	if ( queue )
	{
		g_timeout_add( 20, (GSourceFunc)Meg_Queue_FreeFinished, (gpointer)queue );
	}
	return TRUE;

}


#if defined(__GNUWIN32__) && GTK_MAJOR_VERSION < 3
void meg_windows_url_handler( GtkLinkButton *button, const gchar *link_, gpointer user_data);

/********************************
* Meg_Event_WebBug
* Event:
* Result:
*/
gboolean Meg_Event_WebBug()
{
	meg_windows_url_handler( NULL, BUG_URL, NULL);
	return TRUE;
}

/********************************
* Meg_Event_WebForum
* Event:
* Result:
*/
gboolean Meg_Event_WebForum()
{
	meg_windows_url_handler( NULL, FORUM_URL, NULL);
	return TRUE;
}

/********************************
* Meg_Event_WebManual
* Event:
* Result:
*/
gboolean Meg_Event_WebManual()
{
	meg_windows_url_handler( NULL, MANUAL_URL, NULL);
	return TRUE;
}
#else

/********************************
* Meg_Event_WebBug
* Event:
* Result:
*/
gboolean Meg_Event_WebBug()
{
	return gtk_show_uri( NULL, BUG_URL, GDK_CURRENT_TIME, NULL);
}

/********************************
* Meg_Event_WebForum
* Event:
* Result:
*/
gboolean Meg_Event_WebForum()
{
	return gtk_show_uri( NULL, FORUM_URL, GDK_CURRENT_TIME, NULL);
}

/********************************
* Meg_Event_WebManual
* Event:
* Result:
*/
gboolean Meg_Event_WebManual()
{
	return gtk_show_uri( NULL, MANUAL_URL, GDK_CURRENT_TIME, NULL);
}

#endif

/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
/* ma_help.c
	XML based Help System
*/

/* Global Header */
#include "global.h"
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#include "ma_misc.h"

/* Functions local */

/* Global Variables */
GtkWidget * alchera_help_textview, * alchera_help_label, * alchera_help_filelist = NULL, * alchera_help_toolbutton = NULL;
GHashTable * alchera_help_folders_list = NULL;

gchar * help_global_directory  = NULL;
gchar * help_local_directory  = NULL;
gchar * help_language = "en";


/* Functions */
void Meg_Help_ScanDirectory(GtkTreeStore * store, GtkTreeIter * parent, gchar * dir);
gboolean Meg_HelpParser_Load( GtkTextView * textview, gchar * content );
gboolean Meg_HelpParser_Event( GtkWidget * text_view, GdkEvent * ev );

/* UI */
#include "ui/help_page.gui.h"
const gchar * alchera_help_ui = GUIHELP_PAGE


/* Functions */



/********************************
* Meg_Help_GetFilePath
*
*/

gchar * Meg_Help_GetFilePath( const gchar * file )
{
	gsize c = 0;
	gchar * help_file_name = NULL;
	gchar * help_file_path = NULL;

	if ( !help_global_directory )
	{
		help_global_directory = Meg_Directory_Share("help");
	}


	if ( file )
	{
		help_file_name = g_strdup(file);
		while ( help_file_name[c] )
		{
			if ( help_file_name[c] == '/' )
				help_file_name[c] = '_';
			c++;
		}
		help_file_path = g_build_filename( help_global_directory, help_language, help_file_name, NULL );
	}


	return help_file_path;
}




/********************************
* Meg_Help_Load
*
*/
void Meg_Help_Load( const gchar * file, GtkWidget * textview )
{
	gchar * help_file_path, * content = NULL;

	help_file_path = Meg_Help_GetFilePath( file );

	if ( !g_file_get_contents(help_file_path, &content, NULL, NULL) )
	{
		/* Meg_Error_Print( __func__, __LINE__, "Can not open help file '%s'", file ); */
		if ( content )
		{
			g_free(content);
		}
		content = g_strdup_printf("<div>Can not open help file '%s'</div>",file);

	}

	if ( content )
	{
		if ( !Meg_HelpParser_Load( GTK_TEXT_VIEW(textview), content ) )
		{
			Meg_Error_Print(  (char*)__func__, __LINE__, "Can not parse help file '%s'", file);
		}
	}
	g_free(content);
	g_free(help_file_path);
}
/********************************
*
*
*/
void Meg_Help_SwitchPage( gchar * file )
{
	Meg_Help_Load( file, alchera_help_textview );
}

/********************************
*
*
*/
GdkPixbuf * Meg_Help_GetImage( gchar * file )
{
	GdkPixbuf * pixbuf = NULL;
	gchar * path = NULL;

	path = g_build_filename( Meg_Directory_Share("help"), "image", file, NULL);
	if ( path )
	{
		pixbuf = gdk_pixbuf_new_from_file( path, NULL );
	}
	return pixbuf;
}

/********************************
*
*
*/
gchar * Meg_Help_GetText( gchar * file )
{
	gchar * help_file_path = NULL, * content = NULL;
	help_file_path = Meg_Help_GetFilePath(  file );
	g_file_get_contents(help_file_path, &content, NULL, NULL);
	g_free(help_file_path);

	return content;

}

/********************************
*
*
*/
void Meg_Help_Open( gchar * file )
{
	Meg_Help_Load( file, alchera_help_textview );
	g_signal_emit_by_name( GTK_TOOL_ITEM(alchera_help_toolbutton), "clicked" );
}

/********************************
*
*
*/
gchar *g_strjoinv_count( const gchar *separator, gchar **str_array, guint count )
{
	char *res;
	size_t slen, len, i;

	if (separator != NULL)
		slen = strlen(separator);
	else
		slen = 0;

	len = 0;
	for (i = 0; str_array[i] != NULL && i < count ; i++)
	{
		len += strlen( str_array[i] );
		len += slen;
	}
	if ( len == 0 )
		return g_strdup ("");
	if ( slen > 0 && len > 0 )
		len -= slen;
	len++;
	res = (char*)g_malloc(len);
	strcpy (res, str_array [0]);
	for (i = 1; str_array [i] != NULL && i < count ; i++)
	{
		if (separator != NULL)
			strcat(res, separator);
		strcat (res, str_array [i]);
	}
	return res;
}

/********************************
*
*
*/
void Meg_Help_ScanAddFile( gchar * file, GtkTreeStore * store )
{
	GtkTreeIter iter;
	GtkTreeIter * parent = NULL;
	gchar ** info = g_strsplit( file, "|", 3 );


	if ( g_strv_length(info) == 3 )
	{

		parent = g_hash_table_lookup( alchera_help_folders_list, info[1] );
		if ( g_hash_table_lookup( alchera_help_folders_list, info[2] ) == NULL )
		{
			gtk_tree_store_append( store, &iter, parent );
			gtk_tree_store_set( store, &iter, 0, info[0], 1, info[2], 2, "", 3, 1, -1 );
		}
	}

	g_strfreev(info);
}

/********************************
*
*
*/
void Meg_Help_ScanDirectory( GtkTreeStore * store, GtkTreeIter * parent, gchar * dir )
{
	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store), 0, Meg_Misc_TreeIterCompare, GINT_TO_POINTER(2), NULL);
	gtk_tree_sortable_set_sort_column_id( GTK_TREE_SORTABLE(store), 0, GTK_SORT_ASCENDING );

	gchar * full_path = NULL, * full_dir = NULL;
	gchar * path = g_build_path( G_DIR_SEPARATOR_S, Meg_Directory_Share("help"), "en", dir, NULL );
	GDir * directory = g_dir_open( path, 0, NULL );
	GSList * file_list = NULL;


	if ( directory )
	{
		const gchar * file = g_dir_read_name( directory );
		while ( file != NULL )
		{
			if ( file[0] != '.')
			{
				full_path = g_build_filename( path, file, NULL);
				if ( g_str_has_suffix(file, ".xml") )
				{
					if ( dir )
						full_dir = g_build_filename( dir, file, NULL );
					else
						full_dir = g_strdup( file );

					gchar * nice_name = g_strndup( file, g_utf8_strlen(file, -1) - 4);
					gchar ** folders = g_strsplit( nice_name, "_", 0);
					guint length = g_strv_length(folders);
					gchar * parent_folder_name = NULL;

					parent_folder_name = g_strjoinv_count("_", folders, length - 1 );

					if ( length > 1 )
					{
						guint c = 0;
						GtkTreeIter * folder = NULL;
						GtkTreeIter * parent_folder = NULL;

						while ( c < length - 1)
						{
							gchar * folder_name = NULL;
							gchar * folder_file = NULL;

							folder_name = g_strjoinv_count( "_", folders, c + 1 );
							folder_file = g_strdup_printf("%s.xml", folder_name );
							folder = g_hash_table_lookup( alchera_help_folders_list, folder_file );

							if ( !folder )
							{
								folder = g_new0(GtkTreeIter, 1);

								gtk_tree_store_append( store, folder, parent_folder );
								gtk_tree_store_set( store, folder, 0, g_strdup(folders[c]), 1, g_strdup(folder_file), 2, "", 3, 1, -1 );
								g_hash_table_insert( alchera_help_folders_list, g_strdup(folder_file), folder );
								g_hash_table_insert( alchera_help_folders_list, g_strdup(folder_name), folder );
							}

							parent_folder = folder;

							g_free( folder_name );
							g_free( folder_file );
							c++;
						}

						g_free( nice_name );
						nice_name = g_strdup( folders[length-1] );

					}
					file_list = g_slist_append( file_list, (gpointer)g_strdup_printf("%s|%s|%s", nice_name, parent_folder_name, full_dir) );

					g_free( nice_name );
					g_strfreev( folders );
				}
				g_free( full_path );
			}
			file = g_dir_read_name( directory );
		}
		g_dir_close( directory );
	}
	g_slist_foreach( file_list, (GFunc)Meg_Help_ScanAddFile, store);
}

/********************************
* Event:
* Result:
*/
void Meg_Help_Refresh( GtkButton * button, gpointer user_data )
{
	if ( alchera_help_filelist )
	{
		if ( alchera_help_folders_list )
			g_hash_table_destroy( alchera_help_folders_list );
		alchera_help_folders_list = g_hash_table_new( g_str_hash, g_str_equal );
		GtkTreeModel * store = gtk_tree_view_get_model( GTK_TREE_VIEW(alchera_help_filelist) );
		gtk_tree_store_clear( GTK_TREE_STORE(store) );
		Meg_Help_ScanDirectory( GTK_TREE_STORE(store), NULL, NULL);
	}
}

/********************************
*
*
*/
void Meg_Help_LoadFile(GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
	gboolean is_a_file = FALSE;
	gchar * file = NULL;
	GtkTreeIter iter;
	GtkTreeModel * model;
	GtkTreeSelection * select = gtk_tree_view_get_selection( tree_view );

	if ( gtk_tree_selection_get_selected(select, &model, &iter) )
	{

		gtk_tree_model_get( model, &iter, 1, &file, 3, &is_a_file, -1 );
		if ( is_a_file )
		{
			Meg_Help_Load( file, alchera_help_textview );
		}
		g_free( file );
	}
}


/********************************
* Event:
* Result:
*/
void MegWidget_Help_Create()
{
	GtkWidget * main_widget;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(alchera_help_ui, __func__, __LINE__);
	g_return_if_fail( ui );

	/* widget */
	main_widget = GET_WIDGET( ui, "alchera_help_widget");
	alchera_help_textview = GET_WIDGET( ui, "alchera_help_textview");
	alchera_help_filelist = GET_WIDGET( ui, "alchera_help_filelist");
	alchera_help_label = GET_WIDGET( ui, "alchera-label");


	/* Signals */
	g_signal_connect( alchera_help_textview, "event-after", G_CALLBACK(Meg_HelpParser_Event), NULL );
	g_signal_connect( alchera_help_filelist, "row-activated", G_CALLBACK(Meg_Help_LoadFile), NULL );
	g_signal_connect( alchera_help_filelist, "realize", G_CALLBACK(Meg_Help_Refresh), NULL );

	/* Setting */
	g_object_ref( main_widget );

	alchera_help_toolbutton = Meg_Main_AddSection( main_widget, "Help (F1)", PAGE_ICON_HELP );

	g_object_unref(ui);


}

void MegWidget_Help_Init()
{


}

void MegWidget_Help_Close()
{


}



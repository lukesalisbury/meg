/****************************
Copyright © 2007-2014 Luke Salisbury
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
const gchar * alchera_help_ui = GUI_PAGE_HELP;

/* Functions */

/**
 * @brief Meg_Help_GetFilePath
 * @param file
 * @return
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

/**
 * @brief Meg_Help_GeneratePage
 * @param function
 * @return
 */
GString * Meg_Help_GeneratePage( EditorDatabaseListing * function )
{
	GString * content = g_string_new("");
	gchar * escaped_string;

	g_string_append_printf( content, "<div xmlns=\"http://www.w3.org/1999/xhtml\">" );
	g_string_append_printf( content, "<h2>%s</h2>", function->name );

	escaped_string = g_markup_printf_escaped("<p class=\"fundef\">%s%s</p>",  function->name, function->arguments_string );
	content = g_string_append( content, escaped_string );
	g_free(escaped_string);

	g_string_append_printf( content, "<p>%s</p>", function->info );

	GList * argument = function->arguments;
	while ( argument )
	{
		EditorDatabaseListing * argument_item = (EditorDatabaseListing *)argument->data;

		escaped_string = g_markup_printf_escaped("<p><strong>Argument '%s':</strong>%s</p>", argument_item->name, (argument_item->info ? argument_item->info : "") );
		content = g_string_append( content, escaped_string );
		g_free(escaped_string);

		argument = g_list_next( argument );
	}

	if ( function->return_info )
	{
		g_string_append_printf( content, "<p><strong>Returns:</strong> %s</p>", function->return_info );
	}

	if ( function->user_data )
	{
		escaped_string = g_markup_printf_escaped( "<br/><p><strong>Example</strong></p><p><pre>%s</pre></p>", (gchar*)function->user_data );
		content = g_string_append( content, escaped_string );
		g_free(escaped_string);

	}

	g_string_append_printf( content, "</div>" );

	return content;

}



/********************************
* Meg_Help_Load
*
*/
void Meg_Help_Load( const gchar * file, GtkWidget * textview )
{
	g_return_if_fail( textview != NULL );

	if ( g_str_has_prefix(file, "ScriptAPI/") )
	{
		EditorDatabaseListing * function = NULL;
		GSList * function_list = AL_Editor_Database( NULL );
		GSList * item = function_list;
		while ( item != NULL )
		{
			EditorDatabaseListing * search = (EditorDatabaseListing * )item->data;
			if ( g_str_has_suffix(file, search->name) )
			{
				function = search;
				break;
			}
			item = g_slist_next(item);
		}

		if ( function == NULL )
		{
			if ( !Meg_HelpParser_Load( GTK_TEXT_VIEW(textview), "<div>Function Not Found</div>" ) )
			{
				Meg_Error_Print( (char*)__func__, __LINE__, "Can not parse help file '%s'", file);
			}
		}
		else
		{
			GString * string = Meg_Help_GeneratePage( function );
			if ( !Meg_HelpParser_Load( GTK_TEXT_VIEW(textview), string->str ) )
			{
				Meg_Error_Print( (char*)__func__, __LINE__, "Can not parse help file '%s'", string->str);
			}
			g_string_free( string, TRUE );
		}

	}
	else
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
				Meg_Error_Print( (char*)__func__, __LINE__, "Can not parse help file '%s'", file);
			}
		}

		g_free(content);
		g_free(help_file_path);
	}
}

/**
 * @brief Meg_Help_SwitchPage
 * @param file
 */
void Meg_Help_SwitchPage( gchar * file )
{
	Meg_Help_Load( file, alchera_help_textview );
}

/**
 * @brief Meg_Help_GetImage
 * @param file
 * @return
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

/**
 * @brief Meg_Help_GetText
 * @param file
 * @return
 */
gchar * Meg_Help_GetText( gchar * file )
{
	gchar * help_file_path = NULL, * content = NULL;
	help_file_path = Meg_Help_GetFilePath(  file );
	g_file_get_contents(help_file_path, &content, NULL, NULL);
	g_free(help_file_path);

	return content;

}

/**
 * @brief Meg_Help_Open
 * @param file
 */
void Meg_Help_Open( gchar * file )
{
	Meg_Help_Load( file, alchera_help_textview );
	//g_signal_emit_by_name( GTK_TOOL_ITEM(alchera_help_toolbutton), "clicked" );
	gtk_toggle_tool_button_set_active( GTK_TOGGLE_TOOL_BUTTON(alchera_help_toolbutton), TRUE );
}

/**
 * @brief g_strjoinv_count
 * @param separator
 * @param str_array
 * @param count
 * @return
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

/**
 * @brief Meg_Help_ScanAddFile
 * @param file
 * @param store
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

/**
 * @brief Meg_Help_AddDirectoryList
 * @param store
 * @param title
 * @param folder_file
 * @param folder_name
 * @param parent_folder
 * @return
 */
GtkTreeIter * Meg_Help_AddDirectoryList(  GtkTreeStore * store, const gchar * title, const gchar * folder_file, const gchar * folder_name, GtkTreeIter * parent_folder )
{
	GtkTreeIter * folder = g_new0(GtkTreeIter, 1);

	gtk_tree_store_append( store, folder, parent_folder );
	gtk_tree_store_set( store, folder, 0, g_strdup(title), 1, g_strdup(folder_file), 2, "", 3, 1, -1 );
	g_hash_table_insert( alchera_help_folders_list, g_strdup(folder_file), folder );
	g_hash_table_insert( alchera_help_folders_list, g_strdup(folder_name), folder );

	return folder;
}

/**
 * @brief Meg_Help_ScanDirectory
 * @param store
 * @param parent
 * @param dir
 */
void Meg_Help_ScanDirectory( GtkTreeStore * store, GtkTreeIter * parent, gchar * dir )
{
	gtk_tree_sortable_set_sort_func( GTK_TREE_SORTABLE(store), 0, Meg_Misc_TreeIterCompare, GINT_TO_POINTER(2), NULL);
	gtk_tree_sortable_set_sort_column_id( GTK_TREE_SORTABLE(store), 0, GTK_SORT_ASCENDING );

	gchar * full_path = NULL, * full_dir = NULL;
	gchar * path = g_build_path( G_DIR_SEPARATOR_S, Meg_Directory_Share("help"), "en", dir, NULL );
	GDir * directory = g_dir_open( path, 0, NULL );
	GSList * file_list = NULL;
	GSList * function_list = AL_Editor_Database( NULL );

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
								folder = Meg_Help_AddDirectoryList( store, folders[c], folder_file, folder_name, parent_folder );
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

	if ( g_slist_length(file_list) == 0 )
	{
		file_list = g_slist_append( file_list, "Editor|Editor|Editor.xml" );
		file_list = g_slist_append( file_list, "Tutorials|Tutorials|Tutorials.xml" );
	}

	/* Script API Scan */
	file_list = g_slist_append( file_list, "ScriptAPI|ScriptAPI|ScriptAPI.xml" );
	Meg_Help_AddDirectoryList( store, "ScriptAPI", "ScriptAPI.xml", "ScriptAPI", NULL );

	GSList * item = function_list;
	while ( item != NULL )
	{
		EditorDatabaseListing * function = (EditorDatabaseListing * )item->data;
		file_list = g_slist_append( file_list, (gpointer)g_strdup_printf("%s|%s|ScriptAPI/%s", (char*)function->name, "ScriptAPI", (char*)function->name) );
		item = g_slist_next(item);
	}

	/* */
	g_slist_foreach( file_list, (GFunc)Meg_Help_ScanAddFile, store);
}

/**
 * @brief Meg_Help_Refresh
 * @param button
 * @param user_data
 */
void Meg_Help_Refresh( GtkButton * button, gpointer user_data )
{
	GtkTreeModel * store;
	if ( alchera_help_filelist )
	{
		if ( alchera_help_folders_list )
			g_hash_table_destroy( alchera_help_folders_list );

		alchera_help_folders_list = g_hash_table_new( g_str_hash, g_str_equal );
		store = gtk_tree_view_get_model( GTK_TREE_VIEW(alchera_help_filelist) );

		gtk_tree_store_clear( GTK_TREE_STORE(store) );
		Meg_Help_ScanDirectory( GTK_TREE_STORE(store), NULL, NULL);
	}
}

/**
 * @brief Meg_Help_LoadFile
 * @param tree_view
 * @param path
 * @param column
 * @param user_data
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


/**
 * @brief MegWidget_Help_Create
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

/**
 * @brief MegWidget_Help_Init
 */
void MegWidget_Help_Init()
{
	Meg_Help_Open( "index.xml" );

}

/**
 * @brief MegWidget_Help_Close
 */
void MegWidget_Help_Close()
{


}



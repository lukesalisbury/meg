/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Standard Headers */
#include "global.h"
#include "ma_web.h"

/* Required Headers */
#include <libsoup/soup.h>
#include <libsoup/soup-method.h>

/* External Functions */



/* Local Type */


/* Global Variables */
extern GtkWidget * mokoi_questions_treeview;
extern GtkWidget * mokoi_questions_text;


/* Local Variables */


/* UI */


/* Functions */
void Meg_Questions_AppendAnswerText( GHashTable * table, GtkTextBuffer * buffer )
{
	gchar * title = NULL;
	gchar * content;
	GtkTextIter iter;

	soup_value_hash_lookup( table, "title", G_TYPE_STRING, &title);
	soup_value_hash_lookup( table, "content", G_TYPE_STRING, &content);

	gtk_text_buffer_get_end_iter( buffer, &iter);

	gtk_text_buffer_insert_with_tags_by_name( buffer, &iter, title, -1, "special", "normal", "bold", NULL);
	gtk_text_buffer_insert_with_tags_by_name( buffer, &iter, "\n", -1, "special", "normal", NULL );
	gtk_text_buffer_insert_with_tags_by_name( buffer, &iter, content, -1, "special", "normal", NULL );
	gtk_text_buffer_insert( buffer, &iter, "\n", -1 );

}

void Meg_Questions_AppendQuestionText( GHashTable * table, GtkTextBuffer * buffer )
{
	gchar * title = NULL;
	gchar * content;
	GtkTextIter iter;



	soup_value_hash_lookup( table, "title", G_TYPE_STRING, &title);
	soup_value_hash_lookup( table, "content", G_TYPE_STRING, &content);

	gtk_text_buffer_get_end_iter( buffer, &iter);

	gtk_text_buffer_insert_with_tags_by_name( buffer, &iter, title, -1, "bold", "large", NULL);
	gtk_text_buffer_insert( buffer, &iter, "\n\n", -1);
	gtk_text_buffer_insert_with_tags_by_name( buffer, &iter, content, -1, "normal", NULL );
	gtk_text_buffer_insert( buffer, &iter, "\n\n", -1);

}

void Meg_Questions_Append( GHashTable * table )
{
	GtkTreeModel * model = gtk_tree_view_get_model( GTK_TREE_VIEW(mokoi_questions_treeview) );
	GtkTreeIter iter;


	gchar * title = NULL;
	gchar * ident_string;
	gint ident = 0;

	soup_value_hash_lookup( table, "title", G_TYPE_STRING, &title);
	soup_value_hash_lookup( table, "postid", G_TYPE_STRING, &ident_string);

	ident = (gint)g_ascii_strtoll(ident_string, NULL, 10);

	gtk_list_store_append( GTK_LIST_STORE(model), &iter );
	gtk_list_store_set( GTK_LIST_STORE(model), &iter, 0, g_strdup(title), 1, ident, -1 );

}



/********************************
* Meg_Questions_GetAll
*
*/
void Meg_Questions_GetAll()
{
	guint status;
	SoupMessage * msg;
	SoupSession * session = NULL;

	session = soup_session_sync_new();

	msg = soup_xmlrpc_request_new( "http://openzelda.net/qa/xml-rpc", "q2a.listQuestion", G_TYPE_STRING, "test", G_TYPE_STRING, "test", G_TYPE_STRING, "sort", G_TYPE_STRING, "asc", NULL );
	status = soup_session_send_message( session, msg );
	//g_print("Status: %d\nText: %s\n", status, msg->response_body->data );


	GError *error = NULL;
	GHashTable *hash;
	if ( soup_xmlrpc_extract_method_response( msg->response_body->data, msg->response_body->length, &error, G_TYPE_HASH_TABLE, &hash) )
	{
		GHashTable *data_hash = NULL;
		GValueArray * array = NULL;

		soup_value_hash_lookup( hash, "data", G_TYPE_VALUE_ARRAY, &array);
		if ( array )
		{
			int i;
			for (i = 0; i < array->n_values; i++) {
				soup_value_array_get_nth(array, i, G_TYPE_HASH_TABLE, &data_hash);
				Meg_Questions_Append( data_hash );
			}
		}
		g_hash_table_destroy(data_hash);


	}
	//g_hash_table_foreach (hash, print_struct_field, NULL);
	g_hash_table_destroy (hash);



}


void Meg_Questions_Get_Download( gpointer data)
{


}

/********************************
* Meg_Questions_Get
*
*/
void Meg_Questions_Get( gint ident )
{
	guint status;
	SoupMessage * msg;
	SoupSession * session = NULL;

	GtkTextBuffer * buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(mokoi_questions_text) );

	session = soup_session_sync_new();

	//$args ($username, $password, $data['postid', 'action', 'action_id', 'action_data'])

	msg = soup_xmlrpc_request_new( "http://openzelda.net/qa/xml-rpc", "q2a.getQuestion", G_TYPE_STRING, "postid", G_TYPE_INT, ident, NULL );
	status = soup_session_send_message( session, msg );
	//g_print("Status: %d\nText: %s\n", status, msg->response_body->data );

	gtk_text_buffer_set_text( buffer, "", -1);

	GError *error = NULL;
	GHashTable *hash;
	if ( soup_xmlrpc_extract_method_response( msg->response_body->data, msg->response_body->length, &error, G_TYPE_HASH_TABLE, &hash) )
	{

		GHashTable *data_hash = NULL;
		GValueArray * array = NULL;

		soup_value_hash_lookup( hash, "data", G_TYPE_HASH_TABLE, &data_hash);

		Meg_Questions_AppendQuestionText( data_hash, buffer );

		g_hash_table_destroy(data_hash);


		soup_value_hash_lookup( hash, "data2", G_TYPE_VALUE_ARRAY, &array);
		if ( array )
		{
			int i;
			for (i = 0; i < array->n_values; i++) {
				soup_value_array_get_nth(array, i, G_TYPE_HASH_TABLE, &data_hash);
				Meg_Questions_AppendAnswerText( data_hash, buffer );
				g_hash_table_destroy(data_hash);
			}
		}



	}
	//g_hash_table_foreach (hash, print_struct_field, NULL);
	g_hash_table_destroy (hash);

}


void Meg_Questions_DisplayItem( GtkTreeView * tree_view, GtkTreeViewColumn *column, GtkTreePath *path, gpointer data )
{
	gint id = -1;
	GtkTreeIter iter;
	GtkTreeModel * model;
	GtkTreeSelection * selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(tree_view) );

	if ( gtk_tree_selection_get_selected(selection, &model, &iter) )
	{

		gtk_tree_model_get( model, &iter, 1, &id, -1);
		if ( id > 0 )
		{
			Meg_Questions_Get( id );
		}
	}
}








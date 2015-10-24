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
#include "managed_entity.h"
#include <glib/gstdio.h>
#include <glib/gprintf.h>


/* Global Variables */
extern GError * mokoiError;
extern GtkWidget * mokoiEntityTreeview;

/* Local Functions  */
void ManagedEntityParser_StartHandler( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values,  gpointer data, GError **error);
void ManagedEntityParser_EndHandler( GMarkupParseContext *context, const gchar *element_name, gpointer data, GError **error);
void ManagedEntityParser_TextHandler( GMarkupParseContext *context, const gchar *passthrough_text, gsize text_len, gpointer data, GError **error);

/* Local Variables */
static GMarkupParser mokoiEntityParser = {ManagedEntityParser_StartHandler, ManagedEntityParser_EndHandler, NULL, ManagedEntityParser_TextHandler, NULL};
gint mokoiEntityParserMode = TPM_NEW;

EntityState * current_state = NULL;
EntityFunction * current_function = NULL;

/* state managemant */
GList * mokoiEntityStateList = NULL;
GtkTreeStore * mokoiEntityActionsStore = NULL;
GtkTreeStore * mokoiEntityCausesStore = NULL;
GtkWidget * mokoi_property_image = NULL;
EntityState * mokoiEntityStateCurrent = NULL;

const gchar * mokoiUI_ManagedProperty = "";
const gchar * mokoiUI_ManagedState = "";
const gchar * mokoiUI_ManagedFunction = "";

/* UI */

const gchar * mokoiUI_ManagedEntityEditor = GUI_MANAGED_ENTITY_EDITOR;

/********************************
* Compares EntityState name against a string
*/
gint EntityState_Compare (gconstpointer a, gconstpointer b)
{
	return g_ascii_strcasecmp(((EntityState*)a)->name, (gchar*)b);
}

/********************************
* Refresh Functions Treeview
*/
void EntityState_ListFunction()
{
	GtkTreeIter iter_child;
	GList * scan;
	EntityFunction * data;

	gtk_tree_store_clear(mokoiEntityActionsStore);
	gtk_tree_store_clear(mokoiEntityCausesStore);

	scan = g_list_first(mokoiEntityStateCurrent->actions);
	if ( scan )
	{
		while (scan)
		{
			data = (EntityFunction*)scan->data;
			gtk_tree_store_append (mokoiEntityActionsStore, &iter_child, NULL);
			gtk_tree_store_set (mokoiEntityActionsStore, &iter_child, 0, data->name, 1, TRUE, 2, (gpointer)data, -1);
			scan = g_list_next(scan);
		}
	}

	scan = g_list_first(mokoiEntityStateCurrent->causes);
	if ( scan )
	{
		while ( scan )
		{
			data = (EntityFunction*)scan->data;
			gtk_tree_store_append (mokoiEntityCausesStore, &iter_child, NULL);
			gtk_tree_store_set (mokoiEntityCausesStore, &iter_child, 0, data->name, 1, TRUE, 2, (gpointer)data, -1);
			scan = g_list_next(scan);
		}
	}
}

/********************************
* Append New State to Entity
*/
void ManagedEntity_AddState(gchar * name)
{
	EntityState * state = g_new0(EntityState, 1);
	state->name = g_strdup(name);
	state->actions = NULL;
	state->causes = NULL;
	state->directions = 0;
	state->base_sprite = NULL;
	state->collisions = NULL;

	mokoiEntityStateList = g_list_append(mokoiEntityStateList, state);
}

/********************************
* Append New State to Entity
*/
void EntityState_AddFunction(EntityFunction * function)
{
	if ( mokoiEntityStateCurrent )
	{
		mokoiEntityStateCurrent->actions = g_list_append(mokoiEntityStateCurrent->actions, function);
		EntityState_ListFunction();
	}
}

/********************************
* Event:
* Result: Brings up Add State dialog
*/
void EntityState_Add( GtkButton * button, GtkComboBox * widget )
{
	GtkWidget * dialog, * frame, * entry;

	dialog = gtk_dialog_new_with_buttons ("Add State", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	frame = gtk_frame_new ("Name of State");
	entry = gtk_entry_new ();

	gtk_box_pack_start (GTK_BOX( gtk_dialog_get_content_area(GTK_DIALOG(dialog))), frame, TRUE, TRUE, 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);
	gtk_container_add (GTK_CONTAINER(frame), entry);

	gtk_widget_show_all(dialog);

	if ( gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT )
	{
		const gchar * state_name = gtk_entry_get_text(GTK_ENTRY (entry));
		if ( g_utf8_strlen(state_name, -1) > 1 )
		{
			if ( !g_list_find_custom(mokoiEntityStateList, state_name, (GCompareFunc)EntityState_Compare) )
			{
				Meg_ComboText_AppendText(widget, state_name);
				ManagedEntity_AddState((gchar *)state_name);
			}
		}
	}
	gtk_widget_destroy (dialog);
}

/********************************
* Event:
* Result: Remove Currrent State
*/
void EntityState_Remove( GtkButton * button, GtkComboBox * widget )
{
	GList * active_state = NULL;
	gchar * active_text = Meg_ComboText_GetText(widget);
	gint active_int = gtk_combo_box_get_active(widget);
	GtkWidget * dialog = NULL;

	if ( active_int != -1)
	{
		active_state = g_list_find_custom(mokoiEntityStateList, active_text, (GCompareFunc)EntityState_Compare);
		if ( active_state )
		{
			if ( ((EntityState *)active_state->data)->locked )
			{
				dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "State '%s' is locked and can't be removed.", active_text);
				gtk_dialog_run (GTK_DIALOG (dialog));
			}
			else
			{
				dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Remove State: '%s'", active_text);
				if ( gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES )
				{
					g_list_free( ((EntityState *)active_state->data)->actions );
					g_list_free( ((EntityState *)active_state->data)->collisions );
					/*gtk_combo_box_remove_text(widget, active_int);*/
				}
			}
		}
		else
		{
			dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "State not found.");
			gtk_dialog_run (GTK_DIALOG (dialog));
		}
		gtk_widget_destroy (dialog);
	}
	g_free(active_text);
}

/********************************
* Event:
* Result:
*/
void EntityState_Changed( GtkComboBox *widget, gpointer user_data)
{
	gchar * active_text;
	GList * active_state;
	GdkPixbuf * sprite = NULL;

	active_text = Meg_ComboText_GetText(widget);
	active_state = g_list_find_custom(mokoiEntityStateList, active_text, (GCompareFunc)EntityState_Compare);

	if ( active_state )
	{
		mokoiEntityStateCurrent = (EntityState*)active_state->data;
		sprite = AL_GetSprite( mokoiEntityStateCurrent->base_sprite );
		gtk_image_set_from_pixbuf(GTK_IMAGE(mokoi_property_image), sprite);
		EntityState_ListFunction();
	}

	g_free(active_text);
}


/********************************
* Event:
* Result:
*/
void ManagedEntity_FunctionAdd ( GtkWidget * treeview, GtkWidget * button )
{
	GtkListStore * store;
	GtkTreeIter iter;
	GList * functions;

	functions = (GList *)g_object_get_data(G_OBJECT(treeview), "function_list");
	EntityFunction * func = g_new0(EntityFunction, 1);

	func->name = "Custom";
	func->predefined = NULL;
	func->custom = "";
	functions = g_list_append(functions, func);

	store = (GtkListStore *)gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	gtk_list_store_append (store, &iter);
	gtk_list_store_set (store, &iter, 0, func->name, 1, func, -1);

}


/********************************
* Event:
* Result:
*/
gboolean ManagedEntity_FunctionMenu( GtkWidget *treeview, GdkEventButton *event, GList * functions)
{
	/*
	GtkTreeSelection *selection;
	if ( event->type == GDK_BUTTON_PRESS && event->button == 3 )
	{
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
		if ( gtk_tree_selection_count_selected_rows(selection) <= 1 )
		{
			GtkTreePath *path;
			if ( gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), (gint) event->x, (gint) event->y, &path, NULL, NULL, NULL) )
			{
				gtk_tree_selection_unselect_all(selection);
				gtk_tree_selection_select_path(selection, path);
				gtk_tree_path_free(path);
			}
		}
		ManagedEntity_Menu(treeview, functions);
		return TRUE;
	}
	*/
	return FALSE;
}


/********************************
* Event:
* Result:
*/
gboolean ManagedEntity_MenuClick( GtkWidget * treeview, GdkEventButton * event, gpointer data )
{
	GtkTreeSelection * selection;
	if ( event->type == GDK_BUTTON_PRESS && event->button == 3 )
	{
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
		if ( gtk_tree_selection_count_selected_rows(selection) <= 1 )
		{
			GtkTreePath *path;
			if ( gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), (gint) event->x, (gint) event->y, &path, NULL, NULL, NULL) )
			{
				gtk_tree_selection_unselect_all(selection);
				gtk_tree_selection_select_path(selection, path);
				gtk_tree_path_free(path);
			}
		}

		return TRUE;
	}

	return FALSE;
}

/********************************
* Event:
* Result:
*/
void EntityState_ChangeSprite(gpointer user_data, GtkButton *button)
{
	Meg_Log_Print( LOG_NONE,"EntityState_ChangeSprite: not implemented yet.\n");
}

/********************************
* ManagedEntity_Close
*
@ widget:
@ event:
@ managed_entity:
*/
gboolean ManagedEntity_Close( GtkWidget * widget, GdkEvent * event, MokoiManagedEntity * managed_entity )
{
	gint result = 0;
	GtkWidget * dialog = NULL;
	GtkWidget * window = gtk_widget_get_toplevel( widget );

	dialog = gtk_message_dialog_new( GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "Do you wish to close '%s'", managed_entity->file );
	gtk_dialog_add_buttons( GTK_DIALOG(dialog), GTK_STOCK_YES, 1, GTK_STOCK_CANCEL, 0, NULL );

	result = gtk_dialog_run( GTK_DIALOG(dialog) );
	gtk_widget_destroy( dialog );

	if ( result == 0 )
	{
		return TRUE;
	}
	else
	{
		gchar * tree_path = (gchar *)g_object_get_data( G_OBJECT(window), "alchera-list-path");
		GtkTreeModel * list = gtk_tree_view_get_model( GTK_TREE_VIEW(mokoiEntityTreeview) );
		GtkTreeIter iter;
		if ( gtk_tree_model_get_iter_from_string( list, &iter, tree_path ) )
		{
			gtk_tree_store_set( GTK_TREE_STORE(list), &iter, 5, "", 4, NULL, -1);
		}
		ManagedEntity_Save( NULL, managed_entity );
	}
	return FALSE;
}


/********************************
* EntityEditor_Close2
* Calls Close on Editor when pressing the close button
@ button:
@ textview:
*/
gboolean ManagedEntity_Close2( GtkButton * button, MokoiManagedEntity * managed_entity )
{
	GtkWidget * window = gtk_widget_get_toplevel( GTK_WIDGET(button) );
	if ( !ManagedEntity_Close(window, NULL, managed_entity) )
	{
		gtk_widget_destroy( window );
	}

	return FALSE;

}


/********************************
* EntityEditor_Close2
* Calls Close on Editor when pressing the close button
@ button:
@ textview:
*/
gboolean ManagedEntity_Save2( GtkButton * button, MokoiManagedEntity * managed_entity )
{
	ManagedEntity_Save( NULL, managed_entity );
	return FALSE;

}


/********************************
* Event:
* Result:
*/

GtkWidget *  ManagedEntity_Edit(gchar * filename)
{
	GtkWidget * window, * tree_listing, * button_save, * button_close, * icon_options;
	GtkListStore * store_states, * store_icon;
	GtkTreeIter iter;
	EntityState * state;
	GtkTreeViewColumn * column;
	MokoiManagedEntity * managed_entity = ManagedEntity_Load(filename);

	if ( !managed_entity )
	{
		Meg_Error_Print( __func__, __LINE__, "Managed Entity load failed.");
		return FALSE;
	}

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(mokoiUI_ManagedEntityEditor, __func__, __LINE__);
	g_return_val_if_fail( ui, FALSE );

	/* Widget */
	window = GET_WIDGET( ui, "window");
	tree_listing = GET_WIDGET( ui, "tree_listing");
	button_save = GET_WIDGET( ui, "button_save" );
	button_close = GET_WIDGET( ui, "button_close" );
	tree_listing = GET_WIDGET( ui, "tree_listing" );
	icon_options = GET_WIDGET( ui, "icon_options" );

	store_states = GET_LISTSTORE( ui, "store_states" );
	store_icon = GET_LISTSTORE( ui, "store_icon" );

	/* Iconview*/
	GdkPixbuf * test_image = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, 22, 22 );
	gdk_pixbuf_fill( test_image, 0xDDDDDDFF );

	gtk_list_store_append( store_icon, &iter );
	gtk_list_store_set( store_icon, &iter, 0, "Action", 2, test_image, -1 );


	/*0=>Icon 1=>Ident, Auto Advance, Type, Width, Height  */
	gtk_tree_view_set_model( GTK_TREE_VIEW(tree_listing), GTK_TREE_MODEL(managed_entity->store) );


	/* Signals */
	g_signal_connect( button_save, "clicked", G_CALLBACK(ManagedEntity_Save2), managed_entity );
	g_signal_connect( button_close, "clicked", G_CALLBACK(ManagedEntity_Close2), managed_entity );
	g_signal_connect( window, "delete-event", G_CALLBACK(ManagedEntity_Close), managed_entity );
	g_signal_connect( tree_listing, "button_press_event", G_CALLBACK(ManagedEntity_MenuClick), NULL );

/*
	gtk_tree_view_set_model(GTK_TREE_VIEW(state_list), GTK_TREE_MODEL(state_list_store));
	gtk_tree_view_set_model(GTK_TREE_VIEW(subroutine_list), GTK_TREE_MODEL(runtime_list_store));
	gtk_tree_view_set_model(GTK_TREE_VIEW(runtime_list), GTK_TREE_MODEL(subroutine_list_store));

	column = gtk_tree_view_column_new_with_attributes ("States", gtk_cell_renderer_text_new(), "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(state_list), column);

	char *markup = g_markup_printf_escaped ("<b>%s</b>\n%s", "Entity Properties", g_path_get_basename(mokoiManagedFile.file) );
	gtk_label_set_markup (GTK_LABEL (label), markup);
	g_free (markup);

	if ( g_list_length(mokoiManagedFile.states) )
	{
		GList * scan = g_list_first(mokoiManagedFile.states);
		while ( scan )
		{
			state = (EntityState*)scan->data;
			gtk_list_store_append (state_list_store, &iter);
			gtk_list_store_set (state_list_store, &iter, 0, state->name, 1, state, -1);
			scan = g_list_next(scan);
		}
	}
*/
	gtk_widget_show_all( window );

	/*
	if ( gtk_dialog_run(GTK_DIALOG(window)) == GTK_RESPONSE_ACCEPT )
	{
		EntityParser_Save(filename);
	}
	gtk_widget_destroy(window);
	*/
	return window;
}

/********************************
* Event:
* Result:
*/

void ManagedEntity_StateProperties(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	GtkWidget * window, * label, * actions_list, * causes_list;
	GtkListStore * actions_list_store, * causes_list_store;
	GtkTreeIter iter;
	EntityState * state;
	EntityFunction * func;
	GtkTreeModel * model;
	GtkTreeSelection * select;

	select = gtk_tree_view_get_selection(tree_view);
	if ( gtk_tree_selection_get_selected(select, &model, &iter) )
	{
		gtk_tree_model_get (model, &iter, 1, &state, -1);
	}

	if ( !state )
	{
		Meg_Error_Print( __func__, __LINE__, "State missing.");
		return;
	}

	/* UI */
	GError * error = NULL;
	GtkBuilder * ui = gtk_builder_new();
	if ( !gtk_builder_add_from_string(ui, mokoiUI_ManagedState , -1, &error) )
	{
		Meg_Error_Print( __func__, __LINE__, "UI creation error '%s'.", error->message );
		return;
	}
	gtk_builder_connect_signals( ui, NULL );

	window = GET_WIDGET( ui, "mokoi_entity_state_window");
	label = GET_WIDGET( ui, "alchera-label");
	actions_list = GET_WIDGET( ui, "mokoi_actions_list");
	causes_list = GET_WIDGET( ui, "mokoi_causes_list");


	/* Name, EntityFunction */
	actions_list_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	causes_list_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);

	gtk_tree_view_set_model(GTK_TREE_VIEW(actions_list), GTK_TREE_MODEL(actions_list_store));
	g_signal_connect(G_OBJECT(actions_list), "button-press-event", (GCallback)ManagedEntity_FunctionMenu, state->actions);
//	g_signal_connect(G_OBJECT(actions_list), "popup-menu", (GCallback)ManagedEntity_Menu, state->actions);
	g_object_set_data(G_OBJECT(actions_list), "function_list", (gpointer)state->actions);

	gtk_tree_view_set_model(GTK_TREE_VIEW(causes_list), GTK_TREE_MODEL(causes_list_store));
	g_signal_connect(G_OBJECT(causes_list), "button-press-event", (GCallback)ManagedEntity_FunctionMenu, state->causes);
//	g_signal_connect(G_OBJECT(causes_list), "popup-menu", (GCallback)ManagedEntity_Menu, state->causes);
	g_object_set_data(G_OBJECT(causes_list), "function_list", (gpointer)state->causes);

	column = gtk_tree_view_column_new_with_attributes ("", gtk_cell_renderer_text_new(), "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(actions_list), column);

	column = gtk_tree_view_column_new_with_attributes ("", gtk_cell_renderer_text_new(), "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(causes_list), column);

	char *markup = g_markup_printf_escaped ("<b>%s: </b>%s", "State", state->name );
	gtk_label_set_markup (GTK_LABEL (label), markup);
	g_free (markup);

	gtk_list_store_append (causes_list_store, &iter);
	gtk_list_store_set (causes_list_store, &iter, 0, "Test", 1, NULL, -1);

	gtk_list_store_append (actions_list_store, &iter);
	gtk_list_store_set (actions_list_store, &iter, 0, "Test", 1, NULL, -1);

	GList * scan = NULL;
	if ( g_list_length(state->causes) )
	{
		scan = g_list_first(state->causes);
		while ( scan )
		{
			func = (EntityFunction*)scan->data;
			gtk_list_store_append (causes_list_store, &iter);
			gtk_list_store_set (causes_list_store, &iter, 0, func->name, 1, func, -1);
			scan = g_list_next(scan);
		}
	}
	if ( g_list_length(state->actions) )
	{
		scan = g_list_first(state->actions);
		while ( scan )
		{
			func = (EntityFunction*)scan->data;
			gtk_list_store_append (actions_list_store, &iter);
			gtk_list_store_set (actions_list_store, &iter, 0, func->name, 1, func, -1);
			scan = g_list_next(scan);
		}
	}
	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(window) ) );
	gtk_dialog_run(GTK_DIALOG(window));
	gtk_widget_destroy(window);

}


/********************************
* Event: ManagedEntity_FunctionWindow
* Result:
* Uses G_CONNECT_SWAPPED
*/
G_MODULE_EXPORT void ManagedEntity_FunctionProperties(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	GtkWidget * window, * custom_func, * once_func;
	GtkTreeIter iter;
	GtkTextIter start, end;
	EntityFunction * func;
	GtkTreeModel * model;
	GtkTreeSelection * select;
	GtkTextBuffer * buffer;

	select = gtk_tree_view_get_selection(tree_view);
	if ( gtk_tree_selection_get_selected(select, &model, &iter) )
	{
		gtk_tree_model_get (model, &iter, 1, &func, -1);
	}

	if ( !func )
	{
		Meg_Error_Print( __func__, __LINE__, "Function missing.");
		return;
	}

	/* UI */
	GError * error = NULL;
	GtkBuilder * ui = gtk_builder_new();
	if ( !gtk_builder_add_from_string(ui, mokoiUI_ManagedFunction , -1, &error) )
	{
		Meg_Error_Print( __func__, __LINE__, "UI creation error '%s'.", error->message );
		return;
	}
	gtk_builder_connect_signals( ui, NULL );

	window = GET_WIDGET( ui, "mokoi_entity_function_window");
	custom_func = GET_WIDGET( ui, "mokoi_entityfunction_custom");
	once_func = GET_WIDGET( ui, "mokoi_entityfunction_once");

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(custom_func));
	gtk_text_buffer_set_text(buffer, func->custom, -1);

	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(window) ) );
	if ( gtk_dialog_run(GTK_DIALOG(window)) == GTK_RESPONSE_ACCEPT )
	{
		gtk_text_buffer_get_bounds( buffer, &start, &end );
		func->custom = g_strdup(gtk_text_buffer_get_text(buffer, &start, &end, FALSE));
	}
	gtk_widget_destroy( window );

}

/*******************************
* Event:
* Result:
* Uses G_CONNECT_SWAPPED
*/
G_MODULE_EXPORT void Meg_EntityList_Properties_AddState(GtkTreeView * user_data, GtkButton * button)
{
	EntityState * state = g_new0(EntityState, 1);
	state->name = "New State";
	state->causes = NULL;
	state->actions = NULL;
	state->directions = 0;
	state->base_sprite = NULL;
	state->collisions = NULL;

	GtkTreeIter iter;
	GtkTreeModel * state_model = gtk_tree_view_get_model(user_data);
	if ( state_model )
	{
		gtk_list_store_append (GTK_LIST_STORE(state_model), &iter);
		gtk_list_store_set (GTK_LIST_STORE(state_model), &iter, 0, state->name, 1, state, -1);
	}
}

/*******************************
* Event:
* Result:
*/
void Meg_EntityList_Properties_RemoveState( GtkTreeView * user_data, GtkButton * button )
{
	Meg_Error_Print( __func__, __LINE__, "Meg_EntityList_Properties_RemoveState to be added");
}

/*******************************
* Event:
* Result:
*/
void Meg_EntityList_Properties_AddRuntime( GtkButton * button, GtkTreeView * user_data )
{
	/*
	EntityOptions * options = g_new0(EntityOptions, 1);
	options->name = "New Option";
	options->predefined = NULL;
	options->custom = NULL;
	mokoiManagedFile.options = g_list_append(mokoiManagedFile.options, (gpointer)options);

	GtkTreeIter iter;
	GtkTreeModel * options_model = gtk_tree_view_get_model(user_data);
	if ( options_model )
	{
		gtk_list_store_append (GTK_LIST_STORE(options_model), &iter);
		gtk_list_store_set (GTK_LIST_STORE(options_model), &iter, 0, options->name, 1, options, -1);
	}
*/
}

/*******************************
* Event:
* Result:
*/
void Meg_EntityList_Properties_RemoveRuntime( GtkButton * button, GtkTreeView * user_data )
{
	Meg_Error_Print( __func__, __LINE__, "Meg_EntityList_Properties_RemoveRuntime to be added");
}

/*******************************
* Event:
* Result:
*/
void Meg_EntityList_Properties_AddRoutines( GtkButton * button, GtkTreeView * user_data )
{
	Meg_Error_Print( __func__, __LINE__, "Meg_EntityList_Properties_AddRoutines to be added. Meanwhile just copy the file into the game's script folder");
}

/*******************************
* Event:
* Result:
*/
void Meg_EntityList_Properties_RemoveRoutines( GtkButton * button, GtkTreeView * user_data )
{
	Meg_Error_Print( __func__, __LINE__, "Meg_EntityList_Properties_RemoveRoutines to be added");
}



/********************************
* ManagedEntityParser_TextHandler
* Part of the XML parser
*/
void ManagedEntityParser_TextHandler(GMarkupParseContext *context, const gchar *passthrough_text, gsize text_len,  gpointer data, GError **error)
{
	MokoiManagedEntity * managed_file = (MokoiManagedEntity *)data;
	if ( managed_file->mode == TPM_FUNCTION )
	{
		current_function->custom = g_strdup(passthrough_text);
	}
}

/********************************
* ManagedEntityParser_StartHandler
* Part of the XML parser
*/
void ManagedEntityParser_AddState(MokoiManagedEntity * managed_file, const gchar **attribute_names, const gchar **attribute_values)
{
	EntityState * state = g_new0(EntityState, 1);
	state->name = NULL;
	state->causes = NULL;
	state->actions = NULL;
	state->directions = 0;
	state->base_sprite = NULL;
	state->collisions = NULL;
	for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
	{
		if ( !g_ascii_strcasecmp(*attribute_names, "id") )
		{
			state->name = g_strdup(*attribute_values);
			if ( !g_ascii_strcasecmp(*attribute_values, "init") || !g_ascii_strcasecmp(*attribute_values, "default") || !g_ascii_strcasecmp(*attribute_values, "destroy") )
			{
				state->locked = TRUE;
			}
		}
		else if ( !g_ascii_strcasecmp(*attribute_names, "object") )
		{
			state->base_sprite = g_strdup(*attribute_values);
		}
		else if ( !g_ascii_strcasecmp(*attribute_names, "directions") )
		{
			state->directions = atoi(*attribute_values);
		}
	}

	GtkTreeIter current_iter;
	gtk_tree_store_append( managed_file->store, &current_iter, managed_file->parent_iter );
	gtk_tree_store_set( managed_file->store, &current_iter, 0, g_strdup(state->name), 1, (gpointer)state, 2, TPM_STATE, -1 );

}

/********************************
* ManagedEntityParser_StartHandler
* Part of the XML parser
*/
void ManagedEntityParser_AddFunction(MokoiManagedEntity * managed_file, const gchar **attribute_names, const gchar **attribute_values)
{
	EntityFunction * func = g_new0(EntityFunction, 1);
	func->name = NULL;
	func->predefined =  NULL;
	func->custom = "";

	for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
	{
		if ( !g_ascii_strcasecmp(*attribute_names, "id") )
		{
			func->name = g_strdup(*attribute_values);
		}
		else if ( !g_ascii_strcasecmp(*attribute_names, "function") )
		{
			func->predefined = g_strdup(*attribute_values);
		}
	}

	GtkTreeIter current_iter;
	gtk_tree_store_append( managed_file->store, &current_iter, managed_file->parent_iter );
	gtk_tree_store_set( managed_file->store, &current_iter, 0, g_strdup(func->name), 1, (gpointer)func, 2, TPM_FUNCTION, -1 );

}

/********************************
* ManagedEntityParser_StartHandler
* Part of the XML parser
*/
void ManagedEntityParser_StartHandler(GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values,  gpointer data, GError **error)
{
	MokoiManagedEntity * managed_file = (MokoiManagedEntity *)data;

	switch ( managed_file->mode )
	{
		case TPM_NEW:
			managed_file->mode = TPM_ROOT;
			break;
		case TPM_ROOT:
			if ( !g_ascii_strcasecmp(element_name, "state") )
			{
				managed_file->mode = TPM_STATE;
				ManagedEntityParser_AddState(managed_file, attribute_names, attribute_values);
			}
			else if ( !g_ascii_strcasecmp(element_name, "options") )
			{

			}
			break;
		case TPM_STATE:
		{
			/* "cause", "action", collision */
			if ( !g_ascii_strcasecmp(element_name, "cause") )
			{
				if ( current_state )
				{
					/* Add Function */
					ManagedEntityParser_AddFunction(managed_file,attribute_names, attribute_values);
					managed_file->mode = TPM_FUNCTION;
				}
			}
			else if ( !g_ascii_strcasecmp(element_name, "action") )
			{
				if ( current_state )
				{
					ManagedEntityParser_AddFunction(managed_file,attribute_names, attribute_values);
					managed_file->mode = TPM_FUNCTION;
				}
			}
			else if ( !g_ascii_strcasecmp(element_name, "collision") )
			{
			}
		}
	}
}

/********************************
* ManagedEntityParser_EndHandler
* Part of the XML parser
*/
void ManagedEntityParser_EndHandler(GMarkupParseContext *context, const gchar *element_name, gpointer data, GError **error)
{
	MokoiManagedEntity * managed_file = (MokoiManagedEntity *)data;
	if ( managed_file->mode == TPM_FUNCTION )
	{
		managed_file->mode = TPM_STATE;
	}
	else if ( managed_file->mode == TPM_STATE )
	{
		managed_file->mode = TPM_ROOT;
	}

}

gboolean ManagedEntity_Save_Foreach( GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data )
{
	gboolean selected;
	gchar * string, * file_path;
	gtk_tree_model_get( model, iter, 0, &string, -1 );

	g_print( "%s\t>\t%s\n", gtk_tree_path_to_string(path), string );

	g_free( string );

	return FALSE;
}

/********************************
* ManagedEntity_Save
*
*/
gboolean ManagedEntity_Save(gchar * entity_name, MokoiManagedEntity * managed_entity )
{
	GList * scan = NULL;
	gchar * template_name;
	GString * template_content = g_string_new("<entity xmlns=\"http://mokoi.sourceforge.net/projects/mokoi\">\n");
	GString * entity_content = g_string_new("//DO NOT EDIT THIS FILE \n\n#include <managed_values>\n\n");
	GString * entity_functions = g_string_new("\n\n//Functions\n");

	template_name = g_strconcat (entity_name, ".managed", NULL);

	gtk_tree_model_foreach( GTK_TREE_MODEL(managed_entity->store), (GtkTreeModelForeachFunc)ManagedEntity_Save_Foreach, NULL);

/*
	if ( g_list_length(mokoiManagedFile.subroutines) )
	{
		scan = g_list_first(mokoiManagedFile.subroutines);
		while ( scan )
		{
			g_string_append_printf( template_content, "<include file=\"%s\"/>\n", (gchar*)scan->data);
			g_string_append_printf( entity_content, "#include <%s>\n", (gchar*)scan->data);
		}
	}
	if ( g_list_length(mokoiManagedFile.states) )
	{
		g_string_append_printf( entity_content, "states[][] = {");
		scan = g_list_first(mokoiManagedFile.states);
		while ( scan )
		{
			EntityState * state = (EntityState*)scan->data;

			g_string_append_printf( template_content, "<state id=\"%s\" object=\"%s\" dirs=\"%d\">\n", state->name, (state->base_sprite ? state->base_sprite : ""), state->directions);
			g_string_append_printf( entity_content, "\"%s\",", state->name);

			GList * functions = NULL;
			EntityFunction * func = NULL;
			guint count = 0;
			guint num_causes = 0;

			num_causes = g_list_length(state->causes);

			if ( num_causes )
			{
				g_string_append_printf( entity_functions, "\npublic c_%s() {\n new _subfunctions_ = %d;\n", state->name, num_causes);
				functions = g_list_first(state->causes);
				while ( functions )
				{
					func = (EntityFunction*)functions->data;
					if ( func->predefined )
					{
						g_string_append_printf( template_content, "<cause id=\"%s\" function=\"%s\"/>", func->name, func->predefined);
					}
					else
					{
						g_string_append_printf( template_content, "<cause id=\"%s\"><![CDATA[\n%s]]></action>", func->name, func->custom);
						g_string_append_printf( entity_functions, "_subfunctions_ -= c_%s_%d();\n", state->name, count);
					}
					functions = g_list_next(functions);
					count++;
				}
				g_string_append_printf( entity_functions, "\n return !_subfunctions_; \n}\n");
			}
*/
			/*g_string_append_printf( entity_content, "\npublic a_%s() {\n", g_strdelimit(g_strstrip(state->name), "_-|> <.\\/?*", (gchar)'_'));*/

//			if ( g_list_length(state->collisions) )
//			{
				/*fprintf( template_file, "<collision x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\"/>", collision->x, collision->y, collision->width, collision->height);*/
/*			}
			if ( g_list_length(state->actions) )
			{
				functions = g_list_first(state->actions);
				while (functions)
				{
					func = (EntityFunction*)functions->data;
					if ( func->predefined )
					{
						g_string_append_printf( template_content, "<action id=\"%s\" function=\"%s\"/>", func->name, func->predefined);
						g_string_append_printf( entity_content, "\n%s();", func->predefined);
					}
					else
					{
						g_string_append_printf( template_content, "<action id=\"%s\"><![CDATA[\n%s]]></action>", func->name, func->custom);

						g_string_append_printf( entity_content, "\n%s", func->custom);
					}
					functions = g_list_next(functions);
				}
			}
			g_string_append( template_content, "\n</state>\n");
			scan = g_list_next(scan);
		}
		g_string_append( entity_content, "};");
	}
	g_string_append( template_content, "\n</entity>");
*/
	/* Write managed file */
	/*g_warning( "Output: %s", template_content->str );*/
/*
	g_file_set_contents(template_name, template_content->str, -1, &mokoiError);
	if ( mokoiError )
	{
		Meg_Error_Print( __func__, __LINE__, "Managed Entity could not be saved. Reason: %s", mokoiError->message );
		g_clear_error( &mokoiError );
		return FALSE;
	}
	g_free(template_name);
	g_string_free(template_content, FALSE);


	g_string_append_printf( entity_content, "\n%s\n#include <managed>", entity_functions->str);

	g_file_set_contents(entity_name, entity_content->str, -1, &mokoiError);
	if ( mokoiError )
	{
		Meg_Error_Print( __func__, __LINE__, "Entity could not be saved. Reason: %s", mokoiError->message );
		g_clear_error( &mokoiError );
		return FALSE;
	}

*/
	return TRUE;
}


/********************************
* ManagedEntity_Load
*
*/
MokoiManagedEntity * ManagedEntity_Load( gchar * entity_name )
{
	GError * error  = NULL;
	gchar * file_content = NULL;
	GMarkupParseContext * ctx = NULL;

	MokoiManagedEntity * managed_file = g_new0(MokoiManagedEntity, 1);

	managed_file->file = g_strconcat(entity_name, ".managed", NULL);
	managed_file->store = gtk_tree_store_new( 3, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_UINT);
	managed_file->mode = TPM_NEW;

	Meg_file_get_contents(managed_file->file, &file_content, NULL, &error);

	if ( error )
	{
		Meg_Error_Print( __func__, __LINE__, "Managed Entity setting could not loaded. Reason: %s", error->message );
		g_clear_error (&error);
	}

	ctx = g_markup_parse_context_new( &mokoiEntityParser, (GMarkupParseFlags)G_MARKUP_TREAT_CDATA_AS_TEXT, (gpointer)managed_file, NULL );
	if ( !g_markup_parse_context_parse(ctx, file_content, -1, &error) )
	{
		Meg_Error_Print( __func__, __LINE__, "Parsing setting error. Reason: %s", error->message );
		g_clear_error (&error);
	}

	g_markup_parse_context_end_parse(ctx, &error);
	g_markup_parse_context_free(ctx);

	return managed_file;
}

/********************************
* ManagedEntity_New
* Create new template entity
*/
gboolean ManagedEntity_New(gchar * filename, gchar * image_text)
{
	gchar * managed_file = NULL, * file_content = NULL;
	Meg_file_set_contents(filename, "/*DO NOT EDIT THIS FILE*/\n#include <managed_base>\n#include <managed>", -1, &mokoiError);

	if ( mokoiError )
	{
		Meg_Error_Print( __func__, __LINE__, "Can't save entity: %s (%s)", filename, mokoiError->message);
		g_clear_error(&mokoiError);
		return FALSE;
	}

	managed_file = g_strconcat (filename, ".managed", NULL);
	file_content = g_strdup_printf("<entity xmlns=\"http://mokoi.sourceforge.net/projects/mokoi\"><state id=\"init\"  lock=\"lock\" object=\"%s\" dirs=\"1\"/><state id=\"default\" lock=\"lock\"  object=\"%s\" dirs=\"1\"/><state id=\"destroy\" lock=\"lock\" object=\"%s\" dirs=\"1\"/></entity>", image_text, image_text, image_text);

	Meg_file_set_contents(managed_file, file_content,-1, &mokoiError);
	if ( mokoiError )
	{
		Meg_Error_Print( __func__, __LINE__, "Can't open entity: %s (%s)", managed_file, mokoiError->message);
		g_clear_error(&mokoiError);
		return FALSE;
	}
	return TRUE;
}

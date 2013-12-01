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
#include "entity_functions.h"
#include "section_functions.h"
#include "runtime_parser.h"
#include "runtime_options.h"
#include "object_advance.h"
#include "loader_functions.h"

/* Global Functions */
guint Language_Size();


/* Global Variables */
extern gchar * mokoiBasePath;


/* UI */
#include "ui/object_entity.gui.h"
#include "ui/object_shape.gui.h"
#include "ui/object_sprite.gui.h"
#include "ui/object_line.gui.h"
#include "ui/object_text.gui.h"

const gchar * mokoiUI_ObjectEntites = GUIOBJECT_ENTITY
const gchar * mokoiUI_ObjectShape = GUIOBJECT_SHAPE
const gchar * mokoiUI_ObjectSprite = GUIOBJECT_SPRITE
const gchar * mokoiUI_ObjectText = GUIOBJECT_TEXT
const gchar * mokoiUI_ObjectLine = GUIOBJECT_LINE


/********************************
* Object_EntityWindowEdit
*
*/
void Object_EntityWindowEdit( GtkWidget * combo_file, GtkWidget * button )
{
	const gchar * entity = Meg_ComboText_GetText( GTK_COMBO_BOX(combo_file) );
	if ( g_utf8_strlen(entity, -1) )
	{
		gchar * file_path = g_build_filename( mokoiBasePath, "scripts", entity, NULL );
		EntityEditor_New( file_path );
		g_free(file_path);
	}
}


/********************************
* Mokoi_Map_ObjectEntity_Toggle
*
* Usage:
*/
void Mokoi_Map_ObjectEntity_Toggle(GtkWidget * textbox, GtkCheckButton * togglebutton)
{
	gtk_widget_set_sensitive( GTK_WIDGET(textbox), gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(togglebutton) ) );
}


/********************************
* ObjectAdvance_Text_TextNumber
*
* Usage: Advance Text Object
*/
void ObjectAdvance_Text_Spin( GtkSpinButton * spin, GtkWidget * textbox )
{
	gchar * text = NULL;
	gint string = gtk_spin_button_get_value_as_int( spin );


	if ( string == -1 )
	{
		gtk_widget_set_sensitive( GTK_WIDGET(textbox), TRUE );
		text = (gchar *)g_object_get_data( G_OBJECT(textbox), "old-string" );
	}
	else
	{
		gtk_widget_set_sensitive( GTK_WIDGET(textbox), FALSE );
		text = AL_String_Get( "lang/00.txt", string );
	}

	if ( text )
	{
		gtk_entry_set_text( GTK_ENTRY(textbox), text );
	}
	else
	{
		gtk_entry_set_text( GTK_ENTRY(textbox), "Error retrieving text" );
	}
}

/********************************
* ObjectAdvance_EntityWidget_New
*
@
@
*/
GtkWidget * ObjectAdvance_EntityWidget_New( MokoiMapObject * object )
{
	gchar * entity_file = NULL;
	GtkWidget * entity_widget;
	GtkWidget * runtime_box, * edit_id, * combo_file, * check_global, * add_button;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create( mokoiUI_ObjectEntites, __func__, __LINE__ );
	g_return_val_if_fail( ui, FALSE );

	/* Widget */
	entity_widget = GET_WIDGET( ui, "entity_page");
	runtime_box = GET_WIDGET( ui, "box_runtime");
	edit_id = GET_WIDGET( ui, "entry_entityid");
	combo_file = GET_WIDGET( ui, "combo_file");
	check_global = GET_WIDGET( ui, "check_global");
	add_button = GET_WIDGET( ui, "button_addoption");


	/* Settings */
	g_object_set_data( G_OBJECT(entity_widget), "mokoi-edit-id", edit_id);
	g_object_set_data( G_OBJECT(entity_widget), "mokoi-combo-file", combo_file);
	g_object_set_data( G_OBJECT(entity_widget), "mokoi-check-global", check_global);

	/* Set Default Value */
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(check_global), object->entity_global );

	/*  Object ID */
	if ( object->object_id )
	{
		gtk_entry_set_text( GTK_ENTRY(edit_id), object->object_id );
	}

	/* Entity File */
	if ( object->entity_file && object->entity_language )
	{
		entity_file = g_strdup_printf("%s.%s", object->entity_file, object->entity_language );
		Meg_ComboText_AppendText( GTK_COMBO_BOX(combo_file), entity_file );
	}
	Meg_ComboFile_Scan( combo_file, "/scripts/", ".mps", TRUE, 0 );
	Meg_ComboFile_Scan( combo_file, "/scripts/", ".lua", FALSE, 0 );
	Meg_ComboFile_Scan( combo_file, "/scripts/", ".js", FALSE, 0 );
	Meg_ComboFile_Scan( combo_file, "/scripts/", ".sq", FALSE, 0 );

	/* Options Treeview settings */
	if ( entity_file )
	{
		gchar * option_path = g_strdup_printf("/scripts/%s.options", entity_file );
		GHashTable * default_settings = RuntimeParser_Load( option_path );
		g_hash_table_foreach( default_settings, (GHFunc)RuntimeSetting_Append, (gpointer)object->settings );
		g_hash_table_remove_all( default_settings );
		g_free(option_path);
	}

	if ( g_hash_table_size(object->settings) )
	{
		g_object_set_data( G_OBJECT(runtime_box), "table-y", GUINT_TO_POINTER(0) );
		g_hash_table_foreach( object->settings, (GHFunc)RuntimeSetting_CreateWidget, runtime_box );
		g_hash_table_foreach( object->settings, (GHFunc)RuntimeSetting_AttachWidget, runtime_box );
	}

	g_object_set_data( G_OBJECT(runtime_box), "runtime-hashtable", object->settings);
	g_signal_connect(add_button, "clicked", (GCallback)RuntimeSetting_AddOption, runtime_box);

	g_free(entity_file);

	return entity_widget;
}

/********************************
* ObjectAdvance_EntityWidget_Save
*
@ parent:
@ settings:
*/
void ObjectAdvance_EntityWidget_Save( GtkWidget * parent, MokoiMapObject * object )
{
	GtkWidget * edit_id, * combo_file, * check_global;
	const gchar * name;
	gchar * entity;

	edit_id = g_object_get_data( G_OBJECT(parent), "mokoi-edit-id" );
	combo_file = g_object_get_data( G_OBJECT(parent), "mokoi-combo-file" );
	check_global = g_object_get_data( G_OBJECT(parent), "mokoi-check-global" );

	name = gtk_entry_get_text( GTK_ENTRY(edit_id) );
	entity = Meg_ComboText_GetText( GTK_COMBO_BOX(combo_file) );

	if ( entity == NULL )
	{
		REPLACE_STRING( object->object_id, NULL );
		REPLACE_STRING( object->entity_file, NULL );
		REPLACE_STRING( object->entity_language, NULL );

		object->entity_global = FALSE;
	}
	else if ( !g_ascii_strcasecmp("(New)", entity) )
	{
		/* TODO Create New entity */
		REPLACE_STRING( object->object_id, NULL );
		REPLACE_STRING( object->entity_file, NULL );
		REPLACE_STRING( object->entity_language, NULL );

		object->entity_global = FALSE;

	}
	else if ( g_ascii_strcasecmp("(None)", entity) ) // Entity Selected
	{
		if ( gtk_entry_get_text_length( GTK_ENTRY(edit_id) ) )
		{
			REPLACE_STRING( object->object_id, g_strdup(name) );
		}
		else
		{
			REPLACE_STRING( object->object_id, NULL );
		}

		gchar ** file = g_strsplit( entity, ".", 2);
		if ( g_strv_length(file) == 2 )
		{
			REPLACE_STRING( object->entity_file, g_strdup(file[0]) );
			REPLACE_STRING( object->entity_language, g_strdup(file[1]) );
			RuntimeSetting_SetDefaultValues( object );
		}
		g_strfreev( file );

		object->entity_global = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_global));
	}
	else
	{
		REPLACE_STRING( object->object_id, NULL );
		REPLACE_STRING( object->entity_file, NULL );
		REPLACE_STRING( object->entity_language, NULL );

		object->entity_global = FALSE;
	}
	g_free(entity);


	g_hash_table_foreach( object->settings, (GHFunc)RuntimeSetting_SaveWidget_Foreach, NULL );
}


/********************************
* ObjectAdvance_Shape
*
*/
gboolean ObjectAdvance_Shape( MokoiMapObject * obj, GtkWindow * window )
{
	GtkWidget * dialog, * label;
	GtkWidget * button_colour, * spin_x, * spin_y, * spin_z, * spin_w, * spin_h;
	GtkWidget * entity_parent;
	gint map_width = 320, map_height = 240;
	gint result = 0;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create( mokoiUI_ObjectShape,  __func__, __LINE__ );

	dialog = GET_WIDGET( ui, "object_advance");
	label = GET_WIDGET( ui, "alchera-label");
	entity_parent = GET_WIDGET( ui, "notebook1");

	button_colour = GET_WIDGET( ui, "button_colour");
	spin_x = GET_WIDGET( ui, "spin_x");
	spin_y = GET_WIDGET( ui, "spin_y");
	spin_z = GET_WIDGET( ui, "spin_z");
	spin_w = GET_WIDGET( ui, "spin_w");
	spin_h = GET_WIDGET( ui, "spin_h");


	/* Get Map Dimension */
	if ( obj->parent )
	{
		map_width = obj->parent->width;
		map_height = obj->parent->height;
	}

	/* Set Default Value */
	Meg_Misc_SetLabel_Print( label, "<b>Edit Shape Object</b>\n%s (%d)", obj->name, obj->object->id );

	gtk_spin_button_set_range( GTK_SPIN_BUTTON(spin_x), -100.0, (gdouble)map_width + 100.0 );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_x), obj->object->x );

	gtk_spin_button_set_range( GTK_SPIN_BUTTON(spin_y), -100.0, (gdouble)map_height + 100.0);
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_y), obj->object->y );

	gtk_spin_button_set_range( GTK_SPIN_BUTTON(spin_z), 0.0, 6.0 );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_z), (gdouble)obj->object->layer);



	gtk_spin_button_set_increments( GTK_SPIN_BUTTON(spin_w), (gdouble)obj->object->tw, (gdouble)obj->object->tw );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_w), obj->object->w);

	gtk_spin_button_set_increments( GTK_SPIN_BUTTON(spin_h), (gdouble)obj->object->th, (gdouble)obj->object->th );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_h), obj->object->h );

	Meg_ColorButton_SetColor( GTK_COLOR_BUTTON(button_colour), &obj->object->colour );


	/* Add Entity Widget to notebook */
	GtkWidget * notebookpage = NULL;
	GtkWidget * widget_entity = ObjectAdvance_EntityWidget_New( obj );
	notebookpage = gtk_notebook_get_nth_page( GTK_NOTEBOOK(entity_parent), 1 );
	if ( notebookpage )
	{
		gtk_container_add( GTK_CONTAINER(notebookpage), widget_entity );
	}

	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ) );
	gtk_window_set_transient_for( GTK_WINDOW(dialog), window );

	/* Run Dialog and save results */
	result = gtk_dialog_run( GTK_DIALOG(dialog) );
	switch ( result )
	{
		case GTK_RESPONSE_APPLY:
		{
			obj->object->x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_x));
			obj->object->y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_y));
			obj->object->w = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_w));
			obj->object->h = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_h));
			obj->object->layer = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_z));

			Meg_ColorButton_GetColor( GTK_COLOR_BUTTON(button_colour), &obj->object->colour );
			gdk2rgbacolor( &obj->object->colour, &obj->colour8 );

			ObjectAdvance_EntityWidget_Save( widget_entity, obj );
		}
	}
	gtk_widget_destroy( dialog );

	return (result == GTK_RESPONSE_APPLY ? TRUE : FALSE);
}

/********************************
* ObjectAdvance_Sprite
*
*/
gboolean ObjectAdvance_Sprite( MokoiMapObject * obj, GtkWindow * window )
{
	GtkWidget * dialog, * label;
	GtkWidget * check_mirror, * spin_rot, * button_colour, * spin_x, * spin_y, * spin_z, * spin_w, * spin_h;
	GtkWidget * entity_parent;
	gint map_width = 320, map_height = 240;
	gint result = 0;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create( mokoiUI_ObjectSprite, __func__, __LINE__ );

	dialog = GET_WIDGET( ui, "object_advance");
	label = GET_WIDGET( ui, "alchera-label");
	check_mirror = GET_WIDGET( ui, "check_mirror");
	spin_rot = GET_WIDGET( ui, "spin_rot");
	button_colour = GET_WIDGET( ui, "button_colour");
	spin_x = GET_WIDGET( ui, "spin_x");
	spin_y = GET_WIDGET( ui, "spin_y");
	spin_z = GET_WIDGET( ui, "spin_z");
	spin_w = GET_WIDGET( ui, "spin_w");
	spin_h = GET_WIDGET( ui, "spin_h");

	entity_parent = GET_WIDGET( ui, "notebook1" );

	/* Get Map Dimension */
	if ( obj->parent )
	{
		map_width = obj->parent->width;
		map_height = obj->parent->height;
	}

	/* Set Default Value */
	Meg_Misc_SetLabel_Print( label, "<b>Edit Sprite</b>\n%s (%d)", obj->name, obj->object->id );

	gtk_spin_button_set_range( GTK_SPIN_BUTTON(spin_x), -200.0, (gdouble)map_width );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_x), obj->object->x );

	gtk_spin_button_set_range( GTK_SPIN_BUTTON(spin_y), -200.0, (gdouble)map_height);
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_y), obj->object->y );

	gtk_spin_button_set_range( GTK_SPIN_BUTTON(spin_z), 0.0, 6.0 );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_z), (gdouble)obj->object->layer);

	gtk_spin_button_set_increments( GTK_SPIN_BUTTON(spin_w), (gdouble)obj->object->tw, (gdouble)obj->object->tw );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_w), obj->object->w);

	gtk_spin_button_set_increments( GTK_SPIN_BUTTON(spin_h), (gdouble)obj->object->th, (gdouble)obj->object->th );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_h), obj->object->h );

	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_rot), (gdouble)obj->object->rotate*90 );

	Meg_ColorButton_SetColor( GTK_COLOR_BUTTON(button_colour), &obj->object->colour );

	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(check_mirror), obj->object->flip );


	/* Add Entity Widget to notebook */
	GtkWidget * notebookpage = NULL;
	GtkWidget * widget_entity = ObjectAdvance_EntityWidget_New( obj );
	notebookpage = gtk_notebook_get_nth_page( GTK_NOTEBOOK(entity_parent), 1 );
	if ( notebookpage )
	{
		gtk_container_add( GTK_CONTAINER(notebookpage), widget_entity );
	}

	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ) );
	gtk_window_set_transient_for( GTK_WINDOW(dialog), window );

	result = gtk_dialog_run( GTK_DIALOG(dialog) );
	switch ( result )
	{
		case GTK_RESPONSE_APPLY:
		{
			obj->object->x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_x));
			obj->object->y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_y));
			obj->object->w = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_w));
			obj->object->h = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_h));
			obj->object->layer = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_z));

			obj->object->flip = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_mirror));
			obj->object->rotate = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_rot)) /90;

			Meg_ColorButton_GetColor( GTK_COLOR_BUTTON(button_colour), &obj->object->colour );
			gdk2rgbacolor( &obj->object->colour, &obj->colour8 );

			ObjectAdvance_EntityWidget_Save( widget_entity, obj );
		}
	}

	gtk_widget_destroy( dialog );

	return (result == GTK_RESPONSE_APPLY ? TRUE : FALSE);
}


/********************************
* ObjectAdvance_Line
*
*/
gboolean ObjectAdvance_Line( MokoiMapObject * obj, GtkWindow * window )
{
	GtkWidget * dialog, * label;
	GtkWidget * button_colour, * spin_x, * spin_y, * spin_z, * spin_w, * spin_h;
	GtkWidget * entity_parent;
	gint map_width = 320, map_height = 240;
	gint result = 0;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create( mokoiUI_ObjectLine,  __func__, __LINE__ );

	dialog = GET_WIDGET( ui, "object_advance");
	label = GET_WIDGET( ui, "alchera-label");
	button_colour = GET_WIDGET( ui, "button_colour");
	spin_x = GET_WIDGET( ui, "spin_x");
	spin_y = GET_WIDGET( ui, "spin_y");
	spin_z = GET_WIDGET( ui, "spin_z");
	spin_w = GET_WIDGET( ui, "spin_w");
	spin_h = GET_WIDGET( ui, "spin_h");

	entity_parent = GET_WIDGET( ui, "notebook1" );

	/* Get Map Dimension */
	if ( obj->parent )
	{
		map_width = obj->parent->width;
		map_height = obj->parent->height;
	}

	/* Set Default Value */
	Meg_Misc_SetLabel_Print( label, "<b>Edit Line</b>\n%s (%d)", obj->name, obj->object->id );

	gtk_spin_button_set_range( GTK_SPIN_BUTTON(spin_x), -200.0, (gdouble)map_width );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_x), obj->object->x );

	gtk_spin_button_set_range( GTK_SPIN_BUTTON(spin_y), -200.0, (gdouble)map_height);
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_y), obj->object->y );

	gtk_spin_button_set_range( GTK_SPIN_BUTTON(spin_z), 0.0, 6.0 );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_z), (gdouble)obj->object->layer);

	gtk_spin_button_set_increments( GTK_SPIN_BUTTON(spin_w), (gdouble)obj->object->tw, (gdouble)obj->object->tw );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_w), obj->object->w);

	gtk_spin_button_set_increments( GTK_SPIN_BUTTON(spin_h), (gdouble)obj->object->th, (gdouble)obj->object->th );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_h), obj->object->h );

	Meg_ColorButton_SetColor( GTK_COLOR_BUTTON(button_colour), &obj->object->colour );

	/* Add Entity Widget to notebook */
	GtkWidget * notebookpage = NULL;
	GtkWidget * widget_entity = ObjectAdvance_EntityWidget_New( obj );
	notebookpage = gtk_notebook_get_nth_page( GTK_NOTEBOOK(entity_parent), 1 );
	if ( notebookpage )
	{
		gtk_container_add( GTK_CONTAINER(notebookpage), widget_entity );
	}

	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ) );
	gtk_window_set_transient_for( GTK_WINDOW(dialog), window );

	result = gtk_dialog_run( GTK_DIALOG(dialog) );
	switch ( result )
	{
		case GTK_RESPONSE_APPLY:
		{
			obj->object->x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_x));
			obj->object->y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_y));
			obj->object->w = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_w));
			obj->object->h = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_h));
			obj->object->layer = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_z));

			Meg_ColorButton_GetColor( GTK_COLOR_BUTTON(button_colour), &obj->object->colour );
			gdk2rgbacolor( &obj->object->colour, &obj->colour8 );

			ObjectAdvance_EntityWidget_Save( widget_entity, obj );
		}
	}


	gtk_widget_destroy( dialog );

	return (result == GTK_RESPONSE_APPLY ? TRUE : FALSE);
}

/********************************
* ObjectAdvance_Text
*
*/
gboolean ObjectAdvance_Text( MokoiMapObject * obj, GtkWindow * window )
{
	GtkWidget * dialog, * label;
	GtkWidget * edit_text, * spin_string, * button_colour, * spin_x, * spin_y, * spin_z;
	GtkWidget * entity_parent;
	gint map_width = 320, map_height = 240;
	gint result = 0;

	gint string_number_value = 0;
	gdouble strings_total = 0.00;

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create( mokoiUI_ObjectText, __func__, __LINE__ );

	dialog = GET_WIDGET( ui, "object_advance");
	label = GET_WIDGET( ui, "alchera-label");
	edit_text = GET_WIDGET( ui, "edit_text");
	button_colour = GET_WIDGET( ui, "button_colour");
	spin_string = GET_WIDGET( ui, "spin_string");
	spin_x = GET_WIDGET( ui, "spin_x");
	spin_y = GET_WIDGET( ui, "spin_y");
	spin_z = GET_WIDGET( ui, "spin_z");
	entity_parent = GET_WIDGET( ui, "notebook1" );

	g_signal_connect( spin_string, "value-changed", G_CALLBACK(ObjectAdvance_Text_Spin), (gpointer)edit_text );

	/* Get Map Dimension */
	if ( obj->parent )
	{
		map_width = obj->parent->width;
		map_height = obj->parent->height;
	}


	string_number_value = RuntimeSetting_GetValue( obj->settings, "number" );
	strings_total = (gdouble)( Language_Size() ) - 1.0;



	/* Set Default Value */
	g_object_set_data_full ( G_OBJECT(edit_text), "old-string", (gchar*)obj->object->data, NULL ); // Keeps a copy of Original String.

	Meg_Misc_SetLabel_Print( label, "<b>Edit Text</b>\n%s (%d)", obj->name, obj->object->id );

	gtk_spin_button_set_range(GTK_SPIN_BUTTON(spin_x), -200.0, (gdouble)map_width);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_x), obj->object->x);

	gtk_spin_button_set_range(GTK_SPIN_BUTTON(spin_y), -200.0, (gdouble)map_height);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_y), obj->object->y);

	gtk_spin_button_set_range(GTK_SPIN_BUTTON(spin_z), 0.0, 6.0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_z), (gdouble)obj->object->layer);

	Meg_ColorButton_SetColor( GTK_COLOR_BUTTON(button_colour), &obj->object->colour );


	gtk_spin_button_set_range( GTK_SPIN_BUTTON(spin_string), -1.0, strings_total );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin_string), (gdouble)string_number_value );

	if ( string_number_value == -1)
	{
		gtk_entry_set_text( GTK_ENTRY(edit_text), obj->object->data );
	}

	/* Add Entity Widget to notebook */
	GtkWidget * notebookpage = NULL;
	GtkWidget * widget_entity = ObjectAdvance_EntityWidget_New( obj );
	notebookpage = gtk_notebook_get_nth_page( GTK_NOTEBOOK(entity_parent), 1 );
	if ( notebookpage )
	{
		gtk_container_add( GTK_CONTAINER(notebookpage), widget_entity );
	}

	gtk_widget_show_all( gtk_dialog_get_content_area( GTK_DIALOG(dialog) ));
	gtk_window_set_transient_for( GTK_WINDOW(dialog), window );

	result = gtk_dialog_run( GTK_DIALOG(dialog) );
	switch ( result )
	{
		case GTK_RESPONSE_APPLY:
		{
			const gchar * str_text;
			gint str_value = -1;

			str_value = gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON(spin_string) );
			str_text = gtk_entry_get_text( GTK_ENTRY(edit_text) );

			ObjectAdvance_EntityWidget_Save( widget_entity, obj );

			g_object_set_data_full( G_OBJECT(edit_text), "old-string", (gchar*)obj->object->data, NULL );
			RuntimeSetting_UpdateValue( obj->settings, "number", str_value );

			if ( str_value != -1 )
			{
				REPLACE_STRING( obj->name, g_strdup_printf("String %d", str_value) );
				REPLACE_STRING( obj->object->data, g_strdup_printf("String %d", str_value) );
			}
			else
			{
				if ( str_text && g_utf8_strlen(str_text, -1) )
				{
					REPLACE_STRING( obj->name, g_strdup(str_text) );
					REPLACE_STRING( obj->object->data, g_strdup(str_text) );
				}
				else
				{
					REPLACE_STRING( obj->name, g_strdup("") );
					REPLACE_STRING( obj->object->data, g_strdup("") );
				}
			}

			obj->object->x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_x));
			obj->object->y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_y));
			obj->object->layer = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_z));

			Meg_ColorButton_GetColor( GTK_COLOR_BUTTON(button_colour), &obj->object->colour );
			gdk2rgbacolor( &obj->object->colour, &obj->colour8 );


		}
	}
	gtk_widget_destroy( dialog );

	return (result == GTK_RESPONSE_APPLY ? TRUE : FALSE);
}

/********************************
* ObjectAdvance_Sprite
*
*/
gboolean ObjectAdvance_File( MokoiMapObject * obj, GtkWindow * window )
{
	return FALSE;
}

/********************************
* ObjectAdvance_Polygon
*
*/
gboolean ObjectAdvance_Polygon( MokoiMapObject * obj, GtkWindow * window )
{
	return FALSE;
}


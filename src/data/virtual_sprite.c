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
#include "widgets/widget_map.h"
#include "gtk_compat.h"

/* External Functions */
gboolean Map_ParseXML( MapInfo * map_info, gchar * content );
void Meg_MapEdit_GroupChangedNext( GtkWidget * widget, GtkComboBox * combo_group );
void Meg_MapEdit_GroupChangedPrev( GtkWidget * widget, GtkComboBox * combo_group );
void Meg_MapEdit_GroupChanged( GtkComboBox * widget, GtkWidget * iconview );
void Meg_MapEdit_ObjectSelected( GtkIconView * icon_view, GtkTreePath *path, GtkWidget * map_widget );
void Meg_MapEdit_ObjectDrop( GtkWidget * widget, GdkDragContext * drag_context, GtkSelectionData * data, guint info, guint time, gpointer user_data );
gboolean Meg_MapEdit_ObjectSelectedKey( GtkWidget * layout, GdkEventKey * event, GtkIconView * icon_view );
gchar * MapObject_TypeName( gchar type );

gboolean VirtualSprite_InsertIntoSpriteSheet(gchar * name );

/* Global Variables */
extern const GtkTargetEntry alchera_map_drop_target;

/* Local Variables */

MokoiSheet virtual_spritesheet = {
	NULL,
	NULL,
	1
};

/* UI */
#include "ui/virtual_sprite_dialog.gui.h"
const gchar * ui_data_virtual_sprite_dialog = GUIVIRTUAL_SPRITE_DIALOG;

/********************************
* VirtualSprite_LoadXML
*
*/
MapInfo * VirtualSprite_LoadXML( gchar * id )
{
	MapInfo * map_info = g_new0(MapInfo, 1);
	VirtualObjectList * object_list = NULL;
	gchar * file_path = NULL, * content = NULL;

	map_info->file_type = 1;
	map_info->name = g_strdup(id);



	file_path = g_strdup_printf( "/sprites/virtual/%s.xml", id  );

	if ( Meg_file_get_contents( file_path, &content, NULL, NULL ) )
	{

		map_info->data = object_list = g_new0(VirtualObjectList, 1);


		Map_ParseXML( map_info, content );

		/* Update DisplayList */
		VirtualObject * virtual_object = NULL;
		GList * scan = g_list_first( object_list->objects );
		while ( scan )
		{
			virtual_object = (VirtualObject *)scan->data;

			virtual_object->object->tw = virtual_object->object->th = 1;
			virtual_object->object->type = DT_NONE;
			virtual_object->object->timeout = FALSE;
			switch ( virtual_object->type )
			{
				case 's':
					VirtualObject_UpdateSprite( virtual_object );
					break;
				case 't':
					virtual_object->object->type = DT_TEXT;
					virtual_object->object->data = (gpointer)g_strdup( virtual_object->name );
					break;
				case 'l':
					virtual_object->object->type = DT_LINE;
					break;
				case 'r':
					virtual_object->object->type = DT_RECTANGLE;
					break;
				case 'p':
					virtual_object->object->type = DT_POLYGON;
					virtual_object->object->resizable = FALSE;
					break;
				case 'c':
					virtual_object->object->type = DT_CIRCLE;
					break;
				default:
					virtual_object->object->type = DT_OTHER;
					break;
			}

			if ( virtual_object->object->image )
			{
				virtual_object->object->type = DT_IMAGE;
				virtual_object->object->tw = gdk_pixbuf_get_width( virtual_object->object->image );
				virtual_object->object->th = gdk_pixbuf_get_height( virtual_object->object->image );
			}

			if ( virtual_object->object->type == DT_IMAGE || virtual_object->object->type == DT_ANIMATION )
			{
				if ( virtual_object->object->w < 1.00 )
				{
					virtual_object->object->w = virtual_object->object->tw;
				}
				if ( virtual_object->object->h < 1.00 )
				{
					virtual_object->object->h = virtual_object->object->th;
				}
			}

			map_info->width = MAX( map_info->width, virtual_object->object->x + virtual_object->object->w );
			map_info->height = MAX( map_info->height, virtual_object->object->y + virtual_object->object->h );

			virtual_object->object->active = virtual_object->object->tile = FALSE;

			map_info->displayList = g_list_append(map_info->displayList, virtual_object->object);
			scan = g_list_next( scan );
		}

	}

	g_free(file_path);
	g_free(content);
	return map_info;
}

/********************************
* VirtualSprite_BuildPixbuf
*
*/
GdkPixbuf *  VirtualSprite_BuildPixbuf( gchar * id )
{
	GdkPixbuf * image = NULL;
	MapInfo * map_info = NULL;

	map_info = VirtualSprite_LoadXML( id );

	/* Save Image of Map */
	cairo_surface_t * cst;

	cst = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, map_info->width, map_info->height );

	if ( cst )
	{
		cairo_t *  cr = cairo_create( cst );

		GList * scan = scan = g_list_first( map_info->displayList );
		while ( scan )
		{
			DisplayObject * obj = (DisplayObject *)scan->data;
			Alchera_DisplayObject_DrawForeach( obj, cr );
			scan = g_list_next( scan );
		}
		cairo_destroy( cr );

		image = gdk_pixbuf_get_from_surface( cst, 0, 0, map_info->width, map_info->height );
		cairo_surface_write_to_png( cst, "test.png" );
	}

	Map_Free( map_info, FALSE );

	return image;
}

/********************************
* VirtualSprite_SaveXML
*
*/
gboolean VirtualSprite_SaveXML( MapInfo * map_info )
{
	g_return_val_if_fail( map_info != NULL, FALSE );
	g_return_val_if_fail( map_info->data != NULL, FALSE );

	VirtualObjectList * virtual_object_list = NULL;
	GList * scan = NULL;
	GString * map_string = g_string_new("<map xmlns=\"http://mokoi.info/projects/mokoi\">\n");


	virtual_object_list = (VirtualObjectList*) map_info->data;

	/* Objects */
	scan = g_list_first( virtual_object_list->objects );
	while ( scan )
	{
		gint x,y,w,h,z,l;

		VirtualObject * object = NULL;
		DisplayObject * display_object = NULL;


		object = (VirtualObject *)scan->data;
		g_return_val_if_fail( object != NULL, FALSE );

		display_object = (DisplayObject *)object->object;
		g_return_val_if_fail( display_object != NULL, FALSE );

		if ( display_object->type != DT_DELETE )
		{
			x = (gint)display_object->x;
			y = (gint)display_object->y;
			l = display_object->layer;
			z = l * 1000;

			if ( display_object->type == DT_LINE )
			{
				// Line Object use w/h as 2nd x/y
				w = (gint)(display_object->w + display_object->x);
				h = (gint)(display_object->h + display_object->y);
			}
			else
			{
				w = (gint)display_object->w;
				h = (gint)display_object->h;
			}
			if ( object->ident )
			{
				g_string_append_printf( map_string, "<object value=\"%s\" type=\"%s\" id=\"%s\">\n", object->name, MapObject_TypeName(object->type), object->ident );
			}
			else
			{
				g_string_append_printf( map_string, "<object value=\"%s\" type=\"%s\">\n", object->name, MapObject_TypeName(object->type) );
			}
			g_string_append_printf( map_string, "\t<position x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\" z=\"%d\" l=\"%d\" r=\"%d\" f=\"%d\"/>\n", x, y, w, h, z, l, display_object->rotate*90, display_object->flip);

			if ( display_object->type == DT_POLYGON )
			{
				GSList * point_scan = NULL;
				DisplayObject * point_object = NULL;
				g_string_append_printf( map_string, "\t<option points=\"%u\"/>\n", g_slist_length(display_object->shape) );

				point_scan = display_object->shape;
				while ( point_scan )
				{
					point_object = (DisplayObject *)point_scan->data;
					g_string_append_printf( map_string, "\t<point x=\"%d\" y=\"%d\"/>\n", (gint)point_object->x, (gint)point_object->y );
					point_scan = g_slist_next(point_scan);
				}
			}

			g_string_append_printf( map_string, "</object>\n");
		}

		scan = g_list_next(scan);
	}
	g_string_append_printf( map_string, "</map>");

	/* Write map file */
	gchar * file_path = g_strdup_printf( "/sprites/virtual/%s.xml", map_info->name );
	Meg_file_set_contents( file_path, map_string->str, -1, NULL);
	g_free( file_path );

	VirtualSprite_InsertIntoSpriteSheet( map_info->name );

	return TRUE;
}

/********************************
* VirtualSprite_GetInfo
*
*/
MapInfo * VirtualSprite_GetInfo( gchar * id )
{
	MapInfo * map_info = NULL;

	if ( !g_ascii_strcasecmp( id, "Add New" ) )
	{
		map_info = g_new0(MapInfo, 1);
		map_info->name = g_strdup( "NewVirtual" );
		map_info->width = 64;
		map_info->height = 64;
		map_info->file_type = 1;

		map_info->data = g_new0(VirtualObjectList, 1);
	}
	else
	{
		map_info = VirtualSprite_LoadXML( id );

	}



	/*
	typedef struct {
		DisplayObject * selected;
		GList * displayList;
		GHashTable * settings;
		gchar * name;
		guint32 visible;
		guint width, height;
		GdkRGBA colour;
		gpointer data;
	} MapInfo;
	*/
	return map_info;
}

/********************************
* VirtualSprite_Save
*
*/
void VirtualSprite_Save( GtkButton * button, MapInfo * map_info )
{
	VirtualSprite_SaveXML( map_info );
}

/********************************
* VirtualSprite_UpdateName
*
*/
void VirtualSprite_UpdateName(GtkLabel * label, MapInfo * map_info)
{
	g_return_if_fail( map_info );


	gchar * marked_up_text = NULL;

	marked_up_text = g_markup_printf_escaped( "%s <a href=\"\">rename</a>", map_info->name );
	gtk_label_set_markup( label , marked_up_text );
	g_free( marked_up_text );


}

/********************************
* VirtualSprite_UpdateNameDialog
* File Coping with error handling
*/
void VirtualSprite_UpdateNameDialog(GtkLabel *label, gchar * uri,  MapInfo * map_info)
{
	g_return_if_fail( map_info );


	GtkWidget * dialog = gtk_dialog_new_with_buttons( "Rename Virtual Sprite", Meg_Misc_ParentWindow(GTK_WIDGET(label)), GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL );
	GtkWidget * entry = gtk_entry_new();

	gtk_entry_set_max_length( GTK_ENTRY(entry), 64 );

	gtk_box_pack_start( GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), entry, FALSE, FALSE, 1);
	gtk_widget_show_all( gtk_dialog_get_content_area(GTK_DIALOG(dialog)) );

	gint result = gtk_dialog_run( GTK_DIALOG (dialog) );
	switch (result)
	{
		case GTK_RESPONSE_ACCEPT:
		{
			REPLACE_STRING_DUPE( map_info->name, gtk_entry_get_text( GTK_ENTRY(entry) ) );
			VirtualSprite_UpdateName(label, map_info);
			break;
		}
		case GTK_RESPONSE_REJECT:
		{

			break;
		}
		default:

			break;
	}
	gtk_widget_destroy( dialog );



}

/********************************
* VirtualSprite_DialogDisplay
*
*/
gboolean VirtualSprite_DialogDisplay( gchar * id )
{
	MapInfo * map_info = VirtualSprite_GetInfo( id );

	g_return_val_if_fail( map_info, FALSE );

	/* UI */
	GtkBuilder * ui = Meg_Builder_Create( ui_data_virtual_sprite_dialog, __func__, __LINE__ );
	g_return_val_if_fail( ui, FALSE );

	/* Widgets */
	GtkWidget * window, * editor_viewport, * widget_map, * label_name, * iconview_objects, * combo_groups;
	GtkListStore * store_current_objects, * store_groups;

	window = GET_WIDGET( ui, "window" );
	editor_viewport = GET_WIDGET( ui, "editor_viewport");
	label_name = GET_WIDGET( ui, "label_name");

	iconview_objects = GET_WIDGET( ui, "iconview_objects" );
	combo_groups = GET_WIDGET( ui, "combo_groups" );
	store_current_objects = GET_LISTSTORE( ui, "store_current_objects" );
	store_groups = GET_LISTSTORE( ui, "store_groups" );

	if ( store_groups  )
	{		/* Spritesheet loading */
		char ** files = PHYSFS_enumerateFiles("/sprites/virtual/");
		char ** current;

		for (current = files; *current != NULL; current++)
		{
			if ( g_str_has_suffix( *current, ".xml" ) )
			{
				gchar * file_name = g_strndup(*current, g_utf8_strlen(*current, -1) - 4 ); // Strip .xml
				VirtualSprite_InsertIntoSpriteSheet( file_name );
				g_free(file_name);
			}
		}
		PHYSFS_freeList(files);

		gtk_list_store_clear( store_groups );
		AL_Object_Groups( store_groups, FALSE );
	}

	/* Settings */
	VirtualSprite_UpdateName( GTK_LABEL(label_name), map_info );
	gtk_icon_view_enable_model_drag_source( GTK_ICON_VIEW(iconview_objects), GDK_BUTTON1_MASK, &alchera_map_drop_target, 1, GDK_ACTION_COPY );


	/* Alchera Map widget */
	widget_map = gtk_alchera_map_new( map_info );
	if ( GTK_IS_ALCHERA_MAP(widget_map) )
	{
		gtk_alchera_map_set_support_widget( GTK_ALCHERA_MAP(widget_map), store_current_objects, NULL, NULL );
		gtk_widget_set_size_request( GTK_WIDGET(editor_viewport), 320, 240 );
		gtk_widget_show_all( widget_map );
	}
	gtk_container_add( GTK_CONTAINER(editor_viewport), widget_map );

	/* Signals */
	SET_OBJECT_SIGNAL( ui, "button_save", "clicked", G_CALLBACK(VirtualSprite_Save), map_info );
	SET_OBJECT_SIGNAL_SWAP( ui, "button_close", "clicked", G_CALLBACK(gtk_widget_destroy), window );
	SET_OBJECT_SIGNAL_SWAP( ui, "window", "close", G_CALLBACK(gtk_widget_destroy), window );

	SET_OBJECT_SIGNAL( ui, "label_name", "activate-link", G_CALLBACK(VirtualSprite_UpdateNameDialog), map_info );

	g_signal_connect( G_OBJECT(combo_groups), "changed", G_CALLBACK(Meg_MapEdit_GroupChanged), iconview_objects );

	SET_OBJECT_SIGNAL( ui, "button_nextgroup", "clicked", G_CALLBACK(Meg_MapEdit_GroupChangedNext), combo_groups );
	SET_OBJECT_SIGNAL( ui, "button_prevgroup", "clicked", G_CALLBACK(Meg_MapEdit_GroupChangedPrev), combo_groups );

	g_signal_connect( G_OBJECT(iconview_objects), "item-activated", G_CALLBACK(Meg_MapEdit_ObjectSelected), widget_map );
	g_signal_connect( G_OBJECT(iconview_objects), "drag-data-get", G_CALLBACK(Meg_MapEdit_ObjectDrop), widget_map );

	gtk_dialog_run( GTK_DIALOG(window) );


	return TRUE;
}

/********************************
* VirtualSprite_FindCompare
*
*/
gint VirtualSprite_FindCompare(gconstpointer a, gconstpointer b)
{
	SheetObject * list_object = (SheetObject *)a;
	gchar * file_name = (gchar*)b;

	return g_ascii_strcasecmp( list_object->name, file_name );
}

/********************************
* VirtualSprite_InsertIntoSpriteSheet
*
*/
gboolean VirtualSprite_InsertIntoSpriteSheet( gchar * name )
{
	GSList * scan = NULL;

	scan = g_slist_find_custom( virtual_spritesheet.children, name, VirtualSprite_FindCompare );

	if ( !scan )
	{
		MokoiSprite * sprite = g_new0(MokoiSprite, 1);

		sprite->detail = g_new0(SheetObject, 1);
		sprite->mask = g_new0(MokoiMask, 1);


		sprite->detail->name = g_strdup(name);
		sprite->parent = g_strdup("Virtual");
		sprite->ident = g_strdup_printf( "Virtual:%s", sprite->detail->name );
		sprite->visible = TRUE;
		sprite->image_loaded = FALSE;



		virtual_spritesheet.children = g_slist_append( virtual_spritesheet.children, (gpointer)sprite );

		VirtualSprite_BuildPixbuf( name );

		return TRUE;
	}

	return FALSE;

}


/********************************
* VirtualSprite_UpdateSpriteInfo
*
*/
gboolean VirtualSprite_UpdateSpriteInfo( gchar * name )
{
	GSList * scan = NULL;

	scan = g_slist_find_custom( virtual_spritesheet.detail->children, name, VirtualSprite_FindCompare );

	if ( scan )
	{
		MokoiSprite * sprite = (MokoiSprite*)scan->data;

		sprite->image = VirtualSprite_BuildPixbuf( name );
		sprite->image_loaded = TRUE;

		return TRUE;
	}

	return FALSE;

}

/********************************
* VirtualSprite_GetSpritesheet
*
*/
Spritesheet * VirtualSprite_GetSpritesheet()
{
	if ( virtual_spritesheet.detail == NULL)
	{
		virtual_spritesheet.detail = g_new0(Spritesheet, 1);
		virtual_spritesheet.detail->file = g_strdup("Virtual");
		virtual_spritesheet.detail->image_loaded = FALSE;
		virtual_spritesheet.detail->visible = TRUE;
		virtual_spritesheet.detail->children = NULL;
		virtual_spritesheet.detail->image = NULL;
		virtual_spritesheet.ref++;

		/* Spritesheet loading */
		char ** files = PHYSFS_enumerateFiles("/sprites/virtual/");
		char ** current;

		for (current = files; *current != NULL; current++)
		{
			if ( g_str_has_suffix( *current, ".xml" ) )
			{
				gchar * file_name = g_strndup(*current, g_utf8_strlen(*current, -1) - 4 ); // Strip .xml
				VirtualSprite_InsertIntoSpriteSheet( file_name );
				g_free(file_name);
			}
		}
		PHYSFS_freeList(files);

		VirtualSprite_InsertIntoSpriteSheet( "Add New" );

	}

	return  virtual_spritesheet.detail;
}



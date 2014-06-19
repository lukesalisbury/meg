#ifndef _ALCHERA_LOADER_H_
#define _ALCHERA_LOADER_H_

	#include <gtk/gtk.h>

	/* info + plugin stuff */
	gchar * AL_CreateProject(const gchar * title, const gchar *author);
	gchar * AL_LoadProject( const gchar * path );
	gboolean AL_HasFeature( gint feature );
	gchar * AL_ProjectPath( );
	gchar * AL_ResourcePath(gchar * file , gchar *directory);
	gboolean AL_CloseProject();
	GAsyncQueue *AL_PrecheckFiles();


	/* Project Handling */
	gboolean MegProject_Save();
	gboolean MegProject_Compile();
	gboolean MegProject_Play();
	gchar * MegProject_Title();

	gboolean Meg_Dialog_Export();



	/* Settings */
	void AL_Settings_Refresh();
	void AL_Settings_Clear();
	void AL_Setting_Widget( GtkBuilder * ui );
	gchar * AL_Setting_GetString( gchar * key );
	gint AL_Setting_GetNumber( gchar * key );
	gint AL_Setting_GetDefaultNumber( gchar * key, gint default_value );
	gdouble AL_Setting_GetDouble( gchar * key );
	gboolean AL_Setting_Remove(  gchar * key );
	void AL_Setting_SetString( gchar * key, gchar * value );

	/* Image Handling */
	GdkPixbuf * AL_GetSprite( gchar * name );
	GdkPixbuf * AL_GetImage(gchar * name , gint64 * file_size);

	/* Overview Function */
	guint AL_MapSection_Open( gchar * file, GtkWindow * window );
	gboolean AL_MapSection_Save( guint id, GtkWindow * window );
	gboolean AL_MapSection_Close( guint id, GtkWindow * window );
	guint8 AL_MapSection_Type( guint id, guint8 *type, GtkWindow * window );
	gchar * AL_MapSection_Pressed( guint id, GtkWindow * window, guint x, guint y, guint button, GdkEventType event );
	void AL_MapSection_SizeRequest( guint id, gint * width, gint * height  );
	gboolean AL_MapSection_Redraw( guint id, cairo_t * cr, gint width, gint height );

	/* Sections Function */
	void AL_MapSections_Files( GtkListStore * store );/* G_TYPE_STRING */
	gboolean AL_MapSections_Add( const gchar * name );
	gboolean AL_MapSections_Remove( gchar * name );

	/* Maps */
	gboolean AL_Map_Add( const gchar * name, const gchar * oldpath );
	gboolean AL_Map_Remove( gchar * name );
	void AL_Map_Files( GtkListStore * store );

	gboolean AL_Map_Open(MapInfo * map_info, GtkWindow * window );
	gboolean AL_Map_Save( MapInfo * map_info, GtkWindow * window );
	gboolean AL_Map_Close( MapInfo * map_info, GtkWindow * window );
	gboolean AL_Map_Options(MapInfo * map_info, GtkWindow * window );
	gboolean AL_Map_GraphicsSwitchDialog( MapInfo * map_info, GtkWindow * window );


	void AL_Map_Layers( GtkListStore * store );

	/* Map Display Objects */
	GList * AL_Object_List( MapInfo * map );
	DisplayObject * AL_Object_Add( MapInfo * map_info, gchar * indent, gdouble x, gdouble y, gdouble w, gdouble h, gint z );
	DisplayObject * AL_Object_Get( MapInfo * map_info, gint id);
	gboolean AL_Object_Remove( MapInfo * map_info, gint id);
	gboolean AL_Object_Update( MapInfo * map_info, gint id, gdouble x, gdouble y, gdouble w, gdouble h, gint z );
	gboolean AL_Object_Advance( MapInfo * map_info, gint id, GtkWindow * window );

	void AL_Object_Groups( GtkListStore * list, gboolean display_virtual );
	void AL_Object_Available( GtkListStore * list, gchar * parent ); /* Icon, Ident, Auto Advance, Type, Width, Height */

	/* Sheet/Sprite Management */
	void AL_Sheet_List( GtkListStore * list, gboolean add_all );
	gboolean AL_Sheet_Add( gchar * filename );
	gboolean AL_Sheet_Remove( gchar * filename );
	Spritesheet * AL_Sheet_Get( gchar * file );
	gboolean AL_Sheet_Save( Spritesheet * spritesheet );

	gboolean AL_Sprite_Advance( Spritesheet * spritesheet, gchar * id );
	void AL_Sprite_Add( Spritesheet * spritesheet, GdkRectangle * new_sprite );
	void AL_Sprite_Remove( Spritesheet * spritesheet, gchar * id );
	gboolean AL_Sprite_Resize( Spritesheet * spritesheet, gchar * id, GdkRectangle * area );

	/* Text Editor */
	GSList * AL_Editor_Database( gchar * filename ); /* GSList<EditorDatabaseListing *> */

	/* Language */
	void AL_Language_Types( GtkListStore * list );
	gboolean AL_Language_Add( gchar * file );
	gboolean AL_Language_Remove( gchar * file );
	void AL_String_List( gchar * file, GtkListStore * list);
	gchar * AL_String_Get( gchar * file, guint id );
	gboolean AL_String_Set( gchar * file, guint id, gchar * string );
	gboolean AL_String_Add( GtkListStore * list );
	gboolean AL_String_Remove( guint id, GtkListStore * list );

	/**/
	void Meg_Audio_Init();
	void Meg_EntityList_Init();

#endif /* _ALCHERA_LOADER_H_ */

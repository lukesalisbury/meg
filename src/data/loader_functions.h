/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <gtk/gtk.h>
#include <stdio.h>
#include <zlib.h>

/* Project */
gchar * Project_CleanTitle( const gchar * title );
gboolean Project_ValidDir();
gchar * Project_Directory( gchar * path );
gchar * Project_Directory2( gchar * path,  gchar * path2 );
gchar * Project_File( gchar * directory, gchar * file );
void Project_LoadSpritesheet();
void Project_Save();

/* Misc Functions */

void ToggleButton_Event_WidgetSensitive( GtkToggleButton * button, GtkWidget * widget );
gchar * TextView_GetText( const GtkWidget * text_view );

/* Packages */
void Package_Import( const gchar * file, GtkWidget * progress_widget, GtkTreeModel * model, GtkTreeIter iter, gboolean initial );
void Package_Download( const gchar * url, const gchar * package_title, const gchar * package_version, GtkWidget * progress_widget, GtkTreeModel * model, GtkTreeIter iter, gboolean initial );
gboolean Package_Infomation( const gchar * file, gchar ** package_name, gchar ** package_path, gchar ** package_version, gchar ** package_description, guint8 *package_type );
gboolean Package_ExportList(const gchar * package_path, const gchar * title, const gchar * version, const gchar * description, const gchar * authors, const gchar * terms, const gchar * config, GSList * files );
gboolean Package_ExportFolders(const gchar * package_path, const gchar * title, const gchar * version, const gchar * description, const gchar * authors, const gchar * terms, const gchar * config, GSList * folders);

/* Updates Parser */
void Updates_Request( GtkListStore * store );
gboolean Updates_Download( GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data );

/* File Function */
void file_write_short( FILE * fd, guint16 value );
void file_write_long( FILE * fd, guint32 value );
void file_write_byte( FILE * fd, guint8 value );
guint32 file_read_long( FILE * fd );
guint16 file_read_short( FILE * fd );
guint8 file_read_byte( FILE * fd );

guint file_get_size( gchar * file );
time_t file_get_date( gchar * file );


void rgba2gdkcolor( rgbaColour * rgba, GdkRGBA * gdk );
void gdk2rgbacolor( GdkRGBA * gdk, rgbaColour * rgba );

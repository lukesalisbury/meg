/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef _MISC_FUNCTIONS_H_
#define _MISC_FUNCTIONS_H_

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "physfs/physfs.h"

#define MEG_COMBOFILE_NONE "(None)"

gulong Meg_FileCRC( const gchar * filename );
gboolean Meg_RunProgram( gchar * program, ...);
GKeyFile * Meg_Preference_Storage();
gchar * Meg_String_ReplaceFileExtension( gchar * string, gchar * orignal_ext, const gchar * new_ext  );
gboolean Meg_FileImport(gchar * source, gchar * dest);

/* Gtk Widget helpers and compat wrapper */
GtkWindow * Meg_Misc_ParentWindow( GtkWidget * widget );
gint Meg_Misc_TreeIterCompare( GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data );

void CellRender_Event_FixToggled( GtkCellRendererToggle *cell, gchar * path_str, GtkListStore * store );
void Meg_Dialog_ButtonClose( GtkButton * button, gpointer user_data );

void Meg_ColorButton_GetColor( GtkColorButton * button, GdkRGBA * rgbacolour );
void Meg_ColorButton_SetColor( GtkColorButton * button, GdkRGBA * rgbacolour );

void Meg_Misc_SetLabel( GtkWidget * label, gchar * section, gchar * text, gchar sep );

void Meg_ComboFile_Scan( GtkWidget * widget, gchar * path, gchar * prefix, gboolean none, guint extension_length );
void Meg_ComboFile_Setup( GtkWidget * combo_box, gchar * path, gboolean force_model );

/* GLIB-like wrapper for physfs */
gint64 Meg_file_size( const gchar  *filename );
gchar * Meg_file_get_path( const gchar * filename );
gboolean Meg_file_test( const gchar  *filename, GFileTest test);
gboolean Meg_file_get_contents(const gchar *filename, gchar ** contents, gsize * length, GError ** error);
gboolean Meg_file_set_contents(const gchar *filename, const gchar *contents, gssize length, GError ** error);
GdkPixbuf * Megdk_pixbuf_new_from_file(const char *filename, GError **error);
void Meg_enumerateFilesCallback( const char * dir, PHYSFS_EnumFilesCallback c, void * data );




/* Extra Physfs functions */
void PHYSFS_writeString( PHYSFS_File * file,  const gchar * message, ... );
void PHYSFS_rename(  const gchar * original_path, const gchar * new_path );
gchar * PHYSFS_buildLocalFilename(  const gchar * internel_filename );

#endif

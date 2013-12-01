/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <gtk/gtk.h>
#include "meg_setting.h"
#include "alchera_functions.h"
#include "ma_loader.h"
#include "widgets/widget_map.h"
#include "ma_dialog.h"
#include "ma_events.h"

/* ma_main.c */
/*
GtkWidget * Meg_Main_AddSection( GtkWidget * section_widget, gchar * section_title, gchar * section_icon );
GtkWindow * Meg_Main_GetWindow();
GtkWidget * Meg_Main_GetWidget();
guint Meg_Main_SetStatus( GString * status_text );
void Meg_Main_PrintStatus(const gchar * format, ...);
gboolean Meg_Main_Init();
*/

/* ma_help */
/*
void Meg_Help_Open( gchar * file );
*/

/* ma_directories.c */
/*
gchar * Meg_Directory();
gchar * Meg_Directory_Global_Share( gchar * path );
gchar * Meg_Directory_Share( gchar * folder );
gchar * Meg_Directory_Data();
void Meg_Directory_Set( gchar * excutable_path );
gchar * Meg_Directory_Document();
gchar * Meg_Directory_Config( );
*/
/* main.c */
/*
gboolean Meg_Main_HelpRequest( GtkWidget * widget, GdkEventKey *event, gpointer user_data );
*/
/* ma_log.c */
/*
void Meg_Log_SetBuffer( GtkTextBuffer * buffer );
void Meg_Log_Print( LogStyle style, const gchar * format, ... );
void Meg_Log_Append( gchar * text, LogStyle style );
void Meg_Error_Print( gchar * function, gint line, const gchar * format, ... );
gboolean Meg_Error_Check( GError * error, gboolean fatal, gchar * function_name );



*/
/* ma_misc.c */
/*
void Meg_ComboText_Setup( GtkWidget * combo_box );
gchar * Meg_ComboText_GetText( GtkComboBox * widget );
void Meg_ComboText_AppendText( GtkComboBox * widget, const gchar * text );
*/

#ifndef _ALCHERA_FUNCTIONS_H_
	#define _ALCHERA_FUNCTIONS_H_
	#include <alchera_types.h>
	#include "ma_misc.h"


	/**/
	void Meg_Log_SetBuffer( GtkTextBuffer * buffer );
	void Meg_Log_Print( LogStyle style, const gchar * format, ... );
	void Meg_Log_Append(LogStyle style , gchar * text);
	void Meg_Log_Enable();

	/**/
	void Meg_Error_Print( const gchar * function, gint line, const gchar * format, ... );
	gboolean Meg_Error_Check( GError * error, gboolean fatal, const gchar * function_name );

	/**/
	void Meg_Main_AddPage( GtkWidget * section_widget, gchar * section_title, gchar * section_icon );
	GtkWidget * Meg_Main_AddSection( GtkWidget * section_widget, gchar * section_title, gchar * section_icon );
	GtkWindow * Meg_Main_GetWindow();
	GtkWidget * Meg_Main_GetWidget();

	gboolean Meg_Main_Init();
	gboolean Meg_Main_HelpRequest( GtkWidget * widget, GdkEventKey *event, gpointer user_data );

	/**/
	guint Meg_Main_SetStatus( GString * status_text );
	void Meg_Main_PrintStatus(const gchar * format, ...);

	/* */
	void Meg_MapEdit_OpenFilename( gchar * file );
	gboolean Meg_MapObject_PushAnimation( DisplayObject * object );

	/* */
	gboolean Meg_DirectoryCopy( const gchar * source, const  gchar * destination );
	gboolean Meg_FileCopy(const gchar *source, const gchar *destination);
	GtkBuilder * Meg_Builder_Create(const gchar * text, const char *funcname, gint line );

	/* */
	gchar * Meg_Directory();
	gchar * Meg_Directory_ProgramShare( gchar * path );
	gchar * Meg_Directory_GlobalShare( gchar * path );
	gchar * Meg_Directory_Share( gchar * folder );
	gchar * Meg_Directory_Data(const gchar *folder );
	gchar * Meg_Directory_DataFile(const gchar *folder, const gchar *file );
	void Meg_Directory_Set( gchar * excutable_path );
	gchar * Meg_Directory_Document();
	gchar * Meg_Directory_Config( );
	gboolean Meg_RunProgram( gchar * program, ...);

	/* */
	GtkWidget * Meg_Editor_New( gchar * language );
	void Meg_Editor_SetLineInfo( GtkWidget * widget, gint line, gint type, gchar * message );
	void Meg_Editor_ClearLineInfo( GtkWidget * widget );
	void Meg_Editor_Redo( GtkWidget * widget );
	void Meg_Editor_Undo( GtkWidget * widget );

	/* */
	void Meg_Help_Open( gchar * file );

	/* */
	gboolean Meg_Web_Enable( );
	gboolean Meg_Web_RetrieveFile( GString * address, gchar * user, gchar * pass, gchar * file_name, guint directory );
	gchar * Meg_Web_RetrieveText( GString * address, gchar * user, gchar * pass );
	gboolean Meg_Web_SendFile( GString * address, gchar * user, gchar * pass, gchar * file_name, guint directory );
	gboolean Meg_Web_SendText( GString * address, gchar * user, gchar * pass, gchar * text );
	GAsyncQueue * Meg_Web_RetrieveQueue( const gchar * url, gchar * user, gchar * pass, const gchar * file_name, guint directory );

	/* */
	void Meg_ComboText_Setup(GtkWidget * combo_box , gboolean force_model);
	gchar * Meg_ComboText_GetText( GtkComboBox * widget );
	void Meg_ComboText_AppendText( GtkComboBox * widget, const gchar * text );
	void Meg_ComboText_Clear( GtkComboBox * widget );
	void Meg_ComboText_SetIndex( GtkComboBox * widget, gchar * search );


	/* */
	void Meg_Misc_SetLabel( GtkWidget *label, gchar * section, gchar * text, gchar sep );
	void Meg_Misc_SetLabel_Print( GtkWidget * label, const gchar * format, ... );

#endif


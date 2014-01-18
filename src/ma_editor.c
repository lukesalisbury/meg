/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
/* ma_editor.c
 Helper functions to support GtkSourceView.
*/

/* Global Header */
#include "global.h"
#include "alchera_functions.h"
#ifdef USEGTKSOURCEVIEW
	#include <gtksourceview/gtksourceview.h>
//	#include <gtksourceview/gtksource.h>
	#include <gtksourceview/gtksourcemark.h>
	#include <gtksourceview/gtksourcebuffer.h>
	#include <gtksourceview/gtksourcelanguage.h>
	#include <gtksourceview/gtksourcelanguagemanager.h>
	#include <gtksourceview/gtksourcestyleschememanager.h>
	#include <gtksourceview/gtksourcecompletion.h>
	#include <gtksourceview/gtksourcecompletionprovider.h>
#endif

#if GTK_MAJOR_VERSION == 2
GdkColor error_color = { 0, 65535, 0, 0 };
GdkColor warning_color = { 0, 65535, 65535, 0 };
#else
GdkRGBA error_color = { 1.0, 0.0, 0.0, .2 };
GdkRGBA warning_color = {  0, 1.0, 1.0, .2 };
#endif


#ifdef USEGTKSOURCEVIEW
	#include "lang_provider.c"
	#if GTK_MAJOR_VERSION == 2
	static gchar * Meg_Editor_Tooltip( GtkSourceMark *mark, gpointer user_data )
	{
		gchar * text = g_strdup_printf( "Message: %s", gtk_text_mark_get_name( GTK_TEXT_MARK(mark) ) );
		return text;
	}
	#else
	static gchar * Meg_Editor_Tooltip( GtkSourceMarkAttributes *attributes, GtkSourceMark *mark, GtkWidget * widget )
	{
		gtk_widget_set_has_tooltip( widget, TRUE );
		gchar * text = g_strdup_printf( "Message: %s", gtk_text_mark_get_name( GTK_TEXT_MARK(mark) ) );
		return text;
	}
	#endif
#endif



/********************************
* Alchera_Text_Init
* Creates Entity Editor Window
*/
GtkWidget * Meg_Editor_New( gchar * language )
{
	/* Load and setup UI */
	GtkWidget * widget;

	#ifdef USEGTKSOURCEVIEW
		//GtkTextBuffer * buffer = NULL;
		if ( language )
		{
			GtkSourceStyleSchemeManager * styles = gtk_source_style_scheme_manager_get_default();
			GtkSourceLanguageManager * manager = gtk_source_language_manager_new();
			gchar * paths[3];
			paths[0] = Meg_Directory_ProgramShare( "gtksourceview-2.0/language-specs" );
			paths[1] = Meg_Directory_ProgramShare( "gtksourceview-3.0/language-specs" );
			paths[2] = NULL;

			gtk_source_language_manager_set_search_path( manager, paths );

			GtkSourceLanguage * lang = NULL;
			GtkSourceBuffer * lang_buffer;
			GtkSourceStyleScheme * style;

			gtk_source_style_scheme_manager_append_search_path( styles, Meg_Directory() );

			lang = gtk_source_language_manager_get_language( manager, language );
			style = gtk_source_style_scheme_manager_get_scheme(styles, "tango" );
			if ( lang )
			{
				lang_buffer = gtk_source_buffer_new_with_language( lang );
				widget = gtk_source_view_new_with_buffer( lang_buffer );

				gtk_source_buffer_set_highlight_syntax( lang_buffer, TRUE );
				gtk_source_buffer_set_highlight_matching_brackets( lang_buffer, TRUE );
				gtk_source_buffer_set_style_scheme( lang_buffer, style );
			}
			else
			{
				widget = gtk_source_view_new();
			}
		}
		else
		{
			widget = gtk_source_view_new();
		}
		gtk_widget_set_has_tooltip( widget, TRUE );
		//buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(widget) );

		gtk_source_view_set_show_line_numbers( GTK_SOURCE_VIEW(widget), TRUE );
		gtk_source_view_set_show_line_marks( GTK_SOURCE_VIEW(widget), TRUE );
#if GTK_MAJOR_VERSION == 2
		gtk_source_view_set_mark_category_background( GTK_SOURCE_VIEW(widget), "error", &error_color );
		gtk_source_view_set_mark_category_background( GTK_SOURCE_VIEW(widget), "warning", &warning_color );
		gtk_source_view_set_mark_category_tooltip_func( GTK_SOURCE_VIEW(widget), "warning", Meg_Editor_Tooltip, NULL, NULL);
		gtk_source_view_set_mark_category_tooltip_func( GTK_SOURCE_VIEW(widget), "error", Meg_Editor_Tooltip, NULL, NULL);
#else
		GtkSourceMarkAttributes * error_attrib = gtk_source_mark_attributes_new();
		gtk_source_mark_attributes_set_background( error_attrib, &error_color );
		gtk_source_view_set_mark_attributes( GTK_SOURCE_VIEW(widget),"error", error_attrib, 10);

		GtkSourceMarkAttributes * warning_attrib = gtk_source_mark_attributes_new();
		gtk_source_mark_attributes_set_background( warning_attrib, &warning_color );
		gtk_source_view_set_mark_attributes( GTK_SOURCE_VIEW(widget), "warning", warning_attrib, 10);

		g_signal_connect( G_OBJECT(warning_attrib), "query-tooltip-text", G_CALLBACK(Meg_Editor_Tooltip), widget );
		g_signal_connect( G_OBJECT(error_attrib), "query-tooltip-text", G_CALLBACK(Meg_Editor_Tooltip), widget );
		//g_signal_connect( G_OBJECT(warning_attrib), "query-tooltip-markup", G_CALLBACK(Meg_Editor_Tooltip), widget );
		//g_signal_connect( G_OBJECT(error_attrib), "query-tooltip-markup", G_CALLBACK(Meg_Editor_Tooltip), widget );
#endif
		GtkSourceCompletion * completion = gtk_source_view_get_completion( GTK_SOURCE_VIEW(widget) );

		ScriptAutoCompleteProvider * tp = g_object_new( script_auto_complete_provider_get_type(), NULL );
		tp->priority = 10;
		tp->name = "";
		tp->widget = widget;
		gtk_source_completion_add_provider( completion, GTK_SOURCE_COMPLETION_PROVIDER(tp), NULL );

		g_object_set( completion, "show-icons", FALSE, NULL);
		g_object_set( completion, "select-on-show", FALSE, NULL);
		g_object_set( completion, "remember-info-visibility", FALSE, NULL);
		g_object_set( completion, "show-headers", FALSE, NULL);

		//g_signal_connect( G_OBJECT(widget), "query-tooltip", G_CALLBACK(Meg_Editor_Query_Tooltip), buffer );
	#else
		widget = gtk_text_view_new();
	#endif

	return widget;
}

/********************************
* Meg_Editor_Redo
* Event:
* Result:
*/
void Meg_Editor_Redo( GtkWidget * widget )
{
	#ifdef USEGTKSOURCEVIEW
		GtkTextBuffer * text_buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(widget) );
		if ( gtk_source_buffer_can_redo( GTK_SOURCE_BUFFER(text_buffer) ) )
		{
			gtk_source_buffer_redo( GTK_SOURCE_BUFFER(text_buffer) );
		}
	#else
		Meg_Main_PrintStatus("Not supported without GtkSourceView.");
	#endif
}

/********************************
* Meg_Editor_Undo
* Event:
* Result:
*/
void Meg_Editor_Undo( GtkWidget * widget )
{
	#ifdef USEGTKSOURCEVIEW
		GtkTextBuffer * text_buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(widget) );
		if ( gtk_source_buffer_can_undo( GTK_SOURCE_BUFFER(text_buffer) ) )
		{
			gtk_source_buffer_undo( GTK_SOURCE_BUFFER(text_buffer) );
		}
	#else
		Meg_Main_PrintStatus("Not supported without GtkSourceView.");
	#endif
}

/********************************
* Meg_Editor_SetText
* Event:
* Result:
*/
void Meg_Editor_SetLineInfo( GtkWidget * widget, gint line, gint type, gchar * message )
{
#ifdef USEGTKSOURCEVIEW
	GtkTextIter iter;
	GtkTextBuffer * buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(widget) );

	gtk_text_buffer_get_iter_at_line( buffer, &iter, line -1 );

	gtk_source_buffer_create_source_mark( GTK_SOURCE_BUFFER(buffer), message, (type == 1 ? "error" : "warning"), &iter );
#endif

}

/********************************
* Meg_Editor_SetText
* Event:
* Result:
*/
void Meg_Editor_ClearLineInfo( GtkWidget * widget )
{
#ifdef USEGTKSOURCEVIEW
	GtkTextIter start, end;
	GtkTextBuffer * buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(widget) );

	gtk_text_buffer_get_bounds( buffer, &start, &end );

	gtk_source_buffer_remove_source_marks( GTK_SOURCE_BUFFER(buffer), &start, &end, NULL );
#endif

}

/********************************
* Meg_Editor_SetText
* Event:
* Result:
*/
G_MODULE_EXPORT void Meg_Editor_SetText( GtkWidget * widget, gchar * text )
{

}

/********************************
* Meg_Editor_GetText
* Event:
* Result:
*/
G_MODULE_EXPORT gchar * Meg_Editor_GetText( GtkWidget * widget )
{
	return NULL;
}


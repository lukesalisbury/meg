/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
/* ma_help_parser.c
	XML based Help System
*/

/* Global Header */
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#include "gtk_compat.h"

typedef enum {
	TEXT_NONE = 0,
	TEXT_NORMAL = 1,
	TEXT_BOLD = 2,
	TEXT_ITALIC = 4,
	TEXT_LARGE = 8,
	TEXT_SMALL = 16,
	TEXT_WHITESPACE = 32,
	TEXT_CODE = 64,
	TEXT_BLOCK = 128,
	TEXT_CENTER = 256,
	TEXT_MONO = 512,
	TEXT_SPECIAL = 1024,
	TEXT_LEFTBLOCK = 2048,
	TEXT_RIGHTBLOCK = 4096,
	TEXT_LISTITEM = 8192,
	TEXT_COLUMNS = 16384,
	TEXT_FIRSTCHILD = 32768,
	TEXT_HASINLINESIBLING = 65536,
	TEXT_PARAGRAPH = 131072,
	TEXT_HEADER = 262144,
	TEXT_HTML_H2 = TEXT_LARGE | TEXT_BOLD | TEXT_BLOCK,
	TEXT_HTML_H3 = TEXT_LARGE | TEXT_BOLD | TEXT_BLOCK | TEXT_HEADER,
	TEXT_HTML_H4 = TEXT_BOLD | TEXT_BLOCK | TEXT_HEADER,
	TEXT_HTML_P = TEXT_PARAGRAPH |  TEXT_NORMAL,
	TEXT_HTML_LI = TEXT_LISTITEM |  TEXT_NORMAL,
	TEXT_HTML_UL = TEXT_PARAGRAPH,
	TEXT_HTML_PRE = TEXT_CODE | TEXT_BLOCK | TEXT_PARAGRAPH,
} TextStyle;

typedef struct {
	gint tag;
	gint start;
	gint end;
} HelpStyle;


/* Functions local */
void help_parser_start_element( GMarkupParseContext *context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, gpointer user_data, GError **error );
void help_parser_text( GMarkupParseContext *context, const gchar * text, gsize text_len, gpointer user_data, GError **error);
void help_parser_end_element( GMarkupParseContext *context, const gchar *element_name, gpointer user_data, GError **error);

/* Global Variables */
GtkWidget * meg_help_column_widget = NULL; // Second widget to do coloums
GtkTextTag * helpTextTagBold, * helpTextTagItalic, * helpTextTagNormal, * helpTextTagLarge, * helpTextTagMono, * helpTextTagCenter, * helpTextTagSpecial;
gint alcheraHelpStyle = TEXT_NONE;
gint * alcheraHelpCurrentStyle = NULL;
gboolean alcheraHelpValidTag = TRUE;
gchar * alcheraHelpLink = NULL;
GSList * helpStylesList = NULL;
GtkTextBuffer * help_textbuffer = NULL, * help_parent_textbuffer = NULL;

static GMarkupParser help_parser = { help_parser_start_element, help_parser_end_element, help_parser_text, NULL, NULL};

/* Functions */
GdkPixbuf * Meg_Help_GetImage( const gchar * file );
gchar * Meg_Help_GetText( const gchar * file );
void Meg_Help_SwitchPage( gchar * file );


/**
 * @brief hack_ascii_isspace
 * @param c
 * @return true if c is between 0 & 32
 */
gboolean hack_ascii_isspace( guchar c )
{
	if ( c < 32 && c > 0 )
		return TRUE;
	return FALSE;
}


/**
 * @brief hack_strchug
 * @param string Text buffer to apply
 * @return modified string
 */
gchar * hack_strchug( gchar * string )
{
	guchar *start;
	g_return_val_if_fail( string != NULL, NULL );
	for (start = (guchar*) string; *start && hack_ascii_isspace(*start); start++)
		;
	g_memmove( string, start, strlen((gchar *)start) + 1);
	return string;
}

/**
 * @brief hack_strchomp
 * @param string Text buffer to apply
 * @return  modified string
 */
gchar * hack_strchomp( gchar * string )
{
	gsize len;
	g_return_val_if_fail( string != NULL, NULL );
	len = strlen (string);
	while (len--)
	{
		if ( hack_ascii_isspace((guchar) string[len]))
			string[len] = '\0';
		else
			break;
	}
	return string;
}

/********************************
* help_parser_apply_tags
*
@ buffer: Text buffer to apply
*/
void help_parser_apply_tags( GtkTextBuffer * buffer )
{
	GtkTextTagTable * table = gtk_text_buffer_get_tag_table(buffer);
	if ( !gtk_text_tag_table_get_size(table) )
	{
		gtk_text_buffer_create_tag( buffer, "first", "background", "#FF0000", "foreground", "#00F0F0",NULL);
		gtk_text_buffer_create_tag( buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
		gtk_text_buffer_create_tag( buffer, "italic", "font", "Italic", NULL);
		gtk_text_buffer_create_tag( buffer, "large", "font", "Sans 16", NULL);
		gtk_text_buffer_create_tag( buffer, "normal", "wrap-mode", GTK_WRAP_WORD, "justification", GTK_JUSTIFY_LEFT, NULL);
		gtk_text_buffer_create_tag( buffer, "mono", "font", "Monospace", "paragraph-background", "#DDDDDD", "foreground", "black",  "indent", 10, "wrap-mode", GTK_WRAP_NONE, NULL);
		gtk_text_buffer_create_tag( buffer, "center", "justification", GTK_JUSTIFY_CENTER, "wrap-mode", GTK_WRAP_NONE, NULL);
		gtk_text_buffer_create_tag( buffer, "special", "paragraph-background", "#EEEEEE", "foreground", "black", "left-margin", 10, "right-margin", 10, "justification", GTK_JUSTIFY_CENTER, NULL);

		gtk_text_buffer_create_tag( buffer, "paragraph", "pixels-above-lines", 6, NULL);
	}
}

/**
 * @brief Meg_HelpParser_Load
 * @param textview
 * @param content
 * @return
 */
gboolean Meg_HelpParser_Load( GtkTextView * textview, gchar * content )
{
	gboolean successful = FALSE;
	GError * err = NULL;
	GMarkupParseContext * ctx;

	alcheraHelpStyle = 0;

	help_textbuffer = help_parent_textbuffer = gtk_text_view_get_buffer(textview);
	help_parser_apply_tags( help_textbuffer );
	gtk_text_buffer_set_text( help_textbuffer, "", -1);


	ctx = g_markup_parse_context_new( &help_parser, (GMarkupParseFlags)0, textview, NULL );
	if ( g_markup_parse_context_parse( ctx, content, -1, &err ) )
	{
		g_markup_parse_context_end_parse( ctx, &err );
		g_markup_parse_context_free( ctx );

		/*
		GtkTextChildAnchor * anchor = NULL;
		GList * widgets = NULL;
		GtkTextIter iter;

		gtk_text_buffer_get_start_iter( alchera_help_parent_textbuffer, &iter );
		while ( gtk_text_iter_forward_char(&iter) )
		{
			if ( gtk_text_iter_get_child_anchor(&iter) )
			{
				g_print("asdf");
				anchor = gtk_text_iter_get_child_anchor( &iter );
				widgets = gtk_text_child_anchor_get_widgets( anchor );
				if ( widgets )
				{

					gtk_widget_show_all( (GtkWidget*)widgets->data );
				}
			}
		}
		*/
		successful = TRUE;
	}
	return successful;
}


/**
 * @brief Meg_HelpParser_Event
 * @param text_view
 * @param ev
 * @return
 */
gboolean Meg_HelpParser_Event( GtkWidget * text_view, GdkEvent * ev )
{
	GtkTextIter start, end, iter;
	GtkTextBuffer * buffer;
	GdkEventButton * event;
	gint x, y;
	GSList * tags = NULL, * tagp = NULL;
	gchar * page = NULL, * full_name;

	if ( ev->type != GDK_BUTTON_RELEASE )
		return FALSE;

	event = (GdkEventButton *)ev;

	if ( event->button != 1 )
		return FALSE;

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));

	/* we shouldn't follow a link if the user has selected something */
	gtk_text_buffer_get_selection_bounds (buffer, &start, &end);
	if (gtk_text_iter_get_offset (&start) != gtk_text_iter_get_offset (&end))
		return FALSE;

	gtk_text_view_window_to_buffer_coords( GTK_TEXT_VIEW(text_view), GTK_TEXT_WINDOW_WIDGET, event->x, event->y, &x, &y);
	gtk_text_view_get_iter_at_location( GTK_TEXT_VIEW(text_view), &iter, x, y);


	tags = gtk_text_iter_get_tags(&iter);
	for (tagp = tags;  tagp != NULL;  tagp = tagp->next)
	{
		GtkTextTag *tag = tagp->data;
		page = (gchar *)(g_object_get_data( G_OBJECT(tag), "page" ));

		if ( page )
		{
			Meg_Help_SwitchPage(page);
			break;
		}
	}

	if (tags)
		g_slist_free (tags);
	return FALSE;
}


/* Parsers Functions */

/**
 * @brief help_parser_append_newline
 */
void help_parser_append_newline()
{
	GtkTextIter last_iter;
	gtk_text_buffer_get_end_iter( help_textbuffer, &last_iter);
	gtk_text_iter_backward_char(&last_iter);
	if ( gtk_text_iter_get_char(&last_iter) != 10 )
	{
		gtk_text_buffer_get_end_iter( help_textbuffer, &last_iter);
		gtk_text_buffer_insert( help_textbuffer, &last_iter, "\n", -1);
	}
}


/**
 * @brief help_parser_append_text
 * @param text
 */

void help_parser_append_text( const gchar * text )
{
	HelpStyle * current_style = (HelpStyle*)helpStylesList->data;

	GtkTextIter iter, iter2;

	if ( !help_textbuffer )
	{
		return;
	}

	gtk_text_buffer_get_end_iter( help_textbuffer, &iter);


	if ( current_style )
	{
		HelpStyle * parent_style = g_slist_nth_data(helpStylesList,1);


		gtk_text_buffer_get_end_iter( help_textbuffer, &iter);

		current_style->start = gtk_text_iter_get_offset(&iter);

		if ( (current_style->tag & TEXT_LISTITEM) )
		{
			gtk_text_buffer_insert( help_textbuffer, &iter, " • ", -1);
		}

		gtk_text_buffer_insert( help_textbuffer, &iter, text, -1);

		if ( alcheraHelpLink )
		{
			GtkTextTag * tag = gtk_text_buffer_create_tag( help_textbuffer, NULL, "foreground", "blue", "underline", PANGO_UNDERLINE_SINGLE, NULL);

			gtk_text_buffer_get_iter_at_offset( help_textbuffer, &iter2, current_style->start);
			g_object_set_data_full( G_OBJECT(tag), "page", g_strdup(alcheraHelpLink), g_free );
			gtk_text_buffer_apply_tag( help_textbuffer, tag, &iter, &iter2 );

			g_free( alcheraHelpLink );
			alcheraHelpLink = NULL;
		}
	}
}


/**
 * @brief help_parser_start_element
 * @param context
 * @param element_name
 * @param attribute_names
 * @param attribute_values
 * @param user_data
 * @param error
 */
void help_parser_start_element( GMarkupParseContext * context, const gchar *element_name, const gchar **attribute_names, const gchar **attribute_values, gpointer user_data, GError **error )
{
	if ( help_textbuffer == NULL )
		return;

	GtkTextView * textview = GTK_TEXT_VIEW(user_data);
	GtkTextIter iter;
	HelpStyle * current_style = g_new0(HelpStyle, 1);
	HelpStyle * parent_style = NULL;

	if ( helpStylesList )
	{
		parent_style = (HelpStyle*)helpStylesList->data;

		/* Columns. Create new Text View */
		if ( (parent_style->tag & TEXT_COLUMNS) && meg_help_column_widget )
		{
			GtkWidget * new_view = gtk_text_view_new();
			gtk_box_pack_start( GTK_BOX(meg_help_column_widget), new_view, 1, 1, 0 );

			help_textbuffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(new_view) );

			gtk_text_view_set_editable( GTK_TEXT_VIEW(new_view), FALSE );
			gtk_text_view_set_wrap_mode( GTK_TEXT_VIEW(new_view), GTK_WRAP_WORD );

			help_parser_apply_tags( help_textbuffer );
		}
	}

	/*  Check Current Element */
	if ( !g_ascii_strncasecmp(element_name, "h2", 2) )
		current_style->tag = TEXT_HTML_H2;
	else if (!g_ascii_strncasecmp(element_name, "h3", 2) )
		current_style->tag = TEXT_HTML_H3;
	else if (!g_ascii_strncasecmp(element_name, "h4", 2) )
		current_style->tag = TEXT_HTML_H4;
	else if ( !g_ascii_strncasecmp(element_name, "pre", 3) )
		current_style->tag = TEXT_CODE | TEXT_BLOCK;
	else if ( !g_ascii_strncasecmp(element_name, "li", 2) )
		current_style->tag = TEXT_HTML_LI;
	else if ( !g_ascii_strncasecmp(element_name, "p", 1) )
		current_style->tag = TEXT_HTML_P;
	else if	( !g_ascii_strncasecmp(element_name, "div", 3) )
		current_style->tag = TEXT_NORMAL | TEXT_BLOCK;
	else if	( !g_ascii_strncasecmp(element_name, "dd", 2) || !g_ascii_strncasecmp(element_name, "dt", 2) )
		current_style->tag = TEXT_NORMAL | TEXT_BLOCK;
	else if ( !g_ascii_strncasecmp(element_name, "ul", 2))
		current_style->tag = TEXT_NORMAL | TEXT_PARAGRAPH;
	else if ( !g_ascii_strncasecmp(element_name, "strong", 6) )
		current_style->tag = TEXT_BOLD;
	else if ( !g_ascii_strncasecmp(element_name, "em", 2))
		current_style->tag = TEXT_ITALIC;
	else if ( !g_ascii_strncasecmp(element_name, "img", 3) )
	{
		/* img required src attribute */
		for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
		{
			if ( !g_ascii_strcasecmp (*attribute_names, "src") )
			{
				GdkPixbuf * pixbuf = Meg_Help_GetImage( *attribute_values );
				if ( pixbuf )
				{
					gtk_text_buffer_get_end_iter( help_textbuffer, &iter);
					gtk_text_buffer_insert_pixbuf( help_textbuffer, &iter, pixbuf);
					gtk_text_buffer_insert( help_textbuffer, &iter, "\n", -1);
				}
			}
		}
	}
	else if ( !g_ascii_strncasecmp(element_name, "a", 1) )
	{
		/* a required src attribute */
		current_style->tag = TEXT_NORMAL;
		for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
		{
			if ( !g_ascii_strcasecmp (*attribute_names, "href") )
			{
				alcheraHelpLink = g_strdup(*attribute_values);
			}
		}
	}
	else if ( !g_ascii_strncasecmp(element_name, "include", 7) )
	{
		current_style->tag |= TEXT_NORMAL;
		for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
		{
			if ( !g_ascii_strcasecmp (*attribute_names, "src") )
			{
				gchar * content = Meg_Help_GetText( *attribute_values );
				if ( content )
				{
					help_parser_append_text( content );
				}
				g_free(content);
			}
		}
	}

	/* Check against styles */
	if ( (current_style->tag & TEXT_BLOCK) )
	{
		for (; *attribute_names && *attribute_values; attribute_names++, attribute_values++)
		{
			if ( !g_ascii_strcasecmp( *attribute_names, "class" ) )
			{
				if ( !g_ascii_strcasecmp( *attribute_values, "columns" ) )
				{
					current_style->tag = TEXT_COLUMNS | TEXT_BLOCK;
					meg_help_column_widget = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 4 );

					GtkTextIter iter;
					GtkTextChildAnchor * alchera_help_col_widget = NULL;
					gtk_text_buffer_get_end_iter( help_textbuffer, &iter );
					alchera_help_col_widget = gtk_text_buffer_create_child_anchor( help_textbuffer, &iter );
					gtk_text_view_add_child_at_anchor( textview, meg_help_column_widget, alchera_help_col_widget );


				}
			}
			else if ( !g_ascii_strcasecmp( *attribute_names, "xmlns" ) )
			{
				current_style->tag = TEXT_NORMAL;
			}
		}
	}

//	if ( helpStylesList )
//	{
//		current_style->tag |= TEXT_FIRSTCHILD;
//	}

	/*  */

	helpStylesList = g_slist_prepend(helpStylesList, current_style);

	gboolean hasInlinePrevSibling = FALSE;

	/* Mark For Prev inline sibling */
	if ( parent_style )
	{
		if ( (parent_style->tag & TEXT_HASINLINESIBLING) )
		{
			hasInlinePrevSibling = !!(current_style->tag & TEXT_BLOCK);
			parent_style->tag &= ~TEXT_HASINLINESIBLING;
		}
		else
		{
			parent_style->tag |= TEXT_HASINLINESIBLING;
			current_style->tag |= TEXT_FIRSTCHILD;
		}
	}

	if ( !g_ascii_strncasecmp(element_name, "br", 2) || hasInlinePrevSibling )
	{
		GtkTextIter iter;
		gtk_text_buffer_get_end_iter( help_textbuffer, &iter);
		gtk_text_buffer_insert( help_textbuffer, &iter, "\n", -1);
	}
}

/**
 * @brief help_parser_text
 * @param context
 * @param text
 * @param text_len
 * @param user_data
 * @param error
 */
void help_parser_text( GMarkupParseContext *context, const gchar * text, gsize text_len, gpointer user_data, GError **error )
{
	HelpStyle * current_style = (HelpStyle*)g_slist_nth_data(helpStylesList, 0);

	if ( text_len )
	{
		gchar * new_string = g_strdup(text);
		if ( (current_style->tag & TEXT_MONO) )
		{
			help_parser_append_text(new_string);
		}
		else
		{
			hack_strchug( hack_strchomp(new_string) );
			if ( g_utf8_strlen(new_string, -1) )
			{
				help_parser_append_text(new_string);
			}
		}
	}
}

/**
 * @brief help_parser_end_element
 * @param context
 * @param element_name
 * @param user_data
 * @param error
 */
void help_parser_end_element( GMarkupParseContext *context, const gchar *element_name, gpointer user_data, GError **error )
{
	if ( help_textbuffer == NULL )
		return;

	GtkTextIter start_iter, end_iter;
	HelpStyle * current_style = (HelpStyle*)helpStylesList->data;

	gtk_text_buffer_get_iter_at_offset( help_textbuffer, &start_iter, current_style->start);
	gtk_text_buffer_get_end_iter( help_textbuffer, &end_iter );

	if ( (current_style->tag & TEXT_COLUMNS) )
	{
		if ( meg_help_column_widget )
		{
			meg_help_column_widget = NULL;
			help_textbuffer = help_parent_textbuffer;
		}
	}

	if ((current_style->tag & TEXT_NORMAL))
		gtk_text_buffer_apply_tag_by_name( help_textbuffer, "normal", &start_iter, &end_iter);
	if ((current_style->tag & TEXT_BOLD))
		gtk_text_buffer_apply_tag_by_name( help_textbuffer, "bold", &start_iter, &end_iter);
	if ((current_style->tag & TEXT_ITALIC))
		gtk_text_buffer_apply_tag_by_name( help_textbuffer, "italic", &start_iter, &end_iter);
	if ((current_style->tag & TEXT_LARGE))
		gtk_text_buffer_apply_tag_by_name( help_textbuffer, "large", &start_iter, &end_iter);
	if ((current_style->tag & TEXT_CODE))
		gtk_text_buffer_apply_tag_by_name( help_textbuffer, "mono", &start_iter, &end_iter);
	if ((current_style->tag & TEXT_CENTER))
		gtk_text_buffer_apply_tag_by_name( help_textbuffer, "center", &start_iter, &end_iter);
	if ((current_style->tag & TEXT_SPECIAL))
		gtk_text_buffer_apply_tag_by_name( help_textbuffer, "special", &start_iter, &end_iter);
	if ((current_style->tag & TEXT_HEADER))
		gtk_text_buffer_apply_tag_by_name( help_textbuffer, "paragraph", &start_iter, &end_iter);
	if ((current_style->tag & TEXT_PARAGRAPH))
		gtk_text_buffer_apply_tag_by_name( help_textbuffer, "paragraph", &start_iter, &end_iter);
	if ( (current_style->tag & TEXT_BLOCK) || (current_style->tag & TEXT_PARAGRAPH)|| (current_style->tag & TEXT_LISTITEM) )
	{
		gtk_text_buffer_insert_with_tags_by_name( help_textbuffer, &end_iter, "\n", -1, "normal", NULL);
	}

	helpStylesList = g_slist_remove(helpStylesList, current_style);


}


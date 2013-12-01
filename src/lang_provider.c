/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcecompletion.h>
#include <gtksourceview/gtksourcecompletioninfo.h>
#include <gtksourceview/gtksourcecompletionitem.h>

typedef struct _ScriptAutoCompleteProvider ScriptAutoCompleteProvider;
typedef struct _ScriptAutoCompleteProviderClass ScriptAutoCompleteProviderClass;

struct _ScriptAutoCompleteProvider
{
	GObject parent;
	GList * proposals;
	gint priority;
	gchar * name;
	GdkPixbuf * icon;
	GSList * database;
	gchar * function;
	GtkWidget * widget;
};

struct _ScriptAutoCompleteProviderClass
{
	GObjectClass parent_class;
};

static void script_auto_complete_provider_iface_init(GtkSourceCompletionProviderIface * iface);
GType script_auto_complete_provider_get_type(void);


#if GTK_MAJOR_VERSION > 2
G_DEFINE_TYPE_WITH_CODE( ScriptAutoCompleteProvider, script_auto_complete_provider, G_TYPE_OBJECT, G_IMPLEMENT_INTERFACE(GTK_SOURCE_TYPE_COMPLETION_PROVIDER, script_auto_complete_provider_iface_init) )
#else
G_DEFINE_TYPE_WITH_CODE( ScriptAutoCompleteProvider, script_auto_complete_provider, G_TYPE_OBJECT, G_IMPLEMENT_INTERFACE(GTK_TYPE_SOURCE_COMPLETION_PROVIDER, script_auto_complete_provider_iface_init) )
#endif

static gboolean script_auto_complete_valid_word( gunichar ch )
{
	return g_unichar_isalnum(ch);
}

static gchar * script_auto_complete_get_word( GtkTextIter * iter )
{
	GtkTextIter start = *iter;
	GtkTextIter end = *iter;

	while( TRUE )
	{
		if ( gtk_text_iter_starts_word( &start ) )
		{
			break;
		}
		gtk_text_iter_backward_char(&start);

		if ( !script_auto_complete_valid_word( gtk_text_iter_get_char(&start) ) )
		{
			break;
		}
	}

	while( TRUE )
	{
		if ( gtk_text_iter_ends_word( &end ) )
		{
			break;
		}
		gtk_text_iter_forward_char(&end);

		if ( !script_auto_complete_valid_word( gtk_text_iter_get_char(&end) ) )
		{
			break;
		}
	}
	
	if ( gtk_text_iter_equal(&start, &end) )
	{
		return NULL;
	}
	else
	{
		return gtk_text_iter_get_text( &start, &end );
	}
}

static gchar * script_auto_complete_provider_get_name( GtkSourceCompletionProvider * provider )
{
	return g_strdup(((ScriptAutoCompleteProvider *)provider)->name);
}

static gint script_auto_complete_provider_get_priority( GtkSourceCompletionProvider * provider )
{
	return ((ScriptAutoCompleteProvider *)provider)->priority;
}

static gboolean script_auto_complete_provider_match( GtkSourceCompletionProvider * provider, GtkSourceCompletionContext * context )
{
	gboolean results = TRUE;
	/*
	gchar * word = NULL;
	GtkTextIter iter;
	gtk_source_completion_context_get_iter( context, &iter);

	word = script_auto_complete_get_word( &iter );

	if (word == NULL || g_utf8_strlen(word, -1) < 2 )
		results = FALSE;
	gtk_widget_set_has_tooltip( ((ScriptAutoCompleteProvider *)provider)->widget, results );
	*/
	return results;
}

static void script_auto_complete_provider_populate( GtkSourceCompletionProvider * provider, GtkSourceCompletionContext * context )
{
	gchar * word = NULL;
	GtkTextIter iter;
	gtk_source_completion_context_get_iter( context, &iter);

	word = script_auto_complete_get_word( &iter );

	gtk_widget_set_has_tooltip( ((ScriptAutoCompleteProvider *)provider)->widget, TRUE );
	if (word == NULL || g_utf8_strlen(word, -1) < 2 )
	{
		gtk_source_completion_context_add_proposals( context, provider, NULL, TRUE ); /* Clear */
		return;
	}
	else
	{
		GList * proposals = NULL;
		GSList * q = ((ScriptAutoCompleteProvider *)provider)->database;
		if ( q )
		{
			while ( q )
			{
				EditorDatabaseListing * listing = (EditorDatabaseListing *)(q->data);
				if ( listing)
				{
					gchar * markup2 = g_markup_printf_escaped("%s<i>%s</i>", listing->name, listing->arguments);
/*
					if ( !g_ascii_strcasecmp( listing->name, word ) )
					{
						gchar * markup = g_markup_printf_escaped("<i>Arg:%s</i>\n\n%s", listing->arguments, listing->info);
						gtk_widget_set_tooltip_markup( ((ScriptAutoCompleteProvider *)provider)->widget, markup );
						gtk_widget_set_has_tooltip( ((ScriptAutoCompleteProvider *)provider)->widget, TRUE );
					}
					else
						*/
						if ( g_str_has_prefix( listing->name, word ) )
					{
						proposals = g_list_prepend( proposals, gtk_source_completion_item_new_with_markup(markup2, listing->name, NULL, listing->info ) );
					}
				}
				q = g_slist_next( q );

			}
		}
		gtk_source_completion_context_add_proposals( context, provider, proposals, TRUE);
	}
}

static GdkPixbuf * script_auto_complete_provider_get_icon( GtkSourceCompletionProvider *provider)
{
	return NULL;
}

static GtkSourceCompletionActivation script_auto_complete_provider_get_activation( GtkSourceCompletionProvider *provider )
{
	return GTK_SOURCE_COMPLETION_ACTIVATION_USER_REQUESTED;
}


static gboolean script_auto_complete_provider_activate_proposal( GtkSourceCompletionProvider * provider, GtkSourceCompletionProposal * proposal, GtkTextIter * iter)
{

	return FALSE;
}

static void script_auto_complete_provider_iface_init( GtkSourceCompletionProviderIface * iface )
{
	iface->get_name = script_auto_complete_provider_get_name;
	iface->populate = script_auto_complete_provider_populate;
	iface->match = script_auto_complete_provider_match;
	iface->get_priority = script_auto_complete_provider_get_priority;
	iface->get_icon = script_auto_complete_provider_get_icon;
	iface->get_activation = script_auto_complete_provider_get_activation;
	iface->activate_proposal = script_auto_complete_provider_activate_proposal;
}


static void script_auto_complete_provider_class_init( ScriptAutoCompleteProviderClass * klass )
{
}

static void script_auto_complete_provider_init( ScriptAutoCompleteProvider *self )
{
	self->database = AL_Editor_Database("");
}


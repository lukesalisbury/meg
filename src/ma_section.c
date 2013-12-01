/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Global Header */
#include "global.h"

/* Required Headers */
#include "ma_map.h"
#include "ma_map_section.h"


/* GtkWidget */

/* Global Variables */
GtkListStore * sectionFileStore = NULL;
GtkWidget * sectionNotebook = NULL;

/* External Functions */
void Meg_Help_Load( const gchar * file, GtkWidget * textview );

/* UI */
#include "ui/section_page.gui.h"
const gchar * alchera_section_ui_main = GUISECTION_PAGE;


/********************************
* MegWidget_Section_Create
* Initialises Map + Overview Page
*/
void MegWidget_Section_Create()
{
	/* UI */
	GtkBuilder * ui = Meg_Builder_Create(alchera_section_ui_main, __func__, __LINE__);
	g_return_if_fail( ui );

	/* Widget */
	GtkWidget * widget, * treeview_section, * help_text;

	sectionNotebook = GET_WIDGET( ui, "section_notebook" );
	widget = GET_WIDGET( ui, "section_widget" );
	help_text = GET_WIDGET( ui, "help_text" );

	treeview_section = GET_WIDGET( ui, "treeview_section" );

	sectionFileStore = GET_LISTSTORE( ui, "store_section" );

	g_object_ref(widget);

	/* Tab Setting */
	g_object_set_data( G_OBJECT(widget), "meg-help-page", g_strdup(PROGRAMSHELPDIRECTORY"/SectionPage.xml") ); //Set the help page
	Meg_Main_AddSection(widget, "Sections", "gtk-justify-fill");

	/* Signals */
	g_signal_connect( G_OBJECT(treeview_section), "row-activated", G_CALLBACK(Meg_MapSection_OpenFile), NULL);
	g_signal_connect( G_OBJECT(treeview_section), "popup-menu", G_CALLBACK(Meg_MapSection_ListMenu), NULL);

	SET_OBJECT_SIGNAL(ui, "actionAddSection", "activate", G_CALLBACK(Meg_MapSection_AddFile), treeview_section );
	SET_OBJECT_SIGNAL(ui, "actionRemoveSection", "activate", G_CALLBACK(Meg_MapSection_RemoveFile), treeview_section );

	/* */
	Meg_Help_Load(PROGRAMSHELPDIRECTORY"/SectionPage.xml", help_text);

}


/********************************
* MegWidget_Section_Refresh
*
*/
void MegWidget_Section_Refresh()
{

	AL_MapSections_Files(sectionFileStore);

}

/********************************
* MegWidget_Section_Refresh
*
*/
void MegWidget_Section_Init()
{

	MegWidget_Section_Refresh();
}

/********************************
* MegWidget_Section_Refresh
*
*/
void MegWidget_Section_Close()
{
	gtk_list_store_clear( sectionFileStore );
}

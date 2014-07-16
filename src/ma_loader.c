/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* ma_loader.c
Loading and unloader of projects

*/

/* Global Header */
#include "global.h"
#include "alchera_loader.h"
#include "ma_loader.h"
#include "ma_widgets.h"

/* GtkWidget */
extern GtkWidget * alchera_main_toolbar;

/* Global Variables */
gchar * project_file_path = NULL;

/* External Functions */


/********************************
* Meg_Loaders_AddRecent
*
*/
gboolean Meg_Loaders_AddRecent( const gchar * file_path )
{
	/* Update recent manager */
	gchar * file_url = g_filename_to_uri(file_path, NULL, NULL);
	GtkRecentManager * manager = gtk_recent_manager_get_default();

	GtkRecentData * recent_data;
	static gchar *groups[1] = { NULL };

	recent_data = g_slice_new(GtkRecentData);
	recent_data->display_name = MegProject_Title();
	recent_data->description = g_strdup(MIMETYPE_DESCRIPTION);
	recent_data->mime_type = ROOT_MIMETYPE;
	recent_data->app_name = (gchar *) g_get_application_name();
	recent_data->app_exec = g_strjoin (" ", g_get_prgname(), "%u", NULL);
	recent_data->groups = groups;
	recent_data->is_private = FALSE;

	gtk_recent_manager_add_full( manager, file_url, recent_data );

	g_free( recent_data->app_exec );
	g_slice_free( GtkRecentData, recent_data );



	g_free(file_url);

	return TRUE;
}

/********************************
* Meg_Loaders_Init
*
*/
gboolean Meg_Loaders_Init( gchar * project_path )
{
	GtkToolItem * default_item = NULL;
	gchar * project_directory = NULL;

	project_directory = AL_LoadProject( project_path );

	if ( project_directory )
	{
		/* Update Widgets */
		project_file_path = g_strdup(project_directory);

		MegWidget_Project_Init();
		MegWidget_Language_Init();
		MegWidget_Spritesheet_Init();
		MegWidget_Map_Init();
		MegWidget_Section_Init();
		MegWidget_EntityList_Init();
		MegWidget_Audio_Init();
		MegWidget_Questions_Init();

		/* Switch to project info widget */
		default_item = gtk_toolbar_get_nth_item( GTK_TOOLBAR(alchera_main_toolbar), 0 );
		gtk_toggle_tool_button_set_active( GTK_TOGGLE_TOOL_BUTTON(default_item), TRUE );
		//g_signal_emit_by_name( default_item, "clicked" );

		g_free(project_directory);

		return TRUE;
	}



	Meg_Error_Print( __func__, __LINE__, "Invalid Project: %s", project_path);
	return FALSE;
}

/********************************
* Meg_Loaders_CreateNew
*
*/
gboolean Meg_Loaders_CreateNew( const gchar * title, const gchar * author )
{
	gboolean loaded = FALSE;
	gchar * path_string = NULL;

	path_string = AL_CreateProject( title, author );
	if ( path_string )
	{
		Meg_Loaders_Init( path_string );
		loaded = TRUE;
	}

	if ( !loaded )
	{
		Meg_Error_Print( __func__, __LINE__, "Could not create '%s'.", title);
	}
	return loaded;
}

/********************************
* Meg_Loaders_Close
*
*/
gboolean Meg_Loaders_Close(  )
{
	if ( project_file_path )
	{
		MegWidget_Project_Close();
		MegWidget_Language_Close();
		MegWidget_Spritesheet_Close();
		MegWidget_Map_Close();
		MegWidget_Section_Close();
		MegWidget_EntityList_Close();
		MegWidget_Audio_Close();
		MegWidget_Questions_Close();

		AL_CloseProject( );

		CLEAR_STRING(project_file_path);

		return TRUE;
	}

	return FALSE;

}

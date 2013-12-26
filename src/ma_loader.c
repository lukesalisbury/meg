/****************************
Copyright © 2007-2013 Luke Salisbury
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
* Alchera_Loaders_Init
*
*/
gboolean Alchera_Loaders_Init( gchar * project_path )
{
	gchar * project_directory = NULL;

	project_directory = AL_LoadProject( project_path );

	if ( project_directory )
	{
		/* Update recent manager */
		GtkRecentManager * manager = gtk_recent_manager_get_default();
		GtkRecentData * recent_data;
		static gchar *groups[1] = { NULL };

		recent_data = g_slice_new(GtkRecentData);
		recent_data->display_name = AL_Title();
		recent_data->description = g_strdup(project_directory);
		recent_data->mime_type = ROOT_MIMETYPE;
		recent_data->app_name = (gchar *) g_get_application_name();
		recent_data->app_exec = g_strjoin (" ", g_get_prgname(), "%u", NULL);
		recent_data->groups = groups;
		recent_data->is_private = FALSE;
		gtk_recent_manager_add_full( manager, g_filename_to_uri(project_directory, NULL,NULL), recent_data );
		g_free( recent_data->app_exec );
		g_slice_free( GtkRecentData, recent_data );

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
		GtkToolItem * default_item = gtk_toolbar_get_nth_item( GTK_TOOLBAR(alchera_main_toolbar), 0 );
		g_signal_emit_by_name( default_item, "clicked" );

		return TRUE;
	}

	g_free(project_directory);

	Meg_Error_Print( __func__, __LINE__, "Invalid Project: %s", project_path);
	return FALSE;
}

/********************************
* Alchera_Loaders_CreateNew
*
*/
gboolean Alchera_Loaders_CreateNew( const gchar * title )
{
	gboolean loaded = FALSE;
	gchar * path_string = NULL;
	path_string = AL_CreateProject( title );
	if ( path_string )
	{
		Alchera_Loaders_Init( path_string );
		loaded = TRUE;
	}

	if ( !loaded )
	{
		Meg_Error_Print( __func__, __LINE__, "Could not create '%s'.", title);
	}
	return loaded;
}

/********************************
* Alchera_Loaders_Close
*
*/
gboolean Alchera_Loaders_Close(  )
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

		g_free(project_file_path);
		project_file_path = NULL;

		return TRUE;
	}




	return FALSE;

}

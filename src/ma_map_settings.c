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
#include "ma_map_edit.h"
#include "ma_map_settings.h"
#include "data/runtime_options.h"
#include "data/entity_editor.h"

/* GtkWidget */


/* Global Variables */

/* External Functions */
gboolean AL_Map_ContructRuntimeWidget( MapInfo * map_info, GtkWidget * box_runtime );

/* UI */




/********************************
* Meg_MapSettings_Refresh
*
*/
void Meg_MapSettings_Refresh( GtkWidget * map_widget )
{
	MapEditAdvanceWidget * settings_widgets = NULL;

	g_return_if_fail( map_widget != NULL );
	g_return_if_fail( GTK_IS_ALCHERA_MAP(map_widget) );

	settings_widgets = g_object_get_data( G_OBJECT(map_widget), "setting-widget" );

	if ( settings_widgets )
	{
		Meg_MapSettings_Centerview_Get( GTK_TOGGLE_BUTTON(settings_widgets->check_centerview), map_widget );
		Meg_MapSettings_Independent_Get( GTK_TOGGLE_BUTTON(settings_widgets->check_independent), map_widget  );
		Meg_MapSettings_Wrap_Get( GTK_COMBO_BOX(settings_widgets->combo_wrapmode), map_widget );

		Meg_MapSettings_Width_Get( GTK_SPIN_BUTTON(settings_widgets->spin_w), map_widget );
		Meg_MapSettings_Height_Get( GTK_SPIN_BUTTON(settings_widgets->spin_h), map_widget );

		Meg_MapSettings_Colour_Get( GTK_COLOR_BUTTON(settings_widgets->button_colour), map_widget );
	}
}

/********************************
* Meg_MapSettings_Create
*
*/
void Meg_MapSettings_Create( GtkWidget * map_widget, MapEditAdvanceWidget * settings_widgets )
{
	g_return_if_fail( map_widget != NULL );
	g_return_if_fail( GTK_IS_ALCHERA_MAP(map_widget) );
	g_return_if_fail( settings_widgets != NULL );

	Meg_MapSettings_Centerview_Get( GTK_TOGGLE_BUTTON(settings_widgets->check_centerview), map_widget );
	Meg_MapSettings_Independent_Get( GTK_TOGGLE_BUTTON(settings_widgets->check_independent), map_widget  );
	Meg_MapSettings_Wrap_Get( GTK_COMBO_BOX(settings_widgets->combo_wrapmode), map_widget );

	Meg_MapSettings_Width_Get( GTK_SPIN_BUTTON(settings_widgets->spin_w), map_widget );
	Meg_MapSettings_Height_Get( GTK_SPIN_BUTTON(settings_widgets->spin_h), map_widget );

	Meg_MapSettings_Colour_Get( GTK_COLOR_BUTTON(settings_widgets->button_colour), map_widget );

	AL_Map_ContructRuntimeWidget( gtk_alchera_map_get_info( GTK_ALCHERA_MAP(map_widget) ), settings_widgets->box_runtime );

	g_object_set_data( G_OBJECT(map_widget), "setting-widget", settings_widgets );

	g_signal_connect( G_OBJECT(settings_widgets->check_centerview), "toggled", G_CALLBACK(Meg_MapSettings_Centerview_Set), map_widget );
	g_signal_connect( G_OBJECT(settings_widgets->check_independent), "toggled", G_CALLBACK(Meg_MapSettings_Independent_Set), map_widget );
	g_signal_connect( G_OBJECT(settings_widgets->combo_wrapmode), "changed", G_CALLBACK(Meg_MapSettings_Wrap_Set), map_widget );
	g_signal_connect( G_OBJECT(settings_widgets->spin_h), "value-changed", G_CALLBACK(Meg_MapSettings_Height_Set), map_widget );
	g_signal_connect( G_OBJECT(settings_widgets->spin_w), "value-changed", G_CALLBACK(Meg_MapSettings_Width_Set), map_widget );
	g_signal_connect( G_OBJECT(settings_widgets->button_colour), "color-set", G_CALLBACK(Meg_MapSettings_Colour_Set), map_widget );

}






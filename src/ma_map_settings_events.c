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
#include "ma_map_edit.h"
#include "ma_map_settings.h"
#include "data/entity_options.h"
#include "data/entity_editor.h"

/* GtkWidget */


/* Global Variables */

/* External Functions */

/* UI */






/********************************
* Meg_MapSettings_Width_Set
* Event:
* Result: Updates Map Width
*/
void Meg_MapSettings_Width_Set( GtkSpinButton * spinbutton, GtkWidget * map_widget )
{
	g_return_if_fail( map_widget );
	g_return_if_fail( GTK_IS_ALCHERA_MAP(map_widget) );

	MapInfo * map_info = gtk_alchera_map_get_info( GTK_ALCHERA_MAP(map_widget) );

	map_info->width = gtk_spin_button_get_value_as_int( spinbutton );
	gtk_alchera_map_refresh( GTK_ALCHERA_MAP(map_widget) );
}

/********************************
* Meg_MapSettings_Height_Set
* Event:
* Result: Updates Map Height
*/
void Meg_MapSettings_Height_Set( GtkSpinButton * spinbutton, GtkWidget * map_widget )
{
	g_return_if_fail( map_widget );
	g_return_if_fail( GTK_IS_ALCHERA_MAP(map_widget) );

	MapInfo * map_info = gtk_alchera_map_get_info( GTK_ALCHERA_MAP(map_widget) );

	map_info->height = gtk_spin_button_get_value_as_int( spinbutton );
	gtk_alchera_map_refresh( GTK_ALCHERA_MAP(map_widget) );
}

/********************************
* Meg_MapSettings_Centerview_Set
* Event:
* Result:
*/
void Meg_MapSettings_Centerview_Set( GtkToggleButton * togglebutton, GtkWidget * map_widget )
{
	g_return_if_fail( map_widget );
	g_return_if_fail( GTK_IS_ALCHERA_MAP(map_widget) );

	MapInfo * map_info = gtk_alchera_map_get_info( GTK_ALCHERA_MAP(map_widget) );

	EntityOption_UpdateBoolean( map_info->settings, "centerview", gtk_toggle_button_get_active( togglebutton ), "hidden" );
}

/********************************
* Meg_MapSettings_Independent_Set
* Event:
* Result:
*/
void Meg_MapSettings_Independent_Set( GtkToggleButton * togglebutton, GtkWidget * map_widget )
{
	g_return_if_fail( map_widget );
	g_return_if_fail( GTK_IS_ALCHERA_MAP(map_widget) );

	MapInfo * map_info = gtk_alchera_map_get_info( GTK_ALCHERA_MAP(map_widget) );

	EntityOption_UpdateBoolean( map_info->settings, "independent", gtk_toggle_button_get_active( togglebutton ), "hidden" );
}

/********************************
* Meg_MapSettings_Wrap_Set
* Event:
* Result:
*/
void Meg_MapSettings_Wrap_Set( GtkComboBox * widget, GtkWidget * map_widget )
{
	g_return_if_fail( map_widget );
	g_return_if_fail( GTK_IS_ALCHERA_MAP(map_widget) );

	MapInfo * map_info = gtk_alchera_map_get_info( GTK_ALCHERA_MAP(map_widget) );

	EntityOption_UpdateValue( map_info->settings, "wrap", gtk_combo_box_get_active( GTK_COMBO_BOX(widget) ), "hidden" );
}



/********************************
* Meg_MapSettings_Width_Get
* Event:
* Result: Updates Map Width
*/
void Meg_MapSettings_Width_Get( GtkSpinButton * spinbutton, GtkWidget * map_widget )
{
	g_return_if_fail( map_widget );
	g_return_if_fail( GTK_IS_ALCHERA_MAP(map_widget) );

	MapInfo * map_info = gtk_alchera_map_get_info( GTK_ALCHERA_MAP(map_widget) );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spinbutton), (gdouble)map_info->width );
}

/********************************
* Meg_MapSettings_Height_Get
* Event:
* Result: Updates Map Height
*/
void Meg_MapSettings_Height_Get( GtkSpinButton * spinbutton, GtkWidget * map_widget )
{
	g_return_if_fail( map_widget );
	g_return_if_fail( GTK_IS_ALCHERA_MAP(map_widget) );

	MapInfo * map_info = gtk_alchera_map_get_info( GTK_ALCHERA_MAP(map_widget) );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(spinbutton), (gdouble)map_info->height );
}

/********************************
* Meg_MapSettings_Centerview_Get
* Event:
* Result:
*/
void Meg_MapSettings_Centerview_Get( GtkToggleButton * togglebutton, GtkWidget * map_widget )
{
	g_return_if_fail( map_widget );
	g_return_if_fail( GTK_IS_ALCHERA_MAP(map_widget) );

	MapInfo * map_info = gtk_alchera_map_get_info( GTK_ALCHERA_MAP(map_widget) );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(togglebutton), EntityOption_BooleanCheck( map_info->settings, "centerview" ) );
}

/********************************
* Meg_MapSettings_Independent_Get
* Event:
* Result:
*/
void Meg_MapSettings_Independent_Get( GtkToggleButton * widget, GtkWidget * map_widget )
{
	g_return_if_fail( map_widget );
	g_return_if_fail( GTK_IS_ALCHERA_MAP(map_widget) );

	MapInfo * map_info = gtk_alchera_map_get_info( GTK_ALCHERA_MAP(map_widget) );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(widget), EntityOption_BooleanCheck( map_info->settings, "independent" )  );
}

/********************************
* Meg_MapSettings_Wrap_Get
* Event:
* Result:
*/
void Meg_MapSettings_Wrap_Get(GtkComboBox * widget, GtkWidget *map_widget )
{
	g_return_if_fail( map_widget );
	g_return_if_fail( GTK_IS_ALCHERA_MAP(map_widget) );

	MapInfo * map_info = gtk_alchera_map_get_info( GTK_ALCHERA_MAP(map_widget) );
	gtk_combo_box_set_active( GTK_COMBO_BOX(widget), EntityOption_GetValue( map_info->settings, "wrap" ) );
}


/********************************
* Meg_MapSettings_OpenEntity
* Event:
* Result:
*/
void Meg_MapSettings_OpenEntity( GtkButton * widget, GtkWidget * map_widget )
{
	g_return_if_fail( map_widget );
	g_return_if_fail( GTK_IS_ALCHERA_MAP(map_widget) );

	MapInfo * map_info = gtk_alchera_map_get_info( GTK_ALCHERA_MAP(map_widget) );
	EntityEditor_New( MAP_DATA(map_info)->entity_filename );

}


/********************************
* Event: Colour Button Changed
* Result: Set New background colour
*/
void Meg_MapSettings_Colour_Set( GtkColorButton * widget, GtkWidget * map_widget  )
{

	g_return_if_fail( map_widget );
	g_return_if_fail( GTK_IS_ALCHERA_MAP(map_widget) );

	MapInfo * map_info = gtk_alchera_map_get_info( GTK_ALCHERA_MAP(map_widget) );

	Meg_ColorButton_GetColor( widget, &map_info->colour ); /* FIX: GTK3 */

	gtk_alchera_map_refresh( GTK_ALCHERA_MAP(map_widget) );
}

/********************************
* Event: Colour Button Changed
* Result: Set New background colour
*/
void Meg_MapSettings_Colour_Get(GtkColorButton *widget,  GtkWidget *map_widget )
{
	g_return_if_fail( map_widget );
	g_return_if_fail( GTK_IS_ALCHERA_MAP(map_widget) );

	MapInfo * map_info = gtk_alchera_map_get_info( GTK_ALCHERA_MAP(map_widget) );

	Meg_ColorButton_SetColor( widget, &map_info->colour );
}

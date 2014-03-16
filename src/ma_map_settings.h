/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef MA_MAP_SETTINGS
#define MA_MAP_SETTINGS

#include "global.h"

typedef struct
{
	GtkWidget * spin_w, * spin_h, * button_colour;
	GtkWidget * combo_wrapmode, * check_centerview, *check_independent, * box_runtime;
} MapEditAdvanceWidget;



void Meg_MapSettings_Refresh( GtkWidget * map_widget );
void Meg_MapSettings_Create(GtkWidget * map_widget , MapEditAdvanceWidget *settings_widgets);

void Meg_MapSettings_OpenEntity( GtkButton * widget, GtkWidget * map_widget );
void Meg_MapSettings_GraphicSwitch( GtkButton * widget, GtkWidget * map_widget );

void Meg_MapSettings_Width_Get( GtkSpinButton * spinbutton, GtkWidget * map_widget );
void Meg_MapSettings_Height_Get( GtkSpinButton * spinbutton, GtkWidget * map_widget );
void Meg_MapSettings_Centerview_Get( GtkToggleButton * togglebutton, GtkWidget * map_widget );
void Meg_MapSettings_Independent_Get( GtkToggleButton * togglebutton, GtkWidget * map_widget );
void Meg_MapSettings_Wrap_Get( GtkComboBox * widget, GtkWidget * map_widget );

void Meg_MapSettings_Width_Set(GtkSpinButton * spinbutton, GtkWidget * map_widget );
void Meg_MapSettings_Height_Set( GtkSpinButton * spinbutton, GtkWidget * map_widget );
void Meg_MapSettings_Centerview_Set( GtkToggleButton * togglebutton, GtkWidget * map_widget );
void Meg_MapSettings_Independent_Set( GtkToggleButton * togglebutton, GtkWidget * map_widget );
void Meg_MapSettings_Wrap_Set( GtkComboBox * widget, GtkWidget * map_widget );


void Meg_MapSettings_Colour_Get( GtkColorButton * widget, GtkWidget * map_widget );
void Meg_MapSettings_Colour_Set( GtkColorButton * widget, GtkWidget * map_widget );



#endif

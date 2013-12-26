#define GUIABOUT_WINDOW "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<interface>\n  <object class=\"GtkAboutDialog\" id=\"alchera_about_dialog\">\n\t<property name=\"can_focus\">False</property>\n\t<property name=\"events\">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>\n\t<property name=\"border_width\">5</property>\n\t<property name=\"title\" translatable=\"yes\">About Meg</property>\n\t<property name=\"modal\">True</property>\n\t<property name=\"window_position\">center-on-parent</property>\n\t<property name=\"destroy_with_parent\">True</property>\n\t<property name=\"type_hint\">dialog</property>\n\t<property name=\"program_name\">Meg</property>\n\t<property name=\"version\">0.95</property>\n\t<property name=\"copyright\" translatable=\"yes\">Copyright \302\251 2007-2013 Luke Salisbury</property>\n\t<property name=\"comments\" translatable=\"yes\">Mokoi Game Editor (GTK)</property>\n\t<property name=\"website\">http://mokoi.info/</property>\n\t<property name=\"website_label\" translatable=\"yes\">mokoi.info</property>\n\t<property name=\"license\" translatable=\"yes\">This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.\n\n\nPermission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:\n\n1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.\n2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.\n3. This notice may not be removed or altered from any source distribution.</property>\n\t<property name=\"authors\">Luke Salisbury</property>\n\t<property name=\"documenters\"></property>\n\t<property name=\"artists\">Luke Salisbury, Tango Project</property>\n\t<property name=\"logo_icon_name\">mokoi-game-editor</property>\n\t<property name=\"wrap_license\">True</property>\n\t<signal name=\"close\" handler=\"gtk_widget_destroy\" swapped=\"no\"/>\n\t<signal name=\"response\" handler=\"gtk_widget_destroy\" swapped=\"no\"/>\n\t<child internal-child=\"vbox\">\n\t  <object class=\"GtkVBox\" id=\"dialog-vbox1\">\n\t\t<property name=\"visible\">True</property>\n\t\t<property name=\"can_focus\">False</property>\n\t\t<property name=\"events\">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>\n\t\t<property name=\"orientation\">vertical</property>\n\t\t<property name=\"spacing\">2</property>\n\t\t<child internal-child=\"action_area\">\n\t\t  <object class=\"GtkButtonBox\" id=\"dialog-action_area1\">\n\t\t\t<property name=\"visible\">True</property>\n\t\t\t<property name=\"can_focus\">False</property>\n\t\t\t<property name=\"events\">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>\n\t\t\t<property name=\"layout_style\">end</property>\n\t\t  </object>\n\t\t  <packing>\n\t\t\t<property name=\"expand\">False</property>\n\t\t\t<property name=\"fill\">True</property>\n\t\t\t<property name=\"pack_type\">end</property>\n\t\t\t<property name=\"position\">0</property>\n\t\t  </packing>\n\t\t</child>\n\t\t<child>\n\t\t  <placeholder/>\n\t\t</child>\n\t  </object>\n\t</child>\n  </object>\n</interface>\n";

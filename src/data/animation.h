/****************************
Copyright © 2012-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef ANIMATION_H
#define ANIMATION_H

#include "loader_structs.h"
#include "ma_types.h"

typedef struct {
	guint length_ms;
	GdkPoint offset;
	GdkPixbuf * sprite;
} AnimationPreviewFrame;

typedef struct {
	GList * frames; /* <AnimationPreviewFrame> */
	GtkWidget * widget;
	guint timer;
	guint current;
	guint id;
	gchar * parent;
} AnimationPreview;

gboolean Animation_AdvanceDialog( Spritesheet *sheet, SheetObject * sprite );

void Animation_ChangeFrameLength( GtkSpinButton * widget, SheetObject * sprite );
void Animation_ChangePosition( GtkCellRendererSpin *renderer , gchar * path, gchar * new_text, GtkListStore * store);

gboolean Animation_Preview_Pressed( GtkWidget * widget, GdkEventButton * event, SheetObject * sprite );
void Animation_Preview_Play( GtkButton *button, SheetObject * sprite);
gboolean Animation_Preview_Update( GtkWidget * widget, GdkEventExpose *event, AnimationPreview * preview );
gboolean Animation_Preview_Draw(GtkWidget * widget, cairo_t * cr, AnimationPreview * preview );

void Animation_AddPreview( GtkComboBox * combo, GtkWidget * image );
void Animation_AddSelected( GtkMenuItem * menuitem, GtkWidget * widget );

void Animation_RemoveSelected( GtkMenuItem * menuitem, GtkWidget * widget );
gboolean Animation_PopupMenu( GtkWidget * widget, GtkListStore * store ) ;
gboolean Animation_MouseMenu( GtkWidget * widget, GdkEventButton * event, GtkListStore * store );
gboolean Animation_Tooltip( GtkWidget *widget, gint x, gint y, gboolean keyboard_tip, GtkTooltip * tooltip, gpointer data);


#endif // ANIMATION_H

/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef MA_QUESTION
#define MA_QUESTION




	#if defined(USE_SOUP)

		#include <libsoup/soup.h>
		#include <libsoup/soup-method.h>

		void Meg_Questions_Get( gint ident );
		void Meg_Questions_GetAll( GtkListStore * store );

		void Meg_Questions_DisplayItem( GtkTreeView * tree_view, GtkTreeViewColumn *column, GtkTreePath *path, gpointer data );
	#endif

	gboolean MegWidget_Questions_Destroy(GtkWidget * widget, GdkEvent * event, gpointer user_data );
	void MegWidget_Questions_Refresh(GtkWidget * widget, gpointer user_data);

#endif



/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Standard Headers */
#include "loader_global.h"
#include "loader_functions.h"

/* Required Headers */
#include "audio_playback.h"


/* External Functions */

/* Local Type */

/* Global Variables */


/* Local Variables */

/* UI */



/********************************
* Audio_Chooser_Play
* Play select file from the Add File Chooser.
@ button:
@ chooser:
*/
void Audio_Chooser_Play( GtkButton * button, GtkFileChooser * chooser )
{
	GtkTreeIter iter;
	gchar * file = gtk_file_chooser_get_filename( chooser );
	if ( file )
	{
		Audio_Music_Play( file, NULL, iter );
	}
}




/********************************
* Audio_Chooser_AddFilter
*
@ file_chooser:
@ title:
@ pattern:
*/
void Audio_Chooser_AddFilter( GtkWidget * file_chooser, const gchar * title, const gchar * pattern )
{
	gchar ** patterns;
	gint i;

	GtkFileFilter * f = gtk_file_filter_new();
	gtk_file_filter_set_name(f, title);

	patterns = g_strsplit(pattern, ";", -1);
	for(i = 0; patterns[i] != NULL; i++)
		gtk_file_filter_add_pattern(f, patterns[i]);
	g_strfreev(patterns);
	gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(file_chooser), f );
}



/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Standard Headers */
#include "loader_global.h"

/* Required Headers */

/* External Functions */

/* Local Type */

/* Global Variables */

/* Local Variables */

/* UI */

/* Events */


/********************************
* AL_GetImage
* Returns 'name' GdkPixbuf
@ name: file name
*/
GdkPixbuf * AL_GetImage( gchar * name, gint64 * file_size )
{
	GdkPixbuf * sheet_image = NULL;
	gchar * file_name;
	GError * error = NULL;

	file_name = g_strdup_printf("/sprites/%s", name );
	sheet_image = Megdk_pixbuf_new_from_file( file_name, &error );

	if ( error )
	{
		Meg_Error_Print( __func__, __LINE__, "Error '%s'.", error->message );
	}

	if ( !sheet_image )
	{
		sheet_image = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, 40, 40 );
		gdk_pixbuf_fill( sheet_image, 0xFF0000FF );
	}

	if ( file_size != NULL )
	{
		*file_size = Meg_file_size(file_name);
	}

	g_free(file_name);

	return sheet_image;
}

/********************************
* AL_GetSprite
* Returns 'name' sprite
@ name: sprite name
*/
GdkPixbuf * AL_GetSprite( gchar * name )
{
	if ( !name )
	{
		return NULL;
	}

	GdkPixbuf * sprite_image = NULL;
	gchar ** ident_split = g_strsplit_set( name, ":", 2 );

	if ( ident_split )
	{
		sprite_image = Sprite_GetPixbuf( ident_split[1], ident_split[0] );
	}

	if ( !ident_split || !sprite_image )
	{
		sprite_image = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, 8, 8 );
		gdk_pixbuf_fill( sprite_image, 0xFF0000FF );
	}

	g_strfreev( ident_split );

	return sprite_image;
}

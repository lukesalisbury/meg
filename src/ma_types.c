/****************************
Copyright © 2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Global Header */
#include "ma_types.h"


/* Local Functions */


/* Global Variables */


/* UI */


/* Functions */


/********************************
* SheetObject_New
*
*/
SheetObject * SheetObject_New( gpointer data, gboolean (*free)(gpointer) )
{
	SheetObject * object = g_new0(SheetObject, 1);

	if ( data )
	{
		object->data = data;
		object->free = free;
	}

	return object;
}

/********************************
* SheetObject_Clear
*
*/
gboolean SheetObject_Clear( SheetObject * object )
{
	gboolean result = FALSE;
	if ( object )
	{
		CLEAR_STRING( object->display_name );
		CLEAR_STRING( object->ident_string );
		CLEAR_STRING( object->parent_sheet );
	}
	return result;
}

/********************************
* SheetObject_Free
*
*/
gboolean SheetObject_Free( SheetObject * object )
{
	gboolean result = FALSE;
	if ( object )
	{
		CLEAR_STRING( object->display_name );
		CLEAR_STRING( object->ident_string );
		CLEAR_STRING( object->parent_sheet );

		if ( object->free )
		{
			result = (*object->free)( object->data );
		}
		else if ( object->data )
		{
			g_free( object->data );
			result = TRUE;
		}

		g_free( object );
	}
	return result;
}

/********************************
* SheetObject_Ref
*
*/
SheetObject * SheetObject_Ref( SheetObject * object )
{
	if ( object )
	{
		object->ref++;
	}
	return object;
}

/********************************
* SheetObject_Unref
*
*/
SheetObject * SheetObject_Unref( SheetObject * object )
{
	if ( object )
	{
		object->ref--;
		if ( object->ref == 0 )
		{
			if ( SheetObject_Free(object) )
			{
				return NULL;
			}
		}
	}
	return object;
}

/********************************
* Spritesheet_New
*
*/
Spritesheet * Spritesheet_New( gpointer data, gboolean (*free)(gpointer) )
{
	Spritesheet * object = g_new0(Spritesheet, 1);

	if ( data )
	{
		object->data = data;
		object->free = free;
	}

	return object;
}

/********************************
* Spritesheet_Free
*
*/
gboolean Spritesheet_Free( Spritesheet * object )
{
	gboolean result = FALSE;
	if ( object )
	{
		if ( object->free )
		{
			result = object->free( object->data );
		}
		else if ( object->data )
		{
			g_free( object->data );
			result = TRUE;
		}

		g_free( object );
	}
	return result;
}

/********************************
* Spritesheet_Ref
*
*/
Spritesheet * Spritesheet_Ref( Spritesheet * object )
{
	if ( object )
	{
		object->ref++;
	}
	return object;
}

/********************************
* Spritesheet_Unref
*
*/
Spritesheet * Spritesheet_Unref( Spritesheet * object )
{
	if ( object )
	{
		object->ref--;
		if ( object->ref == 0 )
		{
			if ( Spritesheet_Free(object) )
			{
				return NULL;
			}
		}
	}
	return object;
}


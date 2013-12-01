/****************************
Copyright © 2007-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Standard Headers */
#include "global.h"

/* Required Headers */
#include <libsoup/soup.h>
#include <libsoup/soup-method.h>

/* External Functions */

/* Local Type */

/* Global Variables */

/* Local Variables */


/* UI */


/********************************
* Meg_Questions_Get
*
*/
void Meg_Questions_Get()
{
	guint status;
	SoupMessage * msg;
	SoupSession * session = NULL;

	session = soup_session_sync_new();

	msg = soup_xmlrpc_request_new( "http://openzelda.net/qa/xml-rpc", "q2a.listQuestion", G_TYPE_STRING, "test", G_TYPE_STRING, "test", G_TYPE_STRING, "sort", G_TYPE_STRING, "asc", NULL );
	status = soup_session_send_message( session, msg );
	g_print("Status: %d\nText: %s\n", status, msg->response_body->data );



}



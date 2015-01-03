/****************************
Copyright © 2007-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifdef USE_CURL

/* Global Header */
#include "global.h"
#include "ma_web.h"

#include <curl/curl.h>

struct MemoryStruct {
	char *memory;
	size_t size;
};


static size_t Meg_Curl_WriteMemory( void * contents, size_t size, size_t nmemb, void * userp )
{
	size_t realsize = size * nmemb;
	GString * content = (GString*)userp;

	g_string_append_len(content, contents, realsize);

	return realsize;
}

static size_t Meg_Curl_Write( void *ptr, size_t size, size_t nmemb, FILE * stream)
{
	return fwrite( ptr, size, nmemb, stream );
}

static int Meg_Curl_Progress( WebRetrieve * request, double t, double d, double ultotal, double ulnow)
{
/*  printf("%d / %d (%g %%)\n", d, t, d*100.0/t);*/

	if ( request->label )
	{
		/*
		gchar * string = g_strdup_printf("%.0f / %.0f (%g %%)\n", d, t, d*100.0/t );
		gtk_label_set_text( GTK_LABEL(request->label), string );
		g_free( string );
		*/
	}
	else
	{

	}


	return 0;
}


/*****************************
* Meg_Web_Enable
*
*/
gboolean Meg_Web_Enable( )
{
	return TRUE;
}


/*****************************
* Meg_Web_RetrieveFileOverwrite
*
*/
gboolean Meg_Web_RetrieveFileOverwrite( WebRetrieve * request, gchar * user, gchar * pass )
{
	CURLcode results = CURLE_UNSUPPORTED_PROTOCOL;
	CURL * curl = NULL;
	FILE * output_file = NULL;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();


	if ( curl )
	{
		output_file = fopen( request->filename, "wb");

		curl_easy_setopt( curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Meg)" );
		curl_easy_setopt( curl, CURLOPT_FOLLOWLOCATION, 1 );
		curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0 );

		curl_easy_setopt( curl, CURLOPT_URL, request->url );
		curl_easy_setopt( curl, CURLOPT_WRITEDATA, output_file );
		curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, Meg_Curl_Write );
		curl_easy_setopt( curl, CURLOPT_NOPROGRESS, 1);
		curl_easy_setopt( curl, CURLOPT_PROGRESSFUNCTION, Meg_Curl_Progress);
		curl_easy_setopt( curl, CURLOPT_PROGRESSDATA, request );

		results = curl_easy_perform(curl);

		/* Check for errors */
		if ( results != CURLE_OK )
		{
			Meg_Log_Print(LOG_NONE, "curl_easy_perform() failed: '%s'", curl_easy_strerror(results) );
		}
		curl_easy_cleanup(curl);

		fclose(output_file);
	}


	curl_global_cleanup();

	return (results = CURLE_OK);
}

/*****************************
* Meg_Web_RetrieveText
*
*/
gchar * Meg_Web_RetrieveText( WebRetrieve * request, gchar * user, gchar * pass )
{
	gchar * text = NULL;


	CURLcode results = CURLE_UNSUPPORTED_PROTOCOL;
	CURL * curl = NULL;
	GString * content = g_string_new("");


	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();


	if ( curl )
	{
		curl_easy_setopt( curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Meg)" );
		curl_easy_setopt( curl, CURLOPT_FOLLOWLOCATION, 1 );
		curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0 );

		curl_easy_setopt( curl, CURLOPT_URL, request->url );
		curl_easy_setopt( curl, CURLOPT_WRITEDATA, content );
		curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, Meg_Curl_WriteMemory );
		curl_easy_setopt( curl, CURLOPT_NOPROGRESS, 1);
		curl_easy_setopt( curl, CURLOPT_PROGRESSFUNCTION, Meg_Curl_Progress);
		curl_easy_setopt( curl, CURLOPT_PROGRESSDATA, request );

		results = curl_easy_perform(curl);

		/* Check for errors */
		if ( results != CURLE_OK )
		{
			Meg_Log_Print(LOG_NONE, "curl_easy_perform() failed: '%s'", curl_easy_strerror(results) );
		}
		else
		{
			text = g_strdup( content->str );
		}
		curl_easy_cleanup(curl);

	}


	curl_global_cleanup();

	g_string_free( content, TRUE );

	return text;

}

/*****************************
* Meg_Web_SendFile
*
*/
gboolean Meg_Web_SendFile( WebRetrieve * request, gchar * user, gchar * pass, gchar * file_name )
{
	return FALSE;
}

/*****************************
* Meg_Web_SendText
*
*/
gboolean Meg_Web_SendText( WebRetrieve * request, gchar * user, gchar * pass, gchar * text )
{
	return FALSE;
}


#endif


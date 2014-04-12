/****************************
Copyright (c) 2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <assert.h>
#include <string.h>
#include <glib.h>


int main( int argc, char *argv[] )
{
	char * args[] = {"mokoipawnc","--xmloutput", "-O./","test.mps", NULL};
	char *output = NULL;
	char *error = NULL;
	GError * err = NULL;

	args[0] = g_build_filename(g_path_get_dirname(argv[0] ), "mokoipawnc3", NULL);
	if ( g_spawn_sync( NULL, //working_directory
		args, //gchar **argv,
		NULL, //gchar **envp,
		G_SPAWN_SEARCH_PATH, //GSpawnFlags flags,
		NULL, //GSpawnChildSetupFunc child_setup,
		NULL, //gpointer user_data,
		&output, //gchar **standard_output,
		&error, //gchar **standard_error,
		NULL, //gint *exit_status,
		&err //GError **error
	))
	{
		g_print("output: %s\n", output);
		g_print("error: %s\n", error);
	}
	else
	{
		g_print("error: %s\n", err->message);
		g_error_free(err);
	}
	return 0;
}


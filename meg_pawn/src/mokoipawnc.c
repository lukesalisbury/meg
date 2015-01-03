/****************************
Copyright (c) 2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef MEGSHAREDIRECTORY
#define MEGSHAREDIRECTORY "mokoi-1.0"
#endif

//#include <assert.h>
#include <string.h>
#include <glib.h>
#include <gmodule.h>

#if PAWN == 4
int compiler_main( int argc, char *argv[], int (*error_handling)(int number, char * message, char *filename, int firstline, int lastline, va_list argptr) );
#else
int ( *compiler_main )( int argc, char *argv[], int (*error_handling)(int number, char * message, char *filename, int firstline, int lastline, va_list argptr) );
#endif

gboolean xml_output = FALSE;
gboolean ignore_errors = FALSE;

GString * xml_content = NULL;

GModule * compiler = NULL;
int fatal_error_count = 0;

gboolean use_original_filename = FALSE;

char * source_filepath = NULL;
char * source_filename = NULL;
char * source_directory = NULL;
char * temporary_filepath = NULL;

char * output_filename = NULL;
char * output_directory = NULL;
char * output_filename_64 = NULL;
char * output_filename_32 = NULL;

char * program_directory = NULL;
char * project_directory = NULL;
char * parent_directory = NULL;
char * routines_directory = NULL;
char * routines_global_directory = NULL;
char * include_directory = NULL;

char * filename = NULL;



int handling_error_code( int number, char * message, char *filename, int firstline, int lastline, va_list argptr )
{
	if ( ignore_errors )
	{
		return 0;
	}

	if ( !filename )
	{
		return 0;
	}

	static char * prefix[4] = { "error", "fatalerror", "warning", "" };
	char * vmsg = g_strdup_vprintf(message,argptr);
	char * source_file = filename;
	if ( number > 0 && number < 200 )
	{
		fatal_error_count++;
	}

	//Replace the temporary file name with the original
	if ( strcmp( source_filepath, filename ) == 0  )
	{
		if ( use_original_filename )
			source_file = source_filename;
	}

	if ( number > 0 && number < 300 )
	{
		char * pre = prefix[number/100];
		if ( xml_output )
		{
			char * output = NULL;

			if ( firstline >= 0 )
			{
				output = g_markup_printf_escaped(
							"<%s file=\"%s\" line=\"%d\" lastline=\"%d\" code=\"%d\" message=\"%s\"/>\n",
							pre, source_file, firstline, lastline, number, vmsg
							);
			}
			else
			{
				output = g_markup_printf_escaped(
							"<%s file=\"%s\" line=\"%d\" code=\"%d\" message=\"%s\"/>\n",
							pre, source_file, lastline, number, vmsg
							);
			}
			if ( output )
			{
				xml_content = g_string_append(xml_content, output);
			}
			g_free(output);
		}
		else
		{
			if ( firstline>=0)
				g_print("%s(%d -- %d) : %s %03d: ",source_file,firstline,lastline,pre,number);
			else
				g_print("%s(%d) : %s %03d: ",source_file,lastline,pre,number);
			g_print("%s", vmsg);
		}
	}
	g_free(vmsg);


	return 0;
}

void fix_path( gchar * str )
{
	/* Replace / with \ */
	gsize c = 0;
	while ( str[c] )
	{
		if ( str[c] == '/' )
			str[c] = G_DIR_SEPARATOR;
		c++;
	}
}

gchar * get_global_include_path( )
{
	gchar * directory = NULL;
	guint lc = 0;
	const gchar * const * data_dirs;

	data_dirs = g_get_system_data_dirs();

	while ( data_dirs[lc] != NULL && directory == NULL )
	{
		if ( g_str_has_suffix(data_dirs[lc], "share") || g_str_has_suffix(data_dirs[lc], "share/") )
		{
			gchar * include_path = g_build_path(G_DIR_SEPARATOR_S, data_dirs[lc], MEGSHAREDIRECTORY, "include", NULL );
			if ( g_file_test(include_path, G_FILE_TEST_IS_DIR ))
			{
				directory = g_strdup( include_path );
			}
			g_free(include_path);
		}
		lc++;
	}

	if ( !directory )
	{
		gchar * include_path = g_build_path(G_DIR_SEPARATOR_S, program_directory, "..", "share", MEGSHAREDIRECTORY, "include", NULL );
		if ( g_file_test(include_path, G_FILE_TEST_IS_DIR ))
		{
			directory = g_strdup( include_path );
		}
		g_free(include_path);

	}

	return directory;
}


void main_args( int argc, char *argv[])
{
	int c = argc;
	while ( c > 1 ) {
		--c;
		if ( argv[c][0] != '-' )
		{
			source_filepath = g_strdup(argv[c]);
			fix_path( source_filepath );

			source_directory = g_path_get_dirname(source_filepath);
			source_filename = g_path_get_basename(source_filepath);

			if ( parent_directory )
			{
				routines_directory = g_build_path( G_DIR_SEPARATOR_S, parent_directory, source_directory, "routines", NULL );
				if ( g_str_has_suffix(source_directory, G_DIR_SEPARATOR_S"maps") )
				{
					routines_global_directory = g_build_path( G_DIR_SEPARATOR_S, parent_directory, source_directory, "..", "routines", NULL );
				}
			}
			else
			{
				routines_directory = g_build_path( G_DIR_SEPARATOR_S, source_directory, "routines", NULL );
				if ( g_str_has_suffix(source_directory, G_DIR_SEPARATOR_S"maps") )
				{
					routines_global_directory = g_build_path( G_DIR_SEPARATOR_S, source_directory, "..", "routines", NULL );
				}
			}
		}
		else if ( !g_strcmp0("--xmloutput", argv[c]) )
		{
			xml_output = TRUE;
		}
		else if ( g_str_has_prefix( argv[c], "--temporary=" ) )
		{
			char * dir = argv[c] + 12;
			temporary_filepath = g_strdup(dir);
		}
		else if ( g_str_has_prefix( argv[c], "--project=" ) )
		{
			char * dir = argv[c] + 10;
			project_directory = g_strdup(dir);
		}
		else if ( g_str_has_prefix( argv[c], "--parent=" ) )
		{
			char * dir = argv[c] + 9;
			parent_directory = g_strdup(dir);
		}
	}
}

void print_args( int argc, char *argv[])
{

	int c = argc;
	while ( c > 1 ) {
		--c;
		g_print("%d: %s\n", c, argv[c] );
	}
}


// Strip file extension
char * strip_file_extension( char * input_filename )
{
	char * output_filename = g_strdup( input_filename );
	char * dotpos = g_utf8_strrchr( output_filename, -1, '.' );
	if ( dotpos )
	{
		*dotpos = '\0';
	}

	return output_filename;
}

/*
 pawn_compiler $file --temporary=$file --project=$dir
 --temporary use this file instead.
 --project base of the project.
*/


int main( int argc, char *argv[] )
{
	int arg_c = 6;
	char * arg_v[7];

	//0: Current executable
	program_directory = g_path_get_dirname(argv[0]);
	arg_v[0] = g_strdup(argv[0]);

	// Get Global include path
	include_directory = get_global_include_path();
	if ( !include_directory )
	{
		include_directory = g_strdup(".");
	}

	// Scan arguments
	main_args(argc, argv);
	if ( !source_filepath )
	{
		g_print("Mokoi Pawn Compiler\n");
		g_print("Headers located at %s\n", get_global_include_path() );

		g_print("No file given\n");
//		g_print("--temporary use this file instead.\n");
//		g_print("--project base of the project.\n");
		g_print("\n");
		return 0;
	}

	// Select which file to compile
	if ( temporary_filepath )
	{
		filename = temporary_filepath;
	}
	else
	{
		filename = source_filepath;
	}

	// Set output filename
	output_filename = strip_file_extension( source_filename );

	if ( project_directory )
	{
		if ( g_str_has_prefix( source_filepath, project_directory ) )
		{
			char * dir = source_directory + strlen(project_directory);
			output_directory = g_build_path(G_DIR_SEPARATOR_S, project_directory, "c", dir, NULL);
		}
		else if (source_directory)
		{
			output_directory = g_build_path(G_DIR_SEPARATOR_S, project_directory, "c", source_directory, NULL);
		}
		else
		{
			output_directory = g_strdup(project_directory);
		}
	}
	else
	{
		output_directory = g_strdup(source_directory);
	}

	g_mkdir_with_parents( output_directory, 0755 );

	output_filename_64 = g_strdup_printf( "%s.amx64", output_filename );
	output_filename_32 = g_strdup_printf( "%s.amx", output_filename );


	if ( xml_output )
	{
		xml_content = g_string_new("");
		g_string_printf(xml_content, "<results file=\"%s\">\n", source_filename);
	}
	else
	{
		g_print( "Saving to %s %s\n", output_directory, output_filename_32 );
	}

	arg_v[1] = g_strdup( filename ); //1: source file
	arg_v[2] = g_strconcat( "-i", include_directory, NULL ); //2: global header path
	arg_v[3] = g_strconcat( "-i", routines_directory, NULL ); //3: project header path

	/*
	g_print( "filename %s\n", filename );
	g_print( "source_filepath %s\n", source_filepath );
	g_print( "source_filename %s\n", source_filename );
	g_print( "source_directory %s\n", source_directory );
	g_print( "temporary_filepath %s\n", temporary_filepath );
	g_print( "output_filename %s\n", output_filename );
	g_print( "output_directory %s\n", output_directory );
	g_print( "output_filename_64 %s\n", output_filename_64 );
	g_print( "output_filename_32 %s\n", output_filename_32 );
	g_print( "program_directory %s\n", program_directory );
	g_print( "project_directory %s\n", project_directory );
	g_print( "routines_directory %s\n", routines_directory );
	g_print( "routines_global_directory %s\n", routines_global_directory );
	g_print( "include_directory %s\n", include_directory );
	*/

	if ( routines_global_directory )
	{
		arg_c++;
		arg_v[6] = g_strconcat( "-i", routines_global_directory, NULL ); //3: project header path
	}

#if PAWN == 4

	//4: output file
	arg_v[4] = g_strconcat( "-o", output_directory, G_DIR_SEPARATOR_S, output_filename_32, NULL );
	arg_v[5] = g_strdup("-C32");

	//print_args(arg_c, arg_v);
	compiler_main(arg_c, arg_v, handling_error_code);

	g_free(arg_v[4]);
	g_free(arg_v[5]);

	if ( !fatal_error_count )
	{
		//4: output file
		arg_v[4] = g_strconcat( "-o", output_directory, G_DIR_SEPARATOR_S, output_filename_64, NULL );
		arg_v[5] = g_strdup("-C64");

		//print_args(arg_c, arg_v);
		compiler_main(arg_c, arg_v, handling_error_code);

		g_free(arg_v[4]);
		g_free(arg_v[5]);
	}



#else
/*
	arg_v[5] = g_strdup("-d1");
	gchar * mod_path;

	mod_path = g_module_build_path(g_path_get_dirname(argv[0]),"mokoipawn32");
	compiler = g_module_open(mod_path, G_MODULE_BIND_LAZY);

	if ( compiler )
	{
		if ( g_module_symbol( compiler, "compiler_main", (gpointer *)&compiler_main) )
		{
			if ( !xml_output )
			{
				g_print("Pawn Compiler (32bit)\n");
			}
			//4: output file
			arg_v[4] = g_strconcat( "-o", output_directory, G_DIR_SEPARATOR_S, output_filename, ".amx", NULL );

			//Compile file
			compiler_main(arg_c, arg_v, handling_error_code);

		}
		else
			g_warning("32bit Compiler not found\n%s", g_module_error() );
		g_module_close( compiler );
	}
	else
	{
		g_warning("32bit Compiler not found %s %d\n%s", mod_path, g_file_test(mod_path,G_FILE_TEST_EXISTS), g_module_error() );
	}


	mod_path = g_module_build_path(g_path_get_dirname(argv[0]),"mokoipawn64");
	compiler = g_module_open(mod_path, G_MODULE_BIND_LAZY);
	if ( compiler )
	{
		if ( g_module_symbol( compiler, "compiler_main", (gpointer *)&compiler_main) )
		{
			if ( arg_v[4] )
				g_free(arg_v[4]);

			if ( !xml_output )
			{
				g_print("Pawn Compiler (64bit)\n");
			}
			//4: output file
			arg_v[4] = g_strconcat( "-o", output_directory, G_DIR_SEPARATOR_S, output_filename, ".amx64", NULL );

			//Compile file
			ignore_errors = TRUE;
			compiler_main(arg_c, arg_v, handling_error_code);


		}
		else
			g_warning("64bit Compiler not found\n%s", g_module_error() );
		g_module_close( compiler );
	}
	else
	{
		g_warning("64bit Compiler not found %s %d\n%s", mod_path, g_file_test(mod_path,G_FILE_TEST_EXISTS), g_module_error() );
	}

*/
#endif
	if ( xml_output )
	{
		xml_content = g_string_append(xml_content, "</results>\n");
		if ( xml_content )
			g_print( "%s", xml_content->str );
		else
			g_print("<error source=\"%s\"/>\n", source_filepath);

	}


	g_free(arg_v[0]);
	g_free(arg_v[1]);
	g_free(arg_v[2]);
	g_free(arg_v[3]);
	
	return 0;
}



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
#include "loader_functions.h"
#include "entity_functions.h"
#include "game_compiler.h"
#include "logger_functions.h"
#include <glib/gstdio.h>
#if (GTK_MAJOR_VERSION == 2)
#include "gdkkeysyms-backward.h"
#endif

#ifdef G_OS_WIN32
#include <io.h>			/* For read(), write() etc */
#endif


#if PAWN_VERSION == 4
#define PAWN_COMPILER "pawn_compiler4"
#else
#define PAWN_COMPILER "pawn_compiler3"
#endif

typedef struct {
	gchar * filename;
	GtkWidget * log;
	GtkWidget * text;
	gboolean error;
} EntityErrorInfo;

/* External Functions */
void entity_error_end_element(GMarkupParseContext *context, const gchar * element_name, gpointer user_data, GError **error);
void entity_error_start_element(GMarkupParseContext * context, const gchar * element_name, const gchar ** attribute_names, const gchar ** attribute_values, gpointer user_data, GError ** error);

/* Global Variables */
extern gchar * mokoiBasePath;
extern GtkWidget * mokoiCompileLog;
extern gboolean mokoiUsingPackage;

/* Local Variables */
ProgressDialogWidgets mokoiEntityRebuild;
static GMarkupParser entity_error_parser = {
		entity_error_start_element,
		entity_error_end_element,
		NULL,
		NULL,
		NULL
};

/* UI */

const gchar * mokoiUI_EntityRebuild = GUI_PROGRESS_DIALOG;

/* Events */

/* Functions */

/*
 *error handling
 *<results>
 * <error file="%s" line="%d\" lastline=\"%d\" code=\"%d\" message=\"%s\"/>
 * <fatalerror file="%s" line="%d\" lastline=\"%d\" code=\"%d\" message=\"%s\"/>
 * <warning file="%s" line="%d\" lastline=\"%d\" code=\"%d\" message=\"%s\"/>
 *</results>
*/

void entity_error_start_element( GMarkupParseContext * context, const gchar * element_name, const gchar ** attribute_names, const gchar ** attribute_values, gpointer             user_data, GError ** error)
{
	EntityErrorInfo * eeinfo = (EntityErrorInfo*)user_data;
	const gchar **name_cursor = attribute_names;
	const gchar **value_cursor = attribute_values;

	if ( !g_ascii_strcasecmp(element_name, "results") )
	{
		
		while (*name_cursor)
		{
			if ( !g_ascii_strcasecmp(*name_cursor, "file") )
				eeinfo->filename = g_strdup(*value_cursor);
			name_cursor++;
			value_cursor++;
		}
		g_print("Compiling '%s'\n", eeinfo->filename );
	}

	if ( !g_ascii_strcasecmp(element_name, "error") || !g_ascii_strcasecmp(element_name, "fatalerror") || !g_ascii_strcasecmp(element_name, "warning") )
	{
		gint number = 0;
		const gchar * message = NULL;
		const gchar * filename = NULL;

		gint line = 0;
		gint lastline = -1;
		while (*name_cursor)
		{
			if ( !g_ascii_strcasecmp(*name_cursor, "file") )
				filename = *value_cursor;
			else if ( !g_ascii_strcasecmp(*name_cursor, "message") )
				message = *value_cursor;
			else if ( !g_ascii_strcasecmp(*name_cursor, "line") )
				line = g_ascii_strtoll( *value_cursor, NULL, 10);
			else if ( !g_ascii_strcasecmp(*name_cursor, "lastline") )
				lastline = g_ascii_strtoll( *value_cursor, NULL, 10);
			else if ( !g_ascii_strcasecmp(*name_cursor, "code") )
				number = g_ascii_strtoll( *value_cursor, NULL, 10);
			name_cursor++;
			value_cursor++;
		}
		if ( g_ascii_strcasecmp(element_name, "warning") ) //If not a warning, mark as error
			eeinfo->error = TRUE;


		gchar * name = NULL;
		gchar * log_message = NULL;
		if ( filename )
		{
			name = g_path_get_basename(filename);
			if ( lastline >= 0 )
				log_message = g_strdup_printf("%s [Lines %d -- %d] %s %03d\n%s\n", name, line, lastline, element_name, number, message );
			else
				log_message = g_strdup_printf("%s [Line %d] %s %03d\n%s\n", name, line, element_name, number, message );

			if ( eeinfo->text && !g_ascii_strcasecmp(eeinfo->filename, name) )
			{
				Meg_Editor_SetLineInfo( eeinfo->text, line, (eeinfo->error  ? 1 : 0), g_strdup(log_message) );
			}
			Logger_FormattedLogLine( eeinfo->log, eeinfo->text, (gchar*)filename, line, (eeinfo->error ? LOG_ERROR : LOG_WARNING), log_message );
		}
		else
		{
			g_print ("Compiler Error: %s\n", message);
		}
		g_free(name);
		g_free(log_message);

	}

}

void entity_error_end_element(GMarkupParseContext *context, const gchar*element_name, gpointer user_data, GError **error)
{


}


/********************************
* EntityCompiler_TemporaryFile
* Copy a file to temporary file for compiling on external compiler
@ filename:
@ origdir:
@ fname:
- returns complete file path
*/
gchar * EntityCompiler_TemporaryFile( gchar * filename, gboolean * remove )
{
	const gchar * tmpl = "mXXXXXX.p";
	const gchar * dir = NULL;
	gchar * filepath = NULL;
	gboolean is_package_entity = FALSE;

	dir = PHYSFS_getRealDir(filename);


	if ( dir == NULL )
		is_package_entity = TRUE;
	else if ( !g_file_test(dir, G_FILE_TEST_IS_DIR) || g_str_has_suffix(dir, ".package") )
		is_package_entity = TRUE;


	if ( mokoiUsingPackage && is_package_entity )
	{
		gint fd = g_file_open_tmp( tmpl, &filepath, NULL );
		if ( fd == -1 )
		{
			g_warning("EntityCompiler: Can not create a temp file for %s\n", filename);
			return NULL;
		}
		else
		{
			gchar * content = NULL;
			gsize length = 0;
			Meg_file_get_contents(filename, &content, &length, NULL);
			write(fd, content, length);
			close(fd);
		}
	}


	return filepath;
}


/********************************
* EntityCompiler_ExtractRoutines
* count the anomunt of entities
@ data: int *
@ origdir:
@ fname:
*/
static void EntityCompiler_ExtractRoutines(void * data, const char *origdir, const char *fname)
{
	if ( g_str_has_suffix(fname, ".inc") && !g_str_has_prefix(fname, "private_") )
	{
		gchar * internal_file_path = g_strdup_printf("%s/%s", origdir, fname);
		gchar * content = NULL;

		PHYSFS_mkdir(origdir);
		PHYSFS_delete( internal_file_path );
		if ( Meg_file_get_contents(internal_file_path, &content, NULL, NULL) )
			Meg_file_set_contents(internal_file_path, content, -1, NULL);
	}
}

/********************************
* EntityCompiler_RebuildCount
* count the anomunt of entities
@ data: int *
@ origdir:
@ fname:
*/
static void EntityCompiler_RebuildCount(void * data, const char *origdir, const char *fname)
{
	int * c = (int *)data;
	if ( g_str_has_suffix(fname, ".mps") )
	{
		*c += 1;
	}
}

/********************************
* EntityCompiler_RebuildForEach
* compiles entity file
@ data:
@ origdir:
@ fname:
*/
static void EntityCompiler_RebuildForEach(void *data, const char *origdir, const char *fname)
{
	if ( g_str_has_suffix(fname, ".mps") )
	{
		gchar * filename = g_strdup_printf("%s/%s", origdir, fname);

		EntityCompiler_File( filename, NULL, NULL );

		g_free(filename);
	}
}

/********************************
* EntityCompiler_PawnScript
* compiles entity file
@ string_inputfile
- returns XML error handling
*/
gchar * EntityCompiler_PawnScript( gchar * input_file, gchar * temporary_filepath  )
{
	GError * err = NULL;
	gint exit_status;

	gchar * entity_file = NULL, * compiled_entity_file = NULL;
	gchar * working_directory = NULL;
	gchar * full_output_directory = NULL;

	gchar * args[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
	gchar * output = NULL;
	gchar * output_error = NULL;

	full_output_directory = g_build_path( "/", AL_ProjectPath(), "c", g_path_get_dirname(input_file), NULL);

	g_mkdir_with_parents( full_output_directory, 0755 );

	#ifdef apple
	working_directory = Meg_Directory();
	#endif

	args[0] = g_build_filename( Meg_Directory(), PAWN_COMPILER, NULL );
	args[1] = g_strconcat( AL_ProjectPath(), input_file, NULL );
	args[2] = g_strdup( "--xmloutput");
	args[3] = g_strconcat( "--project=", AL_ProjectPath(), NULL );

	if ( temporary_filepath )
		args[4] = g_strconcat( "--temporary=", temporary_filepath,  NULL );
	else
		args[4] = g_strdup( "--notemporary" );

	if ( !g_spawn_sync( working_directory, //working_directory
		args, //gchar **argv,
		NULL, //gchar **envp,
		G_SPAWN_SEARCH_PATH, //GSpawnFlags flags,
		NULL, //GSpawnChildSetupFunc child_setup,
		NULL, //gpointer user_data,
		&output, //gchar **standard_output,
		&output_error, //gchar **standard_error,
		&exit_status, //gint *exit_status,
		&err //GError **error
	))
	{
		output = g_strdup_printf("<results><fatalerror message=\"%s\" file=\"%s\" /></results>", g_markup_escape_text(err->message, -1), args[0] );
		g_clear_error(&err);
	}

	if ( exit_status && !output )
	{
		output = g_strdup_printf("<results><fatalerror message=\"Possible Compiler Crash.\" file=\"%s\" /></results>", args[0] );
	}

	g_free(full_output_directory);

	g_free(args[0]);
	g_free(args[1]);
	g_free(args[2]);
	g_free(args[3]);
	g_free(args[4]);

	/* Check if compiler left an empty file */
	entity_file = g_build_filename( AL_ProjectPath(), "c", input_file, NULL);
	compiled_entity_file = Meg_String_ReplaceFileExtension( entity_file, ".mps", ".amx" );
	if ( file_get_size( compiled_entity_file ) == 0 )
	{
		g_unlink(compiled_entity_file);
	}
	g_free(compiled_entity_file);
	g_free(entity_file);



	return output;
}

/********************************
* EntityCompiler_ScanErrors
* compiles entity file
@ string_inputfile
@ logwidget:  TextView Widget to output errors to.
@ textwidget: TextView Widget to mark with errors.
*/
gboolean EntityCompiler_ScanErrors( gchar * compiler_output, GtkWidget * logwidget, GtkWidget * textwidget )
{

	EntityErrorInfo eeinfo;
	GMarkupParseContext * ctx;

	eeinfo.filename = NULL;
	eeinfo.log = logwidget;
	eeinfo.text = textwidget;
	eeinfo.error = FALSE;


	GError  * err = NULL;
	ctx = g_markup_parse_context_new( &entity_error_parser, (GMarkupParseFlags)0, &eeinfo, NULL );
	g_markup_parse_context_parse( ctx, compiler_output, -1, &err );
	if ( err == NULL )
	{
		g_markup_parse_context_end_parse( ctx, &err );
	}
	g_markup_parse_context_free( ctx );
	if ( err != NULL )
	{
		eeinfo.error = TRUE;
		Logger_FormattedLog( eeinfo.log, LOG_ERROR, "Entity Unknown Error:\n%s\n\n%s\n", err->message, compiler_output);
		g_clear_error(&err);
	}
	return eeinfo.error;

}

/********************************
* Entity_ExtractRoutines
*
*/
void Entity_ExtractRoutines()
{
	if ( mokoiUsingPackage )
	{
		Meg_enumerateFilesCallback("/scripts/maps/routines", EntityCompiler_ExtractRoutines, NULL);
		Meg_enumerateFilesCallback("/scripts/routines", EntityCompiler_ExtractRoutines, NULL);
	}
}



/********************************
* EntityCompiler_File
* compiles entity file
@ string_inputfile
@ logwidget:  TextView Widget to output errors to.
@ textwidget: TextView Widget to mark with errors.
*/
gboolean EntityCompiler_FileWithRountines( gchar * inputfile, GtkWidget * logwidget, GtkWidget * textwidget )
{
	Entity_ExtractRoutines();
	return EntityCompiler_File( inputfile, logwidget, textwidget );
}




void Logger_FormattedLogTop( GtkWidget * widget, LogStyle style, const gchar * format, ...);
/********************************
* EntityCompiler_File
* compiles entity file
@ string_inputfile
@ logwidget:  TextView Widget to output errors to.
@ textwidget: TextView Widget to mark with errors.
*/
gboolean EntityCompiler_File( gchar * input_file, GtkWidget * logwidget, GtkWidget * textwidget )
{
	gboolean error_compiling = FALSE;
	gboolean free_file = FALSE;
	gchar * basename = g_path_get_basename(input_file);
	gchar * output = NULL;
	gchar * temporary_filepath = NULL;

	temporary_filepath = EntityCompiler_TemporaryFile( input_file, &free_file );

	output = EntityCompiler_PawnScript( input_file, temporary_filepath );

	/* Being called from Text Editor */
	if ( textwidget != NULL )
	{
		Meg_Editor_ClearLineInfo( textwidget );
	}

	if ( logwidget != NULL )
	{
		Logger_SetBufferTag( gtk_text_view_get_buffer( GTK_TEXT_VIEW(logwidget) ) );
	}

	error_compiling = EntityCompiler_ScanErrors(output, logwidget, textwidget );
	if ( error_compiling )
	{
		Logger_FormattedLog(logwidget, LOG_BOLD, "%s %s\n\n", "Error compiling: ", basename);
		//Logger_FormattedLogTop(logwidget, LOG_BOLD, "%s %s\n\n", "Error compiling: ", basename);
	}
	else
	{
		Logger_FormattedLog(logwidget, LOG_BOLD, "%s %s\n\n", "Script compiled: ", basename);
	}

	g_free(basename);
	g_free(temporary_filepath);
	return !error_compiling;
}

/********************************
* Entity_RebuildMaps
*
*/
void Entity_RebuildDirectory( ProgressDialogWidgets * wids, const gchar * directory  )
{
	Entity_ExtractRoutines();
	Meg_enumerateFilesCallback(directory, EntityCompiler_RebuildForEach, NULL);
}

/********************************
* Entity_RebuildAll_Thread
*
*/
void Entity_RebuildAll_Thread( ProgressDialogWidgets * wids  )
{
	Logger_SetQueue( wids->queue ); // redirects a Logger_* to pass messages

	Entity_ExtractRoutines();

	Meg_enumerateFilesCallback("/scripts/maps/", EntityCompiler_RebuildForEach, NULL);
	Meg_enumerateFilesCallback("/scripts/", EntityCompiler_RebuildForEach, NULL);


	Logger_SetQueue( NULL );  // resets logger
	g_async_queue_push( wids->queue, g_thread_self() ); // closes thread. Also tells the watch that the dialog can close.
}

/********************************
* Entity_Count
* Count the number entities for the Entity_RebuildAll dialog box
*/
gint Entity_Count( )
{
	gint c = 0;

	Meg_enumerateFilesCallback("/scripts/maps/", EntityCompiler_RebuildCount, &c);
	Meg_enumerateFilesCallback("/scripts/", EntityCompiler_RebuildCount, &c);

	return c;
}


/********************************
* Entity_RebuildAll
*
*/
void Entity_RebuildAll( )
{
	ProgressDialog_Clear( &mokoiEntityRebuild );
	if ( ProgressDialog_Create( &mokoiEntityRebuild, "Rebuilding Entities", NULL) )
	{
		mokoiEntityRebuild.items = Entity_Count();
		ProgressDialog_Activate( &mokoiEntityRebuild, (GThreadFunc)Entity_RebuildAll_Thread, (GSourceFunc)ProgressDialog_Watch );
	}
}

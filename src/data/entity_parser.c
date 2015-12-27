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
#include "stretchy_buffer.h"

gint meg_ascii_strcasecmp(const gchar * a, const gchar * b)
{
	if ( a == NULL )
		return -1;
	if ( b == NULL )
		return 1;
	return g_ascii_strcasecmp(a,b);
}


typedef enum {
	whitespace,
	letter,
	number,
	hex,
	comma,
	semicolon,
	lparen,
	rparen,
	lfparen,
	rfparen,
	quote,
	singlequote,
	times,
	slash,
	plus,
	minus,
	eql,
	neq,
	lss,
	leq,
	gtr,
	geq,
	expressand,
	expressor,
	keyword,
	definedline,
	commentline,
	commentstart,
	commentend,
	lineend,
	endfile
} Symbol;


char * symbol_names[] = {
	"whitespace",
	"letter",
	"number",
	"hex",
	"comma",
	"semicolon",
	"lparen",
	"rparen",
	"lfparen",
	"rfparen",
	"quote",
	"singlequote",
	"times",
	"slash",
	"plus",
	"minus",
	"eql",
	"neq",
	"lss",
	"leq",
	"gtr",
	"geq",
	"&&",
	"||",
	"keyword",
	"definedline",
	"commentline",
	"commentstart",
	"commentend",
	"lineend",
	"endfile"
};


#define SYMBOL_NAME(S) symbol_names[S]

typedef struct {
	gsize line_number;
	char * name;
	char * expression;
	char * value;
} ScriptBlock;

typedef struct {
	guint type;
	gsize line_number;
	char * name;
	char * value;
} ScriptVariable;


typedef struct {
	char * name;
	gsize line_number;
	gsize current_variable;
	ScriptVariable ** variables;
	ScriptBlock ** code;
} ScriptFunction;

typedef struct {
	char * file_content;
	gsize file_position;
	gsize file_size;
	gsize line_number;
	gint prev, curr, next;
	Symbol sym;
	gsize depth;
	ScriptVariable ** variables;
	ScriptFunction ** functions;
	ScriptFunction * function;

} ScriptScanner;

void scan_error(ScriptScanner * script, char * message)
{
	g_print("Script Parser Error: %s", message);
	script->sym = endfile;
}

void scan_add_variable(ScriptScanner * script ,gchar * name, gchar * value, gchar * mem_type)
{
	ScriptVariable * variable = g_new0(ScriptVariable, 1);

	variable->name = g_strdup(name);
	variable->value = value ? g_strdup(value) : NULL;
	variable->type = 0;
	variable->line_number = script->line_number;

	if ( !g_strcmp0(mem_type, "new") )
		variable->type = 1;
	if ( !g_strcmp0(mem_type, "EQUALS") )
		variable->type = 2;
	if ( !g_strcmp0(mem_type, "FUNCTION") )
		variable->type = 3;


	if ( script->depth && script->function)
	{
		sb_push(script->function->variables, variable);
	}
	else
	{
		sb_push(script->variables, variable);
	}
}

void scan_add_function(ScriptScanner * script, gchar * name)
{
	ScriptFunction * func  = g_new0(ScriptFunction, 1);


	func->name = g_strdup(name);
	func->line_number = script->line_number;

	sb_push(script->functions, func);

	script->function = func;
}


gboolean scan_watch(ScriptScanner * script)
{
	return ( script->sym == endfile || script->curr < 1 );
}

gint scan_input_char(ScriptScanner * script, gint push)
{
	gint c = script->file_content[script->file_position];

	if ( c != EOF )
	{
		script->file_position += push;
	}

	return c;
}

gint scan_input_utf8(ScriptScanner * script, gint push)
{
	guint uc = script->file_content[script->file_position] & 0xFF;
	gint c = script->file_content[script->file_position];
	gint c2 = 0, c3 = 0, c4 = 0;


	if ( uc > 0 && uc < 128 )
	{
		script->file_position += push;
	}
	else if ( uc >= 128 && uc < 224 )
	{
		if ( script->file_position+1 < script->file_size )
		{
			c2 = script->file_content[script->file_position+1] & 0x3f;
			c = ((c << 6) & 0x7ff) + c2;
			script->file_position += (2*push);

		}
		else
		{
			scan_error(script, "UTF8 Parse Error ");
		}
	}
	else if ( uc >= 224 && uc < 240 )
	{
		if ( script->file_position+2 < script->file_size )
		{
			c2 = script->file_content[script->file_position+1] & 0xff;
			c3 = script->file_content[script->file_position+2] & 0x3f;

			c = ((c << 12) & 0xffff) + ((c2 << 6) & 0xfff) + c3;
			script->file_position += (3*push);
		}
		else
		{
			scan_error(script, "UTF8 Parse Error ");
		}
	}
	else if ( uc >= 240 && uc < 245 )
	{
		if ( script->file_position+3 < script->file_size )
		{
			c2 = script->file_content[script->file_position+1] & 0xff;
			c3 = script->file_content[script->file_position+2] & 0xff;
			c4 = script->file_content[script->file_position+3] & 0x3f;

			c = ((c << 18) & 0xffff) + ((c2 << 12) & 0x3ffff) + ((c3 << 6) & 0xfff) + c4;
			script->file_position += (4*push);
		}
		else
		{
			scan_error(script, "UTF8 Parse Error ");
		}

	}
	else
	{
		c = EOF;
	}

	return c;
}

void scan_input(ScriptScanner * script)
{
	script->prev = script->curr;
	if ( script->file_content && script->file_position < script->file_size)
	{
		script->curr = scan_input_utf8(script, 1);

		if ( script->file_position < script->file_size)
			script->next = scan_input_utf8(script, 0);
		else
			script->next = EOF;
	}
	else
	{
		script->curr = script->next = EOF;
	}
}

void scan_input_byte(ScriptScanner * script)
{
	script->prev = script->curr;
	if ( script->file_content && script->file_position < script->file_size)
	{
		script->curr = scan_input_char(script, 1);

		if ( script->file_position < script->file_size)
			script->next = scan_input_char(script, 0);
		else
			script->next = EOF;
	}
	else
	{
		script->curr = script->next = EOF;
	}
}

gboolean scan_peek_input( ScriptScanner * script, gint request )
{
	if ( script->next == request )
	{
		scan_input(script);
		return TRUE;
	}
	return FALSE;
}

void scan_getsymbol(ScriptScanner * script)
{
	if (script->curr == ' ' || script->curr == '\t' || script->curr == '\r' ){
		script->sym = whitespace;
	}
	else if (script->curr == '\n')
	{
		script->sym = lineend;
		script->line_number++;
	}
	else if(script->curr <= 0 || script->curr == 0xff ) {
		script->sym = endfile;
	} else if (script->curr == '*'){
		if (scan_peek_input( script,'/'))
			script->sym = commentend;
		else
			script->sym = times;
	} else if (script->curr == '+'){
		script->sym = plus;
	} else if (script->curr == '-'){
		script->sym = minus;
	} else if (script->curr == '/'){
		if (scan_peek_input( script, '/'))
			script->sym = commentline;
		else if (scan_peek_input( script,'*'))
			script->sym = commentstart;
		else
			script->sym = slash;
	} else if (script->curr == '='){
		script->sym = eql;
	} else if (script->curr == '('){
		script->sym = lparen;
	} else if (script->curr == ')'){
		script->sym = rparen;
	} else if (script->curr == '{'){
		script->sym = lfparen;
	} else if (script->curr == '}'){
		script->sym = rfparen;
	} else if (script->curr == '"'){
		if (script->prev == '\\' ) // Escape character
			script->sym = letter;
		else
			script->sym = quote;
	} else if (script->curr == '\''){
		if (scan_peek_input( script, '\''))
			script->sym = singlequote;
		else
			script->sym = letter;
	} else if (script->curr == '&'){
		if (scan_peek_input( script, '&'))
			script->sym = expressand;
		else
			script->sym = letter;
	} else if (script->curr == '|'){
		if (scan_peek_input( script, '|'))
			script->sym = expressor;
		else
			script->sym = letter;
	} else if (script->curr == ';'){
		script->sym = semicolon;
	} else if (script->curr == '#'){
		script->sym = definedline;
	} else {
		script->sym = letter;
	}
}

gint scan_validcharacter(ScriptScanner * script)
{
	if (script->curr == 59)
		return 0;
	if (script->curr >= 48)
		return 1;
	if (script->curr == '.')
		return 1;
	return 0;
}

void scan_nextsymbol(ScriptScanner * script, gint print)
{
	scan_input(script);
	scan_getsymbol(script);

	if ( script->sym != whitespace && script->sym != endfile && print) {
		//g_print("\n - Input: %s '%c'", SYMBOL_NAME(script->sym), script->curr);
	}
}

void scan_skip_whitespace(ScriptScanner * script) {
	if ( script->sym == endfile || ( script->sym != whitespace) )
		return;

	do {
		scan_nextsymbol(script, 0);
	} while ( ( script->sym != whitespace) && script->sym != endfile  );

}

void scan_skip_character(ScriptScanner * script, gint skip) {
	if ( script->sym == endfile )
		return;

	do {
		scan_nextsymbol(script, 0);
	} while ( script->sym != endfile && script->sym != lineend && (script->sym == whitespace  || script->curr == skip ) );

}

void scan_skip_to_nextsymbol(ScriptScanner * script) {
	if ( script->sym == endfile || (script->sym != lineend && script->sym != whitespace) )
		return;
	do {
		scan_nextsymbol(script, 0);
	} while ( (script->sym == lineend || script->sym == whitespace) && script->sym != endfile  );

}

gsize scan_nextword(ScriptScanner * script, gchar * word, gsize word_length )
{
	gsize i = 0;
	do {
		word[i] = (gchar)script->curr;
		i++;

		scan_input_byte(script);
		scan_getsymbol(script);
	} while (i < word_length && scan_validcharacter(script) && script->curr != EOF );
	scan_skip_whitespace(script);
	return i;
}

gchar * scan_new_nextword(ScriptScanner * script, gsize word_length )
{
	gchar * word = g_new0(gchar, word_length);
	gsize i = 0;
	do {
		word[i] = (gchar)script->curr;
		i++;

		scan_input_byte(script);
		scan_getsymbol(script);
	} while (i < word_length && scan_validcharacter(script) && script->curr != EOF );
	scan_skip_whitespace(script);
	return word;
}

void scan_section(ScriptScanner * script, Symbol opening, Symbol closing, char * name) {
	scan_skip_whitespace(script);
	if ( script->sym == opening ) {
		GString * string = g_string_new(NULL);
		do {
			scan_nextsymbol(script, 0);
			if ( script->sym != closing && script->sym != endfile)
				string = g_string_append_unichar(string, script->curr );
		} while ( script->sym != closing && script->sym != endfile  );
	}
}



/**
 * @brief Writes to word until an  semicolon or lineend
 * @param script
 * @param word
 * @param word_length
 * @return
 */
gsize scan_word(ScriptScanner * script, gchar * word, gsize word_length )
{
	gsize i = 0;
	do {
		if (i < word_length)
			word[i] = (gchar)script->curr;
		i++;

		scan_input_byte(script);
		scan_getsymbol(script);
	} while ( ( script->sym != semicolon && script->sym != lineend ) && script->curr != EOF );

	return i;
}

void scan_variable_allocation(ScriptScanner * script, gchar * variable, gchar * mem_type) {
	scan_skip_whitespace(script);
	if ( script->sym == semicolon || script->sym == lineend ) {
		scan_add_variable(script, variable, NULL, mem_type);
	}
	else if ( script->sym == eql ) {
		scan_nextsymbol(script, 0);
		scan_skip_whitespace(script);

		gchar * value = g_new0(gchar, 128);

		scan_word(script, value, 128);

		scan_add_variable(script, variable, value, mem_type);
		g_free(value);
	}

}

void scan_variable(ScriptScanner * script) {
	scan_skip_whitespace(script);
	if ( script->sym == letter ) {
		gchar * word = g_new0(gchar, 128);
		gsize word_length = scan_nextword(script, word, 127 );

		scan_variable_allocation(script, word, "new");

		g_free(word);
	}

}

/**
 * @brief scan_expression_section
 * @param script
 * @param opening
 * @param closing
 */
void scan_expression_section(ScriptScanner * script, Symbol opening, Symbol closing) {
	scan_skip_to_nextsymbol(script);
	if ( script->sym == lparen ) {
		do {
			scan_nextsymbol(script, 0);
			scan_skip_to_nextsymbol(script);
			if ( script->sym == letter ) {
				gchar * word = g_new0(gchar, 128);
				gsize i = 0;
				gsize b = 0;
				do {
					if ( script->curr == '(') {
						b++;
					} else if ( script->curr == ')') {
						b--;
					}

					word[i] = (gchar)script->curr;
					i++;

					scan_input_byte(script);
					scan_getsymbol(script);
				} while (i < 128 && ( script->sym != expressand && script->sym != expressor ) && !( script->sym == rparen && !b ) && script->curr != EOF );
				//g_print("\n\t %s", word);
				g_free(word);
			}
		} while ( script->sym != rparen && script->sym != endfile && script->curr != EOF   );
	}
}

/**
 * @brief scan_argument_section
 * @param script
 * @param opening
 * @param closing
 */
void scan_argument_section(ScriptScanner * script, Symbol opening, Symbol closing) {
	scan_skip_to_nextsymbol(script);
	if ( script->sym == lparen ) {
		do {
			scan_nextsymbol(script, 0);
			scan_skip_to_nextsymbol(script);
			if ( script->sym == letter ) {
				gchar * word = g_new0(gchar, 128);
				gsize i = 0;
				do {
					word[i] = (gchar)script->curr;
					i++;

					scan_input_byte(script);
					scan_getsymbol(script);
				} while (i < 128 && ( script->sym != comma && script->sym != rparen ) && script->curr != EOF );

				g_free(word);
			}
		} while ( script->sym != rparen && script->sym != endfile  );

	}
}

/**
 * @brief scan_code_section
 * @param script
 * @param opening
 * @param closing
 */
void scan_code_section(ScriptScanner * script, Symbol opening, Symbol closing) {
	scan_skip_to_nextsymbol(script);
	if ( script->sym == letter ) {
		opening = letter;
		closing = lineend;
	}
	if ( script->sym == opening ) {
		//g_print("\n > Code Line: %d", script->line_number);
		do {
			if ( script->sym == letter ) {
				gchar * word = g_new0(gchar, 128);
				gsize word_length = scan_nextword(script, word, 127 );

				if ( meg_ascii_strcasecmp(word, "new") == 0 ) {
					scan_variable(script);
				} else if ( meg_ascii_strcasecmp(word, "if") == 0 || meg_ascii_strcasecmp(word, "switch") == 0     ) {
					scan_argument_section(script, lparen, rparen);
					scan_code_section(script, lfparen, rfparen);
				} else if ( meg_ascii_strcasecmp(word, "else") == 0   ) {
					scan_argument_section(script, lparen, rparen);
					scan_code_section(script, lfparen, rfparen);
				} else if ( script->curr == '(' ) {
					scan_section(script, lparen, rparen, "()");
					scan_nextsymbol(script, 0);
				}
				g_free(word);
			}

			scan_nextsymbol(script, 0);
		} while ( script->sym != closing && script->sym != endfile  );
		scan_nextsymbol(script, 0);
	}
}

/**
 * @brief scan_function_section
 * @param script
 * @param opening
 * @param closing
 */
void scan_function_section(ScriptScanner * script, Symbol opening, Symbol closing) {
	scan_skip_to_nextsymbol(script);
	if ( script->sym == opening ) {
		do {
			if ( script->sym == letter ) {
				gchar * keyword = g_new0(gchar, 128);
				scan_nextword(script, keyword, 127 );

				if ( meg_ascii_strcasecmp(keyword, "new") == 0 ) {
					scan_variable(script);
					//scan_script_section(script, keyword);
				} else if ( meg_ascii_strcasecmp(keyword, "static") == 0 ) {
					scan_variable(script);
					//scan_script_section(script, keyword);
				} else if ( meg_ascii_strcasecmp(keyword, "if") == 0 || meg_ascii_strcasecmp(keyword, "switch") == 0 || meg_ascii_strcasecmp(keyword, "while") == 0  ) {
					scan_expression_section(script, lparen, rparen);
					scan_function_section(script, lfparen, rfparen);
				} else if ( meg_ascii_strcasecmp(keyword, "else") == 0   ) {
					scan_expression_section(script, lparen, rparen);
					scan_function_section(script, lfparen, rfparen);
				} else if ( meg_ascii_strcasecmp(keyword, "do") == 0   ) {
					scan_argument_section(script, lparen, rparen);
					scan_function_section(script, lfparen, rfparen);
				} else {
					if ( script->curr == '(' )
					{
						scan_argument_section(script, lparen, rparen);
						scan_add_variable(script, NULL, keyword, "FUNCTION");
					}
					else
					{
						scan_variable_allocation(script, keyword, "EQUALS");
					}
				}
				g_free(keyword);
			}
			else
			{
				switch ( script->sym )
				{
					case commentstart:
						scan_section(script, commentstart, commentend, "Comment");
						break;
					case commentline:
						scan_section(script, commentline, lineend, "Comment");
						break;
					case definedline:
						scan_section(script, definedline, lineend, "define/include");
						break;
					default:
						break;
				}
			}
			scan_nextsymbol(script, 0);
		} while ( script->sym != closing && script->sym != endfile  );
		scan_nextsymbol(script, 0);
	}
}


/**
 * @brief scan_block_section
 * @param script
 * @param prev_word
 */
void scan_block_section(ScriptScanner * script, char * prev_word) {
	scan_skip_whitespace(script);

	if ( script->sym == letter ) {
		gchar * keyword = g_new0(gchar, 128);
		gsize word_length = scan_nextword(script, keyword, 127);

		if ( meg_ascii_strcasecmp(keyword, "new") == 0 ) {
			scan_block_section(script, keyword);
		} else if ( meg_ascii_strcasecmp(keyword, "stock") == 0 ) {
			scan_block_section(script, keyword);
		} else if ( meg_ascii_strcasecmp(keyword, "public") == 0   ) {
			if ( meg_ascii_strcasecmp(prev_word, "forward") == 0   ) {

				scan_block_section(script, keyword);
				scan_expression_section(script, lparen, rparen);
				scan_skip_character(script, ';');
			} else {
				scan_block_section(script, keyword);
			}


		} else if ( meg_ascii_strcasecmp(keyword, "forward") == 0   ) {
			scan_block_section(script, keyword);
		} else if ( script->curr == '(' ) {
			scan_add_function(script,keyword);
			script->depth++;
			scan_section(script, lparen, rparen, "()");
			scan_nextsymbol(script, 0);
			scan_function_section(script, lfparen, rfparen);
			script->depth--;
			script->function = NULL;
		} else if ( prev_word ) {
			scan_variable_allocation(script, keyword, prev_word);
		}

		g_free(keyword);
	}
}

/** Script's Root **/
void scan_block(ScriptScanner * script) {
	scan_nextsymbol(script, 0);
	scan_skip_to_nextsymbol(script);
	switch ( script->sym )
	{
		case commentstart:
			scan_section(script, commentstart, commentend, "Comment");
			break;
		case commentline:
			scan_section(script, commentline, lineend, "Comment");
			break;
		case definedline:
			scan_section(script, definedline, lineend, "define/include");
			break;
		case lfparen:
			scan_code_section(script, lfparen, rfparen);
			break;
		default:
			scan_block_section(script, NULL);
			break;
	}

}

void scan_print_script(ScriptScanner * script)
{
	g_print("\n-------------------------\n");
	for( guint8 t = 0; t < sb_count(script->variables); t++ )
	{
		if( script->variables[t]->name )
		{
			g_print("new %s = %s; Line: %d \n", script->variables[t]->name, script->variables[t]->value, script->variables[t]->line_number+1 );
		}
	}
	for( guint8 y = 0; y < sb_count(script->functions); y++ )
	{
		if( script->functions[y]->name )
		{
			g_print("Function %s, Line: %d {\n", script->functions[y]->name, script->functions[y]->line_number+1 );
			for( guint8 u = 0; u < sb_count(script->functions[y]->variables); u++ )
			{
				if( script->functions[y]->variables[u]->name )
				{
					g_print("\t%d %s = %s;\n", script->functions[y]->variables[u]->type, script->functions[y]->variables[u]->name, script->functions[y]->variables[u]->value );
				}
				else
				{
					g_print("\t%d %s;\n", script->functions[y]->variables[u]->type, script->functions[y]->variables[u]->value );
				}
			}
			g_print("}\n");
		}
	}
}

void scan_script_clear(ScriptScanner * script)
{
	for( guint8 t = 0; t < sb_count(script->variables); t++ )
	{
		g_free( script->variables[t]->name);
		g_free( script->variables[t]->value);
	}
	sb_free(script->variables);


	for( guint8 y = 0; y < sb_count(script->functions); y++ )
	{

		for( guint8 u = 0; u < sb_count(script->functions[y]->variables); u++ )
		{
			g_free( script->functions[y]->variables[u]->name);
			g_free( script->functions[y]->variables[u]->value);
		}

		g_free( script->functions[y]->name );
		sb_free( script->functions[y]->variables );
	}
}
void script_generate_tree(ScriptScanner * script, GtkTreeStore * list)
{
	GtkTreeIter iter, parent;

	gtk_tree_store_append( list, &parent, NULL );
	gtk_tree_store_set( list, &parent, 0, g_strdup( "Variables" ), 1, 1, 2, 0, 3, "gtk-directory", -1 );

	for( guint8 t = 0; t < sb_count(script->variables); t++ )
	{
		if( script->variables[t]->name )
		{
			gtk_tree_store_append( list, &iter, &parent );
			gtk_tree_store_set( list, &iter, 0, g_strdup( script->variables[t]->name ), 1, script->variables[t]->line_number+1, 2, 0, 3, "gtk-jump-to", -1 );
		}
	}
	for( guint8 y = 0; y < sb_count(script->functions); y++ )
	{
		if( script->functions[y]->name )
		{
			gtk_tree_store_append( list, &parent, NULL );
			gtk_tree_store_set( list, &parent, 0, g_strdup( script->functions[y]->name ), 1, script->functions[y]->line_number+1, 2, 0, 3, "gtk-directory", -1 );
			for( guint8 u = 0; u < sb_count(script->functions[y]->variables); u++ )
			{
				//gtk_tree_store_append( list, &iter, &parent );
				//gtk_tree_store_set( list, &iter, 0, g_strdup( script->functions[y]->variables[u]->name ), 1, script->functions[y]->variables[u]->line_number+1, 2, 0, 3, "gtk-jump-to", -1 );
			}
		}
	}
}

void scan_script(const gchar * file_name, GtkTreeModel * model )
{
	ScriptScanner script = { 0 };
	if ( PHYSFS_exists( file_name ) )
	{
		if ( Meg_file_get_contents( file_name, &script.file_content, &script.file_size, NULL) )
		{
			while ( script.sym != endfile )
			{
				scan_block(&script);
			}
			//scan_print_script(&script);
			script_generate_tree(&script, GTK_TREE_STORE(model) );
			scan_script_clear(&script);
		}
	}
}


// re2c $INPUT -o $OUTPUT -8 --case-ranges -i
#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <stdbool.h>

#define BUFFSIZE 409600
enum TAG
{
  ERROR				//0
    , END			//1
    , TEST			//2
    , POINT			//3
    , COMMA			//4
    , SEMICOLON			//5
    , LEFT_PARENTHESIS		//6
    , RIGHT_PARENTHESIS		//7
    , LEFT_SQUARE_BRACKET	//8
    , RIGHT_SQUARE_BRACKET	//9
    , WHITESPACE		//10
    , TAB			//11
    , NEWLINE			//12
    , RELOP			//13
    , MATH			//14
    , NUMBER			//15
    , DATE			//16
    , LITERAL			//17
    , PREPROCESSOR		//18
    , AREA			//19
    , ID			//20
};

char *tag_text[] = {
  "ERROR"			//0
    , "END"			//1
    , "TEST"			//2
    , "POINT"			//3
    , "COMMA"			//4
    , "SEMICOLON"		//5
    , "LEFT_PARENTHESIS"	//6
    , "RIGHT_PARENTHESIS"	//7
    , "LEFT_SQUARE_BRACKET"	//8
    , "RIGHT_SQUARE_BRACKET"	//9
    , "WHITESPACE"		//10
    , "TAB"			//11
    , "NEWLINE"			//12
    , "RELOP"			//13
    , "MATH"			//14
    , "NUMBER"			//15
    , "DATE"			//16
    , "LITERAL"			//17
    , "PREPROCESSOR"		//18
    , "AREA"			//19
    , "ID"			//20
};

enum TAG_ATTR
{
  LT				//0
    , LE			//1
    , EQ			//2
    , NE			//3
    , GT			//4
    , GE			//5
    , PLUS			//6
    , MINUS			//7
    , MULTIPLY			//8
    , DIVISION			//9
    , REMAINDER_OF_DIVISION	//10
};

char *tag_attr_text[] = {
  "<"				//0
    , "<="			//1
    , "="			//2
    , "<>"			//3
    , ">"			//4
    , ">="			//5
    , "+"			//6
    , "-"			//7
    , "*"			//8
    , "/"			//9
    , "%"			//10
};

typedef struct
{
  enum TAG tag;
} token;

typedef struct
{
  token base;
  enum TAG_ATTR attr;
} token_attr;

typedef struct
{
  token base;
  char *text;
} token_table;

token tokens[] = {
  {ERROR}			//0
  , {END}			//1
  , {TEST}			//2
  , {POINT}			//3
  , {COMMA}			//4
  , {SEMICOLON}			//5
  , {LEFT_PARENTHESIS}		//6
  , {RIGHT_PARENTHESIS}		//7
  , {LEFT_SQUARE_BRACKET}	//8
  , {RIGHT_SQUARE_BRACKET}	//9
  , {WHITESPACE}		//10
  , {TAB}			//11
  , {NEWLINE}			//12
};

token_attr tokens_attr[] = {
  {{RELOP}, LT}			//0
  , {{RELOP}, LE}		//1
  , {{RELOP}, EQ}		//2
  , {{RELOP}, NE}		//3
  , {{RELOP}, GT}		//4
  , {{RELOP}, GE}		//5
  , {{MATH}, PLUS}		//6
  , {{MATH}, MINUS}		//7
  , {{MATH}, MULTIPLY}		//8
  , {{MATH}, DIVISION}		//9
  , {{MATH}, REMAINDER_OF_DIVISION}	//10
};

GHashTable *symbol_table = NULL;
char buf_str[BUFFSIZE] = { 0 };

int fd = 0;
struct stat statbuf = { 0 };

void *src = NULL;
const char *start_pos = NULL;
const char *end_pos = NULL;
const char *limit = NULL;
size_t line = 1;

void
print_error (const char *format, ...)
{
  va_list argptr;
  va_start (argptr, format);
  vprintf (format, argptr);
  va_end (argptr);
  exit (EXIT_FAILURE);
}

token_table *
create_token_table (const enum TAG tag)
{
  token_table *cur_token = NULL;
  size_t n = end_pos - start_pos;
  char *lexeme_upper = NULL;

  assert (n < BUFFSIZE);

  if (tag < PREPROCESSOR)
/*if (tag == NUMBER || tag == DATE || tag == LITERAL)*/
    {
      memcpy (buf_str, start_pos, n);
      buf_str[n] = '\0';
      cur_token = g_hash_table_lookup (symbol_table, buf_str);
    }
  else
/*if (tag == PREPROCESSOR || tag == AREA || tag == ID)*/
    {
      lexeme_upper = g_utf8_strup (start_pos, n);
      cur_token = g_hash_table_lookup (symbol_table, lexeme_upper);

      if (cur_token != NULL)
	g_free (lexeme_upper);
    }

  if (tag == LITERAL)
    {
      const char *ptr_literal = start_pos;
      while (ptr_literal != end_pos)
	{
	  if (*ptr_literal == '\n')
	    {
	      line++;
	    }
	  ptr_literal++;
	}
    }

  if (cur_token == NULL)
    {
      cur_token = (token_table *) calloc (1, sizeof (token_table) + n + 1);

      if (cur_token == NULL)
	print_error
	  ("Не удалось выделить память под элемент таблицы");

      cur_token->base.tag = tag;
      cur_token->text = (char *) cur_token + sizeof (token_table);

      memcpy (cur_token->text, start_pos, n);

      if (lexeme_upper)
	g_hash_table_insert (symbol_table, lexeme_upper, cur_token);
      else
	g_hash_table_insert (symbol_table, g_strdup (buf_str), cur_token);
    }

  return cur_token;

}

void
init_lex (char *file_name)
{
  if ((fd = open (file_name, O_RDONLY)) < 0)
    print_error ("невозможно открыть %s для чтения",
		 file_name);

  if (fstat (fd, &statbuf) < 0)
    print_error
      ("Ошибка вызова функции fstat:%s у файла %s\n",
       strerror (errno), file_name);

  if ((src =
       mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
    print_error ("%s\n",
		 "Ошибка вызова функции mmap для входного файла");

  limit = (const char *) src + statbuf.st_size;
  start_pos = (const char *) src;
  end_pos = NULL;
  symbol_table =
    g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
}

void
destroy_lex ()
{
  munmap (src, statbuf.st_size);
  g_hash_table_destroy (symbol_table);
  symbol_table = NULL;
  line = 1;
}


/*!include:re2c "unicode_categories.re" */

token *
lex ()
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
loop:

  if (end_pos)
    start_pos = end_pos;

  if (start_pos == limit)
    return NULL;

  const char *YYCURSOR = start_pos, *YYLIMIT = limit, *YYMARKER;
  /*!re2c
     re2c:define:YYCTYPE = 'unsigned char';
     re2c:yyfill:enable = 0;
     re2c:eof = 0;

     comment = [\/][\/][^\n]*;
     point = '.';
     comma = ',';
     semicolon = ';';
     left_parenthesis = '(';
     right_parenthesis = ')';
     left_square_bracket = '[';
     right_square_bracket = ']';
     whitespace = [ ]+;
     tab = [\t]+;
     newline = [\n]+;
     lt = '<';
     le = [<][=];
     eq = '=';
     ne = [<][>];
     gt = '>';
     ge = [>][=];
     plus = '+';
     minus = '-';
     multiply = '*';
     division = '/';
     remainder_of_division = '%';
     digit = [0-9];
     digits = digit+;
     number = digits([\.]digits)?;
     date = ['] [^']+ ['];
     literal = ["] ([^"] | ["]["])* ["];
     preprocessor = [&][^\n]+;
     area = [#][^\n]+;
     id = [^'"\.,;()\[\] \t\n=<>+\-*\/%&#]+;

     comment { end_pos = YYCURSOR; goto loop; }
     point { end_pos = YYCURSOR; return &(tokens[POINT]); }
     comma { end_pos = YYCURSOR; return &(tokens[COMMA]); }
     semicolon { end_pos = YYCURSOR; return &(tokens[SEMICOLON]); }
     left_parenthesis { end_pos = YYCURSOR; return &(tokens[LEFT_PARENTHESIS]); }
     right_parenthesis { end_pos = YYCURSOR; return &(tokens[RIGHT_PARENTHESIS]); }
     left_square_bracket { end_pos = YYCURSOR; return &(tokens[LEFT_SQUARE_BRACKET]); }
     right_square_bracket { end_pos = YYCURSOR; return &(tokens[RIGHT_SQUARE_BRACKET]); }
     whitespace { end_pos = YYCURSOR; return &(tokens[WHITESPACE]); }
     tab { end_pos = YYCURSOR; return &(tokens[TAB]); }
     newline { line += YYCURSOR - start_pos ; end_pos = YYCURSOR; goto loop; }
     lt { end_pos = YYCURSOR; return (token *) &(tokens_attr[LT]); }
     le { end_pos = YYCURSOR; return (token *) &(tokens_attr[LE]); }
     eq { end_pos = YYCURSOR; return (token *) &(tokens_attr[EQ]); }
     ne { end_pos = YYCURSOR; return (token *) &(tokens_attr[NE]); }
     gt { end_pos = YYCURSOR; return (token *) &(tokens_attr[GT]); }
     ge { end_pos = YYCURSOR; return (token *) &(tokens_attr[GE]); }
     plus { end_pos = YYCURSOR; return (token *) &(tokens_attr[PLUS]); }
     minus { end_pos = YYCURSOR; return (token *) &(tokens_attr[MINUS]); }
     multiply { end_pos = YYCURSOR; return (token *) &(tokens_attr[MULTIPLY]); }
     division { end_pos = YYCURSOR; return (token *) &(tokens_attr[DIVISION]); }
     remainder_of_division { end_pos = YYCURSOR; return (token *) &(tokens_attr[REMAINDER_OF_DIVISION]); }
     number { end_pos = YYCURSOR; return (token *) create_token_table(NUMBER); }
     date { end_pos = YYCURSOR; return (token *) create_token_table(DATE); }
     literal { end_pos = YYCURSOR; return (token *) create_token_table(LITERAL); }
     preprocessor { end_pos = YYCURSOR; return (token *) create_token_table(PREPROCESSOR); }
     area { end_pos = YYCURSOR; return (token *) create_token_table(AREA); }
     id { end_pos = YYCURSOR; return (token *) create_token_table(ID); }
     $ { end_pos = YYCURSOR; return &(tokens[END]); }
     *  { end_pos = YYCURSOR; return &(tokens[ERROR]); }
   */
#pragma GCC diagnostic pop
}

void
print_token (token * tok, int fd_log)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
  if (tok->tag < RELOP)
    dprintf (fd_log, "%ld(%s)\n", line, tag_text[tok->tag]);
  else if (tok->tag < NUMBER)
    dprintf (fd_log, "%ld(%s):%s\n", line, tag_text[tok->tag],
	     tag_attr_text[((token_attr *) tok)->attr]);
  else
    dprintf (fd_log, "%ld(%s):%s\n", line, tag_text[tok->tag],
	     ((token_table *) tok)->text);
#pragma GCC diagnostic pop
}

int
main (int argc, char **argv)
{

  if (argc != 3)
    print_error ("%s",
		 "Правильный формат:./a.out ./test/test01 ./log");

  int fd_log;

  if ((fd_log =
       open (argv[2], O_WRONLY | O_CREAT | O_APPEND, S_IWUSR | S_IRUSR)) < 0)
    print_error ("невозможно открыть %s для записи",
		 argv[2]);

  token *tok = NULL;

  init_lex (argv[1]);

  while ((tok = lex ()))
    print_token (tok, fd_log);

  destroy_lex ();

  return EXIT_SUCCESS;
}

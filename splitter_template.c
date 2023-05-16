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
  EMPTY				//0
    , PTR			//1
    , LT			//2
    , LE			//3
    , EQ			//4
    , NE			//5
    , GT			//6
    , GE			//7
    , PLUS			//8
    , MINUS			//9
    , MULTIPLY			//10
    , DIVISION			//11
    , REMAINDER_OF_DIVISION	//12
};

char *tag_attr_text[] = {
  "EMPTY"			//0
    , "PTR"			//1
    , "<"			//2
    , "<="			//3
    , "="			//4
    , "<>"			//5
    , ">"			//6
    , ">="			//7
    , "+"			//8
    , "-"			//9
    , "*"			//10
    , "/"			//11
    , "%"			//12
};

typedef struct
{
  enum TAG tag;
  enum TAG_ATTR attr;
  size_t line;
  char *text;
} token;

typedef struct entry_token entry_token;

struct entry_token
{
  entry_token *next;
  token token;
};

entry_token *head_token = NULL;
GHashTable *symbol_table = NULL;
char buf_str[BUFFSIZE] = { 0 };

token *
create_token (const enum TAG tag, const enum TAG_ATTR attr, const size_t line,
	      const char *start_pos, const char *end_pos)
{
  gpointer lexeme = NULL;
  size_t n = end_pos - start_pos;
  enum TAG_ATTR cur_attr = attr;
  size_t cur_line = line;

  assert (n < BUFFSIZE);

  if (tag == ID || tag == PREPROCESSOR || tag == AREA)
    {
      gchar *lexeme_find = g_utf8_strup (start_pos, n);
      lexeme = g_hash_table_lookup (symbol_table, lexeme_find);

      if (lexeme != NULL)
	g_free (lexeme_find);
      else
	{
	  lexeme = (gpointer) calloc (1, n + 1);
	  memcpy (lexeme, start_pos, n);
	  g_hash_table_insert (symbol_table, lexeme_find, lexeme);
	}
    }
  else if (tag == NUMBER || tag == DATE || tag == LITERAL)
    {
      memcpy (buf_str, start_pos, n);
      buf_str[n] = '\0';

      lexeme = g_hash_table_lookup (symbol_table, buf_str);

      if (lexeme == NULL)
	{
	  lexeme = g_strdup (buf_str);
	  g_hash_table_insert (symbol_table, g_strdup (lexeme), lexeme);
	}
    }

  if (tag == LITERAL)
    {
      const char *ptr_literal = start_pos;
      while (ptr_literal != end_pos)
	{
	  if (*ptr_literal == '\n')
	    {
	      cur_line++;
	    }
	  ptr_literal++;
	}
    }

  entry_token *new_token = (entry_token *) malloc (sizeof (entry_token));
  new_token->next = head_token;
  head_token = new_token;

  new_token->token.tag = tag;
  new_token->token.attr = cur_attr;
  new_token->token.line = cur_line;
  new_token->token.text = lexeme;

  return &(new_token->token);

}

void
init_lex ()
{
  symbol_table =
    g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
}

void
destroy_lex ()
{
  entry_token *temp = head_token, *next;
  while (temp != NULL)
    {
      next = temp->next;
      free (temp);
      temp = next;
    }

  g_hash_table_destroy (symbol_table);
  head_token = NULL;
  symbol_table = NULL;
}

void
print_error (const char *format, ...)
{
  va_list argptr;
  va_start (argptr, format);
  vprintf (format, argptr);
  va_end (argptr);
  exit (EXIT_FAILURE);
}

/*!include:re2c "unicode_categories.re" */

token *
lex (const char **start_pos, const char **end_pos, const char *limit,
     size_t line)
{
  const char *YYCURSOR = *start_pos, *YYLIMIT = limit, *YYMARKER;
  size_t cur_line = line;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
loop:
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

     comment { *start_pos = YYCURSOR; goto loop; }
     point { *end_pos = YYCURSOR; return create_token(POINT, EMPTY, cur_line, *start_pos, *end_pos); }
     comma { *end_pos = YYCURSOR; return create_token(COMMA, EMPTY, cur_line, *start_pos, *end_pos); }
     semicolon { *end_pos = YYCURSOR; return create_token(SEMICOLON, EMPTY, cur_line, *start_pos, *end_pos); }
     left_parenthesis { *end_pos = YYCURSOR; return create_token(LEFT_PARENTHESIS, EMPTY, cur_line, *start_pos, *end_pos); }
     right_parenthesis { *end_pos = YYCURSOR; return create_token(RIGHT_PARENTHESIS, EMPTY, cur_line, *start_pos, *end_pos); }
     left_square_bracket { *end_pos = YYCURSOR; return create_token(LEFT_SQUARE_BRACKET, EMPTY, cur_line, *start_pos, *end_pos); }
     right_square_bracket { *end_pos = YYCURSOR; return create_token(RIGHT_SQUARE_BRACKET, EMPTY, cur_line, *start_pos, *end_pos); }
     whitespace { *end_pos = YYCURSOR; return create_token(WHITESPACE, EMPTY, cur_line, *start_pos, *end_pos); }
     tab { *end_pos = YYCURSOR; return create_token(TAB, EMPTY, cur_line, *start_pos, *end_pos); }
     newline { cur_line += YYCURSOR - *start_pos ; *start_pos = YYCURSOR; goto loop; }
     lt { *end_pos = YYCURSOR; return create_token(RELOP, LT, cur_line, *start_pos, *end_pos); }
     le { *end_pos = YYCURSOR; return create_token(RELOP, LE, cur_line, *start_pos, *end_pos); }
     eq { *end_pos = YYCURSOR; return create_token(RELOP, EQ, cur_line, *start_pos, *end_pos); }
     ne { *end_pos = YYCURSOR; return create_token(RELOP, NE, cur_line, *start_pos, *end_pos); }
     gt { *end_pos = YYCURSOR; return create_token(RELOP, GT, cur_line, *start_pos, *end_pos); }
     ge { *end_pos = YYCURSOR; return create_token(RELOP, GE, cur_line, *start_pos, *end_pos); }
     plus { *end_pos = YYCURSOR; return create_token(MATH, PLUS, cur_line, *start_pos, *end_pos); }
     minus { *end_pos = YYCURSOR; return create_token(MATH, MINUS, cur_line, *start_pos, *end_pos); }
     multiply { *end_pos = YYCURSOR; return create_token(MATH, MULTIPLY, cur_line, *start_pos, *end_pos); }
     division { *end_pos = YYCURSOR; return create_token(MATH, DIVISION, cur_line, *start_pos, *end_pos); }
     remainder_of_division { *end_pos = YYCURSOR; return create_token(MATH, REMAINDER_OF_DIVISION, cur_line, *start_pos, *end_pos); }
     number { *end_pos = YYCURSOR; return create_token(NUMBER, PTR, cur_line, *start_pos, *end_pos); }
     date { *end_pos = YYCURSOR; return create_token(DATE, PTR, cur_line, *start_pos, *end_pos); }
     literal { *end_pos = YYCURSOR; return create_token(LITERAL, PTR, cur_line, *start_pos, *end_pos); }
     preprocessor { *end_pos = YYCURSOR; return create_token(PREPROCESSOR, PTR, cur_line, *start_pos, *end_pos); }
     area { *end_pos = YYCURSOR; return create_token(AREA, PTR, cur_line, *start_pos, *end_pos); }
     id { *end_pos = YYCURSOR; return create_token(ID, PTR, cur_line, *start_pos, *end_pos); }
     $ { *end_pos = YYCURSOR; return create_token(END, EMPTY, cur_line, *start_pos, *end_pos); }
     *  { *end_pos = YYCURSOR; return create_token(ERROR, EMPTY, cur_line, *start_pos, *end_pos); }
   */
#pragma GCC diagnostic pop
}

int
main (int argc, char **argv)
{

  if (argc != 3)
    print_error ("%s",
		 "Правильный формат:./a.out ./test/test01 ./log");

  struct stat statbuf;
  int fd, fd_log;

  if ((fd = open (argv[1], O_RDONLY)) < 0)
    print_error ("невозможно открыть %s для чтения",
		 argv[1]);

  if ((fd_log =
       open (argv[2], O_WRONLY | O_CREAT | O_APPEND, S_IWUSR | S_IRUSR)) < 0)
    print_error ("невозможно открыть %s для записи",
		 argv[2]);

  if (fstat (fd, &statbuf) < 0)
    print_error
      ("Ошибка вызова функции fstat:%s у файла %s\n",
       strerror (errno), argv[1]);

  void *src;
  if ((src =
       mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
    print_error ("%s\n",
		 "Ошибка вызова функции mmap для входного файла");

  const char *limit = (const char *) src + statbuf.st_size;
  const char *start_pos = (const char *) src;
  const char *end_pos = NULL;
  token *tok = NULL;
  size_t line = 1;

  init_lex ();
  do
    {
      tok = lex (&start_pos, &end_pos, limit, line);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
if (tok->attr == EMPTY)
      dprintf (fd_log, "%ld(%s)\n", tok->line, 
	       tag_text[tok->tag]);
else if (tok->attr == PTR)
      dprintf (fd_log, "%ld(%s):%s\n", tok->line, tag_text[tok->tag], tok->text
	       );
else
      dprintf (fd_log, "%ld(%s):%s\n", tok->line, tag_text[tok->tag], tag_attr_text[tok->attr]
	       );
#pragma GCC diagnostic pop
      start_pos = end_pos;
      line = tok->line;
    }
  while (start_pos != limit);
  munmap (src, statbuf.st_size);
  destroy_lex ();
  return EXIT_SUCCESS;
}

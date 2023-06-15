// re2c $INPUT -o $OUTPUT -8 --case-ranges -i
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <stdio.h>
#include "lex.h"
#include "syn.h"
#include "print_error.h"

#define AS_PTR(val) (intptr_t) (val)
#define AS_TOKEN(val) (int) (val)

GHashTable *symbol_table = NULL, *reserve_symbol_table = NULL;
char buf_str[BUFFSIZE] = { 0 };

int module_fd = 0;
struct stat statbuf = { 0 };

void *src = NULL;
const char *start_pos = NULL;
const char *end_pos = NULL;
const char *limit = NULL;
size_t line = 1;

int
get_token (int pre_token, YYSTYPE *lvalp, YYLTYPE *llocp)
{
  char *token_val = NULL;
  size_t n = end_pos - start_pos;
  char *lexeme_upper = NULL;
  int token = pre_token;

  assert (n < BUFFSIZE);

  if (token == NUMBER || token == DATE || token == LITERAL)
    {
      memcpy (buf_str, start_pos, n);
      buf_str[n] = '\0';
      token_val = g_hash_table_lookup (symbol_table, buf_str);
    }
  else
/*if (token == PREPROCESSOR || token == DIRECTIVE || token == ID || token == MARK)*/
    {
      lexeme_upper = g_utf8_strup (start_pos, n);

      if (token == ID || token == DIRECTIVE)
	token_val = g_hash_table_lookup (reserve_symbol_table, lexeme_upper);

      if (token_val)
        token =  AS_TOKEN(token_val);
      else
	{
	  if (token == DIRECTIVE)
            token = YYUNDEF;
	  else
	    token_val = g_hash_table_lookup (symbol_table, lexeme_upper);
	}

      if (token_val != NULL)
	{
	  g_free (lexeme_upper);
	  lexeme_upper = NULL;
	}
    }

  if (token == LITERAL)
    {
      const char *ptr_literal = start_pos;
      while (ptr_literal != end_pos)
	if (*ptr_literal++ == '\n')
	  line++;
    }

  if (token_val == NULL)
    {
      token_val = (char *) calloc (1, n + 1);

      if (token_val == NULL)
	print_error
	  ("Не удалось выделить память под элемент таблицы");
      else
	{
	  memcpy (token_val, start_pos, n);
	  if (lexeme_upper)
	    g_hash_table_insert (symbol_table, lexeme_upper, token_val);
	  else
	    g_hash_table_insert (symbol_table, g_strdup (buf_str), token_val);
	}
    }

  if (token == NUMBER || token == DATE || token == LITERAL
      || token == PREPROCESSOR || token == ID || token == MARK)
    *lvalp = token_val;

  llocp->last_line = line;

  return token;

}

void
fill_reserve_symbol_table ()
{
//IF
  g_hash_table_insert (reserve_symbol_table, "ЕСЛИ", AS_PTR(IF));
  g_hash_table_insert (reserve_symbol_table, "IF", AS_PTR(IF));
  g_hash_table_insert (reserve_symbol_table, "ТОГДА", AS_PTR(THEN));
  g_hash_table_insert (reserve_symbol_table, "THEN", AS_PTR(THEN));
  g_hash_table_insert (reserve_symbol_table, "ИНАЧЕЕСЛИ",
		       AS_PTR(ELSIF));
  g_hash_table_insert (reserve_symbol_table, "ELSIF", AS_PTR(ELSIF));
  g_hash_table_insert (reserve_symbol_table, "ИНАЧЕ", AS_PTR(ELSE));
  g_hash_table_insert (reserve_symbol_table, "ELSE", AS_PTR(ELSE));
  g_hash_table_insert (reserve_symbol_table, "КОНЕЦЕСЛИ",
		       AS_PTR(ENDIF));
  g_hash_table_insert (reserve_symbol_table, "ENDIF", AS_PTR(ENDIF));

//DO
  g_hash_table_insert (reserve_symbol_table, "ДЛЯ", AS_PTR(FOR));
  g_hash_table_insert (reserve_symbol_table, "FOR", AS_PTR(FOR));
  g_hash_table_insert (reserve_symbol_table, "КАЖДОГО",
		       AS_PTR(EACH));
  g_hash_table_insert (reserve_symbol_table, "EACH", AS_PTR(EACH));
  g_hash_table_insert (reserve_symbol_table, "ИЗ", AS_PTR(IN));
  g_hash_table_insert (reserve_symbol_table, "IN", AS_PTR(IN));
  g_hash_table_insert (reserve_symbol_table, "ПО", AS_PTR(TO));
  g_hash_table_insert (reserve_symbol_table, "TO", AS_PTR(TO));
  g_hash_table_insert (reserve_symbol_table, "ПОКА", AS_PTR(WHILE));
  g_hash_table_insert (reserve_symbol_table, "WHILE", AS_PTR(WHILE));
  g_hash_table_insert (reserve_symbol_table, "ЦИКЛ", AS_PTR(DO));
  g_hash_table_insert (reserve_symbol_table, "DO", AS_PTR(DO));
  g_hash_table_insert (reserve_symbol_table, "ПРЕРВАТЬ",
		       AS_PTR(BREAK));
  g_hash_table_insert (reserve_symbol_table, "BREAK", AS_PTR(BREAK));
  g_hash_table_insert (reserve_symbol_table, "ПРОДОЛЖИТЬ",
		       AS_PTR(CONTINUE));
  g_hash_table_insert (reserve_symbol_table, "CONTINUE", AS_PTR(CONTINUE));
  g_hash_table_insert (reserve_symbol_table, "КОНЕЦЦИКЛА",
		       AS_PTR(ENDDO));
  g_hash_table_insert (reserve_symbol_table, "ENDDO", AS_PTR(ENDDO));

//FUNCTION
  g_hash_table_insert (reserve_symbol_table, "ПРОЦЕДУРА",
		       AS_PTR(PROCEDURE));
  g_hash_table_insert (reserve_symbol_table, "PROCEDURE",
		       AS_PTR(PROCEDURE));
  g_hash_table_insert (reserve_symbol_table, "ФУНКЦИЯ",
		       AS_PTR(FUNCTION));
  g_hash_table_insert (reserve_symbol_table, "FUNCTION", AS_PTR(FUNCTION));
  g_hash_table_insert (reserve_symbol_table, "КОНЕЦПРОЦЕДУРЫ",
		       AS_PTR(ENDPROCEDURE));
  g_hash_table_insert (reserve_symbol_table, "ENDPROCEDURE",
		       AS_PTR(ENDPROCEDURE));
  g_hash_table_insert (reserve_symbol_table, "КОНЕЦФУНКЦИИ",
		       AS_PTR(ENDFUNCTION));
  g_hash_table_insert (reserve_symbol_table, "ENDFUNCTION",
		       AS_PTR(ENDFUNCTION));
  g_hash_table_insert (reserve_symbol_table, "ПЕРЕМ", AS_PTR(VAR));
  g_hash_table_insert (reserve_symbol_table, "VAR", AS_PTR(VAR));
  g_hash_table_insert (reserve_symbol_table, "ПЕРЕЙТИ",
		       AS_PTR(GOTO));
  g_hash_table_insert (reserve_symbol_table, "GOTO", AS_PTR(GOTO));
  g_hash_table_insert (reserve_symbol_table, "ВОЗВРАТ",
		       AS_PTR(RETURN));
  g_hash_table_insert (reserve_symbol_table, "RETURN", AS_PTR(RETURN));
  g_hash_table_insert (reserve_symbol_table, "И", AS_PTR(AND));
  g_hash_table_insert (reserve_symbol_table, "AND", AS_PTR(AND));
  g_hash_table_insert (reserve_symbol_table, "ИЛИ", AS_PTR(OR));
  g_hash_table_insert (reserve_symbol_table, "OR", AS_PTR(OR));
  g_hash_table_insert (reserve_symbol_table, "НЕ", AS_PTR(NOT));
  g_hash_table_insert (reserve_symbol_table, "NOT", AS_PTR(NOT));
  g_hash_table_insert (reserve_symbol_table, "ПОПЫТКА",
		       AS_PTR(TRY));
  g_hash_table_insert (reserve_symbol_table, "TRY", AS_PTR(TRY));
  g_hash_table_insert (reserve_symbol_table, "ИСКЛЮЧЕНИЕ",
		       AS_PTR(EXCEPT));
  g_hash_table_insert (reserve_symbol_table, "EXCEPT", AS_PTR(EXCEPT));
  g_hash_table_insert (reserve_symbol_table,
		       "ВЫЗВАТЬИСКЛЮЧЕНИЕ",
		       AS_PTR(RAISE));
  g_hash_table_insert (reserve_symbol_table, "RAISE", AS_PTR(RAISE));
  g_hash_table_insert (reserve_symbol_table, "КОНЕЦПОПЫТКИ",
		       AS_PTR(ENDTRY));
  g_hash_table_insert (reserve_symbol_table, "ENDTRY", AS_PTR(ENDTRY));
  g_hash_table_insert (reserve_symbol_table, "НОВЫЙ", AS_PTR(NEW));
  g_hash_table_insert (reserve_symbol_table, "NEW", AS_PTR(NEW));
  g_hash_table_insert (reserve_symbol_table, "ВЫПОЛНИТЬ",
		       AS_PTR(EXECUTE));
  g_hash_table_insert (reserve_symbol_table, "EXECUTE", AS_PTR(EXECUTE));
  g_hash_table_insert (reserve_symbol_table,
		       "ДОБАВИТЬОБРАБОТЧИК",
		       AS_PTR(ADDHANDLER));
  g_hash_table_insert (reserve_symbol_table, "ADDHANDLER",
		       AS_PTR(ADDHANDLER));
  g_hash_table_insert (reserve_symbol_table,
		       "УДАЛИТЬОБРАБОТЧИК",
		       AS_PTR(REMOVEHANDLER));
  g_hash_table_insert (reserve_symbol_table, "REMOVEHANDLER",
		       AS_PTR(REMOVEHANDLER));
//TYPE
  g_hash_table_insert (reserve_symbol_table, "NULL", AS_PTR(T_NULL));
  g_hash_table_insert (reserve_symbol_table, "ИСТИНА",
		       AS_PTR(T_TRUE));
  g_hash_table_insert (reserve_symbol_table, "TRUE", AS_PTR(T_TRUE));
  g_hash_table_insert (reserve_symbol_table, "ЛОЖЬ", AS_PTR(T_FALSE));
  g_hash_table_insert (reserve_symbol_table, "FALSE", AS_PTR(T_FALSE));
  g_hash_table_insert (reserve_symbol_table, "НЕОПРЕДЕЛЕНО",
		       AS_PTR(T_UNDEFINED));
  g_hash_table_insert (reserve_symbol_table, "UNDEFINED",
		       AS_PTR(T_UNDEFINED));
  g_hash_table_insert (reserve_symbol_table, "ЭКСПОРТ",
		       AS_PTR(EXPORT));
  g_hash_table_insert (reserve_symbol_table, "EXPORT", AS_PTR(EXPORT));
  g_hash_table_insert (reserve_symbol_table, "ЗНАЧ", AS_PTR(VAL));
  g_hash_table_insert (reserve_symbol_table, "VAL", AS_PTR(VAL));

//DIRECTIVE
  g_hash_table_insert (reserve_symbol_table, "&НАКЛИЕНТЕ",
		       AS_PTR(ATCLIENT));
  g_hash_table_insert (reserve_symbol_table, "&ATCLIENT",
		       AS_PTR(ATCLIENT));
  g_hash_table_insert (reserve_symbol_table, "&НАСЕРВЕРЕ",
		       AS_PTR(ATSERVER));
  g_hash_table_insert (reserve_symbol_table, "&ATSERVER",
		       AS_PTR(ATSERVER));
  g_hash_table_insert (reserve_symbol_table,
		       "&НАСЕРВЕРЕБЕЗКОНТЕКСТА",
		       AS_PTR(ATSERVERNOCONTEXT));
  g_hash_table_insert (reserve_symbol_table, "&ATSERVERNOCONTEXT",
		       AS_PTR(ATSERVERNOCONTEXT));
  g_hash_table_insert (reserve_symbol_table,
		       "&НАКЛИЕНТЕНАСЕРВЕРЕБЕЗКОНТЕКСТА",
		       AS_PTR(ATCLIENTATSERVERNOCONTEXT));
  g_hash_table_insert (reserve_symbol_table, "&ATCLIENTATSERVERNOCONTEXT",
		       AS_PTR(ATCLIENTATSERVERNOCONTEXT));
  g_hash_table_insert (reserve_symbol_table,
		       "&НАКЛИЕНТЕНАСЕРВЕРЕ",
		       AS_PTR(ATCLIENTATSERVER));
  g_hash_table_insert (reserve_symbol_table, "&ATCLIENTATSERVER",
		       AS_PTR(ATCLIENTATSERVER));


void
init_lex (char *file_name)
{
  if ((module_fd = open (file_name, O_RDONLY)) < 0)
    print_error ("невозможно открыть %s для чтения",
		 file_name);

  if (fstat (module_fd, &statbuf) < 0)
    print_error
      ("Ошибка вызова функции fstat:%s у файла %s\n",
       strerror (errno), file_name);

  if ((src =
       mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, module_fd,
	     0)) == MAP_FAILED)
    print_error ("%s\n",
		 "Ошибка вызова функции mmap для входного файла");

  limit = (const char *) src + statbuf.st_size;
  start_pos = (const char *) src;
  end_pos = NULL;
  symbol_table =
    g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

  reserve_symbol_table = g_hash_table_new (g_str_hash, g_str_equal);
  fill_reserve_symbol_table ();
}


void
destroy_lex ()
{
  munmap (src, statbuf.st_size);
  g_hash_table_destroy (symbol_table);
  g_hash_table_destroy (reserve_symbol_table);
  symbol_table = NULL;
  reserve_symbol_table = NULL;
  line = 1;
}


/*!include:re2c "unicode_categories.re" */

int
yylex (YYSTYPE *lvalp, YYLTYPE *llocp)
{
  llocp->first_line = line;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
loop:

  if (end_pos)
    start_pos = end_pos;

  if (start_pos == limit)
    return YYEOF;

  const char *YYCURSOR = start_pos, *YYLIMIT = limit, *YYMARKER;
  /*!re2c
     re2c:define:YYCTYPE = 'unsigned char';
     re2c:yyfill:enable = 0;
     re2c:eof = 0;

     comment = [\/][\/][^\n]*;
     point = '.';
     comma = ',';
     colon = ':';
     semicolon = ';';
     left_parenthesis = '(';
     right_parenthesis = ')';
     left_square_bracket = '[';
     right_square_bracket = ']';
     whitespace = [ ]+;
     tab = [\t]+;
     newline = [\n];
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
     id = [^'"\.,:;()\[\] \t\n=<>+\-*\/%&#~]+;
     mark = [~] id;
     directive = [&] id;
     preprocessor = [#][^\n]+;

     comment { end_pos = YYCURSOR; goto loop; }
     point { end_pos = YYCURSOR; return AS_TOKEN('.'); }
     comma { end_pos = YYCURSOR; return AS_TOKEN(','); }
     colon { end_pos = YYCURSOR; return AS_TOKEN(':'); }
     semicolon { end_pos = YYCURSOR; return AS_TOKEN(';'); }
     left_parenthesis { end_pos = YYCURSOR; return AS_TOKEN('('); }
     right_parenthesis { end_pos = YYCURSOR; return AS_TOKEN(')'); }
     left_square_bracket { end_pos = YYCURSOR; return AS_TOKEN('['); }
     right_square_bracket { end_pos = YYCURSOR; return AS_TOKEN(']'); }
     whitespace { end_pos = YYCURSOR; goto loop; }
     tab { end_pos = YYCURSOR; goto loop; }
     newline { line++ ; end_pos = YYCURSOR; goto loop; }
     lt { end_pos = YYCURSOR; return AS_TOKEN('<'); }
     le { end_pos = YYCURSOR; return LE; }
     eq { end_pos = YYCURSOR; return AS_TOKEN('='); }
     ne { end_pos = YYCURSOR; return NE; }
     gt { end_pos = YYCURSOR; return AS_TOKEN('>'); }
     ge { end_pos = YYCURSOR; return GE; }
     plus { end_pos = YYCURSOR; return AS_TOKEN('+'); }
     minus { end_pos = YYCURSOR; return AS_TOKEN('-'); }
     multiply { end_pos = YYCURSOR; return AS_TOKEN('*'); }
     division { end_pos = YYCURSOR; return AS_TOKEN('/'); }
     remainder_of_division { end_pos = YYCURSOR; return AS_TOKEN('%'); }
     number { end_pos = YYCURSOR; return (token *) get_token(NUMBER, lvalp, llocp); }
     date { end_pos = YYCURSOR; return (token *) get_token(DATE, lvalp, llocp); }
     literal { end_pos = YYCURSOR; return (token *) get_token(LITERAL, lvalp, llocp); }
     preprocessor { end_pos = YYCURSOR; return (token *) get_token(PREPROCESSOR, lvalp, llocp); }
     directive { end_pos = YYCURSOR; return (token *) get_token(DIRECTIVE, lvalp, llocp); }
     mark { end_pos = YYCURSOR; return (token *) get_token(MARK, lvalp, llocp); }
     id { end_pos = YYCURSOR; return (token *) get_token(ID, lvalp, llocp); }
     $ { end_pos = YYCURSOR; return YYEOF; }
     *  { end_pos = YYCURSOR; return YYUNDEF; }
   */
#pragma GCC diagnostic pop
}


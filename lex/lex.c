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

#include "lex.h"
#include "../print_error.h"

char *tag_text[] = {
  "ERROR"			//0
    , "END"			//1
    , "TEST"			//2
    , "POINT"			//3
    , "COMMA"			//4
    , "COLON"			//5
    , "SEMICOLON"		//6
    , "LEFT_PARENTHESIS"	//7
    , "RIGHT_PARENTHESIS"	//8
    , "LEFT_SQUARE_BRACKET"	//9
    , "RIGHT_SQUARE_BRACKET"	//10
    , "WHITESPACE"		//11
    , "TAB"			//12
    , "NEWLINE"			//13
//IF
    , "IF"			//14
    , "THEN"			//15
    , "ELSIF"			//16
    , "ELSE"			//17
    , "ENDIF"			//18
//DO
    , "FOR"			//19
    , "EACH"			//20
    , "IN"			//21
    , "TO"			//22
    , "WHILE"			//23
    , "DO"			//24
    , "BREAK"			//25
    , "CONTINUE"		//26
    , "ENDDO"			//27
//FUNCTION
    , "PROCEDURE"		//28
    , "FUNCTION"		//29
    , "ENDPROCEDURE"		//30
    , "ENDFUNCTION"		//31
    , "VAR"			//32
    , "GOTO"			//33
    , "RETURN"			//34
    , "AND"			//35
    , "OR"			//36
    , "NOT"			//37
    , "TRY"			//38
    , "EXCEPT"			//39
    , "RAISE"			//40
    , "ENDTRY"			//41
    , "NEW"			//42
    , "EXECUTE"			//43
    , "ADDHANDLER"		//44
    , "REMOVEHANDLER"		//45
//type
    , "T_NULL"			//46
    , "T_TRUE"			//47
    , "T_FALSE"			//48
    , "T_UNDEFINED"		//49
    , "EXPORT"			//50
    , "VAL"			//51
//DIRECTIVE
    , "ATCLIENT"		//52
    , "ATSERVER"		//53
    , "ATSERVERNOCONTEXT"	//54
    , "ATCLIENTATSERVERNOCONTEXT"	//55
    , "ATCLIENTATSERVER"	//56
//OTHER
    , "RELOP"			//
    , "MATH"			//
    , "NUMBER"			//
    , "DATE"			//
    , "LITERAL"			//
    , "PREPROCESSOR"		//
    , "DIRECTIVE"		//
    , "MARK"			//
    , "ID"			//
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

token tokens[] = {
  {ERROR}			//0
  , {END}			//1
  , {TEST}			//2
  , {POINT}			//3
  , {COMMA}			//4
  , {COLON}			//5
  , {SEMICOLON}			//6
  , {LEFT_PARENTHESIS}		//7
  , {RIGHT_PARENTHESIS}		//8
  , {LEFT_SQUARE_BRACKET}	//9
  , {RIGHT_SQUARE_BRACKET}	//10
  , {WHITESPACE}		//11
  , {TAB}			//12
  , {NEWLINE}			//13
//IF
  , {IF}			//14
  , {THEN}			//15
  , {ELSIF}			//16
  , {ELSE}			//17
  , {ENDIF}			//18
//DO
  , {FOR}			//19
  , {EACH}			//20
  , {IN}			//21
  , {TO}			//22
  , {WHILE}			//23
  , {DO}			//24
  , {BREAK}			//25
  , {CONTINUE}			//26
  , {ENDDO}			//27
//FUNCTION
  , {PROCEDURE}			//28
  , {FUNCTION}			//29
  , {ENDPROCEDURE}		//30
  , {ENDFUNCTION}		//31
  , {VAR}			//32
  , {GOTO}			//33
  , {RETURN}			//34
  , {AND}			//35
  , {OR}			//36
  , {NOT}			//37
  , {TRY}			//38
  , {EXCEPT}			//39
  , {RAISE}			//40
  , {ENDTRY}			//41
  , {NEW}			//42
  , {EXECUTE}			//43
  , {ADDHANDLER}		//44
  , {REMOVEHANDLER}		//45
//type
  , {T_NULL}			//46
  , {T_TRUE}			//47
  , {T_FALSE}			//48
  , {T_UNDEFINED}		//49
  , {EXPORT}			//50
  , {VAL}			//51
//DIRECTIVE
  , {ATCLIENT}			//52
  , {ATSERVER}			//53
  , {ATSERVERNOCONTEXT}		//54
  , {ATCLIENTATSERVERNOCONTEXT}	//55
  , {ATCLIENTATSERVER}		//56
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

GHashTable *symbol_table = NULL, *reserve_symbol_table = NULL;
char buf_str[BUFFSIZE] = { 0 };

int fd = 0;
struct stat statbuf = { 0 };

void *src = NULL;
const char *start_pos = NULL;
const char *end_pos = NULL;
const char *limit = NULL;
size_t line = 1;


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
/*if (tag == PREPROCESSOR || tag == DIRECTIVE || tag == ID)*/
    {
      lexeme_upper = g_utf8_strup (start_pos, n);

      if (tag == ID || tag == DIRECTIVE)
	cur_token = g_hash_table_lookup (reserve_symbol_table, lexeme_upper);

      if (cur_token == NULL)
	{
	  if (tag == DIRECTIVE)
	    cur_token = (token_table *) & (tokens[ERROR]);
	  else
	    cur_token = g_hash_table_lookup (symbol_table, lexeme_upper);
	}

      if (cur_token != NULL)
	g_free (lexeme_upper);
    }

  if (tag == LITERAL)
    {
      const char *ptr_literal = start_pos;
      while (ptr_literal != end_pos)
	if (*ptr_literal++ == '\n')
	  line++;
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
fill_reserve_symbol_table ()
{
//IF
  g_hash_table_insert (reserve_symbol_table, "ЕСЛИ", &(tokens[IF]));
  g_hash_table_insert (reserve_symbol_table, "IF", &(tokens[IF]));
  g_hash_table_insert (reserve_symbol_table, "ТОГДА", &(tokens[THEN]));
  g_hash_table_insert (reserve_symbol_table, "THEN", &(tokens[THEN]));
  g_hash_table_insert (reserve_symbol_table, "ИНАЧЕЕСЛИ",
		       &(tokens[ELSIF]));
  g_hash_table_insert (reserve_symbol_table, "ELSIF", &(tokens[ELSIF]));
  g_hash_table_insert (reserve_symbol_table, "ИНАЧЕ", &(tokens[ELSE]));
  g_hash_table_insert (reserve_symbol_table, "ELSE", &(tokens[ELSE]));
  g_hash_table_insert (reserve_symbol_table, "КОНЕЦЕСЛИ",
		       &(tokens[ENDIF]));
  g_hash_table_insert (reserve_symbol_table, "ENDIF", &(tokens[ENDIF]));

//DO
  g_hash_table_insert (reserve_symbol_table, "ДЛЯ", &(tokens[FOR]));
  g_hash_table_insert (reserve_symbol_table, "FOR", &(tokens[FOR]));
  g_hash_table_insert (reserve_symbol_table, "КАЖДОГО",
		       &(tokens[EACH]));
  g_hash_table_insert (reserve_symbol_table, "EACH", &(tokens[EACH]));
  g_hash_table_insert (reserve_symbol_table, "ИЗ", &(tokens[IN]));
  g_hash_table_insert (reserve_symbol_table, "IN", &(tokens[IN]));
  g_hash_table_insert (reserve_symbol_table, "ПО", &(tokens[TO]));
  g_hash_table_insert (reserve_symbol_table, "TO", &(tokens[TO]));
  g_hash_table_insert (reserve_symbol_table, "ПОКА", &(tokens[WHILE]));
  g_hash_table_insert (reserve_symbol_table, "WHILE", &(tokens[WHILE]));
  g_hash_table_insert (reserve_symbol_table, "ЦИКЛ", &(tokens[DO]));
  g_hash_table_insert (reserve_symbol_table, "DO", &(tokens[DO]));
  g_hash_table_insert (reserve_symbol_table, "ПРЕРВАТЬ",
		       &(tokens[BREAK]));
  g_hash_table_insert (reserve_symbol_table, "BREAK", &(tokens[BREAK]));
  g_hash_table_insert (reserve_symbol_table, "ПРОДОЛЖИТЬ",
		       &(tokens[CONTINUE]));
  g_hash_table_insert (reserve_symbol_table, "CONTINUE", &(tokens[CONTINUE]));
  g_hash_table_insert (reserve_symbol_table, "КОНЕЦЦИКЛА",
		       &(tokens[ENDDO]));
  g_hash_table_insert (reserve_symbol_table, "ENDDO", &(tokens[ENDDO]));

//FUNCTION
  g_hash_table_insert (reserve_symbol_table, "ПРОЦЕДУРА",
		       &(tokens[PROCEDURE]));
  g_hash_table_insert (reserve_symbol_table, "PROCEDURE",
		       &(tokens[PROCEDURE]));
  g_hash_table_insert (reserve_symbol_table, "ФУНКЦИЯ",
		       &(tokens[FUNCTION]));
  g_hash_table_insert (reserve_symbol_table, "FUNCTION", &(tokens[FUNCTION]));
  g_hash_table_insert (reserve_symbol_table, "КОНЕЦПРОЦЕДУРЫ",
		       &(tokens[ENDPROCEDURE]));
  g_hash_table_insert (reserve_symbol_table, "ENDPROCEDURE",
		       &(tokens[ENDPROCEDURE]));
  g_hash_table_insert (reserve_symbol_table, "КОНЕЦФУНКЦИИ",
		       &(tokens[ENDFUNCTION]));
  g_hash_table_insert (reserve_symbol_table, "ENDFUNCTION",
		       &(tokens[ENDFUNCTION]));
  g_hash_table_insert (reserve_symbol_table, "ПЕРЕМ", &(tokens[VAR]));
  g_hash_table_insert (reserve_symbol_table, "VAR", &(tokens[VAR]));
  g_hash_table_insert (reserve_symbol_table, "ПЕРЕЙТИ",
		       &(tokens[GOTO]));
  g_hash_table_insert (reserve_symbol_table, "GOTO", &(tokens[GOTO]));
  g_hash_table_insert (reserve_symbol_table, "ВОЗВРАТ",
		       &(tokens[RETURN]));
  g_hash_table_insert (reserve_symbol_table, "RETURN", &(tokens[RETURN]));
  g_hash_table_insert (reserve_symbol_table, "И", &(tokens[AND]));
  g_hash_table_insert (reserve_symbol_table, "AND", &(tokens[AND]));
  g_hash_table_insert (reserve_symbol_table, "ИЛИ", &(tokens[OR]));
  g_hash_table_insert (reserve_symbol_table, "OR", &(tokens[OR]));
  g_hash_table_insert (reserve_symbol_table, "НЕ", &(tokens[NOT]));
  g_hash_table_insert (reserve_symbol_table, "NOT", &(tokens[NOT]));
  g_hash_table_insert (reserve_symbol_table, "ПОПЫТКА",
		       &(tokens[TRY]));
  g_hash_table_insert (reserve_symbol_table, "TRY", &(tokens[TRY]));
  g_hash_table_insert (reserve_symbol_table, "ИСКЛЮЧЕНИЕ",
		       &(tokens[EXCEPT]));
  g_hash_table_insert (reserve_symbol_table, "EXCEPT", &(tokens[EXCEPT]));
  g_hash_table_insert (reserve_symbol_table,
		       "ВЫЗВАТЬИСКЛЮЧЕНИЕ",
		       &(tokens[RAISE]));
  g_hash_table_insert (reserve_symbol_table, "RAISE", &(tokens[RAISE]));
  g_hash_table_insert (reserve_symbol_table, "КОНЕЦПОПЫТКИ",
		       &(tokens[ENDTRY]));
  g_hash_table_insert (reserve_symbol_table, "ENDTRY", &(tokens[ENDTRY]));
  g_hash_table_insert (reserve_symbol_table, "НОВЫЙ", &(tokens[NEW]));
  g_hash_table_insert (reserve_symbol_table, "NEW", &(tokens[NEW]));
  g_hash_table_insert (reserve_symbol_table, "ВЫПОЛНИТЬ",
		       &(tokens[EXECUTE]));
  g_hash_table_insert (reserve_symbol_table, "EXECUTE", &(tokens[EXECUTE]));
  g_hash_table_insert (reserve_symbol_table,
		       "ДОБАВИТЬОБРАБОТЧИК",
		       &(tokens[ADDHANDLER]));
  g_hash_table_insert (reserve_symbol_table, "ADDHANDLER",
		       &(tokens[ADDHANDLER]));
  g_hash_table_insert (reserve_symbol_table,
		       "УДАЛИТЬОБРАБОТЧИК",
		       &(tokens[REMOVEHANDLER]));
  g_hash_table_insert (reserve_symbol_table, "REMOVEHANDLER",
		       &(tokens[REMOVEHANDLER]));
//TYPE
  g_hash_table_insert (reserve_symbol_table, "NULL", &(tokens[T_NULL]));
  g_hash_table_insert (reserve_symbol_table, "ИСТИНА",
		       &(tokens[T_TRUE]));
  g_hash_table_insert (reserve_symbol_table, "TRUE", &(tokens[T_TRUE]));
  g_hash_table_insert (reserve_symbol_table, "ЛОЖЬ", &(tokens[T_FALSE]));
  g_hash_table_insert (reserve_symbol_table, "FALSE", &(tokens[T_FALSE]));
  g_hash_table_insert (reserve_symbol_table, "НЕОПРЕДЕЛЕНО",
		       &(tokens[T_UNDEFINED]));
  g_hash_table_insert (reserve_symbol_table, "UNDEFINED",
		       &(tokens[T_UNDEFINED]));
  g_hash_table_insert (reserve_symbol_table, "ЭКСПОРТ",
		       &(tokens[EXPORT]));
  g_hash_table_insert (reserve_symbol_table, "EXPORT", &(tokens[EXPORT]));
  g_hash_table_insert (reserve_symbol_table, "ЗНАЧ", &(tokens[VAL]));
  g_hash_table_insert (reserve_symbol_table, "VAL", &(tokens[VAL]));

//DIRECTIVE
  g_hash_table_insert (reserve_symbol_table, "&НАКЛИЕНТЕ",
		       &(tokens[ATCLIENT]));
  g_hash_table_insert (reserve_symbol_table, "&ATCLIENT",
		       &(tokens[ATCLIENT]));
  g_hash_table_insert (reserve_symbol_table, "&НАСЕРВЕРЕ",
		       &(tokens[ATSERVER]));
  g_hash_table_insert (reserve_symbol_table, "&ATSERVER",
		       &(tokens[ATSERVER]));
  g_hash_table_insert (reserve_symbol_table,
		       "&НАСЕРВЕРЕБЕЗКОНТЕКСТА",
		       &(tokens[ATSERVERNOCONTEXT]));
  g_hash_table_insert (reserve_symbol_table, "&ATSERVERNOCONTEXT",
		       &(tokens[ATSERVERNOCONTEXT]));
  g_hash_table_insert (reserve_symbol_table,
		       "&НАКЛИЕНТЕНАСЕРВЕРЕБЕЗКОНТЕКСТА",
		       &(tokens[ATCLIENTATSERVERNOCONTEXT]));
  g_hash_table_insert (reserve_symbol_table, "&ATCLIENTATSERVERNOCONTEXT",
		       &(tokens[ATCLIENTATSERVERNOCONTEXT]));
  g_hash_table_insert (reserve_symbol_table,
		       "&НАКЛИЕНТЕНАСЕРВЕРЕ",
		       &(tokens[ATCLIENTATSERVER]));
  g_hash_table_insert (reserve_symbol_table, "&ATCLIENTATSERVER",
		       &(tokens[ATCLIENTATSERVER]));
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

token *
get_token ()
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
     colon = ':';
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
     preprocessor = [#][^\n]+;
     id = [^'"\.,:;()\[\] \t\n=<>+\-*\/%&#~]+;
     mark = [~] id;
     directive = [&] id;

     comment { end_pos = YYCURSOR; goto loop; }
     point { end_pos = YYCURSOR; return &(tokens[POINT]); }
     comma { end_pos = YYCURSOR; return &(tokens[COMMA]); }
     colon { end_pos = YYCURSOR; return &(tokens[COLON]); }
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
     directive { end_pos = YYCURSOR; return (token *) create_token_table(DIRECTIVE); }
     mark { end_pos = YYCURSOR; return (token *) create_token_table(MARK); }
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

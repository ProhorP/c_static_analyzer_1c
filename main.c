#include "main.h"
#include "syn.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

char * choose_string = NULL;//value free at destroy symbol_table
char * field_query = "Поле запроса 1";

GHashTable *symbol_table, *token_table,
           *dynamic_val_table;
char buf_str[BUFFSIZE];

static int module_fd;
struct stat statbuf;

void *src;
const char* start_pos;
const char* end_pos;
const char* limit;
size_t line;

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
    /*if (token == PREPROCESSOR || token == DIRECTIVE || token == ID || token
     * == MARK)*/
    {
      lexeme_upper = g_utf8_strup (start_pos, n);

      if (token == ID || token == DIRECTIVE)
        token_val = g_hash_table_lookup (token_table, lexeme_upper);

      if (token_val)
        {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
          token = AS_TOKEN (token_val);
#pragma GCC diagnostic pop
        }
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
      token_val = (char *)calloc (1, n + 1);
      assert(token_val != NULL);

      if (token_val)
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-conversion"
void
fill_token_table ()
{
  // IF
  g_hash_table_insert (token_table, "ЕСЛИ", AS_PTR (IF));
  g_hash_table_insert (token_table, "IF", AS_PTR (IF));
  g_hash_table_insert (token_table, "ТОГДА", AS_PTR (THEN));
  g_hash_table_insert (token_table, "THEN", AS_PTR (THEN));
  g_hash_table_insert (token_table, "ИНАЧЕЕСЛИ", AS_PTR (ELSIF));
  g_hash_table_insert (token_table, "ELSIF", AS_PTR (ELSIF));
  g_hash_table_insert (token_table, "ИНАЧЕ", AS_PTR (ELSE));
  g_hash_table_insert (token_table, "ELSE", AS_PTR (ELSE));
  g_hash_table_insert (token_table, "КОНЕЦЕСЛИ", AS_PTR (ENDIF));
  g_hash_table_insert (token_table, "ENDIF", AS_PTR (ENDIF));

  // DO
  g_hash_table_insert (token_table, "ДЛЯ", AS_PTR (FOR));
  g_hash_table_insert (token_table, "FOR", AS_PTR (FOR));
  g_hash_table_insert (token_table, "КАЖДОГО", AS_PTR (EACH));
  g_hash_table_insert (token_table, "EACH", AS_PTR (EACH));
  g_hash_table_insert (token_table, "ИЗ", AS_PTR (IN));
  g_hash_table_insert (token_table, "IN", AS_PTR (IN));
  g_hash_table_insert (token_table, "ПО", AS_PTR (TO));
  g_hash_table_insert (token_table, "TO", AS_PTR (TO));
  g_hash_table_insert (token_table, "ПОКА", AS_PTR (WHILE));
  g_hash_table_insert (token_table, "WHILE", AS_PTR (WHILE));
  g_hash_table_insert (token_table, "ЦИКЛ", AS_PTR (DO));
  g_hash_table_insert (token_table, "DO", AS_PTR (DO));
  g_hash_table_insert (token_table, "ПРЕРВАТЬ", AS_PTR (BREAK));
  g_hash_table_insert (token_table, "BREAK", AS_PTR (BREAK));
  g_hash_table_insert (token_table, "ПРОДОЛЖИТЬ", AS_PTR (CONTINUE));
  g_hash_table_insert (token_table, "CONTINUE", AS_PTR (CONTINUE));
  g_hash_table_insert (token_table, "КОНЕЦЦИКЛА", AS_PTR (ENDDO));
  g_hash_table_insert (token_table, "ENDDO", AS_PTR (ENDDO));

  // FUNCTION
  g_hash_table_insert (token_table, "ПРОЦЕДУРА", AS_PTR (PROCEDURE));
  g_hash_table_insert (token_table, "PROCEDURE", AS_PTR (PROCEDURE));
  g_hash_table_insert (token_table, "ФУНКЦИЯ", AS_PTR (FUNCTION));
  g_hash_table_insert (token_table, "FUNCTION", AS_PTR (FUNCTION));
  g_hash_table_insert (token_table, "КОНЕЦПРОЦЕДУРЫ",
                       AS_PTR (ENDPROCEDURE));
  g_hash_table_insert (token_table, "ENDPROCEDURE",
                       AS_PTR (ENDPROCEDURE));
  g_hash_table_insert (token_table, "КОНЕЦФУНКЦИИ",
                       AS_PTR (ENDFUNCTION));
  g_hash_table_insert (token_table, "ENDFUNCTION",
                       AS_PTR (ENDFUNCTION));
  g_hash_table_insert (token_table, "ПЕРЕМ", AS_PTR (VAR));
  g_hash_table_insert (token_table, "VAR", AS_PTR (VAR));
  g_hash_table_insert (token_table, "ПЕРЕЙТИ", AS_PTR (GOTO));
  g_hash_table_insert (token_table, "GOTO", AS_PTR (GOTO));
  g_hash_table_insert (token_table, "ВОЗВРАТ", AS_PTR (RETURN));
  g_hash_table_insert (token_table, "RETURN", AS_PTR (RETURN));
  g_hash_table_insert (token_table, "И", AS_PTR (AND));
  g_hash_table_insert (token_table, "AND", AS_PTR (AND));
  g_hash_table_insert (token_table, "ИЛИ", AS_PTR (OR));
  g_hash_table_insert (token_table, "OR", AS_PTR (OR));
  g_hash_table_insert (token_table, "НЕ", AS_PTR (NOT));
  g_hash_table_insert (token_table, "NOT", AS_PTR (NOT));
  g_hash_table_insert (token_table, "ПОПЫТКА", AS_PTR (TRY));
  g_hash_table_insert (token_table, "TRY", AS_PTR (TRY));
  g_hash_table_insert (token_table, "ИСКЛЮЧЕНИЕ", AS_PTR (EXCEPT));
  g_hash_table_insert (token_table, "EXCEPT", AS_PTR (EXCEPT));
  g_hash_table_insert (token_table, "ВЫЗВАТЬИСКЛЮЧЕНИЕ",
                       AS_PTR (RAISE));
  g_hash_table_insert (token_table, "RAISE", AS_PTR (RAISE));
  g_hash_table_insert (token_table, "КОНЕЦПОПЫТКИ", AS_PTR (ENDTRY));
  g_hash_table_insert (token_table, "ENDTRY", AS_PTR (ENDTRY));
  g_hash_table_insert (token_table, "НОВЫЙ", AS_PTR (NEW));
  g_hash_table_insert (token_table, "NEW", AS_PTR (NEW));
  g_hash_table_insert (token_table, "ВЫПОЛНИТЬ", AS_PTR (EXECUTE));
  g_hash_table_insert (token_table, "EXECUTE", AS_PTR (EXECUTE));
  g_hash_table_insert (token_table, "ДОБАВИТЬОБРАБОТЧИК",
                       AS_PTR (ADDHANDLER));
  g_hash_table_insert (token_table, "ADDHANDLER",
                       AS_PTR (ADDHANDLER));
  g_hash_table_insert (token_table, "УДАЛИТЬОБРАБОТЧИК",
                       AS_PTR (REMOVEHANDLER));
  g_hash_table_insert (token_table, "REMOVEHANDLER",
                       AS_PTR (REMOVEHANDLER));
  // TYPE
  g_hash_table_insert (token_table, "NULL", AS_PTR (T_NULL));
  g_hash_table_insert (token_table, "ИСТИНА", AS_PTR (T_TRUE));
  g_hash_table_insert (token_table, "TRUE", AS_PTR (T_TRUE));
  g_hash_table_insert (token_table, "ЛОЖЬ", AS_PTR (T_FALSE));
  g_hash_table_insert (token_table, "FALSE", AS_PTR (T_FALSE));
  g_hash_table_insert (token_table, "НЕОПРЕДЕЛЕНО",
                       AS_PTR (T_UNDEFINED));
  g_hash_table_insert (token_table, "UNDEFINED",
                       AS_PTR (T_UNDEFINED));
  g_hash_table_insert (token_table, "ЭКСПОРТ", AS_PTR (EXPORT));
  g_hash_table_insert (token_table, "EXPORT", AS_PTR (EXPORT));
  g_hash_table_insert (token_table, "ЗНАЧ", AS_PTR (VAL));
  g_hash_table_insert (token_table, "VAL", AS_PTR (VAL));

  // DIRECTIVE
  g_hash_table_insert (token_table, "&НАКЛИЕНТЕ", AS_PTR (ATCLIENT));
  g_hash_table_insert (token_table, "&ATCLIENT", AS_PTR (ATCLIENT));
  g_hash_table_insert (token_table, "&НАСЕРВЕРЕ", AS_PTR (ATSERVER));
  g_hash_table_insert (token_table, "&ATSERVER", AS_PTR (ATSERVER));
  g_hash_table_insert (token_table, "&НАСЕРВЕРЕБЕЗКОНТЕКСТА",
                       AS_PTR (ATSERVERNOCONTEXT));
  g_hash_table_insert (token_table, "&ATSERVERNOCONTEXT",
                       AS_PTR (ATSERVERNOCONTEXT));
  g_hash_table_insert (token_table, "&НАКЛИЕНТЕНАСЕРВЕРЕБЕЗКОНТЕКСТА",
                       AS_PTR (ATCLIENTATSERVERNOCONTEXT));
  g_hash_table_insert (token_table, "&ATCLIENTATSERVERNOCONTEXT",
                       AS_PTR (ATCLIENTATSERVERNOCONTEXT));
  g_hash_table_insert (token_table, "&НАКЛИЕНТЕНАСЕРВЕРЕ",
                       AS_PTR (ATCLIENTATSERVER));
  g_hash_table_insert (token_table, "&ATCLIENTATSERVER",
                       AS_PTR (ATCLIENTATSERVER));
}
#pragma GCC diagnostic pop

void
fill_symbol_table ()
{
  choose_string = malloc (strlen ("ВЫБРАТЬ") + 1);
  assert (choose_string != NULL);
  strcpy (choose_string, "ВЫБРАТЬ");

  // QUERY
  g_hash_table_insert (symbol_table, g_strdup(choose_string), choose_string);
}

void
init_lex (char *file_name)
{

  set_default();

  module_fd = open (file_name, O_RDONLY);
  assert(module_fd > 0);

  assert(fstat (module_fd, &statbuf) == 0);

  src = mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, module_fd, 0);
  assert(src != MAP_FAILED);

  limit = (const char *)src + statbuf.st_size;
  start_pos = (const char *)src;
  //skip bom symbols
  if (start_pos[0] == (char)0xEF && start_pos[1] == (char)0xBB
      && start_pos[2] == (char)0xBF)
    start_pos += 3;
  end_pos = NULL;
  symbol_table
      = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

  token_table = g_hash_table_new (g_str_hash, g_str_equal);
  dynamic_val_table = g_hash_table_new (g_str_hash, g_str_equal);
  fill_symbol_table ();
  fill_token_table ();
}

void
destroy_lex ()
{
  munmap (src, statbuf.st_size);
  int status = close (module_fd);
  assert (status == 0);
  g_hash_table_destroy (symbol_table);
  g_hash_table_destroy (token_table);
  g_hash_table_destroy (dynamic_val_table);
  set_default();
}

void
set_default ()
{
  memset (buf_str, 0, sizeof (char) * BUFFSIZE);
  memset (&statbuf, 0, sizeof (struct stat));
  module_fd = 0;
  src = NULL;
  start_pos = NULL;
  end_pos = NULL;
  limit = NULL;
  line = 1;
  symbol_table = NULL;
  token_table = NULL;
  dynamic_val_table = NULL;
}

void
insert_dynamic_val_table (char *key, char *value)
{
  PRINT ("key:%s; value=%s\n", key, value);
  g_hash_table_insert (dynamic_val_table, key, value);
}

char *
check_dereference (char *obj, char *field, size_t field_line)
{
  char *val = g_hash_table_lookup (dynamic_val_table, obj);
  if (val == choose_string)
    val = field_query;
  else
    {
      if (val == field_query)
        printf ("Ошибка: разыменование ссылки %s полем %s в строке %ld\n", obj,
                field, field_line);
      val = field;
    }
  return val;
}

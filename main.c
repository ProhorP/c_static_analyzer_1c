#define _POSIX_C_SOURCE 200809L
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

GHashTable *symbol_table = NULL, *reserve_symbol_table = NULL;
char buf_str[BUFFSIZE] = { 0 };

int module_fd = 0;
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
        token_val = g_hash_table_lookup (reserve_symbol_table, lexeme_upper);

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

      if (token_val == NULL)
        print_error ("Не удалось выделить память под элемент таблицы");
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-conversion"
void
fill_reserve_symbol_table ()
{
  // IF
  g_hash_table_insert (reserve_symbol_table, "ЕСЛИ", AS_PTR (IF));
  g_hash_table_insert (reserve_symbol_table, "IF", AS_PTR (IF));
  g_hash_table_insert (reserve_symbol_table, "ТОГДА", AS_PTR (THEN));
  g_hash_table_insert (reserve_symbol_table, "THEN", AS_PTR (THEN));
  g_hash_table_insert (reserve_symbol_table, "ИНАЧЕЕСЛИ", AS_PTR (ELSIF));
  g_hash_table_insert (reserve_symbol_table, "ELSIF", AS_PTR (ELSIF));
  g_hash_table_insert (reserve_symbol_table, "ИНАЧЕ", AS_PTR (ELSE));
  g_hash_table_insert (reserve_symbol_table, "ELSE", AS_PTR (ELSE));
  g_hash_table_insert (reserve_symbol_table, "КОНЕЦЕСЛИ", AS_PTR (ENDIF));
  g_hash_table_insert (reserve_symbol_table, "ENDIF", AS_PTR (ENDIF));

  // DO
  g_hash_table_insert (reserve_symbol_table, "ДЛЯ", AS_PTR (FOR));
  g_hash_table_insert (reserve_symbol_table, "FOR", AS_PTR (FOR));
  g_hash_table_insert (reserve_symbol_table, "КАЖДОГО", AS_PTR (EACH));
  g_hash_table_insert (reserve_symbol_table, "EACH", AS_PTR (EACH));
  g_hash_table_insert (reserve_symbol_table, "ИЗ", AS_PTR (IN));
  g_hash_table_insert (reserve_symbol_table, "IN", AS_PTR (IN));
  g_hash_table_insert (reserve_symbol_table, "ПО", AS_PTR (TO));
  g_hash_table_insert (reserve_symbol_table, "TO", AS_PTR (TO));
  g_hash_table_insert (reserve_symbol_table, "ПОКА", AS_PTR (WHILE));
  g_hash_table_insert (reserve_symbol_table, "WHILE", AS_PTR (WHILE));
  g_hash_table_insert (reserve_symbol_table, "ЦИКЛ", AS_PTR (DO));
  g_hash_table_insert (reserve_symbol_table, "DO", AS_PTR (DO));
  g_hash_table_insert (reserve_symbol_table, "ПРЕРВАТЬ", AS_PTR (BREAK));
  g_hash_table_insert (reserve_symbol_table, "BREAK", AS_PTR (BREAK));
  g_hash_table_insert (reserve_symbol_table, "ПРОДОЛЖИТЬ", AS_PTR (CONTINUE));
  g_hash_table_insert (reserve_symbol_table, "CONTINUE", AS_PTR (CONTINUE));
  g_hash_table_insert (reserve_symbol_table, "КОНЕЦЦИКЛА", AS_PTR (ENDDO));
  g_hash_table_insert (reserve_symbol_table, "ENDDO", AS_PTR (ENDDO));

  // FUNCTION
  g_hash_table_insert (reserve_symbol_table, "ПРОЦЕДУРА", AS_PTR (PROCEDURE));
  g_hash_table_insert (reserve_symbol_table, "PROCEDURE", AS_PTR (PROCEDURE));
  g_hash_table_insert (reserve_symbol_table, "ФУНКЦИЯ", AS_PTR (FUNCTION));
  g_hash_table_insert (reserve_symbol_table, "FUNCTION", AS_PTR (FUNCTION));
  g_hash_table_insert (reserve_symbol_table, "КОНЕЦПРОЦЕДУРЫ",
                       AS_PTR (ENDPROCEDURE));
  g_hash_table_insert (reserve_symbol_table, "ENDPROCEDURE",
                       AS_PTR (ENDPROCEDURE));
  g_hash_table_insert (reserve_symbol_table, "КОНЕЦФУНКЦИИ",
                       AS_PTR (ENDFUNCTION));
  g_hash_table_insert (reserve_symbol_table, "ENDFUNCTION",
                       AS_PTR (ENDFUNCTION));
  g_hash_table_insert (reserve_symbol_table, "ПЕРЕМ", AS_PTR (VAR));
  g_hash_table_insert (reserve_symbol_table, "VAR", AS_PTR (VAR));
  g_hash_table_insert (reserve_symbol_table, "ПЕРЕЙТИ", AS_PTR (GOTO));
  g_hash_table_insert (reserve_symbol_table, "GOTO", AS_PTR (GOTO));
  g_hash_table_insert (reserve_symbol_table, "ВОЗВРАТ", AS_PTR (RETURN));
  g_hash_table_insert (reserve_symbol_table, "RETURN", AS_PTR (RETURN));
  g_hash_table_insert (reserve_symbol_table, "И", AS_PTR (AND));
  g_hash_table_insert (reserve_symbol_table, "AND", AS_PTR (AND));
  g_hash_table_insert (reserve_symbol_table, "ИЛИ", AS_PTR (OR));
  g_hash_table_insert (reserve_symbol_table, "OR", AS_PTR (OR));
  g_hash_table_insert (reserve_symbol_table, "НЕ", AS_PTR (NOT));
  g_hash_table_insert (reserve_symbol_table, "NOT", AS_PTR (NOT));
  g_hash_table_insert (reserve_symbol_table, "ПОПЫТКА", AS_PTR (TRY));
  g_hash_table_insert (reserve_symbol_table, "TRY", AS_PTR (TRY));
  g_hash_table_insert (reserve_symbol_table, "ИСКЛЮЧЕНИЕ", AS_PTR (EXCEPT));
  g_hash_table_insert (reserve_symbol_table, "EXCEPT", AS_PTR (EXCEPT));
  g_hash_table_insert (reserve_symbol_table, "ВЫЗВАТЬИСКЛЮЧЕНИЕ",
                       AS_PTR (RAISE));
  g_hash_table_insert (reserve_symbol_table, "RAISE", AS_PTR (RAISE));
  g_hash_table_insert (reserve_symbol_table, "КОНЕЦПОПЫТКИ", AS_PTR (ENDTRY));
  g_hash_table_insert (reserve_symbol_table, "ENDTRY", AS_PTR (ENDTRY));
  g_hash_table_insert (reserve_symbol_table, "НОВЫЙ", AS_PTR (NEW));
  g_hash_table_insert (reserve_symbol_table, "NEW", AS_PTR (NEW));
  g_hash_table_insert (reserve_symbol_table, "ВЫПОЛНИТЬ", AS_PTR (EXECUTE));
  g_hash_table_insert (reserve_symbol_table, "EXECUTE", AS_PTR (EXECUTE));
  g_hash_table_insert (reserve_symbol_table, "ДОБАВИТЬОБРАБОТЧИК",
                       AS_PTR (ADDHANDLER));
  g_hash_table_insert (reserve_symbol_table, "ADDHANDLER",
                       AS_PTR (ADDHANDLER));
  g_hash_table_insert (reserve_symbol_table, "УДАЛИТЬОБРАБОТЧИК",
                       AS_PTR (REMOVEHANDLER));
  g_hash_table_insert (reserve_symbol_table, "REMOVEHANDLER",
                       AS_PTR (REMOVEHANDLER));
  // TYPE
  g_hash_table_insert (reserve_symbol_table, "NULL", AS_PTR (T_NULL));
  g_hash_table_insert (reserve_symbol_table, "ИСТИНА", AS_PTR (T_TRUE));
  g_hash_table_insert (reserve_symbol_table, "TRUE", AS_PTR (T_TRUE));
  g_hash_table_insert (reserve_symbol_table, "ЛОЖЬ", AS_PTR (T_FALSE));
  g_hash_table_insert (reserve_symbol_table, "FALSE", AS_PTR (T_FALSE));
  g_hash_table_insert (reserve_symbol_table, "НЕОПРЕДЕЛЕНО",
                       AS_PTR (T_UNDEFINED));
  g_hash_table_insert (reserve_symbol_table, "UNDEFINED",
                       AS_PTR (T_UNDEFINED));
  g_hash_table_insert (reserve_symbol_table, "ЭКСПОРТ", AS_PTR (EXPORT));
  g_hash_table_insert (reserve_symbol_table, "EXPORT", AS_PTR (EXPORT));
  g_hash_table_insert (reserve_symbol_table, "ЗНАЧ", AS_PTR (VAL));
  g_hash_table_insert (reserve_symbol_table, "VAL", AS_PTR (VAL));

  // DIRECTIVE
  g_hash_table_insert (reserve_symbol_table, "&НАКЛИЕНТЕ", AS_PTR (ATCLIENT));
  g_hash_table_insert (reserve_symbol_table, "&ATCLIENT", AS_PTR (ATCLIENT));
  g_hash_table_insert (reserve_symbol_table, "&НАСЕРВЕРЕ", AS_PTR (ATSERVER));
  g_hash_table_insert (reserve_symbol_table, "&ATSERVER", AS_PTR (ATSERVER));
  g_hash_table_insert (reserve_symbol_table, "&НАСЕРВЕРЕБЕЗКОНТЕКСТА",
                       AS_PTR (ATSERVERNOCONTEXT));
  g_hash_table_insert (reserve_symbol_table, "&ATSERVERNOCONTEXT",
                       AS_PTR (ATSERVERNOCONTEXT));
  g_hash_table_insert (reserve_symbol_table, "&НАКЛИЕНТЕНАСЕРВЕРЕБЕЗКОНТЕКСТА",
                       AS_PTR (ATCLIENTATSERVERNOCONTEXT));
  g_hash_table_insert (reserve_symbol_table, "&ATCLIENTATSERVERNOCONTEXT",
                       AS_PTR (ATCLIENTATSERVERNOCONTEXT));
  g_hash_table_insert (reserve_symbol_table, "&НАКЛИЕНТЕНАСЕРВЕРЕ",
                       AS_PTR (ATCLIENTATSERVER));
  g_hash_table_insert (reserve_symbol_table, "&ATCLIENTATSERVER",
                       AS_PTR (ATCLIENTATSERVER));
}
#pragma GCC diagnostic pop

void
init_lex (char *file_name)
{
  if ((module_fd = open (file_name, O_RDONLY)) < 0)
    print_error ("невозможно открыть %s для чтения", file_name);

  if (fstat (module_fd, &statbuf) < 0)
    print_error ("Ошибка вызова функции fstat:%s у файла %s\n",
                 strerror (errno), file_name);

  if ((src = mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, module_fd, 0))
      == MAP_FAILED)
    print_error ("%s\n", "Ошибка вызова функции mmap для входного файла");

  limit = (const char *)src + statbuf.st_size;
  start_pos = (const char *)src;
  end_pos = NULL;
  symbol_table
      = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

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

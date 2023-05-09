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

enum TOKEN
{
  ERROR				//0
    , TEST			//1
    , POINT			//2
    , COMMA			//3
    , SEMICOLON			//4
    , LEFT_PARENTHESIS		//5
    , RIGHT_PARENTHESIS		//6
    , LEFT_SQUARE_BRACKET	//7
    , RIGHT_SQUARE_BRACKET	//8
    , WHITESPACE		//9
    , RELOP			//10
    , NUMBER			//11
    , DATE			//12
    , LITERAL			//13
    , ID			//14
};

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

enum TOKEN
lex (const char **start_pos, const char **end_pos)
{
  const char *YYCURSOR = *start_pos, *YYMARKER;
  *end_pos = *start_pos;
  /*!re2c
     re2c:define:YYCTYPE = 'unsigned char';
     re2c:yyfill:enable = 0;

     point = '.';
     comma = ',';
     semicolon = ';';
     left_parenthesis = '(';
     right_parenthesis = ')';
     left_square_bracket = '[';
     right_square_bracket = ']';
     whitespace = ' ';
     relop = [=<>]+;
     digit = [0-9];
     digits = digit+;
     number = digits([\.]digits)?;
     date = ['] [^']+ ['];
     literal = ["] ([^"] | [""])* ["];
     id = [^'"\.,;()\[\] =<>]+;

     point { *end_pos = YYCURSOR; return POINT; }
     comma { *end_pos = YYCURSOR; return COMMA; }
     semicolon { *end_pos = YYCURSOR; return SEMICOLON; }
     left_parenthesis { *end_pos = YYCURSOR; return LEFT_PARENTHESIS; }
     right_parenthesis { *end_pos = YYCURSOR; return RIGHT_PARENTHESIS; }
     left_square_bracket { *end_pos = YYCURSOR; return LEFT_SQUARE_BRACKET; }
     right_square_bracket { *end_pos = YYCURSOR; return RIGHT_SQUARE_BRACKET; }
     whitespace { *end_pos = YYCURSOR; return WHITESPACE; }
     relop { *end_pos = YYCURSOR; return RELOP; }
     number { *end_pos = YYCURSOR; return NUMBER; }
     date { *end_pos = YYCURSOR; return DATE; }
     literal { *end_pos = YYCURSOR; return LITERAL; }
     id { *end_pos = YYCURSOR; return ID; }
     *  { return ERROR; }
   */
}

int
main (int argc, char **argv)
{

  if (argc != 2)
    print_error ("%s", "Не указан входной файл");

  struct stat statbuf;
  int fd;
  char buf[4096] = { 0 };

  if ((fd = open (argv[1], O_RDONLY)) < 0)
    print_error ("невозможно открыть %s для чтения",
		 argv[1]);

  if (fstat (fd, &statbuf) < 0)
    print_error
      ("Ошибка вызова функции fstat:%s у файла %s\n",
       strerror (errno), argv[1]);

  void *src;
  if ((src =
       mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
    print_error ("%s\n",
		 "Ошибка вызова функции mmap для входного файла");

  const char *start_pos = (const char *) src;
  const char *end_pos = NULL;

  enum TOKEN res = lex (&start_pos, &end_pos);
  size_t n = end_pos - start_pos;
  memcpy (buf, start_pos, n);
  buf[n] = '\0';

  printf ("%s\nres:%d\n", buf, res);

  munmap (src, statbuf.st_size);
  return EXIT_SUCCESS;
}

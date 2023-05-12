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

#define BUFFSIZE 409600
enum TOKEN
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
    , PREPROCESSOR			//18
    , AREA			//19
    , ID			//20
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
lex (const char **start_pos, const char **end_pos, const char *limit)
{
  const char *YYCURSOR = *start_pos, *YYLIMIT = limit, *YYMARKER;
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
     relop = [=<>]+;
     math = [+\-*\/%];
     digit = [0-9];
     digits = digit+;
     number = digits([\.]digits)?;
     date = ['] [^']+ ['];
     literal = ["] ([^"] | ["]["])* ["];
     preprocessor = [&][^\n]+;
     area = [#][^\n]+;
     id = [^'"\.,;()\[\] \t\n=<>+\-*\/%&#]+;


     comment { *start_pos = YYCURSOR; goto loop; }
     point { *end_pos = YYCURSOR; return POINT; }
     comma { *end_pos = YYCURSOR; return COMMA; }
     semicolon { *end_pos = YYCURSOR; return SEMICOLON; }
     left_parenthesis { *end_pos = YYCURSOR; return LEFT_PARENTHESIS; }
     right_parenthesis { *end_pos = YYCURSOR; return RIGHT_PARENTHESIS; }
     left_square_bracket { *end_pos = YYCURSOR; return LEFT_SQUARE_BRACKET; }
     right_square_bracket { *end_pos = YYCURSOR; return RIGHT_SQUARE_BRACKET; }
     whitespace { *end_pos = YYCURSOR; return WHITESPACE; }
     tab { *end_pos = YYCURSOR; return TAB; }
     newline { *end_pos = YYCURSOR; return NEWLINE; }
     relop { *end_pos = YYCURSOR; return RELOP; }
     math { *end_pos = YYCURSOR; return MATH; }
     number { *end_pos = YYCURSOR; return NUMBER; }
     date { *end_pos = YYCURSOR; return DATE; }
     literal { *end_pos = YYCURSOR; return LITERAL; }
     preprocessor { *end_pos = YYCURSOR; return PREPROCESSOR; }
     area { *end_pos = YYCURSOR; return AREA; }
     id { *end_pos = YYCURSOR; return ID; }
     $ { *end_pos = YYCURSOR; return END; }
     *  { *end_pos = YYCURSOR; return ERROR; }
   */
#pragma GCC diagnostic pop
}

int
main (int argc, char **argv)
{

  if (argc != 3)
    print_error ("%s", "Правильный формат:./a.out ./test/test01 ./log");

  struct stat statbuf;
  int fd, fd_log;
  char buf[BUFFSIZE] = { 0 };

  if ((fd = open (argv[1], O_RDONLY)) < 0)
    print_error ("невозможно открыть %s для чтения",
		 argv[1]);

  if ((fd_log =
       open (argv[2], O_WRONLY | O_CREAT | O_APPEND,
	     S_IWUSR | S_IRUSR)) < 0)
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
  enum TOKEN res = ERROR;
  size_t n = 0;

  do
    {
      res = lex (&start_pos, &end_pos, limit);
      n = end_pos - start_pos;
      assert(n < BUFFSIZE);
      memcpy (buf, start_pos, n);
      buf[n] = '\0';
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
      dprintf (fd_log, "%s\nres:%d\n", buf, res);
#pragma GCC diagnostic pop
      start_pos = end_pos;
    }
  while (start_pos != limit);

  munmap (src, statbuf.st_size);
  return EXIT_SUCCESS;
}

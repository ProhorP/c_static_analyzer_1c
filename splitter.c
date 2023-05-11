/* Generated by re2c 3.0 on Thu May 11 22:35:57 2023 */
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
    , ID			//18
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




enum TOKEN
lex (const char **start_pos, const char **end_pos, const char *limit)
{
  const char *YYCURSOR = *start_pos, *YYLIMIT = limit, *YYMARKER;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
loop:
  
{
	unsigned char yych;
	unsigned int yyaccept = 0;
	yych = *YYCURSOR;
	switch (yych) {
		case 0x00:
		case 0x01 ... 0x08:
		case '\v' ... 0x1F:
		case '!':
		case '#' ... '$':
		case '&':
		case '-':
		case ':':
		case '?' ... 'Z':
		case '\\':
		case '^' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy69;
			goto yy1;
		case '\t': goto yy3;
		case '\n': goto yy5;
		case ' ': goto yy7;
		case '"': goto yy9;
		case '%':
		case '*' ... '+': goto yy11;
		case '\'': goto yy13;
		case '(': goto yy14;
		case ')': goto yy15;
		case ',': goto yy16;
		case '.': goto yy17;
		case '/': goto yy18;
		case '0' ... '9': goto yy19;
		case ';': goto yy21;
		case '<' ... '>': goto yy22;
		case '[': goto yy24;
		case ']': goto yy25;
		case 0xC2 ... 0xDF: goto yy27;
		case 0xE0: goto yy28;
		case 0xE1 ... 0xEF: goto yy29;
		case 0xF0: goto yy30;
		case 0xF1 ... 0xF3: goto yy31;
		case 0xF4: goto yy32;
		default: goto yy26;
	}
yy1:
	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00:
		case 0x01 ... 0x08:
		case '\v' ... 0x1F:
		case '!':
		case '#' ... '$':
		case '&':
		case '-':
		case '0' ... ':':
		case '?' ... 'Z':
		case '\\':
		case '^' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy2;
			goto yy1;
		case 0xC2 ... 0xDF: goto yy33;
		case 0xE0: goto yy35;
		case 0xE1 ... 0xEF: goto yy36;
		case 0xF0: goto yy37;
		case 0xF1 ... 0xF3: goto yy38;
		case 0xF4: goto yy39;
		default: goto yy2;
	}
yy2:
	{ *end_pos = YYCURSOR; return ID; }
yy3:
	yych = *++YYCURSOR;
	switch (yych) {
		case '\t': goto yy3;
		default: goto yy4;
	}
yy4:
	{ *end_pos = YYCURSOR; return TAB; }
yy5:
	yych = *++YYCURSOR;
	switch (yych) {
		case '\n': goto yy5;
		default: goto yy6;
	}
yy6:
	{ *end_pos = YYCURSOR; return NEWLINE; }
yy7:
	yych = *++YYCURSOR;
	switch (yych) {
		case ' ': goto yy7;
		default: goto yy8;
	}
yy8:
	{ *end_pos = YYCURSOR; return WHITESPACE; }
yy9:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00:
			if (YYLIMIT <= YYCURSOR) goto yy10;
			goto yy40;
		case 0x01 ... 0x7F:
		case 0xC2 ... 0xF4: goto yy41;
		default: goto yy10;
	}
yy10:
	{ *end_pos = YYCURSOR; return ERROR; }
yy11:
	++YYCURSOR;
yy12:
	{ *end_pos = YYCURSOR; return MATH; }
yy13:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00:
			if (YYLIMIT <= YYCURSOR) goto yy10;
			goto yy50;
		case 0x01 ... '&':
		case '(' ... 0x7F:
		case 0xC2 ... 0xF4: goto yy51;
		default: goto yy10;
	}
yy14:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return LEFT_PARENTHESIS; }
yy15:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return RIGHT_PARENTHESIS; }
yy16:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return COMMA; }
yy17:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return POINT; }
yy18:
	yyaccept = 2;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case '/': goto yy58;
		default: goto yy12;
	}
yy19:
	yyaccept = 3;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00:
		case 0x01 ... 0x08:
		case '\v' ... 0x1F:
		case '!':
		case '#' ... '$':
		case '&':
		case '-':
		case ':':
		case '?' ... 'Z':
		case '\\':
		case '^' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy20;
			goto yy1;
		case '.': goto yy59;
		case '0' ... '9': goto yy19;
		case 0xC2 ... 0xDF: goto yy33;
		case 0xE0: goto yy35;
		case 0xE1 ... 0xEF: goto yy36;
		case 0xF0: goto yy37;
		case 0xF1 ... 0xF3: goto yy38;
		case 0xF4: goto yy39;
		default: goto yy20;
	}
yy20:
	{ *end_pos = YYCURSOR; return NUMBER; }
yy21:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return SEMICOLON; }
yy22:
	yych = *++YYCURSOR;
	switch (yych) {
		case '<' ... '>': goto yy22;
		default: goto yy23;
	}
yy23:
	{ *end_pos = YYCURSOR; return RELOP; }
yy24:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return LEFT_SQUARE_BRACKET; }
yy25:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return RIGHT_SQUARE_BRACKET; }
yy26:
	++YYCURSOR;
	goto yy10;
yy27:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy1;
		default: goto yy10;
	}
yy28:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy33;
		default: goto yy10;
	}
yy29:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x80 ... 0xBF: goto yy33;
		default: goto yy10;
	}
yy30:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x90 ... 0xBF: goto yy36;
		default: goto yy10;
	}
yy31:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x80 ... 0xBF: goto yy36;
		default: goto yy10;
	}
yy32:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x80 ... 0x8F: goto yy36;
		default: goto yy10;
	}
yy33:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy1;
		default: goto yy34;
	}
yy34:
	YYCURSOR = YYMARKER;
	switch (yyaccept) {
		case 0: goto yy2;
		case 1: goto yy10;
		case 2: goto yy12;
		case 3: goto yy20;
		default: goto yy43;
	}
yy35:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy33;
		default: goto yy34;
	}
yy36:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy33;
		default: goto yy34;
	}
yy37:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy36;
		default: goto yy34;
	}
yy38:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy36;
		default: goto yy34;
	}
yy39:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy36;
		default: goto yy34;
	}
yy40:
	yych = *++YYCURSOR;
yy41:
	switch (yych) {
		case 0x00:
		case 0x01 ... '!':
		case '#' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy34;
			goto yy40;
		case '"': goto yy42;
		case 0xC2 ... 0xDF: goto yy44;
		case 0xE0: goto yy45;
		case 0xE1 ... 0xEF: goto yy46;
		case 0xF0: goto yy47;
		case 0xF1 ... 0xF3: goto yy48;
		case 0xF4: goto yy49;
		default: goto yy34;
	}
yy42:
	yyaccept = 4;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case '"': goto yy40;
		default: goto yy43;
	}
yy43:
	{ *end_pos = YYCURSOR; return LITERAL; }
yy44:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy40;
		default: goto yy34;
	}
yy45:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy44;
		default: goto yy34;
	}
yy46:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy44;
		default: goto yy34;
	}
yy47:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy46;
		default: goto yy34;
	}
yy48:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy46;
		default: goto yy34;
	}
yy49:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy46;
		default: goto yy34;
	}
yy50:
	yych = *++YYCURSOR;
yy51:
	switch (yych) {
		case 0x00:
		case 0x01 ... '&':
		case '(' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy34;
			goto yy50;
		case '\'': goto yy60;
		case 0xC2 ... 0xDF: goto yy52;
		case 0xE0: goto yy53;
		case 0xE1 ... 0xEF: goto yy54;
		case 0xF0: goto yy55;
		case 0xF1 ... 0xF3: goto yy56;
		case 0xF4: goto yy57;
		default: goto yy34;
	}
yy52:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy50;
		default: goto yy34;
	}
yy53:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy52;
		default: goto yy34;
	}
yy54:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy52;
		default: goto yy34;
	}
yy55:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy54;
		default: goto yy34;
	}
yy56:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy54;
		default: goto yy34;
	}
yy57:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy54;
		default: goto yy34;
	}
yy58:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x00:
		case 0x01 ... '\t':
		case '\v' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy34;
			goto yy58;
		case '\n': goto yy61;
		case 0xC2 ... 0xDF: goto yy62;
		case 0xE0: goto yy63;
		case 0xE1 ... 0xEF: goto yy64;
		case 0xF0: goto yy65;
		case 0xF1 ... 0xF3: goto yy66;
		case 0xF4: goto yy67;
		default: goto yy34;
	}
yy59:
	yych = *++YYCURSOR;
	switch (yych) {
		case '0' ... '9': goto yy68;
		default: goto yy34;
	}
yy60:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return DATE; }
yy61:
	++YYCURSOR;
	{ *start_pos = YYCURSOR; goto loop; }
yy62:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy58;
		default: goto yy34;
	}
yy63:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy62;
		default: goto yy34;
	}
yy64:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy62;
		default: goto yy34;
	}
yy65:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy64;
		default: goto yy34;
	}
yy66:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy64;
		default: goto yy34;
	}
yy67:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy64;
		default: goto yy34;
	}
yy68:
	yych = *++YYCURSOR;
	switch (yych) {
		case '0' ... '9': goto yy68;
		default: goto yy20;
	}
yy69:
	{ *end_pos = YYCURSOR; return END; }
}

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

/* Generated by re2c 3.0 on Wed May 10 19:44:28 2023 */
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
			if (YYLIMIT <= YYCURSOR) goto yy59;
			goto yy1;
		case '\t': goto yy3;
		case '\n': goto yy5;
		case ' ': goto yy7;
		case '"': goto yy9;
		case '%':
		case '*' ... '+':
		case '/': goto yy11;
		case '\'': goto yy12;
		case '(': goto yy13;
		case ')': goto yy14;
		case ',': goto yy15;
		case '.': goto yy16;
		case '0' ... '9': goto yy17;
		case ';': goto yy19;
		case '<' ... '>': goto yy20;
		case '[': goto yy22;
		case ']': goto yy23;
		case 0xC2 ... 0xDF: goto yy25;
		case 0xE0: goto yy26;
		case 0xE1 ... 0xEF: goto yy27;
		case 0xF0: goto yy28;
		case 0xF1 ... 0xF3: goto yy29;
		case 0xF4: goto yy30;
		default: goto yy24;
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
		case 0xC2 ... 0xDF: goto yy31;
		case 0xE0: goto yy33;
		case 0xE1 ... 0xEF: goto yy34;
		case 0xF0: goto yy35;
		case 0xF1 ... 0xF3: goto yy36;
		case 0xF4: goto yy37;
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
			goto yy38;
		case 0x01 ... 0x7F:
		case 0xC2 ... 0xF4: goto yy39;
		default: goto yy10;
	}
yy10:
	{ *end_pos = YYCURSOR; return ERROR; }
yy11:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return MATH; }
yy12:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00:
			if (YYLIMIT <= YYCURSOR) goto yy10;
			goto yy48;
		case 0x01 ... '&':
		case '(' ... 0x7F:
		case 0xC2 ... 0xF4: goto yy49;
		default: goto yy10;
	}
yy13:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return LEFT_PARENTHESIS; }
yy14:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return RIGHT_PARENTHESIS; }
yy15:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return COMMA; }
yy16:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return POINT; }
yy17:
	yyaccept = 2;
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
			if (YYLIMIT <= YYCURSOR) goto yy18;
			goto yy1;
		case '.': goto yy56;
		case '0' ... '9': goto yy17;
		case 0xC2 ... 0xDF: goto yy31;
		case 0xE0: goto yy33;
		case 0xE1 ... 0xEF: goto yy34;
		case 0xF0: goto yy35;
		case 0xF1 ... 0xF3: goto yy36;
		case 0xF4: goto yy37;
		default: goto yy18;
	}
yy18:
	{ *end_pos = YYCURSOR; return NUMBER; }
yy19:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return SEMICOLON; }
yy20:
	yych = *++YYCURSOR;
	switch (yych) {
		case '<' ... '>': goto yy20;
		default: goto yy21;
	}
yy21:
	{ *end_pos = YYCURSOR; return RELOP; }
yy22:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return LEFT_SQUARE_BRACKET; }
yy23:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return RIGHT_SQUARE_BRACKET; }
yy24:
	++YYCURSOR;
	goto yy10;
yy25:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy1;
		default: goto yy10;
	}
yy26:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy31;
		default: goto yy10;
	}
yy27:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x80 ... 0xBF: goto yy31;
		default: goto yy10;
	}
yy28:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x90 ... 0xBF: goto yy34;
		default: goto yy10;
	}
yy29:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x80 ... 0xBF: goto yy34;
		default: goto yy10;
	}
yy30:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x80 ... 0x8F: goto yy34;
		default: goto yy10;
	}
yy31:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy1;
		default: goto yy32;
	}
yy32:
	YYCURSOR = YYMARKER;
	switch (yyaccept) {
		case 0: goto yy2;
		case 1: goto yy10;
		case 2: goto yy18;
		default: goto yy41;
	}
yy33:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy31;
		default: goto yy32;
	}
yy34:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy31;
		default: goto yy32;
	}
yy35:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy34;
		default: goto yy32;
	}
yy36:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy34;
		default: goto yy32;
	}
yy37:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy34;
		default: goto yy32;
	}
yy38:
	yych = *++YYCURSOR;
yy39:
	switch (yych) {
		case 0x00:
		case 0x01 ... '!':
		case '#' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy32;
			goto yy38;
		case '"': goto yy40;
		case 0xC2 ... 0xDF: goto yy42;
		case 0xE0: goto yy43;
		case 0xE1 ... 0xEF: goto yy44;
		case 0xF0: goto yy45;
		case 0xF1 ... 0xF3: goto yy46;
		case 0xF4: goto yy47;
		default: goto yy32;
	}
yy40:
	yyaccept = 3;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case '"': goto yy38;
		default: goto yy41;
	}
yy41:
	{ *end_pos = YYCURSOR; return LITERAL; }
yy42:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy38;
		default: goto yy32;
	}
yy43:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy42;
		default: goto yy32;
	}
yy44:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy42;
		default: goto yy32;
	}
yy45:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy44;
		default: goto yy32;
	}
yy46:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy44;
		default: goto yy32;
	}
yy47:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy44;
		default: goto yy32;
	}
yy48:
	yych = *++YYCURSOR;
yy49:
	switch (yych) {
		case 0x00:
		case 0x01 ... '&':
		case '(' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy32;
			goto yy48;
		case '\'': goto yy57;
		case 0xC2 ... 0xDF: goto yy50;
		case 0xE0: goto yy51;
		case 0xE1 ... 0xEF: goto yy52;
		case 0xF0: goto yy53;
		case 0xF1 ... 0xF3: goto yy54;
		case 0xF4: goto yy55;
		default: goto yy32;
	}
yy50:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy48;
		default: goto yy32;
	}
yy51:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy50;
		default: goto yy32;
	}
yy52:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy50;
		default: goto yy32;
	}
yy53:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy52;
		default: goto yy32;
	}
yy54:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy52;
		default: goto yy32;
	}
yy55:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy52;
		default: goto yy32;
	}
yy56:
	yych = *++YYCURSOR;
	switch (yych) {
		case '0' ... '9': goto yy58;
		default: goto yy32;
	}
yy57:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return DATE; }
yy58:
	yych = *++YYCURSOR;
	switch (yych) {
		case '0' ... '9': goto yy58;
		default: goto yy18;
	}
yy59:
	{ *end_pos = YYCURSOR; return END; }
}

#pragma GCC diagnostic pop
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

  const char *limit = (const char *) src + statbuf.st_size;
  const char *start_pos = (const char *) src;
  const char *end_pos = NULL;
  enum TOKEN res = ERROR;
  size_t n = 0;

  do
    {
      res = lex (&start_pos, &end_pos, limit);
      n = end_pos - start_pos;
      memcpy (buf, start_pos, n);
      buf[n] = '\0';
      printf ("%s\nres:%d\n", buf, res);
      start_pos = end_pos;
    }
  while (start_pos != limit);

  munmap (src, statbuf.st_size);
  return EXIT_SUCCESS;
}
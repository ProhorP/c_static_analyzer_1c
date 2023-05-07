/* Generated by re2c 3.0 on Sun May  7 09:41:58 2023 */
// re2c $INPUT -o $OUTPUT -8 --case-ranges -i
#include <assert.h>
//#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

enum TOKEN {
ERROR //0
, NUMBER //1
, ID //2
, DATE //3
, LITERAL //4
, TEST //5
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
lex (const char **start_pos, const char **end_pos)
{
  const char *YYCURSOR = *start_pos, *YYMARKER;
  *end_pos = *start_pos;
  
{
	unsigned char yych;
	unsigned int yyaccept = 0;
	yych = *YYCURSOR;
	switch (yych) {
		case 0x00 ... 0x1F:
		case '!':
		case '#' ... '&':
		case '(' ... '-':
		case '/':
		case ':':
		case '<' ... 0x7F: goto yy1;
		case '"': goto yy5;
		case '\'': goto yy6;
		case '0' ... '9': goto yy7;
		case 0xC2 ... 0xDF: goto yy9;
		case 0xE0: goto yy10;
		case 0xE1 ... 0xEF: goto yy11;
		case 0xF0: goto yy12;
		case 0xF1 ... 0xF3: goto yy13;
		case 0xF4: goto yy14;
		default: goto yy3;
	}
yy1:
	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00 ... 0x1F:
		case '!':
		case '#' ... '&':
		case '(' ... '-':
		case '/' ... ':':
		case '<' ... 0x7F: goto yy1;
		case 0xC2 ... 0xDF: goto yy15;
		case 0xE0: goto yy17;
		case 0xE1 ... 0xEF: goto yy18;
		case 0xF0: goto yy19;
		case 0xF1 ... 0xF3: goto yy20;
		case 0xF4: goto yy21;
		default: goto yy2;
	}
yy2:
	{ *end_pos = YYCURSOR; return ID; }
yy3:
	++YYCURSOR;
yy4:
	{ return ERROR; }
yy5:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00 ... 0x7F:
		case 0xC2 ... 0xF4: goto yy23;
		default: goto yy4;
	}
yy6:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00 ... '&':
		case '(' ... 0x7F:
		case 0xC2 ... 0xF4: goto yy33;
		default: goto yy4;
	}
yy7:
	yyaccept = 2;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00 ... 0x1F:
		case '!':
		case '#' ... '&':
		case '(' ... '-':
		case '/':
		case ':':
		case '<' ... 0x7F: goto yy1;
		case '.': goto yy40;
		case '0' ... '9': goto yy7;
		case 0xC2 ... 0xDF: goto yy15;
		case 0xE0: goto yy17;
		case 0xE1 ... 0xEF: goto yy18;
		case 0xF0: goto yy19;
		case 0xF1 ... 0xF3: goto yy20;
		case 0xF4: goto yy21;
		default: goto yy8;
	}
yy8:
	{ *end_pos = YYCURSOR; return NUMBER; }
yy9:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy1;
		default: goto yy4;
	}
yy10:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy15;
		default: goto yy4;
	}
yy11:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x80 ... 0xBF: goto yy15;
		default: goto yy4;
	}
yy12:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x90 ... 0xBF: goto yy18;
		default: goto yy4;
	}
yy13:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x80 ... 0xBF: goto yy18;
		default: goto yy4;
	}
yy14:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x80 ... 0x8F: goto yy18;
		default: goto yy4;
	}
yy15:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy1;
		default: goto yy16;
	}
yy16:
	YYCURSOR = YYMARKER;
	switch (yyaccept) {
		case 0: goto yy2;
		case 1: goto yy4;
		case 2: goto yy8;
		default: goto yy25;
	}
yy17:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy15;
		default: goto yy16;
	}
yy18:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy15;
		default: goto yy16;
	}
yy19:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy18;
		default: goto yy16;
	}
yy20:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy18;
		default: goto yy16;
	}
yy21:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy18;
		default: goto yy16;
	}
yy22:
	yych = *++YYCURSOR;
yy23:
	switch (yych) {
		case 0x00 ... '!':
		case '#' ... 0x7F: goto yy22;
		case '"': goto yy24;
		case 0xC2 ... 0xDF: goto yy26;
		case 0xE0: goto yy27;
		case 0xE1 ... 0xEF: goto yy28;
		case 0xF0: goto yy29;
		case 0xF1 ... 0xF3: goto yy30;
		case 0xF4: goto yy31;
		default: goto yy16;
	}
yy24:
	yyaccept = 3;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00 ... '!':
		case '#' ... 0x7F: goto yy22;
		case '"': goto yy24;
		case 0xC2 ... 0xDF: goto yy26;
		case 0xE0: goto yy27;
		case 0xE1 ... 0xEF: goto yy28;
		case 0xF0: goto yy29;
		case 0xF1 ... 0xF3: goto yy30;
		case 0xF4: goto yy31;
		default: goto yy25;
	}
yy25:
	{ *end_pos = YYCURSOR; return LITERAL; }
yy26:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy22;
		default: goto yy16;
	}
yy27:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy26;
		default: goto yy16;
	}
yy28:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy26;
		default: goto yy16;
	}
yy29:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy28;
		default: goto yy16;
	}
yy30:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy28;
		default: goto yy16;
	}
yy31:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy28;
		default: goto yy16;
	}
yy32:
	yych = *++YYCURSOR;
yy33:
	switch (yych) {
		case 0x00 ... '&':
		case '(' ... 0x7F: goto yy32;
		case '\'': goto yy41;
		case 0xC2 ... 0xDF: goto yy34;
		case 0xE0: goto yy35;
		case 0xE1 ... 0xEF: goto yy36;
		case 0xF0: goto yy37;
		case 0xF1 ... 0xF3: goto yy38;
		case 0xF4: goto yy39;
		default: goto yy16;
	}
yy34:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy32;
		default: goto yy16;
	}
yy35:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy34;
		default: goto yy16;
	}
yy36:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy34;
		default: goto yy16;
	}
yy37:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy36;
		default: goto yy16;
	}
yy38:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy36;
		default: goto yy16;
	}
yy39:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy36;
		default: goto yy16;
	}
yy40:
	yych = *++YYCURSOR;
	switch (yych) {
		case '0' ... '9': goto yy42;
		default: goto yy16;
	}
yy41:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return DATE; }
yy42:
	yych = *++YYCURSOR;
	switch (yych) {
		case '0' ... '9': goto yy42;
		default: goto yy8;
	}
}

}

int
main (int argc, char** argv)
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

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

/*!include:re2c "unicode_categories.re" */

enum TOKEN
lex (const char **start_pos, const char **end_pos)
{
  const char *YYCURSOR = *start_pos, *YYMARKER;
  *end_pos = *start_pos;
  /*!re2c
     re2c:define:YYCTYPE = 'unsigned char';
     re2c:yyfill:enable = 0;

     digit = [0-9];
     digits = digit+;
     number = digits([\.]digits)?;
     id = [^ ;\.'"]+;
     date = ['] [^']+ ['];
     literal = ["] ([^"] | [""])* ["];

     number { *end_pos = YYCURSOR; return NUMBER; }
     date { *end_pos = YYCURSOR; return DATE; }
     literal { *end_pos = YYCURSOR; return LITERAL; }
     id { *end_pos = YYCURSOR; return ID; }
     *  { return ERROR; }
   */
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

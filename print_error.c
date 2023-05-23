#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "print_error.h"

void
print_error (const char *format, ...)
{
  va_list argptr;
  va_start (argptr, format);
  vprintf (format, argptr);
  va_end (argptr);
  exit (EXIT_FAILURE);
}

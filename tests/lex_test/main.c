#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../../lex.h"
#include "../../syn.h"
#include "../../print_error.h"

int
main (int argc, char **argv)
{

  yydebug=1;

  if (argc != 3)
    print_error ("%s",
		 "Правильный формат:./a.out ./test/test01 ./log");

  int fd_log, ret_val;

  if ((fd_log =
       open (argv[2], O_WRONLY | O_CREAT | O_APPEND, S_IWUSR | S_IRUSR)) < 0)
    print_error ("невозможно открыть %s для записи",
		 argv[2]);

  init_lex (argv[1]);

  ret_val = yyparse ();

  destroy_lex ();

  return ret_val;
}

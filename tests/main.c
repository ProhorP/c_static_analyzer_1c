#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../lex.h"
#include "../syn.h"
#include "../print_error.h"

int
main (int argc, char **argv)
{

  yydebug=1;

  if (argc != 2)
    print_error ("%s",
		 "Правильный формат:./a.out ./test/test01");

  int ret_val;

  init_lex (argv[1]);

  ret_val = yyparse ();

  destroy_lex ();

  return ret_val;
}

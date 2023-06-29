#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include "../main.h"
#include "../syn.h"

int
main (int argc, char **argv)
{

  #if YYDEBUG
  yydebug = 1;
  #endif

  //Правильный формат:./a.out ./test/test01
  assert(argc == 2);

  int ret_val;

  init_lex (argv[1]);

  ret_val = yyparse ();

  destroy_lex ();

  return ret_val;
}

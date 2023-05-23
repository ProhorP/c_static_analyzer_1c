#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../../lex/lex.h"
#include "../../print_error.h"

int
main (int argc, char **argv)
{

  if (argc != 3)
    print_error ("%s",
		 "Правильный формат:./a.out ./test/test01 ./log");

  int fd_log;

  if ((fd_log =
       open (argv[2], O_WRONLY | O_CREAT | O_APPEND, S_IWUSR | S_IRUSR)) < 0)
    print_error ("невозможно открыть %s для записи",
		 argv[2]);

  token *tok = NULL;

  init_lex (argv[1]);

  while ((tok = get_token ()))
    print_token (tok, fd_log);

  destroy_lex ();

  return EXIT_SUCCESS;
}

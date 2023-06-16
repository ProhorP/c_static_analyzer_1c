#include <stdint.h>

#define BUFFSIZE 409600

#define AS_PTR(val) (intptr_t) (val)
#define AS_TOKEN(val) (int) (val)

void init_lex (char *file_name);

void destroy_lex (void);

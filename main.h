#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <stddef.h>
#include "syn.h"

#define BUFFSIZE 409600
#define AS_PTR(val) (intptr_t) (val)
#define AS_TOKEN(val) (int)(val)

void print_error (const char *format, ...);

void init_lex (char *file_name);

void destroy_lex (void);

int get_token (int pre_token, YYSTYPE *lvalp, YYLTYPE *llocp);

void insert_dynamic_val_table (char *key, char *value);

char *check_dereference (char *obj, char *field, size_t field_line);

extern const char *start_pos;
extern const char *end_pos;
extern const char *limit;
extern size_t line;

extern char * query_string;
extern char * execute_string;
extern char * choose_string;
extern char * field_query;
#endif /* !MAIN_H */

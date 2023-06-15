#define BUFFSIZE 409600

void init_lex (char *file_name);

void destroy_lex (void);

int
yylex (YYSTYPE *lvalp, YYLTYPE *llocp)


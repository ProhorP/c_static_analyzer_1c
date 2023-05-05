// re2c $INPUT -o $OUTPUT -8 --case-ranges -i
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

enum TOKEN {
ERROR //0
, IF //1
, ID //2
, NUMBER //3
 };

/*!include:re2c "unicode_categories.re" */

enum TOKEN
lex (const char *s, const char **new_pos)
{
  const char *YYCURSOR = s, *YYMARKER;
  *new_pos = s;
  /*!re2c
     re2c:define:YYCTYPE = 'unsigned char';
     re2c:yyfill:enable = 0;

     // Simplified "Unicode Identifier and Pattern Syntax"
     // (see https://unicode.org/reports/tr31)
     //id_start_utf    = L | Nl | [$_];
     //id_continue_utf = id_start_utf | Mn | Mc | Nd | Pc | [\u200D\u05F3];
     //identifier_utf  = id_start_utf id_continue*;

     digit = [0-9];
     id_start = [^ 0-9];
     id_continue = [^ ];
     digits = digit+;
     number = digits([\.]digits)?;
     //letter = [A-Za-z] | id_continue;
     if = 'if';
     //id = letter (letter | digit)*;
     id = id_start (id_continue)+;
     if { *new_pos = YYCURSOR; return IF; }
     number { *new_pos = YYCURSOR; return NUMBER; }
     id { *new_pos = YYCURSOR; return ID; }
     *          { return ERROR; }
   */
}

int
main ()
{
  const char *new_pos = NULL;
  enum TOKEN res = ERROR;

#if 0
  const char *lexeme = "0123.222A";
#endif
#if 0
  const char *lexeme = "IF 1";
#endif
#if 0
  const char *lexeme = "ab1c";
#endif
#if 0
  const char *lexeme = "абв";
#endif
#if 1
  const char *lexeme = "1_ЫдентифиZкатор 1";
#endif




  res = lex (lexeme, &new_pos);
  printf ("%s:%d new_pos:%s\n", lexeme, res, new_pos);

  return 0;
}

// re2c $INPUT -o $OUTPUT -8 --case-ranges -i
/*!include:re2c "unicode_categories.re" */
#include "syn.h"
#include "main.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

int
yylex (YYSTYPE *lvalp, YYLTYPE *llocp)
{
  llocp->first_line = line;

  loop:

  if (end_pos)
    start_pos = end_pos;

  if (start_pos == limit)
    return YYEOF;

  const char *YYCURSOR = start_pos, *YYLIMIT = limit, *YYMARKER;
  /*!re2c
     re2c:define:YYCTYPE = 'unsigned char';
     re2c:yyfill:enable = 0;
     re2c:eof = 0;

     comment = [\/][\/][^\n]*;
     point = '.';
     comma = ',';
     colon = ':';
     semicolon = ';';
     left_parenthesis = '(';
     right_parenthesis = ')';
     left_square_bracket = '[';
     right_square_bracket = ']';
     whitespace = [ ]+;
     tab = [\t]+;
     newline = [\n];
     carriage = [\r];
     lt = '<';
     le = [<][=];
     eq = '=';
     ne = [<][>];
     gt = '>';
     ge = [>][=];
     plus = '+';
     minus = '-';
     multiply = '*';
     division = '/';
     remainder_of_division = '%';
     digit = [0-9];
     digits = digit+;
     number = digits([\.]digits)?;
     date = ['] [^']+ ['];
     literal = ["] ([^"] | ["]["])* ["];
     id = [^'"\.,:;()\[\] \t\n\r=<>+\-*\/%&#~]+;
     mark = [~] id;
     directive = [&] id;
     preprocessor = [#][^\n]+;

     comment { end_pos = YYCURSOR; goto loop; }
     point { end_pos = YYCURSOR; return AS_TOKEN('.'); }
     comma { end_pos = YYCURSOR; return AS_TOKEN(','); }
     colon { end_pos = YYCURSOR; return AS_TOKEN(':'); }
     semicolon { end_pos = YYCURSOR; return AS_TOKEN(';'); }
     left_parenthesis { end_pos = YYCURSOR; return AS_TOKEN('('); }
     right_parenthesis { end_pos = YYCURSOR; return AS_TOKEN(')'); }
     left_square_bracket { end_pos = YYCURSOR; return AS_TOKEN('['); }
     right_square_bracket { end_pos = YYCURSOR; return AS_TOKEN(']'); }
     whitespace { end_pos = YYCURSOR; goto loop; }
     tab { end_pos = YYCURSOR; goto loop; }
     newline { line++ ; end_pos = YYCURSOR; goto loop; }
     carriage { end_pos = YYCURSOR; goto loop; }
     lt { end_pos = YYCURSOR; return AS_TOKEN('<'); }
     le { end_pos = YYCURSOR; return LE; }
     eq { end_pos = YYCURSOR; return AS_TOKEN('='); }
     ne { end_pos = YYCURSOR; return NE; }
     gt { end_pos = YYCURSOR; return AS_TOKEN('>'); }
     ge { end_pos = YYCURSOR; return GE; }
     plus { end_pos = YYCURSOR; return AS_TOKEN('+'); }
     minus { end_pos = YYCURSOR; return AS_TOKEN('-'); }
     multiply { end_pos = YYCURSOR; return AS_TOKEN('*'); }
     division { end_pos = YYCURSOR; return AS_TOKEN('/'); }
     remainder_of_division { end_pos = YYCURSOR; return AS_TOKEN('%'); }
     number { end_pos = YYCURSOR; return get_token(NUMBER, lvalp, llocp); }
     date { end_pos = YYCURSOR; return get_token(DATE, lvalp, llocp); }
     literal { end_pos = YYCURSOR; return get_token(LITERAL, lvalp, llocp); }
     preprocessor { end_pos = YYCURSOR; return get_token(PREPROCESSOR, lvalp,
     llocp); } directive { end_pos = YYCURSOR; return get_token(DIRECTIVE,
     lvalp, llocp); } mark { end_pos = YYCURSOR; return get_token(MARK, lvalp,
     llocp); } id { end_pos = YYCURSOR; return get_token(ID, lvalp, llocp); }
     $ { end_pos = YYCURSOR; return YYEOF; }
     *  { end_pos = YYCURSOR; return YYUNDEF; }
   */
}
#pragma GCC diagnostic pop
//end lex.l

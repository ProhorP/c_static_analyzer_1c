/* Infix notation calculator. */

%{
  #include <stdio.h>
  #include <ctype.h>
  #include <stdlib.h>
int yylex (YYSTYPE *lvalp, YYLTYPE *llocp);
void yyerror (YYLTYPE *locp, char const *msg);
%}

/* Bison declarations. */
%verbose
%define parse.trace
%header
%output "syn.c"
%define api.pure full
%define api.value.type {char *}
%printer { fprintf (yyo, "%s", $$); } NUMBER DATE LITERAL PREPROCESSOR ID MARK;
%token 
','
':'
';'
'('
')'
'['
']'
//IF
IF			
THEN			
ELSIF			
ELSE			
ENDIF			
//DO
FOR			
EACH			
IN			
TO			
WHILE			
DO			
BREAK			
CONTINUE			
ENDDO			
//FUNCTION
PROCEDURE		
FUNCTION			
ENDPROCEDURE	
ENDFUNCTION		
VAR			
GOTO			
RETURN			
TRY			
EXCEPT			
RAISE			
ENDTRY			
NEW			
EXECUTE			
ADDHANDLER		
REMOVEHANDLER		
//type
T_NULL
T_TRUE	
T_FALSE		
T_UNDEFINED		
EXPORT			
VAL			
//DIRECTIVE
ATCLIENT			
ATSERVER			
ATSERVERNOCONTEXT		
ATCLIENTATSERVERNOCONTEXT	
ATCLIENTATSERVER		
//OTHER
NUMBER			
DATE			
LITERAL			
PREPROCESSOR		
DIRECTIVE			
MARK			
ID			
%right ASG        /* ASSIGNMENT(=) */
%left OR
%left AND
%left NOT
%left '<' '>' '=' LE GE NE
%left '+' '-'
%left '*' '/' '%'
%precedence NEG APR  /* negation--unary minus; approval--unari plus */
%precedence '.'  /* call method */

%% /* The grammar follows. */
/*
input:
  %empty
| input line
;

line:
  end_line
| exp end_line  { printf ("%d:\t%.10g\n", @$.first_line, $1); }
;

end_line:
'\n'
|';'
;

exp:
  NUM
| exp '+' exp        { $$ = $1 + $3;      }
| exp '-' exp        { $$ = $1 - $3;      }
| exp '*' exp        { $$ = $1 * $3;      }
| exp '/' exp        { $$ = $1 / $3;      }
| '-' exp  %prec NEG { $$ = -$2;          }
| exp '^' exp        { $$ = pow ($1, $3); }
| '(' exp ')'        { $$ = $2;           }
;
*/
all_token:
|','
|':'
|';'
|'('
|')'
|'['
|']'
//IF
|IF			
|THEN			
|ELSIF			
|ELSE			
|ENDIF			
//DO
|FOR			
|EACH			
|IN			
|TO			
|WHILE			
|DO			
|BREAK			
|CONTINUE			
|ENDDO			
//FUNCTION
|PROCEDURE		
|FUNCTION			
|ENDPROCEDURE	
|ENDFUNCTION		
|VAR			
|GOTO			
|RETURN			
|TRY			
|EXCEPT			
|RAISE			
|ENDTRY			
|NEW			
|EXECUTE			
|ADDHANDLER		
|REMOVEHANDLER		
//type
|T_NULL
|T_TRUE	
|T_FALSE		
|T_UNDEFINED		
|EXPORT			
|VAL			
//DIRECTIVE
|ATCLIENT			
|ATSERVER			
|ATSERVERNOCONTEXT		
|ATCLIENTATSERVERNOCONTEXT	
|ATCLIENTATSERVER		
//OTHER
|NUMBER			
|DATE			
|LITERAL			
|PREPROCESSOR		
|DIRECTIVE			
|MARK			
|ID			
//tokens with priorities
|ASG
|OR
|AND
|NOT
|'<'
|'>'
|'='
|LE
|GE
|NE
|'+'
|'-'
|'*'
|'/'
|'%'
|NEG
|APR
|'.'
;

%%

int
main (void)
{
yydebug=1;
  return yyparse ();
}


/* Called by yyparse on error. */
void
yyerror (YYLTYPE *locp, char const *msg)
{
  fprintf (stderr, "%d:%s\n", locp->first_line, msg);
}


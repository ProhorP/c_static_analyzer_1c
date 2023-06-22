/* Infix notation calculator. */

%{
  #include <stdio.h>
  #include <ctype.h>
  #include <stdlib.h>
%}

%code provides {
  int yylex (YYSTYPE *lvalp, YYLTYPE *llocp);
  void yyerror (YYLTYPE *locp, char const *msg);
}

/* Bison declarations. */
%language "c"
//%verbose
%define parse.trace
%define lr.type ielr
%define parse.lac full
%header
%output "syn.c"
%define api.pure full
%define api.value.type {char *}
%printer { fprintf (yyo, "%d:%s", @$.first_line, $$); } NUMBER DATE LITERAL PREPROCESSOR ID MARK;
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
ELSIF			
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
%precedence THEN
%precedence ELSE
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

input:
  %empty
| input any_block
;

any_block:
line
;

line:
  end_line
| expr end_line  { printf ("%s\n", "строка expr обработана"); }
| stmt end_line  { printf ("%s\n", "строка stmt обработана"); }
;

end_line:
';'
;

expr:
  NUMBER
| ID
| expr '=' expr         {printf("%s=%s\n", $1, $3);}
;

stmt:
expr
| stmt '=' stmt         {printf("%s=%s\n", $1, $3);}
| stmt '+' stmt        {printf("%s+%s\n", $1, $3);}
| stmt '-' stmt        {printf("%s-%s\n", $1, $3);}
| stmt '*' stmt        {printf("%s*%s\n", $1, $3);}
| stmt '/' stmt        {printf("%s/%s\n", $1, $3);}
| '-' stmt  %prec NEG {printf("-%s\n", $2);}
| '(' stmt ')'        {printf("(%s)\n", $2);}
;

/*
input:
  %empty
| input all_token
;

all_token:
','
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
*/
%%

/* Called by yyparse on error. */
void
yyerror (YYLTYPE *locp, char const *msg)
{
  fprintf (stderr, "%d:%s\n", locp->first_line, msg);
}


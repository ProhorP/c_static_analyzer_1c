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
//ASSIGNMENT(=) 
%right ASG        
//LOGIC
%left OR
%left AND
%left NOT
//COMPARE
%left '<' '>' '=' LE GE NE
%left '+' '-'
%left '*' '/' '%'
%precedence NEG APR  /* negation--unary minus; approval--unari plus */
//DEREFERENCING
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
';'
| assignment { printf ("%s\n", "строка assignment обработана"); }
;

assignment:
ID '=' exp ';' %prec ASG         {printf("ASG(math) %s=\n", $1);}
;

exp:
math
| logic;

math:
  NUMBER
| ID
| math '+' math        {printf("%s+%s\n", $1, $3);}
| math '-' math        {printf("%s-%s\n", $1, $3);}
| math '*' math        {printf("%s*%s\n", $1, $3);}
| math '/' math        {printf("%s/%s\n", $1, $3);}
| '-' math  %prec NEG  {printf("-%s\n", $2);}
| '+' math  %prec APR  {printf("+%s\n", $2);}
| '(' math ')'         {printf("(%s)\n", $2); $$ = $2;}
;

compare:
| math '<' math {printf("%s<%s\n", $1, $3);}
| math '>' math {printf("%s>%s\n", $1, $3);}
| math '=' math {printf("%s=%s\n", $1, $3);}
| math LE math  {printf("%s<=%s\n", $1, $3);}
| math GE math  {printf("%s>=%s\n", $1, $3);}
| math NE math  {printf("%s<>%s\n", $1, $3);}

logic:
T_TRUE	                {printf("%s\n", "TRUE");}
| T_FALSE		{printf("%s\n", "FALSE");}
| compare               {printf("%s\n", "COMPARE");}
| logic OR logic        {printf("%s OR %s\n", $1, $3);}
| logic AND logic       {printf("%s AND%s\n", $1, $3);}
| NOT logic             {printf("NOT %s\n", "NOT %s", $2);}

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


/* Infix notation calculator. */

%{
//  #include <stdio.h>
 // #include <ctype.h>
 // #include <stdlib.h>
%}

%code provides {
  int yylex (YYSTYPE *lvalp, YYLTYPE *llocp);
  void yyerror (YYLTYPE *locp, char const *msg);
}

/* Bison declarations. */
%language "c"
%verbose
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
THEN			
ELSE
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
//ASSIGNMENT(=) 
%right ASG        
//LOGIC
%left OR
%left AND
%left NOT
//COMPARE
%left '<' '>' '=' LE GE NE
//MATH
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
proc_block
| func_block
| block
;

block:
line            { printf ("%s\n", "line обработана"); }
|if_block       { printf ("%s\n", "if_block обработана"); }
|while_block       { printf ("%s\n", "while_block обработана"); }
|for_block       { printf ("%s\n", "for_block обработана"); }
|foreach_block       { printf ("%s\n", "foreach_block обработана"); }
|BREAK		{ printf ("%s\n", "BREAK обработана"); }
|CONTINUE       { printf ("%s\n", "CONTINUE обработана"); }
|RETURN         { printf ("%s\n", "RETURN обработана"); }
;

blocks:
%empty
| rblock
;

rblock:
block
| rblock block
;

line:
';'
| ID '=' expr ';' %prec ASG         {printf("ASG %s=\n", $1);}
;

expr:
  NUMBER
| ID
| T_TRUE
| T_FALSE
| expr '+' expr        {printf("%s+%s\n", $1, $3);}
| expr '-' expr        {printf("%s-%s\n", $1, $3);}
| expr '*' expr        {printf("%s*%s\n", $1, $3);}
| expr '/' expr        {printf("%s/%s\n", $1, $3);}
| '-' expr  %prec NEG  {printf("-%s\n", $2);}
| '+' expr  %prec APR  {printf("+%s\n", $2);}
| '(' expr ')'         {printf("(%s)\n", $2); $$ = $2;}
| expr '<' expr        {printf("%s<%s\n", $1, $3);}
| expr '>' expr        {printf("%s>%s\n", $1, $3);}
| expr '=' expr        {printf("%s=%s\n", $1, $3);}
| expr LE expr         {printf("%s<=%s\n", $1, $3);}
| expr GE expr         {printf("%s>=%s\n", $1, $3);}
| expr NE expr         {printf("%s<>%s\n", $1, $3);}
| expr OR expr         {printf("%s OR %s\n", $1, $3);}
| expr AND expr        {printf("%s AND%s\n", $1, $3);}
| NOT expr             {printf("NOT %s\n", $2);}

if:
  IF expr THEN blocks
| if ELSIF expr THEN blocks
;

if_block:
if ENDIF
| if ELSE blocks ENDIF
;

while_block:
WHILE expr DO blocks ENDDO
;

for_block:
FOR ID '=' expr TO expr DO blocks ENDDO
;

foreach_block:
FOR EACH ID IN ID DO blocks ENDDO
;

proc_block:
PROCEDURE ID '(' all_params ')' blocks ENDPROCEDURE {printf("import procedure %s\n", $2);}
| PROCEDURE ID '(' all_params ')' EXPORT blocks ENDPROCEDURE {printf("export procedure %s\n", $2);}
;

func_block:
FUNCTION ID '(' params ')' blocks ENDFUNCTION {printf("import function %s\n", $2);}
| FUNCTION ID '(' params ')' EXPORT blocks ENDFUNCTION {printf("export function %s\n", $2);}
;

params:
%empty
| rparams
;

rparams:
ID
| VAL ID
| rparams ',' ID
| rparams ',' VAL ID
;
%%

/* Called by yyparse on error. */
void
yyerror (YYLTYPE *locp, char const *msg)
{
  fprintf (stderr, "%d:%s\n", locp->first_line, msg);
}


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
  line            
| if_block       { printf ("%s\n", "if_block обработана"); }
| while_block       { printf ("%s\n", "while_block обработана"); }
| for_block       { printf ("%s\n", "for_block обработана"); }
| foreach_block       { printf ("%s\n", "foreach_block обработана"); }
| try_block       { printf ("%s\n", "try_block обработана"); }
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
  ';'{ printf ("%s\n", "; обработана"); }
| ID '=' expr ';' %prec ASG         {printf("ASG expr %s=\n", $1);}
| ID '=' literal ';' %prec ASG         {printf("ASG literal %s=\n", $1);}
| ID '=' dereference ';' %prec ASG         {printf("ASG dereference %s=\n", $1);}
| ID '=' date ';' %prec ASG         {printf("ASG date %s=\n", $1);}
| goto       { printf ("%s\n", "goto обработана"); }
| mark       { printf ("%s\n", "mark обработана"); }
| new       { printf ("%s\n", "new обработана"); }
| define_var          { printf ("%s\n", "define_var обработана"); }
| execute { printf ("%s\n", "execute обработана"); }
| addhandler{ printf ("%s\n", "addhandler обработана"); }
| removehandler{ printf ("%s\n", "removehandler обработана"); }
| directive{ printf ("%s\n", "directive обработана"); }
| BREAK		{ printf ("%s\n", "BREAK обработана"); }
| CONTINUE       { printf ("%s\n", "CONTINUE обработана"); }
| RETURN         { printf ("%s\n", "RETURN обработана"); }
| RAISE          { printf ("%s\n", "RAISE обработана"); }
;

directive:
  ATCLIENT
| ATSERVER
| ATSERVERNOCONTEXT
| ATCLIENTATSERVERNOCONTEXT
| ATCLIENTATSERVER
;

define_var:
  VAR ID
;

goto:
  GOTO MARK
;

mark:
  MARK ':'
;

new:
  ID '=' NEW ID
;

expr:
  NUMBER
| ID
| t_bool
| expr '+' expr        {printf("%s+%s\n", $1, $3);}
| expr '-' expr        {printf("%s-%s\n", $1, $3);}
| expr '*' expr        {printf("%s*%s\n", $1, $3);}
| expr '/' expr        {printf("%s/%s\n", $1, $3);}
| '-' expr  %prec NEG  {printf("-%s\n", $2);}
| '+' expr  %prec APR  {printf("+%s\n", $2);}
| '(' expr ')'         {printf("(%s)\n", $2); $$ = $2;}
| expr '<' expr        {printf("%s<%s\n", $1, $3);}
| date '<' date        {printf("%s<%s\n", $1, $3);}
| literal '<' literal        {printf("%s<%s\n", $1, $3);}
| expr '>' expr        {printf("%s>%s\n", $1, $3);}
| date '>' date        {printf("%s>%s\n", $1, $3);}
| literal '>' literal        {printf("%s>%s\n", $1, $3);}
| expr '=' expr        {printf("%s=%s\n", $1, $3);}
| t_lack '=' t_lack        {printf("%s=%s\n", $1, $3);}
| date '=' date        {printf("%s=%s\n", $1, $3);}
| literal '=' literal        {printf("%s=%s\n", $1, $3);}
| expr '=' t_lack        {printf("%s=%s\n", $1, $3);}
| expr '=' date        {printf("%s=%s\n", $1, $3);}
| expr '=' literal        {printf("%s=%s\n", $1, $3);}
| t_lack '=' expr        {printf("%s=%s\n", $1, $3);}
| date '=' expr        {printf("%s=%s\n", $1, $3);}
| literal '=' expr        {printf("%s=%s\n", $1, $3);}
| date '=' t_lack        {printf("%s=%s\n", $1, $3);}
| literal '=' t_lack        {printf("%s=%s\n", $1, $3);}
| t_lack '=' date        {printf("!!!%s=%s\n", $1, $3);}
| t_lack '=' literal        {printf("!!!%s=%s\n", $1, $3);}
| date '=' literal        {printf("!!!%s=%s\n", $1, $3);}
| literal '=' date        {printf("!!!%s=%s\n", $1, $3);}
| expr LE expr         {printf("%s<=%s\n", $1, $3);}
| date LE date         {printf("%s<=%s\n", $1, $3);}
| literal LE literal         {printf("%s<=%s\n", $1, $3);}
| expr GE expr         {printf("%s>=%s\n", $1, $3);}
| date GE date         {printf("%s>=%s\n", $1, $3);}
| literal GE literal         {printf("%s>=%s\n", $1, $3);}
| expr NE expr         {printf("%s<>%s\n", $1, $3);}
| t_lack NE t_lack         {printf("%s<>%s\n", $1, $3);}
| date NE date         {printf("%s<>%s\n", $1, $3);}
| literal NE literal         {printf("%s<>%s\n", $1, $3);}
| t_lack NE expr         {printf("%s<>%s\n", $1, $3);}
| date NE expr         {printf("%s<>%s\n", $1, $3);}
| literal NE expr         {printf("%s<>%s\n", $1, $3);}
| expr NE t_lack         {printf("%s<>%s\n", $1, $3);}
| expr NE date         {printf("%s<>%s\n", $1, $3);}
| expr NE literal         {printf("%s<>%s\n", $1, $3);}
| t_lack NE date         {printf("%s<>%s\n", $1, $3);}
| t_lack NE literal         {printf("%s<>%s\n", $1, $3);}
| date NE t_lack         {printf("%s<>%s\n", $1, $3);}
| literal NE t_lack         {printf("%s<>%s\n", $1, $3);}
| date NE literal         {printf("%s<>%s\n", $1, $3);}
| literal NE date         {printf("%s<>%s\n", $1, $3);}
| expr OR expr         {printf("%s OR %s\n", $1, $3);}
| expr AND expr        {printf("%s AND%s\n", $1, $3);}
| NOT expr             {printf("NOT %s\n", $2);}
;

t_bool:
  T_TRUE
| T_FALSE
;

t_lack:
  T_UNDEFINED
| T_NULL
;

dereference:
  ID '.' ID
| dereference '.' ID
;

literal:
  LITERAL
| literal '+' LITERAL
| literal '+' ID
| literal '+' NUMBER
| literal '+' DATE
| literal '+' t_bool
| literal '+' t_lack
;

date:
  DATE
| date '+' NUMBER
| date '+' t_bool
| date '+' ID
| date '-' NUMBER
| date '-' t_bool
| date '-' ID
;

addhandler:
  ADDHANDLER dereference ',' ID
| ADDHANDLER dereference ',' dereference
;

removehandler:
  REMOVEHANDLER dereference ',' ID
| REMOVEHANDLER dereference ',' dereference
;

execute:
  EXECUTE '(' ID ')'
| EXECUTE '(' literal ')'
;

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
  PROCEDURE ID '(' params ')' blocks ENDPROCEDURE {printf("import procedure %s\n", $2);}
| PROCEDURE ID '(' params ')' EXPORT blocks ENDPROCEDURE {printf("export procedure %s\n", $2);}
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

try_block:
  TRY blocks EXCEPT blocks ENDTRY
;

%%

/* Called by yyparse on error. */
void
yyerror (YYLTYPE *locp, char const *msg)
{
  fprintf (stderr, "%d:%s\n", locp->first_line, msg);
}


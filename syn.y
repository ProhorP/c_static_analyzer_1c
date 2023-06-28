/* Infix notation calculator. */

%{
 // #include <stdio.h>
 // #include <ctype.h>
 // #include <stdlib.h>
  #include "main.h"
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
| ID '=' expr ';' %prec ASG         {printf("ASG expr %s=\n", $1); insert_dynamic_val_table($1, $3);}
| ID '=' literal ';' %prec ASG         {printf("ASG literal %s=\n", $1); insert_dynamic_val_table($1, $3);}
| ID '=' date ';' %prec ASG         {printf("ASG date %s=\n", $1); insert_dynamic_val_table($1, $3);}
| ID '=' new ';' %prec ASG         {printf("ASG new %s=\n", $1); insert_dynamic_val_table($1, $3);}
| goto       { printf ("%s\n", "goto обработана"); }
| mark       { printf ("%s\n", "mark обработана"); }
| define_var          { printf ("%s\n", "define_var обработана"); }
| execute { printf ("%s\n", "execute обработана"); }
| addhandler{ printf ("%s\n", "addhandler обработана"); }
| removehandler{ printf ("%s\n", "removehandler обработана"); }
| directive{ printf ("%s\n", "directive обработана"); }
| rcall  { printf ("%s\n", "rcall обработана"); }
| RETURN ';'        { printf ("%s\n", "RETURN обработана"); }
| RETURN call_val_param ';'     { printf ("%s\n", "RETURN VAL обработана"); }
| BREAK		{ printf ("%s\n", "BREAK обработана"); }
| CONTINUE       { printf ("%s\n", "CONTINUE обработана"); }
| RAISE          { printf ("%s\n", "RAISE обработана"); }
| PREPROCESSOR          { printf ("%s\n", "PREPROCESSOR обработана"); }
;

directive:
  ATCLIENT
| ATSERVER
| ATSERVERNOCONTEXT
| ATCLIENTATSERVERNOCONTEXT
| ATCLIENTATSERVER
;

id_list:
  ID
| id_list ',' ID
;

define_var:
  VAR id_list
| VAR id_list EXPORT
;

goto:
  GOTO MARK
;

mark:
  MARK ':'
;

new:
  NEW ID {$$ = $2;}
| NEW '(' id_list ')'{$$ = $3;}
;

runtime_val:
  ID
| dereference
| rcall
;

expr:
runtime_val
| NUMBER
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
  ID '.' ID {
    printf ("%s.%s\n", $1, $3);
    $$ = check_dereference($1, $3, @3.first_line);
}
| dereference '.' ID {
    printf("dereference.%s\n", $3);
    $$ = check_dereference($1, $3, @3.first_line);
}
;

literal:
  LITERAL
| literal '+' LITERAL
| literal '+' runtime_val
| literal '+' NUMBER
| literal '+' DATE
| literal '+' t_bool
| literal '+' t_lack
;

date:
  DATE
| date '+' NUMBER
| date '+' t_bool
| date '+' runtime_val
| date '-' NUMBER
| date '-' t_bool
| date '-' runtime_val
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
  EXECUTE '(' runtime_val ')'
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
  FOR EACH ID IN runtime_val DO blocks ENDDO
;

proc_block:
  PROCEDURE ID '(' define_params ')' blocks ENDPROCEDURE {printf("import procedure %s\n", $2);}
| PROCEDURE ID '(' define_params ')' EXPORT blocks ENDPROCEDURE {printf("export procedure %s\n", $2);}
;

func_block:
  FUNCTION ID '(' define_params ')' blocks ENDFUNCTION {printf("import function %s\n", $2);}
| FUNCTION ID '(' define_params ')' EXPORT blocks ENDFUNCTION {printf("export function %s\n", $2);}
;

define_params:
  %empty
| define_rparams
;

define_rparams:
  ID
| ID '=' def_val_param
| VAL ID
| VAL ID '=' def_val_param
| define_rparams ',' ID
| define_rparams ',' ID '=' def_val_param
| define_rparams ',' VAL ID
| define_rparams ',' VAL ID '=' def_val_param
;

def_val_param:
  NUMBER
| LITERAL
| DATE
| t_bool
| t_lack
;

rcall:
  call
| rcall '.' call{$$ = $3;}
;

call:
  ID '(' call_params ')'
| dereference '(' call_params ')'
| ID '.' EXECUTE '(' ')'{$$ = $3;}
;

call_params:
  %empty
| call_rparams
;

call_rparams:
  call_param_first
| call_rparams ',' call_val_param
| call_rparams ','
;

call_param_first:
  call_val_param
| ','
;

call_val_param:
  literal
| date
| t_lack
| expr
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


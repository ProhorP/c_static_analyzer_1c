/*
Читайте документацию тут
https://www.gnu.org/software/bison/manual/bison.html
Она очень большая, но понятная как книги Радченко по 1С с множеством примеров
*/

/* Infix notation calculator. */

%{
 // #include <ctype.h>
 // #include <stdlib.h>
  #include <stdio.h>
  #include "main.h"
%}

%code provides {
  int yylex (YYSTYPE *lvalp, YYLTYPE *llocp);
  void yyerror (YYLTYPE *locp, char const *msg);
}

/* Bison declarations. */
%language "c"
%verbose
//%define parse.trace
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
| if_block       { PRINT ("%s\n", "if_block обработана"); }
| while_block       { PRINT ("%s\n", "while_block обработана"); }
| for_block       { PRINT ("%s\n", "for_block обработана"); }
| foreach_block       { PRINT ("%s\n", "foreach_block обработана"); }
| try_block       { PRINT ("%s\n", "try_block обработана"); }
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
  ';'{ PRINT ("%s\n", "; обработана"); }
| object '=' expr ';' %prec ASG         {PRINT("ASG expr %s=\n", $1); insert_dynamic_val_table($1, $3);}
| object '=' literal ';' %prec ASG         {PRINT("ASG literal %s=\n", $1); insert_dynamic_val_table($1, $3);}
| object '=' date ';' %prec ASG         {PRINT("ASG date %s=\n", $1); insert_dynamic_val_table($1, $3);}
| object '=' new ';' %prec ASG         {PRINT("ASG new %s=\n", $1); insert_dynamic_val_table($1, $3);}
| goto       { PRINT ("%s\n", "goto обработана"); }
| mark       { PRINT ("%s\n", "mark обработана"); }
| define_var          { PRINT ("%s\n", "define_var обработана"); }
| execute { PRINT ("%s\n", "execute обработана"); }
| addhandler{ PRINT ("%s\n", "addhandler обработана"); }
| removehandler{ PRINT ("%s\n", "removehandler обработана"); }
| directive{ PRINT ("%s\n", "directive обработана"); }
| rcall  { PRINT ("%s\n", "rcall обработана"); }
| RETURN ';'        { PRINT ("%s\n", "RETURN обработана"); }
| RETURN call_val_param ';'     { PRINT ("%s\n", "RETURN VAL обработана"); }
| BREAK		{ PRINT ("%s\n", "BREAK обработана"); }
| CONTINUE       { PRINT ("%s\n", "CONTINUE обработана"); }
| RAISE          { PRINT ("%s\n", "RAISE обработана"); }
| PREPROCESSOR          { PRINT ("%s\n", "PREPROCESSOR обработана"); }
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
  object
| rcall
;

expr:
runtime_val
| NUMBER
| t_bool
| expr '+' expr        {PRINT("%s+%s\n", $1, $3);}
| expr '-' expr        {PRINT("%s-%s\n", $1, $3);}
| expr '*' expr        {PRINT("%s*%s\n", $1, $3);}
| expr '/' expr        {PRINT("%s/%s\n", $1, $3);}
| '-' expr  %prec NEG  {PRINT("-%s\n", $2);}
| '+' expr  %prec APR  {PRINT("+%s\n", $2);}
| '(' expr ')'         {PRINT("(%s)\n", $2); $$ = $2;}
| expr '<' expr        {PRINT("%s<%s\n", $1, $3);}
| date '<' date        {PRINT("%s<%s\n", $1, $3);}
| literal '<' literal        {PRINT("%s<%s\n", $1, $3);}
| expr '>' expr        {PRINT("%s>%s\n", $1, $3);}
| date '>' date        {PRINT("%s>%s\n", $1, $3);}
| literal '>' literal        {PRINT("%s>%s\n", $1, $3);}
| expr '=' expr        {PRINT("%s=%s\n", $1, $3);}
| t_lack '=' t_lack        {PRINT("%s=%s\n", $1, $3);}
| date '=' date        {PRINT("%s=%s\n", $1, $3);}
| literal '=' literal        {PRINT("%s=%s\n", $1, $3);}
| expr '=' t_lack        {PRINT("%s=%s\n", $1, $3);}
| expr '=' date        {PRINT("%s=%s\n", $1, $3);}
| expr '=' literal        {PRINT("%s=%s\n", $1, $3);}
| t_lack '=' expr        {PRINT("%s=%s\n", $1, $3);}
| date '=' expr        {PRINT("%s=%s\n", $1, $3);}
| literal '=' expr        {PRINT("%s=%s\n", $1, $3);}
| date '=' t_lack        {PRINT("%s=%s\n", $1, $3);}
| literal '=' t_lack        {PRINT("%s=%s\n", $1, $3);}
| t_lack '=' date        {PRINT("!!!%s=%s\n", $1, $3);}
| t_lack '=' literal        {PRINT("!!!%s=%s\n", $1, $3);}
| date '=' literal        {PRINT("!!!%s=%s\n", $1, $3);}
| literal '=' date        {PRINT("!!!%s=%s\n", $1, $3);}
| expr LE expr         {PRINT("%s<=%s\n", $1, $3);}
| date LE date         {PRINT("%s<=%s\n", $1, $3);}
| literal LE literal         {PRINT("%s<=%s\n", $1, $3);}
| expr GE expr         {PRINT("%s>=%s\n", $1, $3);}
| date GE date         {PRINT("%s>=%s\n", $1, $3);}
| literal GE literal         {PRINT("%s>=%s\n", $1, $3);}
| expr NE expr         {PRINT("%s<>%s\n", $1, $3);}
| t_lack NE t_lack         {PRINT("%s<>%s\n", $1, $3);}
| date NE date         {PRINT("%s<>%s\n", $1, $3);}
| literal NE literal         {PRINT("%s<>%s\n", $1, $3);}
| t_lack NE expr         {PRINT("%s<>%s\n", $1, $3);}
| date NE expr         {PRINT("%s<>%s\n", $1, $3);}
| literal NE expr         {PRINT("%s<>%s\n", $1, $3);}
| expr NE t_lack         {PRINT("%s<>%s\n", $1, $3);}
| expr NE date         {PRINT("%s<>%s\n", $1, $3);}
| expr NE literal         {PRINT("%s<>%s\n", $1, $3);}
| t_lack NE date         {PRINT("%s<>%s\n", $1, $3);}
| t_lack NE literal         {PRINT("%s<>%s\n", $1, $3);}
| date NE t_lack         {PRINT("%s<>%s\n", $1, $3);}
| literal NE t_lack         {PRINT("%s<>%s\n", $1, $3);}
| date NE literal         {PRINT("%s<>%s\n", $1, $3);}
| literal NE date         {PRINT("%s<>%s\n", $1, $3);}
| expr OR expr         {PRINT("%s OR %s\n", $1, $3);}
| expr AND expr        {PRINT("%s AND%s\n", $1, $3);}
| NOT expr             {PRINT("NOT %s\n", $2);}
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
    PRINT ("%s.%s\n", $1, $3);
    $$ = check_dereference($1, $3, @3.first_line);
}
| dereference '.' ID {
    PRINT("dereference.%s\n", $3);
    $$ = check_dereference($1, $3, @3.first_line);
}
;

object:
  ID
| dereference
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
  PROCEDURE ID '(' define_params ')' blocks ENDPROCEDURE {PRINT("import procedure %s\n", $2);}
| PROCEDURE ID '(' define_params ')' EXPORT blocks ENDPROCEDURE {PRINT("export procedure %s\n", $2);}
;

func_block:
  FUNCTION ID '(' define_params ')' blocks ENDFUNCTION {PRINT("import function %s\n", $2);}
| FUNCTION ID '(' define_params ')' EXPORT blocks ENDFUNCTION {PRINT("export function %s\n", $2);}
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
  object '(' call_params ')'
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


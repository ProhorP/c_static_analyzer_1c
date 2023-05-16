/* Generated by re2c 3.0 on Tue May 16 17:43:39 2023 */
// re2c $INPUT -o $OUTPUT -8 --case-ranges -i
#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <glib.h>
#include <glib/gprintf.h>

#define BUFFSIZE 409600
enum TAG
{
  ERROR				//0
    , END			//1
    , TEST			//2
    , POINT			//3
    , COMMA			//4
    , SEMICOLON			//5
    , LEFT_PARENTHESIS		//6
    , RIGHT_PARENTHESIS		//7
    , LEFT_SQUARE_BRACKET	//8
    , RIGHT_SQUARE_BRACKET	//9
    , WHITESPACE		//10
    , TAB			//11
    , NEWLINE			//12
    , RELOP			//13
    , MATH			//14
    , NUMBER			//15
    , DATE			//16
    , LITERAL			//17
    , PREPROCESSOR		//18
    , AREA			//19
    , ID			//20
};

char *tag_text[] = {
  "ERROR"			//0
    , "END"			//1
    , "TEST"			//2
    , "POINT"			//3
    , "COMMA"			//4
    , "SEMICOLON"		//5
    , "LEFT_PARENTHESIS"	//6
    , "RIGHT_PARENTHESIS"	//7
    , "LEFT_SQUARE_BRACKET"	//8
    , "RIGHT_SQUARE_BRACKET"	//9
    , "WHITESPACE"		//10
    , "TAB"			//11
    , "NEWLINE"			//12
    , "RELOP"			//13
    , "MATH"			//14
    , "NUMBER"			//15
    , "DATE"			//16
    , "LITERAL"			//17
    , "PREPROCESSOR"		//18
    , "AREA"			//19
    , "ID"			//20
};

enum TAG_ATTRIBUTE
{
  EMPTY				//0
    , PTR			//1
    , LT			//2
    , LE			//3
    , EQ			//4
    , NE			//5
    , GT			//6
    , GE			//7
    , PLUS			//8
    , MINUS			//9
    , MULTIPLY			//10
    , DIVISION			//11
    , REMAINDER_OF_DIVISION	//12
};

char *tag_attribute[] = {
  "EMPTY"			//0
    , "PTR"			//1
    , "LT"			//2
    , "LE"			//3
    , "EQ"			//4
    , "NE"			//5
    , "GT"			//6
    , "GE"			//7
    , "PLUS"			//8
    , "MINUS"			//9
    , "MULTIPLY"		//10
    , "DIVISION"		//11
    , "REMAINDER_OF_DIVISION"	//12
};

typedef struct
{
  enum TAG tag;
  enum TAG_ATTRIBUTE attribute;
  size_t line;
  char *text;
} token;

typedef struct entry_token entry_token;

struct entry_token
{
  entry_token *next;
  token token;
};

entry_token *head_token = NULL;
GHashTable *symbol_table = NULL;

token *
create_token (enum TAG tag, size_t line, const char *start_pos,
	      const char *end_pos)
{
  gpointer lexeme = NULL;
  size_t n = end_pos - start_pos;

  if (tag == ID)
    {
      gchar *lexeme_upper = g_utf8_strup (start_pos, n);
      lexeme = g_hash_table_lookup (symbol_table, lexeme_upper);

      if (lexeme != NULL)
	g_free (lexeme_upper);
      else
	{
	  lexeme = (gpointer) calloc (1, n + 1);
	  memcpy (lexeme, start_pos, n);
	  g_hash_table_insert (symbol_table, lexeme_upper, lexeme);
	}
    }
  else if (tag == LITERAL)
    {
      const char *ptr_literal = start_pos;
      while (ptr_literal != end_pos)
	{
	  if (*ptr_literal == '\n')
	    {
	      line++;
	    }
	  ptr_literal++;
	}
    }

  entry_token *new_token = (entry_token *) malloc (sizeof (entry_token));
  new_token->next = head_token;
  head_token = new_token;

  new_token->token.tag = tag;
  new_token->token.attribute = EMPTY;
  new_token->token.line = line;
  new_token->token.text = lexeme;

  return &(new_token->token);

}

void
init_lex ()
{
  symbol_table =
    g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
}

void
destroy_lex ()
{
  entry_token *temp = head_token, *next;
  while (temp != NULL)
    {
      next = temp->next;
      free (temp);
      temp = next;
    }

  g_hash_table_destroy (symbol_table);
  head_token = NULL;
  symbol_table = NULL;
}

void
print_error (const char *format, ...)
{
  va_list argptr;
  va_start (argptr, format);
  vprintf (format, argptr);
  va_end (argptr);
  exit (EXIT_FAILURE);
}




token *
lex (const char **start_pos, const char **end_pos, const char *limit,
     size_t line)
{
  const char *YYCURSOR = *start_pos, *YYLIMIT = limit, *YYMARKER;
  size_t cur_line = line;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
loop:
  
{
	unsigned char yych;
	unsigned int yyaccept = 0;
	yych = *YYCURSOR;
	switch (yych) {
		case 0x00:
		case 0x01 ... 0x08:
		case '\v' ... 0x1F:
		case '!':
		case '$':
		case ':':
		case '?' ... 'Z':
		case '\\':
		case '^' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy89;
			goto yy1;
		case '\t': goto yy3;
		case '\n': goto yy5;
		case ' ': goto yy7;
		case '"': goto yy9;
		case '#': goto yy11;
		case '%':
		case '*' ... '+':
		case '-': goto yy12;
		case '&': goto yy14;
		case '\'': goto yy15;
		case '(': goto yy16;
		case ')': goto yy17;
		case ',': goto yy18;
		case '.': goto yy19;
		case '/': goto yy20;
		case '0' ... '9': goto yy21;
		case ';': goto yy23;
		case '<' ... '>': goto yy24;
		case '[': goto yy26;
		case ']': goto yy27;
		case 0xC2 ... 0xDF: goto yy29;
		case 0xE0: goto yy30;
		case 0xE1 ... 0xEF: goto yy31;
		case 0xF0: goto yy32;
		case 0xF1 ... 0xF3: goto yy33;
		case 0xF4: goto yy34;
		default: goto yy28;
	}
yy1:
	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00:
		case 0x01 ... 0x08:
		case '\v' ... 0x1F:
		case '!':
		case '$':
		case '0' ... ':':
		case '?' ... 'Z':
		case '\\':
		case '^' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy2;
			goto yy1;
		case 0xC2 ... 0xDF: goto yy35;
		case 0xE0: goto yy37;
		case 0xE1 ... 0xEF: goto yy38;
		case 0xF0: goto yy39;
		case 0xF1 ... 0xF3: goto yy40;
		case 0xF4: goto yy41;
		default: goto yy2;
	}
yy2:
	{ *end_pos = YYCURSOR; return create_token(ID, cur_line, *start_pos, *end_pos); }
yy3:
	yych = *++YYCURSOR;
	switch (yych) {
		case '\t': goto yy3;
		default: goto yy4;
	}
yy4:
	{ *end_pos = YYCURSOR; return create_token(TAB, cur_line, *start_pos, *end_pos); }
yy5:
	yych = *++YYCURSOR;
	switch (yych) {
		case '\n': goto yy5;
		default: goto yy6;
	}
yy6:
	{ cur_line += YYCURSOR - *start_pos ; *start_pos = YYCURSOR; goto loop; }
yy7:
	yych = *++YYCURSOR;
	switch (yych) {
		case ' ': goto yy7;
		default: goto yy8;
	}
yy8:
	{ *end_pos = YYCURSOR; return create_token(WHITESPACE, cur_line, *start_pos, *end_pos); }
yy9:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00:
			if (YYLIMIT <= YYCURSOR) goto yy10;
			goto yy42;
		case 0x01 ... 0x7F:
		case 0xC2 ... 0xF4: goto yy43;
		default: goto yy10;
	}
yy10:
	{ *end_pos = YYCURSOR; return create_token(ERROR, cur_line, *start_pos, *end_pos); }
yy11:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00:
			if (YYLIMIT <= YYCURSOR) goto yy10;
			goto yy52;
		case 0x01 ... '\t':
		case '\v' ... 0x7F:
		case 0xC2 ... 0xF4: goto yy53;
		default: goto yy10;
	}
yy12:
	++YYCURSOR;
yy13:
	{ *end_pos = YYCURSOR; return create_token(MATH, cur_line, *start_pos, *end_pos); }
yy14:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00:
			if (YYLIMIT <= YYCURSOR) goto yy10;
			goto yy61;
		case 0x01 ... '\t':
		case '\v' ... 0x7F:
		case 0xC2 ... 0xF4: goto yy62;
		default: goto yy10;
	}
yy15:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00:
			if (YYLIMIT <= YYCURSOR) goto yy10;
			goto yy70;
		case 0x01 ... '&':
		case '(' ... 0x7F:
		case 0xC2 ... 0xF4: goto yy71;
		default: goto yy10;
	}
yy16:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return create_token(LEFT_PARENTHESIS, cur_line, *start_pos, *end_pos); }
yy17:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return create_token(RIGHT_PARENTHESIS, cur_line, *start_pos, *end_pos); }
yy18:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return create_token(COMMA, cur_line, *start_pos, *end_pos); }
yy19:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return create_token(POINT, cur_line, *start_pos, *end_pos); }
yy20:
	yych = *++YYCURSOR;
	switch (yych) {
		case '/': goto yy78;
		default: goto yy13;
	}
yy21:
	yyaccept = 2;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00:
		case 0x01 ... 0x08:
		case '\v' ... 0x1F:
		case '!':
		case '$':
		case ':':
		case '?' ... 'Z':
		case '\\':
		case '^' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy22;
			goto yy1;
		case '.': goto yy80;
		case '0' ... '9': goto yy21;
		case 0xC2 ... 0xDF: goto yy35;
		case 0xE0: goto yy37;
		case 0xE1 ... 0xEF: goto yy38;
		case 0xF0: goto yy39;
		case 0xF1 ... 0xF3: goto yy40;
		case 0xF4: goto yy41;
		default: goto yy22;
	}
yy22:
	{ *end_pos = YYCURSOR; return create_token(NUMBER, cur_line, *start_pos, *end_pos); }
yy23:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return create_token(SEMICOLON, cur_line, *start_pos, *end_pos); }
yy24:
	yych = *++YYCURSOR;
	switch (yych) {
		case '<' ... '>': goto yy24;
		default: goto yy25;
	}
yy25:
	{ *end_pos = YYCURSOR; return create_token(RELOP, cur_line, *start_pos, *end_pos); }
yy26:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return create_token(LEFT_SQUARE_BRACKET, cur_line, *start_pos, *end_pos); }
yy27:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return create_token(RIGHT_SQUARE_BRACKET, cur_line, *start_pos, *end_pos); }
yy28:
	++YYCURSOR;
	goto yy10;
yy29:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy1;
		default: goto yy10;
	}
yy30:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy35;
		default: goto yy10;
	}
yy31:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x80 ... 0xBF: goto yy35;
		default: goto yy10;
	}
yy32:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x90 ... 0xBF: goto yy38;
		default: goto yy10;
	}
yy33:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x80 ... 0xBF: goto yy38;
		default: goto yy10;
	}
yy34:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x80 ... 0x8F: goto yy38;
		default: goto yy10;
	}
yy35:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy1;
		default: goto yy36;
	}
yy36:
	YYCURSOR = YYMARKER;
	switch (yyaccept) {
		case 0: goto yy2;
		case 1: goto yy10;
		case 2: goto yy22;
		case 3: goto yy45;
		case 4: goto yy54;
		case 5: goto yy63;
		default: goto yy79;
	}
yy37:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy35;
		default: goto yy36;
	}
yy38:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy35;
		default: goto yy36;
	}
yy39:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy38;
		default: goto yy36;
	}
yy40:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy38;
		default: goto yy36;
	}
yy41:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy38;
		default: goto yy36;
	}
yy42:
	yych = *++YYCURSOR;
yy43:
	switch (yych) {
		case 0x00:
		case 0x01 ... '!':
		case '#' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy36;
			goto yy42;
		case '"': goto yy44;
		case 0xC2 ... 0xDF: goto yy46;
		case 0xE0: goto yy47;
		case 0xE1 ... 0xEF: goto yy48;
		case 0xF0: goto yy49;
		case 0xF1 ... 0xF3: goto yy50;
		case 0xF4: goto yy51;
		default: goto yy36;
	}
yy44:
	yyaccept = 3;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case '"': goto yy42;
		default: goto yy45;
	}
yy45:
	{ *end_pos = YYCURSOR; return create_token(LITERAL, cur_line, *start_pos, *end_pos); }
yy46:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy42;
		default: goto yy36;
	}
yy47:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy46;
		default: goto yy36;
	}
yy48:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy46;
		default: goto yy36;
	}
yy49:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy48;
		default: goto yy36;
	}
yy50:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy48;
		default: goto yy36;
	}
yy51:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy48;
		default: goto yy36;
	}
yy52:
	yyaccept = 4;
	yych = *(YYMARKER = ++YYCURSOR);
yy53:
	switch (yych) {
		case 0x00:
		case 0x01 ... '\t':
		case '\v' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy54;
			goto yy52;
		case 0xC2 ... 0xDF: goto yy55;
		case 0xE0: goto yy56;
		case 0xE1 ... 0xEF: goto yy57;
		case 0xF0: goto yy58;
		case 0xF1 ... 0xF3: goto yy59;
		case 0xF4: goto yy60;
		default: goto yy54;
	}
yy54:
	{ *end_pos = YYCURSOR; return create_token(AREA, cur_line, *start_pos, *end_pos); }
yy55:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy52;
		default: goto yy36;
	}
yy56:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy55;
		default: goto yy36;
	}
yy57:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy55;
		default: goto yy36;
	}
yy58:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy57;
		default: goto yy36;
	}
yy59:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy57;
		default: goto yy36;
	}
yy60:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy57;
		default: goto yy36;
	}
yy61:
	yyaccept = 5;
	yych = *(YYMARKER = ++YYCURSOR);
yy62:
	switch (yych) {
		case 0x00:
		case 0x01 ... '\t':
		case '\v' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy63;
			goto yy61;
		case 0xC2 ... 0xDF: goto yy64;
		case 0xE0: goto yy65;
		case 0xE1 ... 0xEF: goto yy66;
		case 0xF0: goto yy67;
		case 0xF1 ... 0xF3: goto yy68;
		case 0xF4: goto yy69;
		default: goto yy63;
	}
yy63:
	{ *end_pos = YYCURSOR; return create_token(PREPROCESSOR, cur_line, *start_pos, *end_pos); }
yy64:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy61;
		default: goto yy36;
	}
yy65:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy64;
		default: goto yy36;
	}
yy66:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy64;
		default: goto yy36;
	}
yy67:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy66;
		default: goto yy36;
	}
yy68:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy66;
		default: goto yy36;
	}
yy69:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy66;
		default: goto yy36;
	}
yy70:
	yych = *++YYCURSOR;
yy71:
	switch (yych) {
		case 0x00:
		case 0x01 ... '&':
		case '(' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy36;
			goto yy70;
		case '\'': goto yy81;
		case 0xC2 ... 0xDF: goto yy72;
		case 0xE0: goto yy73;
		case 0xE1 ... 0xEF: goto yy74;
		case 0xF0: goto yy75;
		case 0xF1 ... 0xF3: goto yy76;
		case 0xF4: goto yy77;
		default: goto yy36;
	}
yy72:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy70;
		default: goto yy36;
	}
yy73:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy72;
		default: goto yy36;
	}
yy74:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy72;
		default: goto yy36;
	}
yy75:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy74;
		default: goto yy36;
	}
yy76:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy74;
		default: goto yy36;
	}
yy77:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy74;
		default: goto yy36;
	}
yy78:
	yyaccept = 6;
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case 0x00:
		case 0x01 ... '\t':
		case '\v' ... 0x7F:
			if (YYLIMIT <= YYCURSOR) goto yy79;
			goto yy78;
		case 0xC2 ... 0xDF: goto yy82;
		case 0xE0: goto yy83;
		case 0xE1 ... 0xEF: goto yy84;
		case 0xF0: goto yy85;
		case 0xF1 ... 0xF3: goto yy86;
		case 0xF4: goto yy87;
		default: goto yy79;
	}
yy79:
	{ *start_pos = YYCURSOR; goto loop; }
yy80:
	yych = *++YYCURSOR;
	switch (yych) {
		case '0' ... '9': goto yy88;
		default: goto yy36;
	}
yy81:
	++YYCURSOR;
	{ *end_pos = YYCURSOR; return create_token(DATE, cur_line, *start_pos, *end_pos); }
yy82:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy78;
		default: goto yy36;
	}
yy83:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0xA0 ... 0xBF: goto yy82;
		default: goto yy36;
	}
yy84:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy82;
		default: goto yy36;
	}
yy85:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x90 ... 0xBF: goto yy84;
		default: goto yy36;
	}
yy86:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0xBF: goto yy84;
		default: goto yy36;
	}
yy87:
	yych = *++YYCURSOR;
	switch (yych) {
		case 0x80 ... 0x8F: goto yy84;
		default: goto yy36;
	}
yy88:
	yych = *++YYCURSOR;
	switch (yych) {
		case '0' ... '9': goto yy88;
		default: goto yy22;
	}
yy89:
	{ *end_pos = YYCURSOR; return create_token(END, cur_line, *start_pos, *end_pos); }
}

#pragma GCC diagnostic pop
}

int
main (int argc, char **argv)
{

  if (argc != 3)
    print_error ("%s",
		 "Правильный формат:./a.out ./test/test01 ./log");

  struct stat statbuf;
  int fd, fd_log;

  if ((fd = open (argv[1], O_RDONLY)) < 0)
    print_error ("невозможно открыть %s для чтения",
		 argv[1]);

  if ((fd_log =
       open (argv[2], O_WRONLY | O_CREAT | O_APPEND, S_IWUSR | S_IRUSR)) < 0)
    print_error ("невозможно открыть %s для записи",
		 argv[2]);

  if (fstat (fd, &statbuf) < 0)
    print_error
      ("Ошибка вызова функции fstat:%s у файла %s\n",
       strerror (errno), argv[1]);

  void *src;
  if ((src =
       mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
    print_error ("%s\n",
		 "Ошибка вызова функции mmap для входного файла");

  const char *limit = (const char *) src + statbuf.st_size;
  const char *start_pos = (const char *) src;
  const char *end_pos = NULL;
  token *tok = NULL;
  size_t line = 1;

  init_lex ();
  do
    {
      tok = lex (&start_pos, &end_pos, limit, line);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
      dprintf (fd_log, "%ld:%s\nres:%d\n", tok->line, tok->text, tok->tag);
#pragma GCC diagnostic pop
      start_pos = end_pos;
      line = tok->line;
    }
  while (start_pos != limit);
  munmap (src, statbuf.st_size);
  destroy_lex ();
  return EXIT_SUCCESS;
}

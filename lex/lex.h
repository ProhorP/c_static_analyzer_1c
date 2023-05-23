#define BUFFSIZE 409600

enum TAG
{
  ERROR				//0
    , END			//1
    , TEST			//2
    , POINT			//3
    , COMMA			//4
    , COLON			//5
    , SEMICOLON			//6
    , LEFT_PARENTHESIS		//7
    , RIGHT_PARENTHESIS		//8
    , LEFT_SQUARE_BRACKET	//9
    , RIGHT_SQUARE_BRACKET	//10
    , WHITESPACE		//11
    , TAB			//12
    , NEWLINE			//13
//IF
    , IF			//14
    , THEN			//15
    , ELSIF			//16
    , ELSE			//17
    , ENDIF			//18
//DO
    , FOR			//19
    , EACH			//20
    , IN			//21
    , TO			//22
    , WHILE			//23
    , DO			//24
    , BREAK			//25
    , CONTINUE			//26
    , ENDDO			//27
//FUNCTION
    , PROCEDURE			//28
    , FUNCTION			//29
    , ENDPROCEDURE		//30
    , ENDFUNCTION		//31
    , VAR			//32
    , GOTO			//33
    , RETURN			//34
    , AND			//35
    , OR			//36
    , NOT			//37
    , TRY			//38
    , EXCEPT			//39
    , RAISE			//40
    , ENDTRY			//41
    , NEW			//42
    , EXECUTE			//43
    , ADDHANDLER		//44
    , REMOVEHANDLER		//45
//type
    , T_NULL			//46
    , T_TRUE			//47
    , T_FALSE			//48
    , T_UNDEFINED		//49
    , EXPORT			//50
    , VAL			//51
//DIRECTIVE
    , ATCLIENT			//52
    , ATSERVER			//53
    , ATSERVERNOCONTEXT		//54
    , ATCLIENTATSERVERNOCONTEXT	//55
    , ATCLIENTATSERVER		//56
//OTHER
    , RELOP			//
    , MATH			//
    , NUMBER			//
    , DATE			//
    , LITERAL			//
    , PREPROCESSOR		//
    , DIRECTIVE			//
    , MARK			//
    , ID			//
};

enum TAG_ATTR
{
  LT				//0
    , LE			//1
    , EQ			//2
    , NE			//3
    , GT			//4
    , GE			//5
    , PLUS			//6
    , MINUS			//7
    , MULTIPLY			//8
    , DIVISION			//9
    , REMAINDER_OF_DIVISION	//10
};

typedef struct
{
  enum TAG tag;
} token;

typedef struct
{
  token base;
  enum TAG_ATTR attr;
} token_attr;

typedef struct
{
  token base;
  char *text;
} token_table;

void init_lex (char *file_name);

void destroy_lex (void);

token *get_token (void);

void print_token (token * tok, int fd_log);

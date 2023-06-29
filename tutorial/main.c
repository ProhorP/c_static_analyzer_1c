//заголовочные файлы:
//1.системные(что в треугольных скобках)
//2.ручные(что в кавычках)
#include "main.h"
#include "syn.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//для проверки разыменования я использую указатели,
//которые будут указывать на строки, заранее определенные
//choose_string указана как NULL, потому что она должна быть в хэш-таблице,
//при уничтожении которой память должна быть освобождена, поэтому мы будет динамически
//выделять строку
char * choose_string = NULL;//value free at destroy symbol_table
//если в dynamic_val_table по ключу(имени переменной) будет найдено это поле,
//значит при разыменовании этого поля будем выводить ошибку
char * field_query = "Поле запроса";

//это 3 хэш-таблицы, у каждой свое назначение:
//1.symbol_table - хранит строки лексем(значимые куски текста из которых состоит код), дополняется во время выполнения
//2.token_table - хранит токены(перечисления по 1С терминологии), заполняется предопределенными значениями и не редактируется
//3.dynamic_val_table - хранит присваиваемые значения в текстовом формате, например:
//П = Новый Запрос;
//в dynamic_val_table в ключ попадет П, а его значение будет Запрос
//Это нужно для финальной стадии, когда будет определяться разыменовывается ли поле из выборки запроса
GHashTable *symbol_table = NULL, *token_table = NULL,
           *dynamic_val_table = NULL;
//выделается место для строк, которые будут сохраняться далее в symbol_table
char buf_str[BUFFSIZE] = { 0 };

//это файловый дескриптов(номер открытоого файла), его будем считывать
int module_fd = 0;
//выделение памяти под служебную информацию
//она будет использоваться для определения длины сканируемого файла
struct stat statbuf = { 0 };

//указатель на открываемый файл
void *src = NULL;
//хранимая в памяти стартовая позиция, const обозначает, что нельзя именять сами данные
//на которые указывает этот указатель
const char *start_pos = NULL;
//сдесь хранится позиция до которой досканировал лексический анализатор
//он сканирует начиная с start_pos
const char *end_pos = NULL;
//указатель на первый байт, после конца файла
//используется лексическим анализатором для проверки окончания файла
const char *limit = NULL;
//счетчик строк сканируемого файла
size_t line = 1;

//функция вызывается лексическим анализатором, если последовательность символов
//соответствет токенам:
//1.идентификатору(ID)
//2.строке(LITERAL)
//3.дате(ADTE)
//4.числу(NUMBER)
//5.директиве компиляции(DIRECTIVE)(все что начинается с &)
//6.препроцессору(PREPROCESSOR)(все что начинается с #)
//7.Метка(MARK)
int
get_token (int pre_token, YYSTYPE *lvalp, YYLTYPE *llocp)
{
//это значение токена
//Например если мы сканируем переменную "Яблоко",
//то токен будет ID, а значение будет "Яблоко"
//Но если мы сканируем слово "Цикл", то токен будет "DO"
//а значения не будет
  char *token_val = NULL;
//длина осканированного участка файла, который соотвествует 
  size_t n = end_pos - start_pos;
//строки будет сравнивать в верхнем регистре, поэтому выделяем пож этот указатель память
  char *lexeme_upper = NULL;
//итоговое значение токена может измениться в ходе этой функции:
//например если мы отсканируем слово "Цикл"
  int token = pre_token;

//assert - это формирование ошибки и прекращение работы программы если
//передаваемое ей условие не выполняется
  assert (n < BUFFSIZE);

  if (token == NUMBER || token == DATE || token == LITERAL)
    {
//для числа,даты и строки нет смысла приводить к верхнему регистру для сравнения
//поэтому мы комируем данные в буфер, вставляем в конец копируемых символов символ конца строки '\0'
//и делаем поиск такого же значения в хэш-таблице symbol_table
      memcpy (buf_str, start_pos, n);
      buf_str[n] = '\0';
      token_val = g_hash_table_lookup (symbol_table, buf_str);
    }
  else
    /*if (token == PREPROCESSOR || token == DIRECTIVE || token == ID || token
     * == MARK)*/
    {
//в остальных случаях на нужно привести к верхнему регистру
//эта функция выделает динамически память, ее потом нужно освободить
      lexeme_upper = g_utf8_strup (start_pos, n);

      if (token == ID || token == DIRECTIVE)
        token_val = g_hash_table_lookup (token_table, lexeme_upper);
//и сначала поискать в таблице токенов 
      if (token_val)
        {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
//и если там что-то нашлось то переназначить токен(пример с яблоком)
          token = AS_TOKEN (token_val);
#pragma GCC diagnostic pop
        }
      else
        {
//если не нашлось и это начиналось с &, значит это непредусмотрая директива компиляции
//и возвращаем неопределенный токен
          if (token == DIRECTIVE)
            token = YYUNDEF;
          else
//ищем в таблице символов
            token_val = g_hash_table_lookup (symbol_table, lexeme_upper);
        }

      if (token_val != NULL)
        {
//если мы нашли такое же значение в хэш-таблице, тогда
//освобождаем память под выделенную строку верхнего регистра
          g_free (lexeme_upper);
          lexeme_upper = NULL;
        }
    }

//если это у нас строка, значит пробежимся по ней и дополнительно посчитаем количество переносов строк
//переносы строк, которые вне тел строк считаются лексическим нализатором
  if (token == LITERAL)
    {
      const char *ptr_literal = start_pos;
      while (ptr_literal != end_pos)
        if (*ptr_literal++ == '\n')
          line++;
    }

//если мы не нашли ничего в symbol_table, значит запишем лексему туда
  if (token_val == NULL)
    {
//выделяем память под длину строки + 1 символ окончания строки
      token_val = (char *)calloc (1, n + 1);
//аварийно завершаем программу, если выделение памяти не удалось
      assert(token_val != NULL);

      if (token_val)
        {
//побайтно копируем лексему в выделенную динамически область
          memcpy (token_val, start_pos, n);
          if (lexeme_upper)
//если мы для сравнения используем верхний регистр, то заносим в таблицу ключ в верхнем регистре
//а значение как реальную лексему
            g_hash_table_insert (symbol_table, lexeme_upper, token_val);
          else
//иначе для ключа динамически выделяем память под ключ и копируем туда строку из буфера(строка, число,дата)
            g_hash_table_insert (symbol_table, g_strdup (buf_str), token_val);
        }
    }

//для токенов, которые могут иметь значения - сохраняем их
//в параметр, передаваемый синтаксическим анализатором
  if (token == NUMBER || token == DATE || token == LITERAL
      || token == PREPROCESSOR || token == ID || token == MARK)
    *lvalp = token_val;
//записываем номер строки, который мог измениться (у строки например, которая может начаться на одной строке
//а закончится на другой)
  llocp->last_line = line;

  return token;
}

//Отключаем проверку конвертации значений компилятора,
//т.к. мы храним за место указателей токены(они меньше памяти занимают,
//что допустимо для такого хранения, но не соответствуют идентичному размеру)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-conversion"
void
fill_token_table ()
{
  // IF
  g_hash_table_insert (token_table, "ЕСЛИ", AS_PTR (IF));
  g_hash_table_insert (token_table, "IF", AS_PTR (IF));
  g_hash_table_insert (token_table, "ТОГДА", AS_PTR (THEN));
  g_hash_table_insert (token_table, "THEN", AS_PTR (THEN));
  g_hash_table_insert (token_table, "ИНАЧЕЕСЛИ", AS_PTR (ELSIF));
  g_hash_table_insert (token_table, "ELSIF", AS_PTR (ELSIF));
  g_hash_table_insert (token_table, "ИНАЧЕ", AS_PTR (ELSE));
  g_hash_table_insert (token_table, "ELSE", AS_PTR (ELSE));
  g_hash_table_insert (token_table, "КОНЕЦЕСЛИ", AS_PTR (ENDIF));
  g_hash_table_insert (token_table, "ENDIF", AS_PTR (ENDIF));

  // DO
  g_hash_table_insert (token_table, "ДЛЯ", AS_PTR (FOR));
  g_hash_table_insert (token_table, "FOR", AS_PTR (FOR));
  g_hash_table_insert (token_table, "КАЖДОГО", AS_PTR (EACH));
  g_hash_table_insert (token_table, "EACH", AS_PTR (EACH));
  g_hash_table_insert (token_table, "ИЗ", AS_PTR (IN));
  g_hash_table_insert (token_table, "IN", AS_PTR (IN));
  g_hash_table_insert (token_table, "ПО", AS_PTR (TO));
  g_hash_table_insert (token_table, "TO", AS_PTR (TO));
  g_hash_table_insert (token_table, "ПОКА", AS_PTR (WHILE));
  g_hash_table_insert (token_table, "WHILE", AS_PTR (WHILE));
  g_hash_table_insert (token_table, "ЦИКЛ", AS_PTR (DO));
  g_hash_table_insert (token_table, "DO", AS_PTR (DO));
  g_hash_table_insert (token_table, "ПРЕРВАТЬ", AS_PTR (BREAK));
  g_hash_table_insert (token_table, "BREAK", AS_PTR (BREAK));
  g_hash_table_insert (token_table, "ПРОДОЛЖИТЬ", AS_PTR (CONTINUE));
  g_hash_table_insert (token_table, "CONTINUE", AS_PTR (CONTINUE));
  g_hash_table_insert (token_table, "КОНЕЦЦИКЛА", AS_PTR (ENDDO));
  g_hash_table_insert (token_table, "ENDDO", AS_PTR (ENDDO));

  // FUNCTION
  g_hash_table_insert (token_table, "ПРОЦЕДУРА", AS_PTR (PROCEDURE));
  g_hash_table_insert (token_table, "PROCEDURE", AS_PTR (PROCEDURE));
  g_hash_table_insert (token_table, "ФУНКЦИЯ", AS_PTR (FUNCTION));
  g_hash_table_insert (token_table, "FUNCTION", AS_PTR (FUNCTION));
  g_hash_table_insert (token_table, "КОНЕЦПРОЦЕДУРЫ",
                       AS_PTR (ENDPROCEDURE));
  g_hash_table_insert (token_table, "ENDPROCEDURE",
                       AS_PTR (ENDPROCEDURE));
  g_hash_table_insert (token_table, "КОНЕЦФУНКЦИИ",
                       AS_PTR (ENDFUNCTION));
  g_hash_table_insert (token_table, "ENDFUNCTION",
                       AS_PTR (ENDFUNCTION));
  g_hash_table_insert (token_table, "ПЕРЕМ", AS_PTR (VAR));
  g_hash_table_insert (token_table, "VAR", AS_PTR (VAR));
  g_hash_table_insert (token_table, "ПЕРЕЙТИ", AS_PTR (GOTO));
  g_hash_table_insert (token_table, "GOTO", AS_PTR (GOTO));
  g_hash_table_insert (token_table, "ВОЗВРАТ", AS_PTR (RETURN));
  g_hash_table_insert (token_table, "RETURN", AS_PTR (RETURN));
  g_hash_table_insert (token_table, "И", AS_PTR (AND));
  g_hash_table_insert (token_table, "AND", AS_PTR (AND));
  g_hash_table_insert (token_table, "ИЛИ", AS_PTR (OR));
  g_hash_table_insert (token_table, "OR", AS_PTR (OR));
  g_hash_table_insert (token_table, "НЕ", AS_PTR (NOT));
  g_hash_table_insert (token_table, "NOT", AS_PTR (NOT));
  g_hash_table_insert (token_table, "ПОПЫТКА", AS_PTR (TRY));
  g_hash_table_insert (token_table, "TRY", AS_PTR (TRY));
  g_hash_table_insert (token_table, "ИСКЛЮЧЕНИЕ", AS_PTR (EXCEPT));
  g_hash_table_insert (token_table, "EXCEPT", AS_PTR (EXCEPT));
  g_hash_table_insert (token_table, "ВЫЗВАТЬИСКЛЮЧЕНИЕ",
                       AS_PTR (RAISE));
  g_hash_table_insert (token_table, "RAISE", AS_PTR (RAISE));
  g_hash_table_insert (token_table, "КОНЕЦПОПЫТКИ", AS_PTR (ENDTRY));
  g_hash_table_insert (token_table, "ENDTRY", AS_PTR (ENDTRY));
  g_hash_table_insert (token_table, "НОВЫЙ", AS_PTR (NEW));
  g_hash_table_insert (token_table, "NEW", AS_PTR (NEW));
  g_hash_table_insert (token_table, "ВЫПОЛНИТЬ", AS_PTR (EXECUTE));
  g_hash_table_insert (token_table, "EXECUTE", AS_PTR (EXECUTE));
  g_hash_table_insert (token_table, "ДОБАВИТЬОБРАБОТЧИК",
                       AS_PTR (ADDHANDLER));
  g_hash_table_insert (token_table, "ADDHANDLER",
                       AS_PTR (ADDHANDLER));
  g_hash_table_insert (token_table, "УДАЛИТЬОБРАБОТЧИК",
                       AS_PTR (REMOVEHANDLER));
  g_hash_table_insert (token_table, "REMOVEHANDLER",
                       AS_PTR (REMOVEHANDLER));
  // TYPE
  g_hash_table_insert (token_table, "NULL", AS_PTR (T_NULL));
  g_hash_table_insert (token_table, "ИСТИНА", AS_PTR (T_TRUE));
  g_hash_table_insert (token_table, "TRUE", AS_PTR (T_TRUE));
  g_hash_table_insert (token_table, "ЛОЖЬ", AS_PTR (T_FALSE));
  g_hash_table_insert (token_table, "FALSE", AS_PTR (T_FALSE));
  g_hash_table_insert (token_table, "НЕОПРЕДЕЛЕНО",
                       AS_PTR (T_UNDEFINED));
  g_hash_table_insert (token_table, "UNDEFINED",
                       AS_PTR (T_UNDEFINED));
  g_hash_table_insert (token_table, "ЭКСПОРТ", AS_PTR (EXPORT));
  g_hash_table_insert (token_table, "EXPORT", AS_PTR (EXPORT));
  g_hash_table_insert (token_table, "ЗНАЧ", AS_PTR (VAL));
  g_hash_table_insert (token_table, "VAL", AS_PTR (VAL));

  // DIRECTIVE
  g_hash_table_insert (token_table, "&НАКЛИЕНТЕ", AS_PTR (ATCLIENT));
  g_hash_table_insert (token_table, "&ATCLIENT", AS_PTR (ATCLIENT));
  g_hash_table_insert (token_table, "&НАСЕРВЕРЕ", AS_PTR (ATSERVER));
  g_hash_table_insert (token_table, "&ATSERVER", AS_PTR (ATSERVER));
  g_hash_table_insert (token_table, "&НАСЕРВЕРЕБЕЗКОНТЕКСТА",
                       AS_PTR (ATSERVERNOCONTEXT));
  g_hash_table_insert (token_table, "&ATSERVERNOCONTEXT",
                       AS_PTR (ATSERVERNOCONTEXT));
  g_hash_table_insert (token_table, "&НАКЛИЕНТЕНАСЕРВЕРЕБЕЗКОНТЕКСТА",
                       AS_PTR (ATCLIENTATSERVERNOCONTEXT));
  g_hash_table_insert (token_table, "&ATCLIENTATSERVERNOCONTEXT",
                       AS_PTR (ATCLIENTATSERVERNOCONTEXT));
  g_hash_table_insert (token_table, "&НАКЛИЕНТЕНАСЕРВЕРЕ",
                       AS_PTR (ATCLIENTATSERVER));
  g_hash_table_insert (token_table, "&ATCLIENTATSERVER",
                       AS_PTR (ATCLIENTATSERVER));
}
#pragma GCC diagnostic pop

//заполняем предварительно отслеживаемыми лексемами
//эти лексемы сохраняются в глобальные указатели
//и далее идет сравнение напрямую с ними
void
fill_symbol_table ()
{
//выделили длину строки + символ конца строки
  choose_string = malloc (strlen ("ВЫБРАТЬ") + 1);
//ругнулись, если выделение не произошло(ОС не дала памяти)
  assert (choose_string != NULL);
//заполнили строкой
  strcpy (choose_string, "ВЫБРАТЬ");

  // QUERY
  g_hash_table_insert (symbol_table, g_strdup(choose_string), choose_string);
}

//обязательная функция инициализации
//так же откры
void
init_lex (char *file_name)
{
//открытие файла с кодом 1С
  module_fd = open (file_name, O_RDONLY);
//ругалка в случае неуспешного открытия
  assert(module_fd > 0);

//получаем служебную инфу(нужен размер файла из нее)
//и ругаемся если что-то пошло не так(статус отличный от 0)
  assert(fstat (module_fd, &statbuf) == 0);

//делаем отображение файла в память, это системный вызов, который
//позволяет работать с файлом, как буд-то он находится в ОЗУ
  src = mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, module_fd, 0);
//ругаемся если что-то пошло не так
  assert(src != MAP_FAILED);

//указываем адрес первого байта после конца файла(недоступная зона)
  limit = (const char *)src + statbuf.st_size;
//указываем адрес первого байта
  start_pos = (const char *)src;
  //т.к. 1С выгружает модули в файлы bsl с кодировкой UTF-8, где первые 3 байта являются служебными
  //то мы переключаем начало байла, проматывая 3 байта
  if (start_pos[0] == (char)0xEF && start_pos[1] == (char)0xBB
      && start_pos[2] == (char)0xBF)
    start_pos += 3;
//указываем конец позиции как NULL
//он изменяется лексическим анализатором по мере продвижения по тексту модуля
  end_pos = NULL;
//предварительные создания хэш-таблиц
//g_hash_table_new_full создает таблицу с завпоминаним методов по очистке памяти для
//ее компонентов
  symbol_table
      = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
//g_hash_table_new просто создает таблицу, но при уничтожении она не вызывает методы очистки компонентов
  token_table = g_hash_table_new (g_str_hash, g_str_equal);
  dynamic_val_table = g_hash_table_new (g_str_hash, g_str_equal);
//наполнение этих таблиц
  fill_symbol_table ();
  fill_token_table ();
}

//обязательная функция очистки памяти
void
destroy_lex ()
{
//открепляем файл от ОЗУ
  munmap (src, statbuf.st_size);
//уничтожаем хэш-таблицы
  g_hash_table_destroy (symbol_table);
  g_hash_table_destroy (token_table);
  g_hash_table_destroy (dynamic_val_table);
//тк мы очистили таблицы, то к ним обращаться больше нельзя
  //поэтому удаляем указатели на них
  symbol_table = NULL;
  token_table = NULL;
  dynamic_val_table = NULL;
  //устанавливаем значения по умолчанию
  line = 1;
}

//процедура вставки значений(ранее писал про сохранение признака Запрос в переменной)
void
insert_dynamic_val_table (char *key, char *value)
{
  //вывод отладочной информации при запуске 'make debug'
  PRINT ("key:%s; value=%s\n", key, value);
  //сама вставка
  g_hash_table_insert (dynamic_val_table, key, value);
}

//наша функци проверки разыменования поля из выборки запроса
//наполнение таблицы происходит при операции присвоения в грамматике syn.y
//функией insert_dynamic_val_table
char *
check_dereference (char *obj, char *field, size_t field_line)
{
//сначала получаем по разыменовываемому элементу его значение
  char *val = g_hash_table_lookup (dynamic_val_table, obj);
  if (val == choose_string)
    //если это значение является выборкой
    //то значит возвращать будем признак поля запроса
    val = field_query;
  else
    {
      if (val == field_query)
        //если разыменование поле содержит значение поля запроса
        //тогда получается мы делаем разыменование(автоматический запрос в БД)
        printf ("Ошибка: разыменование ссылки %s полем %s в строке %ld\n", obj,
                field, field_line);
      //в любом случае возвращаем то поле, к которому обращаются через точку
      val = field;
    }
  return val;
}

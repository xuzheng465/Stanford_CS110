/**
 * This file describes the tokenization rules used by the lexer to provide
 * input to the grammar in commands.y. There are 2 types of tokens returned:
 * 
 *  WORD: words are any string of characters not containing whitespace, or a
 *        string that is enclosed in double quotes which can contain whitespace.
 *
 *  TOKEN: Tokens are used for the 3 special characters '<', '>', and '|' used
 *         to describe i/o redirection.
 *
 *
 *  FLEX will tokenize the input string according to these rules, and where
 *  more than one rule is matched by the input it will choose the rule that
 *  results in a longer token.
 *
 *  This file is compiled to lexer.c by flex in the Makefile provided.
 *
 *  For more information on using flex to lex and tokenize input, take CS143
 */

%{
#include "stsh-parse.h"
#include "scanner.h"
#include "parser.h"
#include <cstdio>

%}

%%

[\t\n\r ]*         { /* ignore whitespace */ }
\<                 { return yylval.token = LT; }
\>                 { return yylval.token = GT; }
\|                 { return yylval.token = PIPE; }
&                  { return yylval.token = AMPERSAND;}
[^\t\n\r ]*        { yylval.word = strdup(yytext); return WORD; }
\"(\\.|[^\"])*\"   { yylval.word = strdup(yytext); return WORD; }

%%

static bool initialized = initScanner();
  
bool initScanner() {
  yy_flex_debug = false;
  return true;
}

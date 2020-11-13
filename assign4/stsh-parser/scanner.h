/**
 * File: scanner.h
 * ---------------
 * Short header file that defines a few types, constants, and helper
 * functions needed by the flex-generated scanner.
 */

#ifndef _scanner_h_
#define _scanner_h_

extern char *yytext;
int yylex();
bool initScanner();

#endif

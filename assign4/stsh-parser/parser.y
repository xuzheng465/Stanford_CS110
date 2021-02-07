/**
 * This file describes the context free grammar used to parse pipelined
 * lists of commands in the yyparse call found in parseCommandLine.
 * For details on the grammar rules, see the description in stsh-parse.h
 *
 * For more information on using bison to semantically parse input, take CS143
 */

%defines "parser.h"

%{
#include <vector>
#include "stsh-parse.h"
   
#include <cstring>     // for strncpy
#include <iostream>    // for cout, endl
   
extern int yylex();
void yyerror(pipeline& finalPipeLine, const char *s) { std::cerr << "ERROR: " << s << std::endl; }
%}

%parse-param {pipeline &finalPipeLine}

%union {
  struct pipeline *pipeline;
  struct command cmd;
  char *word;
  std::vector<command> *cmd_list;
  std::vector<char *> *arg_list;
  int token;
  bool background;
}

%token <word> WORD
%token <token> LT GT PIPE
%token <background> AMPERSAND

%type <pipeline> input in_out_cmd
%type <cmd_list> cmd_list
%type <word> in_redir out_redir
%type <cmd> cmd in_cmd out_cmd
%type <arg_list> arg_list
%type <background> background

%start input

%%


input:     /* empty */                            {  /* empty input, don't modify finalPipeLine */ }
          |  in_out_cmd background                {  /* work is done in internal nodes */ }
          |  in_cmd PIPE cmd_list out_cmd background {  $$ = &finalPipeLine;
                                                     $$->commands.push_back($1); 
                                                     $$->commands.insert($$->commands.end(), $3->begin(), $3->end()); delete $3;
                                                     $$->commands.push_back($4);
                                                  }
;

background:  /* empty */            { finalPipeLine.background = false; }
          |  background AMPERSAND   { finalPipeLine.background = true; }

cmd_list:    /* empty */            { $$ = new std::vector<command>(); }
          |  cmd_list cmd PIPE      { $$ = $1; $$->push_back($2); }
;

in_cmd:      in_redir cmd           { $$ = $2; /* infile handled in internal node */ }
          |  cmd in_redir           { $$ = $1; /* infile handled in internal node */ }
          |  cmd                    { $$ = $1; }
;

out_cmd:     out_redir cmd          { $$ = $2; /* outfile handled in internal node */ }
          |  cmd out_redir          { $$ = $1; /* outfile handled in internal node */ }
          |  cmd                    { $$ = $1; }
;

in_out_cmd:  in_redir out_redir cmd { finalPipeLine.commands.push_back($3); }
          |  in_redir cmd out_redir { finalPipeLine.commands.push_back($2); }
          |  out_redir in_redir cmd { finalPipeLine.commands.push_back($3); }
          |  out_redir cmd in_redir { finalPipeLine.commands.push_back($2); }
          |  cmd in_redir out_redir { finalPipeLine.commands.push_back($1); }
          |  cmd out_redir in_redir { finalPipeLine.commands.push_back($1); }
          |  in_redir cmd           { finalPipeLine.commands.push_back($2); }
          |  cmd in_redir           { finalPipeLine.commands.push_back($1); }
          |  out_redir cmd          { finalPipeLine.commands.push_back($2); }
          |  cmd out_redir          { finalPipeLine.commands.push_back($1); }
          |  cmd                    { finalPipeLine.commands.push_back($1); }
;

in_redir:    LT WORD                { finalPipeLine.input = std::string($2); free($2);}
;

out_redir:   GT WORD                { finalPipeLine.output = std::string($2); free($2);}
;

cmd:    WORD arg_list               { strncpy($$.command, $1, kMaxCommandLength);
                                      free($1);
                                      $$.command[kMaxCommandLength] = '\0';
                                      size_t i;
                                      for (i = 0; i < std::min((size_t)$2->size(), kMaxArguments); i++) {
                                        $$.tokens[i] = $2->at(i);
                                      }
                                      $$.tokens[i] = NULL; // null terminate the arg list
                                      delete $2;
                                    }
;


arg_list:   /* can be empty */      { $$ = new std::vector<char *>(); }
          | arg_list WORD           { $$ = $1; $$->push_back($2); }
;

%%

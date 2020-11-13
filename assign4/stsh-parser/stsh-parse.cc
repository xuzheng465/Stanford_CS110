/**
 * File: tsh-parse.c
 * -----------------
 * Presents the implementation of parseCommandLine, as documented
 * in tsh-parse.h.  It mostly delegates the process to yyparse, which
 * is generated in lexer.c and parser.h/.c by yacc from the context free grammar
 * specified in commands.y and by flex from the tokenization rules in commands.l
 */

#include "stsh-parse.h"
#include "stsh-parse-exception.h"
#include "scanner.h"
#include "parser.h" // for yyparse
#include <string>
#include <cstdlib>
using namespace std;

typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern int yyparse(pipeline &finalPipeline);
extern YY_BUFFER_STATE yy_scan_string(const char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

pipeline::pipeline(const string& str) {
  YY_BUFFER_STATE state = yy_scan_string(str.c_str());
  int result = yyparse(*this);
  yy_delete_buffer(state);
  if (result != 0) throw STSHParseException();
}

pipeline::~pipeline() {
  input.clear();
  output.clear();
  for (const command& cmd: commands) {
    for (size_t i = 0; i <= kMaxArguments && cmd.tokens[i] != NULL; i++) {
      free(cmd.tokens[i]);
    }
  }
}

ostream& operator<<(ostream& os, const pipeline& p) {
  if (!p.input.empty()) os << "Input File: " << p.input << endl;
  if (!p.output.empty()) os << "Output File: " << p.output << endl;
  for (size_t i = 0; i < p.commands.size(); i++) {
    os << "Executable " << i << ": " << p.commands[i].command << endl;
    for (size_t j = 0; j <= kMaxArguments && p.commands[i].tokens[j] != NULL; j++) {
      os << "       Arg " << j << ": " << p.commands[i].tokens[j] << endl;
    }
  }
  return os;
}

/**
 * File: stsh-parse-test.cc
 * ------------------------
 * Provides a test framework to exercise the pipeline class
 * exported by tsh-parse.[h/cc].
 */

#include <iostream>

#include "stsh-parse.h"
#include "stsh-parse-exception.h"
#include "stsh-readline.h"
using namespace std;

int main(int argc, char *argv[]) {
  rlinit(argc, argv);
  while (true) {
    string line;
    readline(line);
    if (line == "q") break;
    if (line.empty()) continue;
    try {
      pipeline p(line);
      cout << p;
    } catch (STSHParseException& e) {
      cerr << e.what() << endl;
    }
  }
  
  return 0;
}

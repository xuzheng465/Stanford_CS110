/**
 * File: trace-error-constants-test.cc
 * -----------------------------------
 * Unit test to ensure that the functionality exported by the
 * trace-error-constants library is fully functional.
 */
 
#include "trace-error-constants.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
  map<int, string> errorConstants;
  compileSystemCallErrorStrings(errorConstants);
  for (const pair<int, string>& p: errorConstants) {
    cout << "  " << p.first << " -> " << p.second << endl;
  }
  return 0;
}
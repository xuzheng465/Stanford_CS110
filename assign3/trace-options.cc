/**
 * File: trace-options.cc
 * ----------------------
 * Presents the implementation of the one function exported by trace-options.h
 */

#include "trace-options.h"
#include <string>
#include "string-utils.h"
using namespace std;

static const string kSimpleFlag = "--simple";
static const string kRebuildFlag = "--rebuild";
size_t processCommandLineFlags(bool& simple, bool& rebuild, char *argv[]) throw (TraceException) {  
  size_t numFlags = 0;
  for (int i = 1; argv[i] != NULL && startsWith(argv[i], "--"); i++) {
    if (argv[i] == kSimpleFlag) simple = true;
    else if (argv[i] == kRebuildFlag) rebuild = true;
    else throw TraceException(string(argv[0]) + ": Unrecognized flag (" + argv[i] + " )");
    numFlags++;
  }
  
  return numFlags;
}

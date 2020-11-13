/**
 * File: stsh-parse-utils.cc
 * -------------------------
 * Provides the implementation of parseNumber.
 */

#include "stsh-parse-utils.h"
#include "stsh-exception.h"
#include <cstdlib>
using namespace std;

size_t parseNumber(const char *arg, const string& usage) {
  if (arg == NULL) throw STSHException(usage);
  char *end;
  long num = strtol(arg, &end, 10);
  if (*end != '\0' || num < 0) throw STSHException(usage);
  return num;
}

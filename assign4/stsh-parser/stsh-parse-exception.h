/**
 * File: stsh-parse-exception.h
 * ----------------------------
 * Defines and inline-implements a custom stsh-parser exception
 * that gets thrown if there are any problems parsing a string
 * intended to be an stsh pipeline.
 */

#ifndef _stst_parse_exception_
#define _stst_parse_exception_

#include "../stsh-exception.h"
#include <string>

class STSHParseException: public STSHException {
 public:
   STSHParseException() : STSHException("General problem parsing string") {}
   STSHParseException(const std::string& message) : STSHException(message) {}
};

#endif

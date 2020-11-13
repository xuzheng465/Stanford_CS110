/**
 * File: stream-tokenizer.cc
 * -------------------------
 * Provides the implementation of the StreamTokenizer method set, which
 * operates on C++ strings, but is sensitive to the possibility that the
 * characters arrays inside are UTF8 encodings of (in some cases, multi-byte)
 * characters.
 */

#include <istream>
#include <string>
#include "stream-tokenizer.h"
#include <libxml/xmlstring.h>
using namespace std;

StreamTokenizer::StreamTokenizer(istream& is,
                                 const string& delimiters,
                                 bool skipDelimiters) : 
  is(is), delimiters(delimiters), skipDelimiters(skipDelimiters) {
}

bool StreamTokenizer::hasMoreTokens() const {
  if (skipDelimiters) {
    while (true) {
      string ch = getNextXMLChar();
      if (ch.empty()) return false;
      if (xmlStrstr(BAD_CAST delimiters.c_str(), BAD_CAST ch.c_str()) == NULL) {
        savedChar = ch;
        return true;
      }
    }
  }
  
  if (!savedChar.empty()) return true;
  savedChar = getNextXMLChar();
  return !is.fail();
}

string StreamTokenizer::nextToken() {
  if (!hasMoreTokens()) return "";
  string token;
  string ch = getNextXMLChar();
  token += ch;
  if (xmlStrstr(BAD_CAST delimiters.c_str(), BAD_CAST ch.c_str()) != NULL) 
    return token;
  
  while (true) {
    ch = getNextXMLChar();
    if (ch.empty() || xmlStrstr(BAD_CAST delimiters.c_str(), BAD_CAST ch.c_str()) != NULL) break;
    token += ch;
  }
  
  if (!ch.empty()) 
    savedChar = ch;
  return token;
}

string StreamTokenizer::getNextXMLChar() const {
  if (!savedChar.empty()) {
    string nextChar = savedChar;
    savedChar = "";
    return nextChar;
  }

  const size_t kMaxUTF8CharBytes = 6;
  char buffer[kMaxUTF8CharBytes + 1] = {0, 0, 0, 0, 0, 0, 0};
  size_t pos = 0;
  do {
    char ch = is.get();
    if (is.fail()) return "";
    buffer[pos++] = ch;
  } while (pos < kMaxUTF8CharBytes && !xmlCheckUTF8(BAD_CAST buffer));
  
  if (xmlCheckUTF8(BAD_CAST buffer)) 
    return string(buffer, buffer + pos);
  return "";
}

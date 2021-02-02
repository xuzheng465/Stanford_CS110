/**
 * File: header.cc
 * ---------------
 * Presents the implementation of the HTTPHeader class, as exported
 * by header.h.
 */

#include "header.h"

#include <sstream>
#include "string-utils.h"

using namespace std;

/** public methods and functions **/

void HTTPHeader::ingestHeader(std::istream& instream) {
  string name;
  while (true) {
    string line;
    getline(instream, line);
    line = rtrim(line);
    if (line.empty()) break;
    if (line[0] == ' ') {
      line = trim(line);
      extendHeader(name, line);
    } else {
      istringstream iss(line);
      getline(iss, name, ':');
      name = trim(name);
      string value;
      getline(iss, value);
      value = trim(value);
      addHeader(name, value);
    }
  }
}

void HTTPHeader::addHeader(const string& name, int value) {
  ostringstream oss;
  oss << value;
  addHeader(name, oss.str());
}

void HTTPHeader::addHeader(const string& name, const string& value) {
  string normalizedName = toLowerCase(name);
  headers[normalizedName] = value;
}

void HTTPHeader::removeHeader(const string& name) {
  headers.erase(toLowerCase(name));
}

bool HTTPHeader::containsName(const string& name) const {
  string normalizedName = toLowerCase(name);
  return headers.find(normalizedName) != headers.end();
}

static const string kEmptyString;
const string& HTTPHeader::getValueAsString(const string& name) const {
  string normalizedName = toLowerCase(name);
  auto found = headers.find(normalizedName);
  return found == headers.end() ? kEmptyString : found->second;
}

long HTTPHeader::getValueAsNumber(const string& name) const {
  string value = getValueAsString(name);
  if (value.empty()) return 0L;
  char *endptr;
  long number = strtol(value.c_str(), &endptr, 0);
  return *endptr == '\0' ? number : 0L;
}

std::ostream& operator<<(std::ostream& os, const HTTPHeader& hh) {
  for (const pair<string, string>& p: hh.headers) {
    os << p.first << ": " << p.second << "\r\n";
  }

  return os;
}

/** Private methods **/

void HTTPHeader::extendHeader(const string& name, const string& value) {
  string normalizedName = toLowerCase(name);
  headers[normalizedName] += ' ' + value;
}

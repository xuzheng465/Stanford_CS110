/**
 * File: blacklist.cc
 * ------------------
 * Presents the implementation of the HTTPBlacklist class, which
 * manages a collection of regular expressions.  Each regular expressions
 * encodes a host aka server name that out proxy views as off limits.
 */

#include "blacklist.h"
#include <fstream>
#include <iostream>
using namespace std;

void HTTPBlacklist::addToBlacklist(const std::string& filename) throw (HTTPProxyException) {
  ifstream infile(filename.c_str());
  if (infile.fail()) {
    ostringstream oss;
    oss << "Filename \"" << filename << "\" of blocked domains could not be found.";
    throw HTTPProxyException(oss.str());
  }

  while (true) {
    string line;
    getline(infile, line);
    if (infile.fail()) break;
    regex re(line);
    blockedDomains.push_back(re);
  }
}

bool HTTPBlacklist::serverIsAllowed(const string& server) const {
  for (const regex& re: blockedDomains) {
    if (regex_match(server, re)) {
      return false;
    }
  }

  return true;
}

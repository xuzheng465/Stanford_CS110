/**
 * File: blacklist.h
 * -----------------
 * Defines a class that helper identify hosts
 * that should be blocked out by our proxy.
 */

#ifndef _blacklist_
#define _blacklist_
#include <vector>
#include <string>
#include <regex>
#include "proxy-exception.h"

class HTTPBlacklist {

 public:

/**
 * Method: addToBlacklist
 * ----------------------
 * Add the list of of blocked domain patterns within the specified
 * file to the blacklist.  The file contents should be
 * a list of regular expressions--one per line--describing
 * a class of server strings that should be blocked.
 *
 * The file might, for instance, look like this:
 * 
 *   (.*)\.berkeley.edu
 *   (.*)\.bing.com
 *   (.*)\.microsoft.com
 *   (.*)\.org
 *
 * If there's any drama (e.g. the file doesn't exist), then an
 * HTTPProxyException is thrown.
 */
  void addToBlacklist(const std::string& filename) throw (HTTPProxyException);

/**
 * Method: serverIsAllowed
 * -----------------------
 * Returns true if and only if access to the the 
 * identified server (i.e www.facebook.com) is permitted. 
 */
  bool serverIsAllowed(const std::string& server) const;

 private:
  std::vector<std::regex> blockedDomains;
};

#endif


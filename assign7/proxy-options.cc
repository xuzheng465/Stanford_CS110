/**
 * File: proxy-options.cc
 * ----------------------
 * Provides the implementation of a collection of functions
 * useful for parsing the command line, as exported by proxy-options.h
 */

#include "proxy-options.h"
#include <climits>         // for USHRT_MAX
#include <cstdlib>         // for getenv
#include <utility>         // for hash<>
#include <sstream>         // for ostringstream
#include <unistd.h>        // for getuid
#include <pwd.h>           // for getpwuid
using namespace std;

/**
 * Function: computeDefaultPortForUser
 * -----------------------------------
 * Uses the loggedin user ID to generate a port number between 1024
 * and USHRT_MAX inclusive.  The idea to hash the loggedinuser ID to
 * a number is in place so that users can launch the proxy to listen to
 * a port that, with very high probability, no other user is likely to 
 * generate.
 */
static const unsigned short kLowestOpenPortNumber = 1024;
unsigned short computeDefaultPortForUser() {
  struct passwd *pwd = getpwuid(getuid());
  if (pwd == NULL || pwd->pw_name == NULL) {
    ostringstream oss;
    oss << "Username could not be determined.  Aborting...";
    throw HTTPProxyException(oss.str());
  }
  
  string username = pwd->pw_name;
  size_t hashValue = hash<string>()(username);
  return hashValue % (USHRT_MAX - kLowestOpenPortNumber) + kLowestOpenPortNumber;
}

/**
 * Function: extractPortNumber
 * ---------------------------
 * Accepts the string form of the port number and converts it
 * to the unsigned short form and returns it.  If there are any
 * problems with the argument (e.g. it's not a number, or it's
 * too big or too small to every work), then an HTTPProxyException
 * is thrown.
 */
unsigned short extractPortNumber(const char *portArgument, const char *flags) throw (HTTPProxyException) {
  if (portArgument == NULL) {
    ostringstream oss;
    oss << "An actual port number must be supplied with the " << flags << " flag.";
    throw HTTPProxyException(oss.str());
  }
  
  char *endptr;
  long rawPort = strtol(portArgument, &endptr, 0);
  if (*endptr != '\0') {
    ostringstream oss;
    oss << "The supplied port number of \"" << portArgument << "\" is malformed.";
    throw HTTPProxyException(oss.str());
  }
  
  if (rawPort < 1 || rawPort > USHRT_MAX) {
    ostringstream oss;
    oss << "Port number must be between 1 and " << USHRT_MAX << ".";
    throw HTTPProxyException(oss.str());
  }
  
  return rawPort;
}

string extractProxyServer(const char *proxyArgument) throw (HTTPProxyException) {
  if (proxyArgument == NULL || string(proxyArgument).empty()) {
    ostringstream oss;
    oss << "An actual hostname must be supplied with the --proxy-server/-s flag.";
    throw HTTPProxyException(oss.str());
  }

  return proxyArgument;
}

long extractLongInRange(const char *str, long min, long max, const char *flags) throw (HTTPProxyException) {
  if (str == NULL) {
    ostringstream oss;
    oss << "An integer string must accompany the " << flags << " flag.";
    throw HTTPProxyException(oss.str());
  }

  char *endptr;
  long l = strtol(str, &endptr, 0);
  if (*endptr != '\0') {
    ostringstream oss;
    oss << "The string argument accompanying " << flags << " isn't a number.";
    throw HTTPProxyException(oss.str());
  }

  if (l < min || l > max) {
    ostringstream oss;
    oss << "The integer string accompanying " << flags << " is out of range.  Supply something within the range [" << min << ", " << max << "].";
    throw HTTPProxyException(oss.str());
  }

  return l;
}

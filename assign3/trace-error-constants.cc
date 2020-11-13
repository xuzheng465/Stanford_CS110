/**
 * File: trace-error-constants.cc
 * ------------------------------
 * Provides the implementation of the compileSystemCallErrorStrings routine, which
 * just parses the relevant system interface file to map errno status codes (e.g. 2) to
 * the #define constants defined for them (e.g. "ENOENT").
 * 
 * Most of the implementation is self-explanatory, although the regular expression deserves
 * some explanation.  
 * 
 *   [^E]*(E\\S*)\\s+(\\d+).*
 *
 * Breakdown:
 *   [^E]* matches the largest prefix leading up to the capital E.
 *   (E\\S*) matches the #define constant token, which must begin with E but otherwise
 *           can be made up of any nonwhitespace characters.  This part of the regex is
 *           wrapped in parens so the substring that matches it can be surfaced via sm[1] of
 *           an smatch called sm.
 *   \\s+ matches the longest contiguous block of whitespace following the #define token.
 *   (\\d+) matches text compromised of one or more digit characters.  It's wrapped in parentheses
 *          so that the digit string can be surfaced via sm[2].
 *   .* matches everything else beyond the digit string
 *
 * [^E]*(E\\S*)\\s+(\\d+).* matches something like
 *
 *     #define EAGAIN 11
 *
 * but (by intention) none of these
 *
 *     #define NUM_CONSTANTS 148
 *     #define ERRNO_H
 *     #define EWOULDBLOCK EAGAIN
 */

#include "trace-error-constants.h"
#include <fstream>
#include <regex>
#include <cassert>
using namespace std;

/**
 * Constant: kErrorHeaderFilenames
 * -------------------------------
 * Collects the header files that need to be parsed on the myths to surface all
 * errno numbers (e.g. 2) to their more easily recognizes constants (e.g. "ENOENT").
 */
static const string kErrorHeaderFilenames[] = { "/usr/include/asm-generic/errno-base.h", "/usr/include/asm-generic/errno.h" };

/**
 * Constant: kErrorConstantDefinePattern
 * -------------------------------------
 * Defines the regular expression pattern used to extract lines defining the constants we're interested in.
 * Read through the head comment for a discussion as to why this regular expression works for us.
 */
static const string kErrorConstantDefinePattern = "[^E]*(E\\S*)\\s+(\\d+).*";

/**
 * Function: processLine
 * ---------------------
 * Tries to match the provided line of text to the relevant regular expression.  If it fails to,
 * then the function returns without modifying the map.  If it succeeds, then a new pair<int, string>
 * is added to the supplied map.
 */
static void processLine(map<int, string>& errorConstants, const string& line) {
  regex re(kErrorConstantDefinePattern); // all constants we're interested in begin with E
  smatch sm;
  if (!regex_match(line, sm, re)) return;
  assert(sm.size() == 3);
  int num = stoi(sm[2]); // stoi converts a digit string to an int
  const string& str = sm[1];
  errorConstants[num] = str;
}

/**
 * Function: compileSystemCallErrorStrings
 * ---------------------------------------
 * Crawls over the files listed in kErrorHeaderFilenames and populates the
 * supplied map with all of the errno #define constants (like ENOENT, ECHILD, EACCES, etc).
 */
void compileSystemCallErrorStrings(map<int, string>& errorConstants) throw (MissingFileException) {
  for (const string& name: kErrorHeaderFilenames) {
    ifstream infile(name);
    if (infile.fail()) 
      throw MissingFileException("Failed to open the file named \"" + name + "\".");

    while (true) {
      string line;
      getline(infile, line);
      if (infile.fail()) break;
      processLine(errorConstants, line);
    }
  }
}

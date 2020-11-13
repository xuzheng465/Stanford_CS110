/**
 * File: trace-system-calls.cc
 * ---------------------------
 * Defines all of the functions needed to parse system include and Linux kernel source files to
 * build a table of all system call numbers, names, and signatures.
 */
 
#include "trace-system-calls.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <cassert>
#include <ext/stdio_filebuf.h>
#include <sys/wait.h>
#include "subprocess.h"
#include "string-utils.h"
#include "trace-exception.h"

using namespace std;
using namespace __gnu_cxx;

/**
 * Functions: overloads of operator<<, operator>> for scParamType
 * --------------------------------------------------------------
 * Self-exaplanatory.
 */

ostream& operator<<(ostream& os, const scParamType& type) {
  switch (type) {
    case SYSCALL_INTEGER: os << "SYSCALL_INTEGER"; break;
    case SYSCALL_STRING: os << "SYSCALL_STRING"; break;
    case SYSCALL_POINTER: os << "SYSCALL_POINTER"; break;
    default: os << "SYSCALL_UNKNOWN_TYPE"; break;
  }

  return os;
}

istream& operator>>(istream& is, scParamType& type) {
  std::string str;
  is >> str;
  if (str == "SYSCALL_INTEGER") {
    type = SYSCALL_INTEGER;
  } else if (str == "SYSCALL_STRING") {
    type = SYSCALL_STRING; 
  } else if (str == "SYSCALL_POINTER") {
    type = SYSCALL_POINTER;
  } else {
    type = SYSCALL_UNKNOWN_TYPE;
  }
  
  return is;
}

/**
 * Constant: kUniversalStandardAbsoluteFilename
 * --------------------------------------------
 * Identifies the file that defines a collection of #define constants that map system call names to numbers.
 * Different architectures use different mappings (e.g. x86_64 used system call number 1 for read, whereas
 * x86_32 maps 1 to exit).  We're only concerned with x86_64, so we engineer the system call information compiler
 * to be x86_64-specific.
 */
static const string kUniversalStandardAbsoluteFilename = "/usr/include/x86_64-linux-gnu/asm/unistd_64.h";

/**
 * Constant: kSystemCallNumberDefinePattern
 * ----------------------------------------
 * Provides the regex pattern we use to identify those lines within kUniversalStandardAbsoluteFilename with
 * name -> number mappings for system calls.  The lines of interest all look like this:
 * 
 *   #define __NR_read 0
 *   #define __NR_write 1
 *   #define __NR_open 2
 *   #define __NR_close 3
 *   #define __NR_stat 4
 *
 * The regex pattern defined below is designed to match these five lives and all of the others structured just like it.
 * Breakdown on why we use [^_]+__NR_(\\w+)\\s+(\\d+)\\s*
 *    
 *     [^_]+ matches everything up through the very first underscore
 *     __NR_ matches precisely that: __NR_
 *     (\\w+) matches the cluster of letters, numbers, and underscores following __NR_.  This cluster is
 *            the name of the system call (e.g. read, fork, waitpid, stat), and that's why we wrap it in
 *            parentheses--so it can be surfaced via sm[1] for the smatch called sm.
 *     \\s+ matches all of the whitespace following the __NR_<system call name>
 *     (\\d+) matches the number that the system call name maps to.  It's also wrapped in parentheses so
 *            that we can surface the integer via sm[2] for the match called sm.
 *     \\s* matches all of the remaining whitespace following the number
 *
 * The full regex, therefore, matches lines like:
 *
 *   #define __NR_read 0
 *   #define __NR_write 1
 *   #define __NR_open 2
 *   #define __NR_close 3
 *   #define __NR_stat 4
 *
 * It does not, however, match lines like this:
 *
 *   #define _NR__read 0
 *   #define __NR_read 14x
 *   #define __NR_read 14 fjkdjkd
 *
 */
static const string kSystemCallNumberDefinePattern = "[^_]+__NR_(\\w+)\\s+(\\d+)\\s*";

/**
 * Function: collectSystemCallNumbers
 * ----------------------------------
 * Reads through kUniversalStandardAbsoluteFilename line by line, ignorning those that don't match
 * kSystemCallNumberDefinePattern, ands add a new number -> name and name -> number association to 
 * systemCallNumbers and systemCallNames for each line that does.
 */
static void collectSystemCallNumbers(map<int, string>& systemCallNumbers, map<string, int>& systemCallNames) {
  ifstream infile(kUniversalStandardAbsoluteFilename);
  if (infile.fail()) 
    throw MissingFileException("Encountered a problem opening \"" + kUniversalStandardAbsoluteFilename);

  regex re(kSystemCallNumberDefinePattern);
  string line;
  while (true) {
    getline(infile, line);
    if (infile.fail()) break;
    smatch sm;
    if (!regex_match(line, sm, re)) continue;
    const string& name = sm[1];
    int number = stoi(sm[2]); // stoi converts a digit string to an int
    systemCallNumbers[number] = name;
    systemCallNames[name] = number;
  }
}

/**
 * Constant: kSystemCallNameAndSignaturePattern
 * --------------------------------------------
 * Defines the pattern used to recognize the argument count and system call name of a string already confirmed
 * to be a SYSCALL_DEFINE macro call.
 *
 * This is the regex pattern of interest:
 *
 *    \\s*SYSCALL_DEFINE([0-6])\\(([^,)]+)([^)]+)*\\)\\s*
 *
 *  Here's the breakdown of what it matches and why:
 *
 *    \\s* matches any leading whitespace
 *    SYSCALL_DEFINE matches precisely that: SYSCALL_DEFINE
 *    ([0-6]) matches a single digit string between 0 and 6, inclusive.  It's wrapped in parentheses
 *            so that the number string can be surfaced via sm[1] of an smatch called sm.
 *    \\s* matches any whitespace between the argument count and the opening parenthesis
 *    \\( matches exactly that: a single open parenthesis (the \\ is needed because '(' is a metacharacter)
 *    ([^,)]+) matches everything up beyond the macro's opening parenthesis up through either the first comma or the closing parenthesis,
 *             whichever comes first.  This is wrapped in parentheses so that whatever matches it can be surfaced via sm[2] and taken
 *             to be the name of a system call.  (There may be some extra whitespace around the system call name, but that gets trimmed elsewhere)
 *    .* matches everything after the , or ) that marks the end of the system call name.
 */
static const string kSystemCallNameAndSignaturePattern = "\\s*SYSCALL_DEFINE([0-6])\\s*\\(([^,)]+).*";
static pair<string, int> processNameAndArgumentCount(const string& macro, map<string, systemCallSignature>& systemCallSignatures) {
  regex re(kSystemCallNameAndSignaturePattern);
  smatch sm;
  assert(regex_match(macro, sm, re));
  assert(sm.size() == 3);
  const string& name = sm[2];
  int numArguments = stoi(sm[1]);
  return make_pair(name, numArguments);
}

/**
 * Function: normalizeType
 * -----------------------
 * Simple conversion function that takes the C data type embedded in a SYSCALL_DEFINE macro and
 * converts it to SYSCALL_STRING, SYSCALL_POINTER, or SYSCALL_INTEGER.
 */
static scParamType normalizeType(const string& type) {
  if (type == "const char __user *" || type == "const char *") return SYSCALL_STRING;
  if (type == "unsigned long" || type.find('*') != string::npos) return SYSCALL_POINTER;
  return SYSCALL_INTEGER;
}

/**
 * Function: processSystemCallArguments
 * ------------------------------------
 * Processes the argument list portion of a SYSCALL_DEFINE macro.  The regex pattern is dynamically generated to
 * match the number of additional macro arguments expected.
 *
 * TODO: // simplify this to a static regex pattern that works for all argument counts.
 */
static void processSystemCallArguments(const string& macro, int numArguments, systemCallSignature& parameterTypes) {
  assert(numArguments > 0);
  string pattern = "[^(]+\\([^,]+"; // skip everything up through and including macro name, open parenthesis, and first token
  for (int i = 0; i < 2 * numArguments; i++) pattern += ",([^,]+)";
  pattern += "\\s*\\)\\s*";
  regex re(pattern);
  smatch sm;
  assert(regex_match(macro, sm, re));
  assert(int(sm.size()) == (2 * numArguments + 1));
  for (int i = 0; i < numArguments; i++) {
    string type = sm[2 * i + 1];
    scParamType normalizedType = normalizeType(trim(type));
    parameterTypes.push_back(normalizedType);
  }
}

/**
 * Function: ingestEntireMacro
 * ---------------------------
 * A SYSCALL_DEFINE macro occasionally stretches over two or more lines.  ingestEntireMacro
 * accepts the first line of the macro and keeps reading in additional lines from the
 * provided ifstream until a close parenthesis is found.  ingestEntireMacro returns
 * the concatenation of the first line and all additional lines read in.
 */
static string ingestEntireMacro(ifstream& infile, const string& firstLine) {
  string macro = firstLine;
  while (macro.find(')') == string::npos) {
    string next;
    getline(infile, next);
    macro += next;
  }
  
  return macro;
}

/**
 * Function: processSystemCallSignature
 * ------------------------------------
 * Digs into the provided macros to pull out its name and argument count (via processNameAndArgumentCount), ensures that
 * the system call has a place in the provided systemCallSignatures and systemCallNames map, and then proceeds to
 * ensure that the relevant entry within systemCallSignatures reflects this argument count and the data types expected (relying
 * on the scParamType to distinguish the types to the extent we need to).
 */
static void processSystemCallSignature(const string& macro, map<string, systemCallSignature>& systemCallSignatures, const map<string, int>& systemCallNames) {
  pair<string, int> p = processNameAndArgumentCount(macro, systemCallSignatures);
  const string& name = trim(p.first);
  int numArguments = p.second;
  if (systemCallNames.find(name) == systemCallNames.cend() ||
      systemCallSignatures.find(name) != systemCallSignatures.cend()) return; // either don't know the system call or we've already processed it
  
  systemCallSignatures[name]; // trivially add an zero-arg signature
  if (numArguments == 0) return; 
  processSystemCallArguments(macro, numArguments, systemCallSignatures[name]);
}

/**
 * Constant: kSystemCallNameAndArgumentCountPattern
 * ------------------------------------------------
 * The signatures of all of the system calls are sprinkled throughout all of the .h files, but
 * the signatures are also supplied by a collection of highly structured C macro throughout the
 * linux kernel source tree.  Specifically, these macros all look like this:
 *
 *   SYSCALL_DEFINE0(fork)
 *   SYSCALL_DEFINE1(close, int, fd)
 *   SYSCALL_DEFINE2(dup2, int, newfd, int, oldfd)
 *   ...
 *   SYSCALL_DEFINE6(futex, int *, uaddr, int op, int val, const struct timespec *, timeout, int *, uaddr2, int val3)
 * 
 * All such macros take at least one argument, and that required argument is the name of the system call.
 * The number of additional arguments is clear from the number in the macro name, and additional macro arguments
 * always come in pairs, e.g. SYSCALL_DEFINE2 takes 4 additional arguments, where each pair provided the type and
 * name of a parameter.
 *
 * kSystemCallNameAndArgumentCountPattern is concerned only with the extraction of the argument count as a way of identifying
 * whether more sophisticated parsing is necessary to infer a full prototype.
 *
 * Breakdown of \\s*SYSCALL_DEFINE([0-6]).* is actually fairly straightforward:
 * 
 *    \\s* matches any leading whitespace preceding the macro name
 *    SYSCALL_DEFINE matches precisely that: SYSCALL_DEFINE
 *    [0-6] matches a single digit character between 0 and 6, inclusive
 *    \\s*\\( matches any whitespace followed by an opening parenthesis
 *    .* matches everything beyond the opening parenthesis
 */
static const string kSystemCallNameAndArgumentCountPattern = "\\s*SYSCALL_DEFINE[0-6]\\s*\\(.*";
static void processSignaturesWithinKernelSourceFile(const string& sourceFileName, 
                                                    map<string, systemCallSignature>& systemCallSignatures, 
                                                    const map<string, int>& systemCallNames) {
  ifstream infile(sourceFileName);
  regex re(kSystemCallNameAndArgumentCountPattern);
  while (true) {
    string line;
    getline(infile, line);
    if (infile.fail()) break;
    if (regex_match(line, re)) {
      string macro = ingestEntireMacro(infile, line);
      processSystemCallSignature(macro, systemCallSignatures, systemCallNames);
    }
  }
}

/**
 * Function: loadSignaturesFromCache
 * ---------------------------------
 */
static const string kCacheFilename = ".trace_signatures.txt";
static bool loadSignaturesFromCache(map<string, systemCallSignature>& systemCallSignatures) {
  ifstream cache(kCacheFilename);
  if (cache.fail()) return false;
  while (true) {
    string name;
    cache >> name;
    if (cache.fail()) break;
    systemCallSignatures[name];
    string count;
    cache >> count;
    for (int i = 0; i < stoi(count); i++) {
      scParamType type;
      cache >> type;
      systemCallSignatures[name].push_back(type);
    }
  }
  return true;
}

/**
 * Function: cacheSignatures
 * -------------------------
 * Because it takes such a long time to compile the system call signature information, and because
 * it doesn't change for months at a time, we store the signature data to a file named kCacheFilename.
 */
static void cacheSignatures(const map<string, systemCallSignature>& systemCallSignatures) {
  ofstream cache(kCacheFilename);
  for (const pair<string, systemCallSignature>& p: systemCallSignatures) {
    cache << p.first << " " << p.second.size();
    for (scParamType type: p.second) cache << " " << type;
    cache << endl;
  }
}

/**
 * Function: processAllKernelSourceFiles
 * -------------------------------------
 * Reads the list of kernel source files printed by the supplied subprocess and parses each one, looking for
 * SYSCALL_DEFINE[0-6] macros.  Most of the work is done by processSignaturesWithinKernelSourceFile.  The code
 * exposed in this function is pretty self-explanatory.
 */
static void processAllKernelSourceFiles(const subprocess_t& sp, map<string, systemCallSignature>& systemCallSignatures, const map<string, int>& systemCallNames) {
  stdio_filebuf<char> processbuf(sp.ingestfd, ios::in);
  istream instream(&processbuf); // wrap the ingest file descriptor in a C++ istream so we can more easily parse each file line by line.
  while (true) {
    string sourceFileName;
    getline(instream, sourceFileName);
    if (instream.fail()) break;
    processSignaturesWithinKernelSourceFile(sourceFileName, systemCallSignatures, systemCallNames);
  }

  waitpid(sp.pid, NULL, 0);
}

/**
 * Constants: kKernelSourceCodeDirectory, kKernelSourceFileFinderCommand
 * ---------------------------------------------------------------------
 * kKernelSourceCodeDirectory defines the directory where the Linux source code currently running on the myths resides.
 * kKernelSourceFileFinderCommand defines the argument vector that should be invoked in a subprocess that knows how to
 * list all of the source file names, one per line, so that each can be opened and searched for SYSCALL_DEFINE macros.
 */
static const string kKernelSourceCodeDirectory = "/usr/src/linux-source-3.13.0/linux-source-3.13.0";
static const char *const kKernelSourceFileFinderCommand[] = {"find", kKernelSourceCodeDirectory.c_str(), "-name", "*.c", "-print", NULL};
static void collectSystemCallSignatures(map<string, systemCallSignature>& systemCallSignatures, const map<string, int>& systemCallNames, bool rebuild) {
  if (!rebuild && loadSignaturesFromCache(systemCallSignatures)) return;
  subprocess_t sp = subprocess(const_cast<char **>(kKernelSourceFileFinderCommand), 
                               /* supplyChildInput = */ false, 
                               /* ingestChildOutput = */ true);
  cout << "Extracting system call signature information from " << kKernelSourceCodeDirectory << "..." << endl;
  cout << "Expect to wait about a minute..... " << flush;
  processAllKernelSourceFiles(sp, systemCallSignatures, systemCallNames);
  cacheSignatures(systemCallSignatures);
  cout << "done!" << endl;
  sleep(2);
}

/**
 * Function: compileSystemCallData
 * -------------------------------
 * Populates the supplied maps with information about system call numbers, names, and signatures.
 * The implementation just passes the buck on to two helper functions.
 */
void compileSystemCallData(map<int, string>& systemCallNumbers,
                           map<std::string, int>& systemCallNames,
                           map<std::string, systemCallSignature>& systemCallSignatures, bool rebuild) {
  if (systemCallNumbers.size() + systemCallNames.size() + systemCallSignatures.size() > 0)
    throw TraceException("The maps supplied to compileSystemCallData must all be empty.");
  collectSystemCallNumbers(systemCallNumbers, systemCallNames);
  collectSystemCallSignatures(systemCallSignatures, systemCallNames, rebuild);
}

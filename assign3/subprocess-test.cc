/**
 * File: subprocess-test.cc
 * ------------------------
 * Simple unit test framework in place to exercise functionality of the subprocess function.
 */

#include "subprocess.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/wait.h>
#include <ext/stdio_filebuf.h>

using namespace __gnu_cxx; // __gnu_cxx::stdio_filebuf -> stdio_filebuf
using namespace std;

/**
 * File: publishWordsToChild
 * -------------------------
 * Algorithmically self-explanatory.  Relies on a g++ extension where iostreams can
 * be wrapped around file desriptors so that we can use operator<<, getline, endl, etc.
 */
const string kWords[] = {"put", "a", "ring", "on", "it"};
static void publishWordsToChild(int to) {
  stdio_filebuf<char> outbuf(to, std::ios::out);
  ostream os(&outbuf); // manufacture an ostream out of a write-oriented file descriptor so we can use C++ streams semantics (prettier!)
  for (const string& word: kWords) os << word << endl;
} // stdio_filebuf destroyed, destructor calls close on desciptor it owns
    
/**
 * File: ingestAndPublishWords
 * ---------------------------
 * Reads in everything from the provided file descriptor, which should be
 * the sorted content that the child process running /usr/bin/sort publishes to
 * its standard out.  Note that we one again rely on the same g++ extenstion that
 * allows us to wrap an iostream around a file descriptor so we have C++ stream semantics
 * available to us.
 */
static void ingestAndPublishWords(int from) {
  stdio_filebuf<char> inbuf(from, std::ios::in);
  istream is(&inbuf);
  while (true) {
    string word;
    getline(is, word);
    if (is.fail()) break;
    cout << word << endl;
  }
} // stdio_filebuf destroyed, destructor calls close on desciptor it owns

/**
 * Function: waitForChildProcess
 * -----------------------------
 * Halts execution until the process with the provided id exits.
 */
static void waitForChildProcess(pid_t pid) {
  if (waitpid(pid, NULL, 0) != pid) {
    throw SubprocessException("Encountered a problem while waiting for subprocess's process to finish.");
  }
}

/**
 * Function: main
 * --------------
 * Serves as the entry point for for the unit test.
 */
const string kSortExecutable = "/usr/bin/sort";
int main(int argc, char *argv[]) {
  try {
    char *argv[] = {const_cast<char *>(kSortExecutable.c_str()), NULL};
    subprocess_t child = subprocess(argv, true, true);
    publishWordsToChild(child.supplyfd);
    ingestAndPublishWords(child.ingestfd);
    waitForChildProcess(child.pid);
    return 0;
  } catch (const SubprocessException& se) {
    cerr << "Problem encountered while spawning second process to run \"" << kSortExecutable << "\"." << endl;
    cerr << "More details here: " << se.what() << endl;
    return 1;
  } catch (...) { // ... here means catch everything else
    cerr << "Unknown internal error." << endl;
    return 2;
  }
}

/**
 * File: subprocess
 * ----------------
 * Exports a custom data type to bundle everything needed to spawn a new process,
 * manage that process, and optionally publish to its stdin and/or read from its stdout.
 *
 * Sample program:
   
const string kWords[] = {"put", "a", "ring", "on", "it"};
static void publishWordsToChild(int to) {
  stdio_filebuf<char> outbuf(to, std::ios::out);
  ostream os(&outbuf);
  for (const string& word: kWords) os << word << endl;
} // stdio_filebuf destroyed, destructor calls close on desciptor it owns
    
static void ingestAndPublishWords(int from) {
  stdio_filebuf<char> inbuf(from, std::ios::in);
  istream is(&inbuf); // manufacture an istream out of a read-only file descriptor so we can use C++ streams (prettier!)
  while (true) {
    string word;
    getline(is, word);
    if (is.fail()) break;
    cout << word << endl;
  }
} // stdio_filebuf destroyed, destructor calls close on desciptor it owns

static void waitForChildProcess(pid_t pid) {
  if (waitpid(pid, NULL, 0) != pid) {
    throw SubprocessException("Encountered a problem while waiting for subprocess's process to finish.");
  }
}

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

 */

#pragma once
#include <unistd.h> // for pid_t
#include "subprocess-exception.h"

/**
 * Constant: kNotInUse
 * -------------------
 * Constant residing in a descriptor field within a subprocess_t when
 * the descriptor isn't being used.
 */ 
static const int kNotInUse = -1;

/**
 * Type: subprocess_t
 * ------------------
 * Bundles information related to the child process created
 * by the subprocess function below.
 * 
 *  pid: the id of the child process created by a call to subprocess
 *  supplyfd: the descriptor where one pipes text to the child's stdin (or kNotInUse if child hasn't rewired its stdin)
 *  ingestfd: the descriptor where the text a child pushes to stdout shows up (or kNotInUse if child hasn't rewired its stdout)
 *
 */
struct subprocess_t {
  pid_t pid;
  int supplyfd;
  int ingestfd;
};
 
/**
 * Function: subprocess
 * --------------------
 * Creates a new process running the executable identified via argv[0].
 *
 *   argv: the NULL-terminated argument vector that should be passed to the new process's main function
 *   supplyChildInput: true if the parent process would like to pipe content to the new process's stdin, false otherwise
 *   ingestChildOutput: true if the parent would like the child's stdout to be pushed to the parent, false otheriwse
 */
subprocess_t subprocess(char *argv[], bool supplyChildInput, bool ingestChildOutput) throw (SubprocessException);

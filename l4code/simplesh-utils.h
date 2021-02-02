/**
 * File: simplesh-utils.h
 * ----------------------
 * Provides a collection of helper functions used by all
 * of the various simplesh implementations.
 */

#ifndef _simplesh_utils_
#define _simplesh_utils_
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>

/**
 * Constants
 */

static const char *const kCommandPrompt = "simplesh>";
static const unsigned short kMaxCommandLength = 2048;
static const unsigned short kMaxArgumentCount = 128;
static const size_t kTenthOfSecond = 100000; // in usecs

static const int kForkFailed = 1;
static const int kWaitFailed = 2;
static const int kTwoForegroundProcesses = 3;
  
/**
 * Function: readCommand
 * ---------------------
 * Halts until the user type a command line to be executes into stdin.
 * A copy of what's typed in is copied into the supplied buffer.
 * The user is expected to type in the length of the supplied buffer so
 * the buffer isn't overwritten.
 */
void readCommand(char command[], size_t len);

/**
 * Function: parseCommandLine
 * --------------------------
 * Accepts the command line and inplace parses it into its constituent
 * tokens, placing the address of each token in the supplied argv array.
 * The client should pass in the allocated length of the argv array so as
 * to prevent overflow.  The number of visible tokens is returned, and the
 * argv array is NULL-terminated.
 */
int parseCommandLine(char *command, char *argv[], int len);

/**
 * Function: handleBuiltin
 * -----------------------
 * Handles all of the simplesh builtins, which at the moment
 * is just "quit".  Returns true if the supplied argv was handled as a builtin,
 * and returns false otherwise. 
 */
bool handleBuiltin(char *argv[]);

/**
 * Function: forkProcess
 * ---------------------
 * Creates a child process using fork, confirm fork didn't fail,
 * and then returns whatever fork returned (child pid in parent, 0 in child)
 */
pid_t forkProcess();

/**
 * Functions: blockSIGCHLD, unblockSIGCHLD
 * ---------------------------------------
 * Sibling functions that block and unblock SIGCHLD.
 */

void blockSIGCHLD();
void unblockSIGCHLD();

#endif


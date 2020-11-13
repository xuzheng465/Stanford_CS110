/**
 * File: stsh-readline.h
 * ---------------------
 * Exports a simple wrapper around the built-in getline function that
 * uses the specified prompt and (optionally) relies on the GNU readline
 * library for tab completion and history support.
 */

#ifndef _stsh_readline_
#define _stsh_readline_

#include <string>

/**
 * Function: rlinit
 * ----------------
 * Configures the stsh-readline module using information provided
 * via the main function's argument count and vector.
 */
void rlinit(int argc, char *argv[]);

/**
 * Function: readline
 * ------------------
 * Prompts the user (unless the prompt has been suppressed via the 
 * --suppress-prompt/-s flag) and places the next line of input
 * into the string referenced by line.  readline returns false iff
 * EOF was detected without any text being entered.
 */
bool readline(std::string& line);

#endif

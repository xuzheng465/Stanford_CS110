/**
 * Contains a small number of helper timing
 * and synchronization routines.
 */

#ifndef _thread_utils_
#define _thread_utils_

#include <cstddef>

/**
 * Function: sleep_for
 * Usage: sleep_for(1000);
 * ----------------------
 * Prompts the currently executing thread to yield and sleep for at
 * least the specified number of milliseconds.
 */
void sleep_for(std::size_t milliseconds);

#endif

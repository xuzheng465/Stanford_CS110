/**
 * Inlines the definition of snooze, which operates
 * much like sleep, except that it effectly can't be
 * interrupted by the delivery of a signal.
 */

#ifndef _sleep_utils_
#define _sleep_utils_
#include <time.h>

/**
 * The sleep function sleeps for the specified number
 * of seconds, unless a signal arrives, in
 * which case the sleep call returns prematurely (and
 * returns the number of additional seconds it could have
 * slept).
 *
 * The snooze function accounts for this and ensures that
 * the calling process effectively sleeps for the full amount
 * of time, whether signals are caught or not.
 */
static inline void snooze(unsigned int numSeconds) {
  struct timespec timeToSnooze = {numSeconds, 0}, timeLeft;
  while (nanosleep(&timeToSnooze, &timeLeft) == -1) {
    timeToSnooze = timeLeft;
  }
}

#endif

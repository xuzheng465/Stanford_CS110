/**
 * File: exit-utils.h
 * ------------------
 * Unifiies a common function that's assert like, but
 * allows for a very detailed error message to be published.
 */

#ifndef _exit_utils_
#define _exit_utils_

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

static inline void exitUnless(bool test, int code, FILE *stream,
			      const char *control, ...) {
  if (test) return;
  va_list arglist;
  va_start(arglist, control);
  vfprintf(stream, control, arglist);
  va_end(arglist);
  exit(code);
}

// the ... makes these difficult to unify
static inline void exitIf(bool test, int code, FILE *stream,
			  const char *control, ...) {
  if (!test) return;
  va_list arglist;
  va_start(arglist, control);
  vfprintf(stream, control, arglist);
  va_end(arglist);
  exit(code);
}

#endif

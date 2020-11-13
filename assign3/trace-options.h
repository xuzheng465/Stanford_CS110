/**
 * File: trace-options.h
 * ---------------------
 * Exports a single function that knows how to process the command line invoking
 * trace.  The command line typically looks like the invocation of another executable, e.g.
 * something like "find /usr/include/ -name *.h -print" preceded by "trace", e.g. 
 * "trace find /usr/include/ -name *.h -print".  However, trace itself can be fed one or two
 * flags, --simple and/or --rebuild.  The first one coaches trace to output a very simplified
 * version of trace, whereas the second one instructs trace to rebuild all of the prototypes
 * from scratch instead of relying on a cached file.
 *
 * If the command line is malformed (e.g. bogus flags, etc), then a TraceException is thrown.
 */

#pragma once
#include "trace-exception.h"

size_t processCommandLineFlags(bool& simple, bool& rebuild, char *argv[]) throw (TraceException);

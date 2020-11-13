/**
 * File: trace-error-constants.h
 * -----------------------------
 * Defines a single routine that builds of a map of errno status codes (e.g. 2) to
 * their more familiar #define constants (expressed as strings, e.g. "ENOENT").
 */
 
#pragma once
#include <map>
#include <string>
#include "trace-exception.h"
 
void compileSystemCallErrorStrings(std::map<int, std::string>& errorConstants) throw (MissingFileException);
 
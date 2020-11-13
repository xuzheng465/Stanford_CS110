/**
 * File: stsh-parse-utils.h
 * ------------------------
 * Defines a single function that's helpful for converting
 * numeric strings to actual numbers.
 */

#pragma once
#include <string>  // for string
#include <cstddef> // for size_t

/**
 * Function: parseNumber
 * ---------------------
 * Accepts the provided string (assumed to be that for a nonnegative number),
 * converts it to a size_t, and returns it.
 */
size_t parseNumber(const char *arg, const std::string& usage);

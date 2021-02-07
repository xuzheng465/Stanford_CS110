/**
 * File: utils.h
 * -------------
 * Defines a collection of short URL and string processing
 * functions helpful for code that needs to be written in
 * news-aggregator.cc
 */

#pragma once
#include <string>

/**
 * Function: getURLServer
 * ----------------------
 * Given a bona fide URL server string (e.g. 
 * "http://cs110.stanford.edu", or "https://graph.facebook.com/jerry"), 
 * return just the server portion of the URL (e.g. "cs110.stanfod.edu", 
 * or "graph.facebook.com").
 */
std::string getURLServer(const std::string& url);

/**
 * Function: shouldTruncate
 * ------------------------
 * Returns true if and only if the string is long enough to truncate when
 * publishing out to the console.
 */
bool shouldTruncate(const std::string& str);

/**
 * Function: truncate
 * ------------------
 * Returns a truncate version of the supplied string.
 * This function should only be called if the supplied string 
 * itself prompts shouldTruncate to return true.
 */
std::string truncate(const std::string& str);

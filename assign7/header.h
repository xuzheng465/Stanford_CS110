/**
 * File: header.h
 * --------------
 * Because the request header and response header, save for the first
 * line, are structurally identical, it makes sense to unify the notion
 * of a header to a single class that can be used by both.
 */

#ifndef _http_header_
#define _http_header_

#include <string>
#include <map>

class HTTPHeader {

/**
 * Overload the << operator so that HTTPHeaders can be published
 * into ostreams (e.g. cout, cerr, ofstreams, and other ostreams
 * wrappers around, say, two-way file descriptors.
 */
  friend std::ostream& operator<<(std::ostream& os, const HTTPHeader& hh);
  
 public:

/**
 * Ingests the entire header of what's assumed to be either an
 * HTTP request or response.
 */
  void ingestHeader(std::istream& instream);

/**
 * Adds (or updates) the provided name so that it's associated
 * with the string form of the supplied integer.  Note that the name comparison is
 * case-insensitive, so that "Expires" and "EXPIRES" are the considered
 * the same.
 */
  void addHeader(const std::string& name, int value);
  
/**
 * Adds (or updates) the provided name so that it's associated
 * with the provided value string.  Note that the name comparison is
 * case-insensitive, so that "Expires" and "EXPIRES" are the considered
 * the same.
 */
  void addHeader(const std::string& name, const std::string& value);

/**
 * Removes the provided name from the request header.
 */
  void removeHeader(const std::string& name);

/**
 * Returns true if and only if the collection of name-value pairs
 * includes the one provided.  Note that the name comparison is
 * case-insensitive, so that "Expires" and "EXPIRES" are the considered
 * the same.
 */
  bool containsName(const std::string& name) const;

/**
 * Returns the string form of the value associated with the provided
 * name.  Note, as above, that the name comparison is case-insensitive, 
 * so that "Expires" and "EXPIRES" are the considered the same.  If the
 * key isn't present, then the empty string is returned.
 */
  const std::string& getValueAsString(const std::string& name) const;

/**
 * Returns the number (as a long) associated with the provided name.
 * Note, as above, that the name comparison is case-insensitive, 
 * so that "Expires" and "EXPIRES" are the considered the same.  If the
 * key isn't present, or if the associated value isn't purely numeric,
 * then 0 is returned.
 */
  long getValueAsNumber(const std::string& name) const;
  
 private:
  std::map<std::string, std::string> headers;
  void extendHeader(const std::string& name, const std::string& value);
};

#endif

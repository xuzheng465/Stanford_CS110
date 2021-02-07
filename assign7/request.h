/**
 * File: request.h
 * ---------------
 * Presents a class designed to model all of a well-formed 
 * HTTP request header.
 */

#ifndef _http_request_
#define _http_request_

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "header.h"
#include "payload.h"
#include "proxy-exception.h"

class HTTPRequest {

/**
 * Provides a version of operator<< that knows how to print an entire
 * HTTPRequest to the provides ostream (which may be cout, an ofstream, or
 * a general ostream constructed around a file descriptor.
 */
  friend std::ostream& operator<<(std::ostream& os, const HTTPRequest& rh);
  
 public:

/**
 * Ingests, parses, and stores the first line of the HTTP request.
 * Recall that the first line of any valid proxied HTTP request is
 * structured as:
 *
 *   <method> <full-URL> <protocol-and-version>
 *
 * e.g. 
 *
 *   GET http://www.facebook.com/jerry HTTP/1.1
 *   POST http://graph.facebook.com/like?url=www.nytimes.com HTTP/1.1
 */
  void ingestRequestLine(std::istream& instream) throw (HTTPBadRequestException);

/**
 * Ingests everything beyond the first line up to the first
 * blank line (where all lines, including the visibly blank line,
 * end in either "\r\n" or just a "\n").  Recall that each
 * line is generally a name-value pair, so that the accumulation of
 * all header lines might look like this:
 *
 *  <key-1>: <value-1>
 *  <key-2>: <value-2>
 *
 * The first ':' character separates the name from the value (except that the
 * "\r\n" or the "\n" at the end of each line is not considered to be part of
 * the value.  In fact, after the split around the ':', the name and value are
 * generally right and left-trimmed.
 *
 * One caveat: if a header line begins with a blank space, then it isn't introducing
 * a new name.  Instead, the line (after being right-trimmed) is providing a continuation 
 * of the previous line's value.
 */
  void ingestHeader(std::istream& instream, const std::string& clientIPAddress);

/**
 * Ingests everything after the blank line following the header.
 * As opposed to the header section, the payload isn't necessarily
 * pure text (it might be a photo with pixel bytes that look like
 * text, or newlines, or gobbledygook).  That means that the payload
 * portion isn't read in as C++ string text, but as general character
 * data.
 */
  void ingestPayload(std::istream& instream);

/**
 * The next six methods are all const, inlined accessors.
 * Their behaviors should all be obvious.
 */
  const std::string& getMethod() const { return method; }
  const std::string& getURL() const { return url; }
  const std::string& getServer() const { return server; }
  unsigned short getPort() const { return port; }
  const std::string& getPath() const { return path; }
  const std::string& getProtocol() const { return protocol; }

/**
 * Returns true if and only if the supplied, case-insensitive
 * name exists within the collection of (zero or more) name-value
 * pairs.
 */
  bool containsName(const std::string& name) const;
  
 private:
  std::string requestLine;
  HTTPHeader requestHeader;
  HTTPPayload payload;
  
  std::string method;
  std::string url;
  std::string server;
  unsigned short port;
  std::string path;
  std::string protocol;
};

#endif

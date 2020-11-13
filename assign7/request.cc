/**
 * File: http-request.cc
 * ---------------------
 * Presents the implementation of the HTTPRequest class and
 * its friend functions as exported by request.h.
 */

#include <sstream>
#include "request.h"
#include "string-utils.h"
using namespace std;

static const string kWhiteSpaceDelimiters = " \r\n\t";
static const string kProtocolPrefix = "http://";
static const unsigned short kDefaultPort = 80;
void HTTPRequest::ingestRequestLine(istream& instream) throw (HTTPBadRequestException) {
  getline(instream, requestLine);
  if (instream.fail()) {
    throw HTTPBadRequestException("First line of request could not be read.");
  }

  requestLine = trim(requestLine);
  istringstream iss(requestLine);
  iss >> method >> url >> protocol;
  server = url;
  size_t pos = server.find(kProtocolPrefix);
  server.erase(0, kProtocolPrefix.size());
  pos = server.find('/');
  if (pos == string::npos) {
    // url came in as something like http://www.google.com, without the trailing /
    // in that case, least server as is (it'd be www.google.com), and manually set
    // path to be "/"
    path = "/";
  } else {
    path = server.substr(pos);
    server.erase(pos);
  }
  port = kDefaultPort;
  pos = server.find(':');
  if (pos == string::npos) return;
  port = strtol(server.c_str() + pos + 1, NULL, 0); // assume port is well-formed
  server.erase(pos);
}

void HTTPRequest::ingestHeader(istream& instream, const string& clientIPAddress) {
  requestHeader.ingestHeader(instream);
}

bool HTTPRequest::containsName(const string& name) const {
  return requestHeader.containsName(name);
}

void HTTPRequest::ingestPayload(istream& instream) {
  if (getMethod() != "POST") return;
  payload.ingestPayload(requestHeader, instream);
}

ostream& operator<<(ostream& os, const HTTPRequest& rh) {
  const string& path = rh.path;
  os << rh.method << " " << path << " " << rh.protocol << "\r\n";
  os << rh.requestHeader;
  os << "\r\n"; // blank line not printed by request header
  os << rh.payload;
  return os;
}

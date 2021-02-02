/**
 * File: response.cc
 * -----------------
 * Presents the implementation of the HTTPResponse class.
 */

#include "response.h"

#include <sstream>
#include "proxy-exception.h"
#include "string-utils.h"
using namespace std;

/** Public methods and functions **/

void HTTPResponse::ingestResponseHeader(istream& instream) {
  string responseCodeLine;
  getline(instream, responseCodeLine);
  istringstream iss(responseCodeLine);
  string protocol;
  iss >> protocol;
  setProtocol(protocol);
  int code;
  iss >> code;
  setResponseCode(code);
  responseHeader.ingestHeader(instream);
}

void HTTPResponse::ingestPayload(std::istream& instream) {
  payload.ingestPayload(responseHeader, instream);
}

void HTTPResponse::setProtocol(const string& protocol) {
  this->protocol = protocol;
}

void HTTPResponse::setResponseCode(int code) {
  this->code = code;
}

void HTTPResponse::setPayload(const string& payload) {
  this->payload.setPayload(responseHeader, payload);
}

bool HTTPResponse::permitsCaching() const {
  if (!responseHeader.containsName("Cache-Control")) return false;
  const string& cacheControlValue = responseHeader.getValueAsString("Cache-Control");
  if (cacheControlValue.find("private") != string::npos) return false;
  if (cacheControlValue.find("no-cache") != string::npos) return false;
  if (cacheControlValue.find("no-store") != string::npos) return false;
  return getTTL() > 0;
}

int HTTPResponse::getTTL() const {
  if (!responseHeader.containsName("Cache-Control")) return 0;
  const string& cacheControlValue = responseHeader.getValueAsString("Cache-Control");
  size_t pos = cacheControlValue.find("max-age=");
  if (pos == string::npos) return 0;
  string maxAgeValue = cacheControlValue.substr(pos + 8);
  istringstream iss(maxAgeValue);
  int maxAge;
  iss >> maxAge;
  return maxAge;
}

ostream& operator<<(ostream& os, const HTTPResponse& hr) {
  os << hr.protocol << " " << hr.code << " " 
     << hr.getStatusMessage(hr.code) << "\r\n";
  os << hr.responseHeader;
  os << "\r\n"; // blank line not printed by response header
  os << hr.payload;
  return os;
}

/** Private Methods **/

string HTTPResponse::getStatusMessage(int code) const {
  switch (code) {
  case 100: return "Continue";
  case 101: return "Switching Protocols";
  case 200: return "OK";
  case 201: return "Created";
  case 202: return "Accepted";
  case 203: return "Non-Authoritative Information";
  case 204: return "No Content";
  case 205: return "Reset Content";
  case 206: return "Partial Content";
  case 300: return "Multiple Choices";
  case 301: return "Permanently Moved";
  case 302: return "Found";
  case 303: return "See Other";
  case 304: return "Not Modified";
  case 305: return "Use Proxy";
  case 307: return "Temporary Redirect";
  case 400: return "Bad Request";
  case 401: return "Unauthorized";
  case 402: return "Payment Required";
  case 403: return "Forbidden";
  case 404: return "Not Found";
  case 405: return "Method Not Allowed";
  case 406: return "Not Acceptable";
  case 407: return "Proxy Authentication Required";
  case 408: return "Request Timeout";
  case 409: return "Conflict";
  case 410: return "Gone";
  case 500: return "Internal Server Error";
  case 501: return "Not Implemented";
  case 502: return "Bad Gateway";
  case 504: return "Gateway Timeout";
  case 505: return "HTTP Version Not Supported";
  case 510: return "General Proxy Failure";
  default: return "Unknown Code";
  }
}

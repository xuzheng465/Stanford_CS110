/**
 * File: request-handler.cc
 * ------------------------
 * Provides the implementation for the HTTPRequestHandler class.
 */

#include "request-handler.h"
#include "response.h"
#include <socket++/sockstream.h> // for sockbuf, iosockstream
using namespace std;

void HTTPRequestHandler::serviceRequest(const pair<int, string>& connection) throw() {
  sockbuf sb(connection.first);
  iosockstream ss(&sb);
  HTTPResponse response;
  response.setResponseCode(200);
  response.setProtocol("HTTP/1.0");
  response.setPayload("Your IP address is " + connection.second);
  ss << response;
  ss.flush();
}

// the following two methods needs to be completed 
// once you incorporate your HTTPCache into your HTTPRequestHandler
void HTTPRequestHandler::clearCache() {}
void HTTPRequestHandler::setCacheMaxAge(long maxAge) {}

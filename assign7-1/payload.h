/**
 * File: payload.h
 * ---------------
 * Defines the class that helps model the payload (e.g. the 
 * body) of either an HTTP request or an HTTP response.
 */

#ifndef _http_payload_
#define _http_payload_

#include "header.h"

#include <iostream>
#include <string>
#include <vector>

class HTTPPayload {

/**
 * Overloads << so that the serialization of an HTTPPayload can
 * be published to the provided ostream.
 */
  friend std::ostream& operator<<(std::ostream& os, const HTTPPayload& hp);

 public:

/**
 * Ingests the entire payload from the provided istream, relying
 * on information present in the supplied HTTPHeader to determine
 * the payload size, and whether or not the payload is complete
 * or chunked.
 */
  void ingestPayload(const HTTPHeader& header, std::istream& instream);

/**
 * Sets the payload to be equal to the stream of characters contained
 * in the payload string.
 */
  void setPayload(HTTPHeader& header, const std::string& payload);

 private:
  std::vector<char> payload;
  bool isChunkedPayload(const HTTPHeader& header) const;
  void ingestChunkedPayload(std::istream& instream);
  void ingestCompletePayload(std::istream& instream, size_t contentLength);
  void appendData(const std::string& content);
  void appendData(const std::vector<char>& content);
};

#endif

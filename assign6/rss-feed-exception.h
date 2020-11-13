/**
 * File: rss-feed-exception.h
 * --------------------------
 * Defines the exception type thrown whenever some network
 * or parsing issue prevents an entire RSS feed document from being
 * parsed properly.
 */

#pragma once
#include <exception>
#include <string>

class RSSFeedException: public std::exception {
 public: 
  RSSFeedException(const std::string& message) throw() : message(message) {}
  ~RSSFeedException() throw() {}
  const char *what() const throw() { return message.c_str(); }
  
 private:
  const std::string message;
};

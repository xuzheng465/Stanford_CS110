/**
 * File: rss-feed-list-exception.h
 * -------------------------------
 * Defines the HTML exception thrown whenever some network
 * or parsing issue prevents an entire RSS feed list from being
 * parsed properly.
 */

#pragma once
#include <exception>
#include <string>

class RSSFeedListException: public std::exception {
 public:
  RSSFeedListException(const std::string& message) throw() : message(message) {}
  ~RSSFeedListException() throw() {}
  const char *what() const throw() { return message.c_str(); }
  
 private:
  const std::string message;
};

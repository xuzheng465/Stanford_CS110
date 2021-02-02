/**
 * File: html-document-exception.h
 * -------------------------------
 * Defines the HTML exception thrown whenever some network
 * or parsing issue prevents an HTML document from being
 * parsed properly.
 */

#pragma once
#include <exception>
#include <string>

class HTMLDocumentException: public std::exception {
 public: 
  HTMLDocumentException(const std::string& message) throw() : message(message) {}
  ~HTMLDocumentException() throw() {}
  const char *what() const throw() { return message.c_str(); }
  
 private:
  const std::string message;
};

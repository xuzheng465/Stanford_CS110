/**
 * File: trace-exception.h
 * -----------------------
 * Defines the hierarchy of exception classes used to
 * create a bulletproof implementation of the trace tool.
 */

#pragma once
#include <exception>
#include <string>

class TraceException: public std::exception {
  public:
    TraceException(const std::string& message): message(message) {}
    const char *what() const noexcept { return message.c_str(); }
  
  private:
    std::string message;
};

class MissingFileException: public TraceException {
  public:
    MissingFileException(const std::string& message): TraceException(message) {}
};

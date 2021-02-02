/**
 * File: subprocess-exception.h
 * ----------------------------
 * Defines an exception class used to identify problems with the
 * subprocess function.
 */
 
#pragma once
#include <exception>
#include <string>

class SubprocessException: public std::exception {
  public:
    SubprocessException(const std::string& message): message(message) {}
    const char *what() const noexcept { return message.c_str(); }
  
  private:
    std::string message;
};

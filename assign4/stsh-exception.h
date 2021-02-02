/**
 * File: stsh-exception.h
 * ----------------------------
 * Defines and inline-implements a custom exception
 * that gets thrown if there are any problems encountered while stsh
 * is running.
 * 
 * Operative idiom with any function or method capable of throwing
 * an STSHException:
 *
 *     try {
 *        function();
 *        obj.method();
 *     } catch (const STSHException& e) {
 *        cerr << e.message() << endl;
 *     }
 */

#pragma once
#include <string>

class STSHException: public std::exception {

 public:
  STSHException(const std::string& message) : message(message) {}
  const char *what() const throw () { return message.c_str(); }

 private:
  std::string message;
};

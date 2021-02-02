/**
 * Holds a few convenience functions for trimming the whitespace
 * from the ends of C++ strings.
 */

#ifndef _string_utils_
#define _string_utils_

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

inline bool startsWith(const std::string& str, const std::string& prefix) {
  return str.find(prefix) == 0;
}

inline bool endsWith(const std::string& str, const std::string& suffix) {
  return (str.size() >= suffix.size()) && equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

inline std::string& ltrim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

inline std::string& rtrim(std::string& s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

inline std::string& trim(std::string &s) {
  return rtrim(ltrim(s));
}

inline std::string toLowerCase(std::string s) {
  for (char& ch: s) {
    ch = tolower(ch);
  }
  return s;
}

#endif

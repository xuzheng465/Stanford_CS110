/**
 * File: proxy-exception.h
 * -----------------------
 * Defines the hierarchy of exceptions used through proxy code base.
 */

#ifndef _http_proxy_exception_
#define _http_proxy_exception_

#include <exception>
#include <string>

class HTTPProxyException: public std::exception {
 public:
  HTTPProxyException() throw() {}
  HTTPProxyException(const std::string& message) throw() : message(message) {}
  const char *what() const throw() { return message.c_str(); }

 protected:
  std::string message;
};

class HTTPCacheConfigException: public HTTPProxyException {
 public:
  HTTPCacheConfigException() throw() {}
  HTTPCacheConfigException(const std::string& message) throw() : HTTPProxyException(message) {}
};

class HTTPCacheAccessException: public HTTPProxyException {
 public:
  HTTPCacheAccessException() throw() {}
  HTTPCacheAccessException(const std::string& message) throw() : HTTPProxyException(message) {}
};

class HTTPBadRequestException: public HTTPProxyException {
 public:
  HTTPBadRequestException() throw() {}
  HTTPBadRequestException(const std::string& message) throw() : HTTPProxyException(message) {}
};

class HTTPRequestException: public HTTPProxyException {
 public:
  HTTPRequestException() throw() {}
  HTTPRequestException(const std::string& message) throw() : HTTPProxyException(message) {}
};

class HTTPResponseException: public HTTPProxyException {
 public:
  HTTPResponseException() throw() {}
  HTTPResponseException(const std::string& message) throw() : HTTPProxyException(message) {}
};

class HTTPCircularProxyChainException: public HTTPProxyException {
 public:
  HTTPCircularProxyChainException() throw() {}
  HTTPCircularProxyChainException(const std::string& message) throw() : HTTPProxyException(message) {}
};

#endif

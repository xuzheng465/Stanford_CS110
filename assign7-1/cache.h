/**
 * File: cache.h
 * -------------
 * Defines a class to help manage an HTTP response cache.
 */

#ifndef _cache_
#define _cache_

#include <cstdlib>
#include <string>
#include <mutex>
#include <sys/time.h>
#include "request.h"
#include "response.h"

class HTTPCache {
 public:

/**
 * Constructs the HTTPCache object.
 */
  HTTPCache();

/**
 * The following three functions do what you'd expect, except that they 
 * aren't thread safe.  In a MT environment, you should acquire the lock
 * on the relevant request before calling.
 */
  bool containsCacheEntry(const HTTPRequest& request, HTTPResponse& response) const;
  bool shouldCache(const HTTPRequest& request, const HTTPResponse& response) const;
  void cacheEntry(const HTTPRequest& request, const HTTPResponse& response);

/**
 * Clears the cache of all entries.
 */
  void clear();
  
/**
 * Sets the maximum number of seconds a cacheable response can live in the cache
 * before it's expunged.  -1 means don't override response's decision on how long a document
 * should be cached, 0 means never cache, any positive number represents how many seconds
 * a cacheable item is allowed to remain in the cache from the time it was placed there.
 */
  void setMaxAge(long maxAge) { this->maxAge = maxAge; }
  
 private:
  std::string getCacheDirectory() const;
  size_t hashRequest(const HTTPRequest& request) const;
  std::string hashRequestAsString(const HTTPRequest& request) const;
  std::string serializeRequest(const HTTPRequest& request) const;
  bool cacheEntryExists(const std::string& filename) const;
  std::string getRequestHashCacheEntryName(const std::string& requestHash) const;
  void ensureDirectoryExists(const std::string& directory, bool empty = false) const;
  std::string getCurrentTime() const;
  std::string getExpirationTime(int ttl) const;
  bool cacheEntryFileNameIsProperlyStructured(const std::string& cachedFileName) const;
  void extractCreateAndExpireTimes(const std::string& cachedFileName, time_t& createTime, time_t& expirationTime) const;
  bool cachedEntryIsValid(const std::string& cachedFileName) const;
  std::string getHostname() const;

  long maxAge;
  std::string cacheDirectory;
};

#endif

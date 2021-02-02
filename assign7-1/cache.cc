/**
 * File: cache.cc
 * --------------
 * Presents the implementation of the HTTPCache class as
 * presented in cache.h.
 *
 * Overall design:
 * + The user's home folder is populated with a hidden directory 
 *   named .proxy-cache-myth31 (or whatever the hostname happens to be).
 *   The directory is hidden to emphasize the fact that it's a configuration
 *   directory for the proxy executable, similar to .emacs, .cvsroot, .gnome,
 *   .ssh, etc.
 *   + Each entry within .proxy-cache-<hostname> is also a directory, and each represents
 *     some HTTP response that was cached.  The name of the directory entry is the hashcode
 *     of the entire HTTP request, because that's easily produced from just the HTTPRequest
 *     before attempting to download the file.
 *     + Each hashcode directory contains a single file storing the full HTTPResponse that was
 *       cached.  The name of the file is structured as "created@<create-time>expires@<expiration-time>.
 */

#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <functional>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>

#include "cache.h"
#include "request.h"
#include "response.h"
#include "proxy-exception.h"
#include "ostreamlock.h"
#include "string-utils.h"
using namespace std;

HTTPCache::HTTPCache(): maxAge(-1) {
  cacheDirectory = getCacheDirectory();
  ensureDirectoryExists(cacheDirectory);
}

static const string kCacheSubdirectoryPrefix = ".proxy-cache";
string HTTPCache::getCacheDirectory() const {
  struct passwd *pwd = getpwuid(getuid());
  if (pwd == NULL || pwd->pw_dir == NULL) {
    ostringstream oss;
    oss << "Home directory could not be determined.  Aborting...";
    throw HTTPProxyException(oss.str());
  }

  string homeDirectory(pwd->pw_dir);
  string hostname = getHostname();
  return homeDirectory + "/" + kCacheSubdirectoryPrefix + "-" + hostname;
}

void HTTPCache::clear() {
  cout << "Clearing the cache... wait for it.... " << flush;
  sleep(2); // just for dramatic effect
  ensureDirectoryExists(cacheDirectory, /* empty = */ true);
  cout << "done!" << endl;
}

bool HTTPCache::shouldCache(const HTTPRequest& request, const HTTPResponse& response) const {
  return maxAge != 0 &&
    request.getMethod() == "GET" && 
    response.getResponseCode() == 200 && 
    response.permitsCaching();
}

bool HTTPCache::containsCacheEntry(const HTTPRequest& request, HTTPResponse& response) const {
  if (maxAge == 0) return false; // maxAge of 0 means nothing is in the cache and we're not caching anything
  if (request.getMethod() != "GET") return false;
  string requestHash = hashRequestAsString(request);
  bool exists = cacheEntryExists(requestHash);
  if (!exists) return false;
  string cachedFileName = getRequestHashCacheEntryName(requestHash);
  if (cachedFileName.empty()) return false;
  string fullCacheEntryName = cacheDirectory + "/" + requestHash + "/" + cachedFileName;
  if (!cachedEntryIsValid(cachedFileName)) { // if it's not valid, then remove it
    cout << oslock << "     [Cache entry with hash of " << requestHash << " has expired... removing...]" << endl << osunlock;
    if (remove(fullCacheEntryName.c_str()) != 0) // remove calls unlink for regular files
      throw HTTPCacheAccessException("Failed to remove the now-expired cache entry named \"" +
                                     fullCacheEntryName + "\".");
    string fullCacheDirectoryName = cacheDirectory + "/" + requestHash;
    if (rmdir(fullCacheDirectoryName.c_str()) != 0) // rmdir succeeds provided the supplied directory is empty (remove would have works as well)
      throw HTTPCacheAccessException("Failed to remove the now-expired cache entry directory named \"" +
                                     fullCacheDirectoryName + "\".");
    return false;
  }

  ifstream instream(fullCacheEntryName.c_str(), ios::in | ios::binary);
  if (!instream)
    throw HTTPCacheAccessException("Unable to open the cache entry named \"" +
                                   fullCacheEntryName + "\" for reading.");
  try {
    response.ingestResponseHeader(instream);
    response.ingestPayload(instream);
    cout << oslock << "     [Using cached copy of previous request for " << request.getURL() << ".]" << endl << osunlock;
    return true;
  } catch (const HTTPProxyException& hpe) {
    cerr << oslock << "     [Problem rehydrating previously cached response for " << request.getURL() << ".]" << endl;
    cerr << "     [Operating as if cached entry doesn't exist and forwarding request to origin server.]" << endl << osunlock;
    return false;
  }
}

static string kCreateHeader = "created@";
static string kExpirationHeader = "expires@";
void HTTPCache::cacheEntry(const HTTPRequest& request, const HTTPResponse& response) {
  string requestHash = hashRequestAsString(request);
  int ttl = response.getTTL();
  if (maxAge > 0) ttl = min<long>(maxAge, ttl);
  string unit = ttl == 1 ? "second" : "seconds";
  cout << oslock << "     [Okay to cache response, so caching response under hash of " 
       << requestHash << " for " << ttl << " " << unit << ".]" << endl << osunlock;
  ensureDirectoryExists(cacheDirectory + "/" + requestHash, /* empty = */ true);
  string cacheEntryName = 
    cacheDirectory + "/" + 
    requestHash + "/" + 
    kCreateHeader + getCurrentTime() + kExpirationHeader + getExpirationTime(response.getTTL());
  ofstream outfile(cacheEntryName.c_str(), ios::out | ios::binary);
  if (!outfile)
    throw HTTPCacheAccessException("Unable to open the cache entry named \"" + cacheEntryName + "\" for writing.");
  outfile << response;
  outfile.flush();
}

size_t HTTPCache::hashRequest(const HTTPRequest& request) const {
  hash<string> hasher;
  return hasher(serializeRequest(request));  
}

string HTTPCache::hashRequestAsString(const HTTPRequest& request) const {
  ostringstream oss;
  oss << hashRequest(request);
  return oss.str();
}

string HTTPCache::serializeRequest(const HTTPRequest& request) const {
  ostringstream oss;
  oss << request;
  return oss.str();
}

bool HTTPCache::cacheEntryExists(const string& requestHash) const {
  string path = cacheDirectory + "/" + requestHash;
  struct stat st;
  if (stat(path.c_str(), &st) != 0) return false;
  if (!S_ISDIR(st.st_mode)) return false;
  
  DIR *dir = opendir(path.c_str());
  if (dir == NULL) 
    throw HTTPCacheAccessException("Cache directory exists, but we don't "
                                   "have permission to open it to decide if \"" + 
                                   requestHash + "\" exists");

  bool exists = false;
  while (!exists) {
    struct dirent entry, *result;
    if (readdir_r(dir, &entry, &result) != 0)
      throw HTTPCacheAccessException("Failed to surface one of the cache directory entries.");
    if (result == NULL) break;
    string dirEntry = entry.d_name;
    exists = dirEntry != "." && dirEntry != "..";
  }

  if (closedir(dir) != 0)
    throw HTTPCacheAccessException("Failed to close the cache directory after clearing its contents.");
  return exists;
}

string HTTPCache::getRequestHashCacheEntryName(const string& requestHash) const {
  string requestHashDirectory = cacheDirectory + "/" + requestHash;
  DIR *dir = opendir(requestHashDirectory.c_str());
  if (dir == NULL) return ""; // just assume it doesn't exist

  string cachedEntryName;
  while (true) {
    struct dirent entry, *result;
    if (readdir_r(dir, &entry, &result) != 0)
      throw HTTPCacheAccessException("Failed to surface one of the cache directory entries.");
    if (result == NULL) break;
    cachedEntryName = entry.d_name;
    if (cachedEntryName != "." && cachedEntryName != "..") break;
  }

  if (closedir(dir) != 0)
    throw HTTPCacheAccessException("Failed to close the cache directory after clearing its contents.");
  if (cachedEntryName == ".." || cachedEntryName == ".") return "";
  return cachedEntryName;
}

static const int kDefaultPermissions = 0755;
void HTTPCache::ensureDirectoryExists(const string& directory, bool empty) const {
  struct stat st;
  if (lstat(directory.c_str(), &st) != 0) {
    if (errno != ENOENT)
      throw HTTPCacheConfigException("Cache directory exists, but some "
                                     "problem prevents us from accessing it.");
    if (mkdir(directory.c_str(), kDefaultPermissions) == -1)
      throw HTTPCacheConfigException("Failed to create cache directory at \"" + directory + "\".");
  } else {
    if (!S_ISDIR(st.st_mode))
      throw HTTPCacheConfigException("File named \"" + directory +
                                     "\" exists, but it is not a directory.");
  }

  if (!empty) return;
  if (!startsWith(directory, cacheDirectory))
    throw HTTPCacheConfigException("You are nuts for calling this function on a directory not rooted in your proxy cache.");

  DIR *dir = opendir(directory.c_str());
  if (dir == NULL) throw HTTPCacheAccessException("Cache directory exists, but we don't "
                                                  "have permission to open it to clear its entries.");
  while (true) {
    struct dirent entry, *result;
    if (readdir_r(dir, &entry, &result) != 0)
      throw HTTPCacheAccessException("Failed to surface one of the cache directory entries.");
    if (result == NULL) break;
    string dirEntry = entry.d_name;
    if (dirEntry == "." || dirEntry == "..") continue;
    dirEntry = directory + "/" + dirEntry;
    if (lstat(dirEntry.c_str(), &st) != 0)
      throw HTTPCacheConfigException(string("Cache is malformed... manually delete ") + dirEntry + " " + strerror(errno));
    if (S_ISDIR(st.st_mode))
      ensureDirectoryExists(dirEntry, /* empty = */ true); // recursively clear it before removing
    if (remove(dirEntry.c_str()) != 0)
      throw HTTPCacheAccessException("Failed to remove cache entry named \"" + dirEntry + "\".");
  }
  
  if (closedir(dir) != 0)
    throw HTTPCacheAccessException("Failed to close some cache directory after clearing its contents.");
}

string HTTPCache::getCurrentTime() const { // not influences by maxAge at all
  struct timeval tv;
  gettimeofday(&tv, NULL); // no error possible when just getting the time
  ostringstream oss;
  oss << tv.tv_sec;
  return oss.str();
}

string HTTPCache::getExpirationTime(int ttl) const {
  if (maxAge >= 0) ttl = min<long>(maxAge, ttl);
  struct timeval tv;
  gettimeofday(&tv, NULL); // no error possible when just getting the time
  if (ttl >= 0) tv.tv_sec += ttl;
  ostringstream oss;
  oss << tv.tv_sec;
  return oss.str();
}

bool HTTPCache::cacheEntryFileNameIsProperlyStructured(const string& cachedFileName) const {
  size_t headers[] = { cachedFileName.find(kCreateHeader), cachedFileName.find(kExpirationHeader) };
  return headers[0] == 0 && headers[1] != string::npos && headers[1] > headers[0];
} // test could be tighter, but I'll let it function more as a sanitycheck until we can use regexes with g++-5.3

void HTTPCache::extractCreateAndExpireTimes(const string& cachedFileName, time_t& createTime, time_t& expirationTime) const {
  if (!cacheEntryFileNameIsProperlyStructured(cachedFileName)) {
    throw HTTPCacheAccessException(cachedFileName + " doesn't confirm to the naming convention we expect... clear the cache.");
  }
  
  size_t first = cachedFileName.find(kCreateHeader) + string(kCreateHeader).size();
  size_t second = cachedFileName.find(kExpirationHeader) + string(kExpirationHeader).size();
  createTime = stol(cachedFileName.substr(first, cachedFileName.find(kExpirationHeader) - first));
  expirationTime = stol(cachedFileName.substr(second));
}

bool HTTPCache::cachedEntryIsValid(const string& cachedFileName) const {
  time_t createTime, expirationTime;
  extractCreateAndExpireTimes(cachedFileName, createTime, expirationTime);
  if (maxAge > 0) expirationTime = min<long>(createTime + maxAge, expirationTime);
  cout << oslock << "     [Cache entry created at " << createTime << ", expires at " << expirationTime << ".]" << endl << osunlock;  
  struct timeval tv;
  gettimeofday(&tv, NULL); // no error possible when just getting the time
  return tv.tv_sec <= expirationTime;
}

string HTTPCache::getHostname() const {
  char name[HOST_NAME_MAX + 1];
  if (gethostname(name, HOST_NAME_MAX + 1) == -1) // function is thread safe
    throw HTTPCacheConfigException("Could not determine the name of your machine.");
  return name;
}

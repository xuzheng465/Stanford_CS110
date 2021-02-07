/**
 * File: scheduler.h
 * -----------------
 * This class defines the HTTPProxyScheduler class, which eventually takes all
 * proxied requests off of the main thread and schedules them to 
 * be handled by a constant number of child threads.
 */

#ifndef _scheduler_
#define _scheduler_
#include <string>
#include "request-handler.h"

class HTTPProxyScheduler {
 public:
  void setProxy(const std::string& server, unsigned short port);
  void clearCache() { requestHandler.clearCache(); }
  void setCacheMaxAge(long maxAge) { requestHandler.setCacheMaxAge(maxAge); }
  void scheduleRequest(int clientfd, const std::string& clientIPAddr) throw ();
  
 private:
  HTTPRequestHandler requestHandler;
};

#endif

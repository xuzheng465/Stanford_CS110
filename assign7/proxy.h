/**
 * File: proxy.h
 * -------------
 * Defines the primary class that manages all HTTP proxying.  It's
 * primary responsibility is to accept all incoming connection requests
 * and to get them off the main thread as quickly as possible (by passing
 * the file descriptor to the scheduler).
 */

#ifndef _http_proxy_
#define _http_proxy_

#include "scheduler.h"
#include "proxy-exception.h"
#include <string>
#include <utility>

class HTTPProxy {
 public:

/**
 * Configures the proxy server to listen for incoming traffic on the
 * specified port.  If the specified port is in use, or the HTTP Proxy
 * server otherwise can't bind to it, then an exception is thrown.
 */
  HTTPProxy(int argc, char *argv[]) throw (HTTPProxyException);

/**
 * Returns the port number our proxy is listening to.
 */
  unsigned short getPortNumber() const { return portNumber; }

/**
 * Returns true if and only if our proxy is directing all requests
 * to another proxy instead of the intended origin servers.
 */
  bool isUsingProxy() const { return usingProxy; }

/**
 * Returns a reference to the proxy server this proxy
 * is actually directing proxy traffic toward, provided this
 * proxy itself is configured to direct all requests to another
 * proxy.  If this method is called even though a secondary proxy
 * isn't being used, then the behavior is undefined.
 */
  const std::string& getProxyServer() const { return proxyServer; };

/**
 * Provided this proxy is directing its traffic toward another proxy
 * instead of the origin servers, this method returns the port number
 * which the secondary proxy is listening to.  If this method is called
 * even though a secondary proxy isn't being used, then the behavior is
 * undefined.
 */
  unsigned short getProxyPortNumber() const { return proxyPortNumber; }

/**
 * Waits for an HTTP request to come in, and does whatever it takes
 * to handle it.  Because acceptAndProxyRequest is assumed to be
 * called to handle a single request as opposed to all of them, we
 * assume any and all exceptions thrown within are just for that request,
 * so we further assume exceptions are handled internally and not
 * thrown.
 */
  void acceptAndProxyRequest() throw(HTTPProxyException);
  
 private:
  unsigned short portNumber;
  bool usingProxy;
  bool usingSpecificProxyPortNumber;
  std::string proxyServer;
  unsigned short proxyPortNumber;
  int listenfd;
  HTTPProxyScheduler scheduler;
  
  /* private methods */
  void configureFromArgumentList(int argc, char *argv[]) throw (HTTPProxyException);
  void createServerSocket();
  void configureServerSocket() const;
  const char *getClientIPAddress(const struct sockaddr_in *clientAddr) const;
};

#endif

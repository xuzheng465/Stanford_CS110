/**
 * File: proxy.cc
 * -------------------
 * Presents the implementation of all HTTPProxy class methods as
 * defined in proxy.h.
 */

#include "proxy.h"
#include <cstring>
#include <sstream> 
#include <string>
#include <climits>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <getopt.h>
#include <unistd.h>
#include "proxy-options.h"
#include "proxy-exception.h"
using namespace std;

/** Public constructor and methods **/

/**
 * Constructs the HTTPProxy so that it listens
 * for all traffic on the specified port number.
 * If there are any problems at all (e.g. listen
 * socket cannot be created, or it can't be bound
 * to the specified port number), then an HTTPProxyException
 * is thrown.
 */
static const int kUnitializedSocket = -1;
HTTPProxy::HTTPProxy(int argc, char *argv[]) throw (HTTPProxyException) :
  portNumber(computeDefaultPortForUser()), usingProxy(false),
  usingSpecificProxyPortNumber(false), proxyPortNumber(computeDefaultPortForUser()), 
  listenfd(kUnitializedSocket) {
  try {
    configureFromArgumentList(argc, argv);
    createServerSocket();
    configureServerSocket();
  } catch (const HTTPProxyException& hpe) {
    if (listenfd != kUnitializedSocket) {
      close(listenfd);
    }
    throw;
  }
}

/**
 * Method: acceptAndProxyRequest
 * -----------------------------
 * General umbrella method that blocks until a request
 * is detected.  When a request is detected, the IP address
 * of the requesting host is extracted, and the request is
 * proxied on to the origin server.
 */
void HTTPProxy::acceptAndProxyRequest() throw (HTTPProxyException) {
  struct sockaddr_in clientAddr;
  socklen_t clientAddrSize = sizeof(clientAddr);
  int connectionfd = accept(listenfd, (struct sockaddr *) &clientAddr, &clientAddrSize);
  if (connectionfd < 0) {
    // connectionfd isn't open, so we're not orphaning any resources
    throw HTTPProxyException
      ("Call to accept failed to return a valid client socket.");
  }
  
  const char *clientIPAddress = getClientIPAddress(&clientAddr);
  try {
    scheduler.scheduleRequest(connectionfd, clientIPAddress);
  } catch (...) {
    cerr << "General failure while in communication with " << clientIPAddress << "." << endl;
    cerr << "But it's just one connection, so we're ignoring..." << endl;
  }
}

/** Private methods **/

static const string kUsageString = 
   "Usage: proxy [--port <port-number>] [--proxy-server <proxy-server> [--proxy-port <port-number>]] [--clear-cache] [--max-age <max-cache-time>]";
void HTTPProxy::configureFromArgumentList(int argc, char *argv[]) throw (HTTPProxyException) {
  struct option options[] = {
    {"port", required_argument, NULL, 'p'},
    {"proxy-port", required_argument, NULL, 'r'},
    {"proxy-server", required_argument, NULL, 's'},
    {"clear-cache", no_argument, NULL, 'c'},
    {"max-age", required_argument, NULL, 'm'},
    {NULL, 0, NULL, 0},
  };

  ostringstream oss;
  pair<string, unsigned short> proxy;
  while (true) {
    int ch = getopt_long(argc, argv, "p:r:s:cm:", options, NULL);
    if (ch == -1) break;
    switch (ch) {
    case 'p':
      portNumber = extractPortNumber(optarg, "--port/-p");
      break;
    case 's':
      proxyServer = extractProxyServer(optarg);
      usingProxy = true;
      break;
    case 'r':
      proxyPortNumber = extractPortNumber(optarg, "--proxy-port/-r");
      usingSpecificProxyPortNumber = true;
      break;
    case 'c':
      scheduler.clearCache();
      break;
    case 'm':
      scheduler.setCacheMaxAge(extractLongInRange(optarg, -1, LONG_MAX, "--max-age/-m"));
      break;
    default:
      oss << "Unrecognized or improperly supplied flag passed to proxy." << endl;
      oss << kUsageString;
      throw HTTPProxyException(oss.str());
    }
  }

  argc -= optind;
  if (argc > 0) {
    oss << "Too many arguments passed to proxy." << endl;
    oss << kUsageString;
    throw HTTPProxyException(oss.str());
  }

  if (!usingProxy && usingSpecificProxyPortNumber) {
    oss << "--proxy-port can only be specified when --proxy-server is as well." << endl;
    oss << kUsageString;
    throw HTTPProxyException(oss.str());
  }

  if (!usingSpecificProxyPortNumber) {
    proxyPortNumber = portNumber;
  }
}

/**
 * Creates a server socket and configures it to
 * be closed more or less immediately if the surrounding
 * application dies or is killed.
 */
void HTTPProxy::createServerSocket() {
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) {
    throw HTTPProxyException
      ("Failed to open a primary socket to poll for connections.");
  }
  
  // the following configures the socket to be auto-closed within a
  // second if the surrounding process dies.  Otherwise, the socket might
  // not be available for up to a minute, and that makes iterative development
  // more difficult to manage if the easier way to kill the proxy server is
  // to just type Ctrl-C.
  const int optval = 1;
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval , sizeof(int));
}

/**
 * Further configures the server socket created via
 * createServerSocket so that it listens for activity from
 * any host whatsoever on the port number passed in to the
 * HTTPProxy constructor.  ::bind actually ties the socket
 * to the provided port number, and listen clarifies how many
 * pending connections can be queued up before the proxy starts
 * refusing connections.
 */
void HTTPProxy::configureServerSocket() const {
  struct sockaddr_in serverAddr;
  bzero(&serverAddr, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(portNumber);
  struct sockaddr *sa = (struct sockaddr *) &serverAddr;

  // ::bind needed to be clear that global bind in the C libs is 
  // being called instead of the std::bind from C++'s STL.
  if (::bind(listenfd, sa, sizeof(serverAddr)) < 0) {
    ostringstream oss;
    oss << "Failed to associate listening socket with port " << portNumber << ".";
    throw HTTPProxyException(oss.str());
  }
  
  const size_t kMaxQueuedRequests = 128; // 128 is the largest allowed value
  if (listen(listenfd, kMaxQueuedRequests) < 0) {
    throw HTTPProxyException
      ("Failed to set listening socket to accept connection requests");
  }
}

/**
 * Converts the IP addressed embedded within the supplied sockaddr
 * to C string form.
 */
const char *HTTPProxy::getClientIPAddress(const struct sockaddr_in *clientAddr) const {
  const char *clientIPAddr = inet_ntoa(clientAddr->sin_addr);
  if (clientIPAddr == NULL) {
    throw HTTPProxyException
      ("Failed to extract an IP address from the client connection.");
  }
  
  return clientIPAddr;
}

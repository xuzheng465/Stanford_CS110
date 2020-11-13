/**
 * Provides a simple implementation of an http proxy and web cache.  
 * Save for the parsing of the command line, all bucks are passed
 * to an HTTPProxy proxy instance, which repeatedly loops and waits 
 * for proxied requests to come through.
 */

#include <iostream> // for cerr
#include <csignal>  // for sigaction, SIG_IGN

#include "proxy.h"
#include "proxy-exception.h"
#include "ostreamlock.h"

using namespace std;

/**
 * Function: alertOfBrokenPipe
 * ---------------------------
 * Simple fault handler that occasionally gets invoked because
 * some pipe is broken.  We actually just print a short message but
 * otherwise allow execution to continue.
 */
static void alertOfBrokenPipe(int unused) {
  cerr << oslock << "Client closed socket.... aborting response." 
       << endl << osunlock;
}

/**
 * Function: killProxyServer
 * -------------------------
 * Simple fault handler that ends the program.
 */
static void killProxyServer(int unused) {
  cout << endl << "Shutting down proxy." << endl;
  exit(0);
}

/**
 * Function: handleBrokenPipes
 * ---------------------------
 * Configures the entire system to trivially handle all broken
 * pipes.  The alternative is to let a single broken pipe bring
 * down the entire proxy.
 */
static void handleBrokenPipes() {
  struct sigaction act;
  act.sa_handler = alertOfBrokenPipe;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(SIGPIPE, &act, NULL);
}

/**
 * Function: handleKillRequests
 * ----------------------------
 * Configures the entire system to quit on 
 * ctrl-c and ctrl-z.
 */
static void handleKillRequests() {
  struct sigaction act;
  act.sa_handler = killProxyServer;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGTSTP, &act, NULL);
}

/**
 * Function: main
 * --------------
 * Defines the simple entry point to the entire proxy
 * application, which save for the configuration issues
 * passes the buck to an instance of the HTTPProxy class.
 */
static const int kFatalHTTPProxyError = 1;
int main(int argc, char *argv[]) {
  handleKillRequests();
  handleBrokenPipes();
  try {
    HTTPProxy proxy(argc, argv);
    cout << "Listening for all incoming traffic on port " << proxy.getPortNumber() << "." << endl;
    if (proxy.isUsingProxy()) {
      cout << "Requests will be directed toward another proxy at " 
           << proxy.getProxyServer() << ":" << proxy.getProxyPortNumber() << "." << endl;
    }
    while (true) {
      proxy.acceptAndProxyRequest();
    }
  } catch (const HTTPProxyException& hpe) {
    cerr << "Fatal Error: " << hpe.what() << endl;
    cerr << "Exiting..... " << endl;
    return kFatalHTTPProxyError;
  }
  
  return 0; // never gets here, but it feels wrong to not type it
}

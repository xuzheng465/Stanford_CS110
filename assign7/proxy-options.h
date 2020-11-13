/**
 * File: proxy-options.h
 * ---------------------
 * Defines a collection of functions that can be
 * uses to parse the command line options available
 * to proxy.  In particular, the following option flags are available to the proxy:
 *
 *  --port <port-number>: allows the user to specify the port to 
 *                        use instead of the default
 *  --proxy <proxy-server>: allows the proxy to itself direct traffic to
 *                          another proxy. We impose the simplification that
 *                          the ports for the primary and secondary proxies
 *                          be there same unless the proxy-port flags is also supplied.
 *  --proxy-port <port-number>: allows the user to specify the port of the secondary proxy
 *                              if the primary-proxy port can't or shouldn't be used.
 *  --max-age <max-cache-time>: overrides the amount of time an entry is permitted to
 *                              to stay in the cache (-1 means no override, 0 means don't
 *                              cache and ignore all cache entries, and a positive number max-cache-time
 *                              means ensure the expiry time of new cache entries is set to
 *                              min(current-time + max-cache-time, current-time + ttl).  When
 *                              a preexisting cache entry is found, we ignore the true expiration
 *                              time if current-time + max-cache-time is smaller, in which case we
 *                              go with that
 */

#pragma once
#include "proxy-exception.h"
#include <string>

/**
 * Function: computeDefaultPortForUser
 * -----------------------------------
 * Constructs the default port for the logged in user.  The
 * implementation uses the logged in user's SUNet ID to generate a
 * port number that, with high probability, won't collide with
 * that generated for other users.
 */
unsigned short computeDefaultPortForUser();

/**
 * Function: extractPortNumber
 * ---------------------------
 * Accepts the provided portArgument string (assumed to be the C string
 * form of an unsigned short) and managed the conversion to an actual unsigned
 * short.  If there are any problems at all with the supplied portArgument,
 * an HTTPProxyException is thrown.
 */
unsigned short extractPortNumber(const char *portArgument, const char *flags) throw (HTTPProxyException);

/**
 * Function: extractProxyServer
 * ----------------------------
 * Validates the structure of the supplied proxyArgument by confirming it's
 * neither NULL nor the empty string.  Everything else is considered legitimate
 * and converted to a C++ string before returning it.
 */
std::string extractProxyServer(const char *proxyArgument) throw (HTTPProxyException);

/**
 * Function: extractLongInRange
 * ----------------------------
 * Convenience function that converts a numeric string into the equivalent
 * long integer and returns it.  If there are any problems (number doesn't
 * fit in a long, string isn't purely numeric), then an HTTPProxyException is thrown.
 */
long extractLongInRange(const char *str, long min, long max, const char *flags) throw (HTTPProxyException);

# Announcements
* Assignments 5 and 6
    * Assignment 5 Due Tonight at 11:59pm.
    * Assignment 6 Out Today, Due Wednesday, November 15<sup>th</sup> at 11:59pm.

* Today's Agenda
    * Cover the implementation of several simple servers.
    * Cover the implementation of a simple client application.
    * Cover the implementation of a program called <b><tt>web-get</tt></b> to emulate
      the functionality of a Linux user program called <b><tt>wget</tt></b>.
        * We'll illustrate how <b><tt>web-get</tt></b> needs to work.
        * We'll implement it in lecture.
        * We'll reveal just how deliberate the HTTP-guided conversation is to download
          an HTML document, an image, a video, or whatever else can be retrived using HTTP.
    * Time permitting, we'll discuss the Unix data structures used to realize the implementation
      of <b><tt>createServerSocket</tt></b> and <b><tt>createClientSocket</tt></b>.

# Hostname Resolution
* <tt><b>gethostbyname</b></tt> and <tt><b>gethostbyaddr</b></tt>
    * Linux C includes directives to convert host names (e.g. "www.facebook.com") to
      IP address (e.g. "31.13.75.17") and vice versa.  Functions
      called <tt><b>gethostbyname</b></tt> and <tt><b>gethostbyaddr</b></tt>, while
      technically deprecated, are still so prevalent that you should know how to use them.
      In fact, your B&amp;O textbook only mentions these deprecated functions:

	~~~{.cpp}
    struct hostent *gethostbyname(const char *name);
    struct hostent *gethostbyaddr(const char *addr, int len, int type);
	~~~

    * Each function populates a statically allocated <tt><b>struct hostent</b></tt>
      describing some host machine on the Internet.
        * <tt><b>gethostbyname</b></tt> assumes argument is a host name, as with
          <tt><b>www.google.com</b></tt>.
        * <tt><b>gethostbyaddr</b></tt> assumes the first argument
          is a binary representation of an IP address (e.g. not the 
          string "171.64.64.137", but the base address of a character 
          array with ASCII values of 171, 64, 64, and 137 laid down
          side by side in <i>network byte order</i>.  The second 
          argument is usually 4 for IPv4 addresses (the ones we're 
          familiar with), but could be more for IPv6 addresses.  
          The third argument is generally <tt><b>AF_INET</b></tt> (for IPv4 addresses), 
          but might be <tt><b>AF_INET6</b></tt> (for IPv6 addresses) or specify
          some other address family.  We'll rely exclusively on <tt><b>AF_INET</b></tt> in CS110.

* <tt><b>struct</b></tt> <tt><b>hostent</b></tt>
    * The <tt><b>struct</b></tt> <tt><b>hostent</b></tt> record packages all of the information
      about a particular host contributing to the Internet.

	~~~{.cpp}
    struct hostent {
      char *h_name;        // official name of host
      char **h_aliases;    // NULL-terminated list of aliases
      int h_addrtype;      // host address type, e.g. AF_INET
      int h_length;        // length of address (4 for IPv4 addresses)
      char **h_addr_list;  // NULL-terminated list of IP addresses
    }; // h_addr_list is really a struct in_addr ** when known to be IPv4 addresses

    struct in_addr {
      unsigned int s_addr  // stored in network byte order (big endian)
    };
	~~~

# Resolving IP Addresses
* <tt><b>gethostbyname</b></tt> often used in network applications.
    * This shouldn't surprise you, as users prefer the host naming
      scheme behind "www.facebook.com", but network communication
      ultimately works with binary representation of "31.13.75.17".
    * Here's the core of the [full program](http://www.stanford.edu/class/cs110/autumn-2017/examples/networking/resolve-hostname.cc)
      that queries the user for hostnames
      and uses <tt><b>gethostbyname</b></tt> to surface information about
      them:

    ~~~{.cpp}
    static void publishIPAddressInfo(const string& host) {
      struct hostent *he = gethostbyname(host.c_str());
      if (he == NULL) { // NULL return value means resolution attempt failed
        cout << host << " could not be resolved to an address." << endl;
        return;
      }
    
      cout << "Official name is \"" << he->h_name << "\"" << endl;
      cout << "IP Addresses: " << endl;
      struct in_addr **addressList = (struct in_addr **) he->h_addr_list;
      while (*addressList != NULL) {
        cout << "+ " << inet_ntoa(**addressList) << endl;
        addressList++;
      }
    }    
    ~~~

    * Note two implementation features:
        * <tt><b>h_addr_list</b></tt> is typed to be a 
          <tt><b>char</b></tt> <tt><b>*</b></tt> array and
          implies it's an array of C strings, even dotted quad
          IP addresses.  
            * That's not correct. <tt><b>h_addr_list</b></tt> is
              really an array of <tt><b>struct</b></tt> <tt><b>in_addr</b></tt> <tt><b>*</b></tt>s.
            * Each <tt><b>in_addr</b></tt> is a gratutitous <tt><b>struct</b></tt>
              wrapper around an unsigned int, which is just big enough to store
              the four bytes of an IP address.  These four bytes are stored
              in network byte order (e.g. big endian order)
        * The <tt><b>inet_ntoa</b></tt> accepts <tt><b>struct</b></tt> <tt><b>in_addr</b></tt>s
          and returns their dotted quad equivalents (e.g. "171.45.34.199") as statically 
          allocated C strings.
    * Technically, <tt><b>gethostbyname</b></tt> and <tt><b>gethostbyaddr</b></tt> are
      deprecated and should replaced by <tt><b>getaddrinfo</b></tt>, <tt><b>getnameinfo</b></tt>, 
      and <tt><b>freeaddrinfo</b></tt>.  But the book relies on the deprecated versions, and I
      still see developers using them, so I'm comfortable using them, too.

# The <tt><b>sockaddr</b></tt> Hierarchy
* Presented below are three data structures which help us model IP address/port pairs.
    * And here they are:

    ~~~{.cpp}
    struct sockaddr_in { // IPv4 Internet-style socket address record
      unsigned short sin_family; // protocol family for socket
      unsigned short sin_port;   // port number (in network byte order)
      struct in_addr sin_addr;   // IP address (in network byte order)
      unsigned char sin_zero[8]; // pad to sizeof(struct sockaddr)
    };

    struct sockaddr_in6 { // IPv6 Internet-style socket address record
      unsigned short sin6_family;  // protocol family for socket
      unsigned short sin6_port;    // port number (in network byte order)
      // more fields, total size is > sizeof(struct sockaddr_in)
    };

    struct sockaddr { // generic socket address record
      unsigned short sa_family; // protocol family for socket
      char sa_data[14];         // address data (and defines full size to be 16 bytes)
    };
    ~~~

    * The <tt><b>sockaddr_in</b></tt> struct is dedicated to IPv4 address/port pairs.
        * The <tt><b>sin_family</b></tt> field should always be initialized
          to be <tt><b>AF_INET</b></tt>, which is a constant used to be clear that
          IPv4 addresses are being used.
            * If it seems redundant that a record dedicated to IPv4 addresses needs
              to store a constant tagging the information in the other fields as
              IPv4, then stay tuned.
        * The <tt><b>sin_port</b></tt> field stores a port number in network byte (aka
          big endian) order.
        * The <tt><b>sin_addr</b></tt> field stores an IPv4 address as a packed, big endian
          int, as you saw with <tt><b>gethostbyname</b></tt> and the <tt><b>struct hostent</b></tt>.
        * The <tt><b>sin_zero</b></tt> field is generally ignored (though it's typically set to
          store all zero bytes).  It exists primarily to pad the record up to 16 bytes.
    * The <tt><b>sockaddr_in6</b></tt> struct is dedicated to IPv6 address/port pairs.
        * The <tt><b>sin6_family</b></tt> field should always be set to <tt><b>AF_INET6</b></tt>.
          As with the <tt><b>sin_family</b></tt> field, <tt><b>sin6_family</b></tt>
          occupies the first two bytes of the record in which it resides.
        * The <tt><b>sin6_port</b></tt> field holds a two-byte, network-byte-ordered port number
          just as <tt><b>sin_port</b></tt>.
        * I don't list the remaining fields, but you can imagine they store some representation
          of the 128-bit IPv6 address.
    * The <tt><b>struct sockaddr</b></tt> type exists as C's best imitation of an abstract base class.
        * You rarely if ever declare variables of type <tt><b>struct sockaddr</b></tt>, but many
          system calls will accept parameters of type <tt><b>struct sockaddr *</b></tt>.
        * Rather than define a set of networking system calls for IPv4 addresses and a
          second set of system calls for IPv6 addresses, Linux defines one set for both.
        * If a system call accepts a parameter of type <tt><b>struct sockaddr *</b></tt>, it really
          excepts the address of either a <tt><b>struct sockaddr_in</b></tt> or a
          <tt><b>struct sockaddr_in6</b></tt>.  The system call relies on the value within
          the first two bytes&mdash;the <b><tt>sa_family</tt></b> field&mdash;to determine what the
          real record type is.

# Implementing <b><tt>createClientSocket</tt></b>
* We relied on the <b><tt>createClientSocket</tt></b> to establish
  a connection with a server running on a specified hostname and port.
    * Here's the code that establishes a connection to the server/port
      pair of interest:

    ~~~{.cpp}
    static const int kClientSocketError = -1;
    int createClientSocket(const string& host, unsigned short port) {
      struct hostent *he = gethostbyname(host.c_str());
      if (he == NULL) return kClientSocketError;

      int s = socket(AF_INET, SOCK_STREAM, 0);
      if (s < 0) return kClientSocketError;
    
      struct sockaddr_in serverAddress;
      memset(&serverAddress, 0, sizeof(serverAddress));
      serverAddress.sin_family = AF_INET;
      serverAddress.sin_port = htons(port);
      serverAddress.sin_addr.s_addr = ((struct in_addr *)he->h_addr)->s_addr;
    
      if (connect(s, (struct sockaddr *) &serverAddress, 
                  sizeof(serverAddress)) == 0) return s;
      close(s);
      return kClientSocketError;
    }
    ~~~

    * At the end of it all, the <tt><b>s</b></tt> is the client-end
      socket descriptor that can be used to manage a two-way conversation
      with the service running on the remote host/port pair.
    * There's obviously some mystery as to how the <tt><b>connect</b></tt>
      system call associates the socket descriptor with the host/port pair,
      but because it's a system-level service (that's what system calls are, no?),
      we really have to choice but to assume it works.

# Implementing <b><tt>createServerSocket</tt></b>
* The code is dense, but totally accessible to us.
    * Provided a server is prepared to listen to a specified port
      on any of its IP addresses, the following function returns
      a properly configured server socket:

    ~~~{.cpp}
    static const int kServerSocketFailure = -1; // sentinel for no valid socket
    static const int kReuseAddresses = 1;   // 1 means true here
    static const int kDefaultBacklog = 128; // allow 128 clients to queue up before they are "accept"ed, drop/ignore 129th
    int createServerSocket(unsigned short port) {
      int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
      if (serverSocket < 0) return kServerSocketFailure;
      if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,
                     &kReuseAddresses, sizeof(int)) < 0) {
        close(serverSocket);
        return kServerSocketFailure;
      } // setsockopt used here so port becomes available even is server crashes and reboots

      struct sockaddr_in serverAddress; // IPv4-style socket address
      memset(&serverAddress, 0, sizeof(serverAddress));
      serverAddress.sin_family = AF_INET; // sin_family field used to self-identify sockaddr type
      serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
      serverAddress.sin_port = htons(port);

      if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) == 0 &&
          listen(serverSocket, kDefaultBacklog) == 0) return serverSocket;

      close(serverSocket);
      return kServerSocketFailure;
    }
    ~~~

    * Note that <tt><b>createServerSocket</b></tt> returns a socket we 
      listen to for incoming connections.
        * It's categorized as a descriptor, so it needs to be closed when we're done with it.
        * It also gets cloned across <tt><b>fork</b></tt> boundaries as any descriptor would.
        * Server sockets, however, are not compatible with <tt><b>read</b></tt> and
          <tt><b>write</b></tt> system calls.  The only "read"-oriented system call it's
          compatible with is <tt><b>accept</b></tt>.


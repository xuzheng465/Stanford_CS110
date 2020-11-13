# Announcements
* Schedule This Week and Next
    * Assignment 5 due Wednesday at 11:59pm
    * Assignment 6 Out Wednesday, due Wednesday, November 15<sup>th</sup>

# Announcements (Continued)
* Today's Agenda
    * Finish up multithreading by completing the <tt><b>ice-cream-parlor</b></tt> simulation.
    * Start networking!
        * Review <tt><b>telnet</b></tt>, ports, Google, RSS News Feeds,
          and the Facebook Graph from the command line and the browser.
        * Implement a time server two ways.
            * Introduce the notion of a socket, which is a glorified descriptor that allows
              two-way communcation (that means reading and writing on the same descriptor).
            * Implement a time server using raw sockets and the <tt><b>write</b></tt> system call.
            * Implement a second time server that layers a third-party C++ stream over the client
              socket so we can use C++'s stream semantics instead of the raw <tt><b>read</b></tt>
              and <tt><b>write</b></tt> system calls.
            * Implement a third time server that's similar to the second one, except that it
              uses multithreading. Yay!
        * Present set of high-level parallels outlining how similar
          normal function calls, system function calls, interprocess communication via
          pipes, and (finally) interhost communication via sockets are all fundamentally
          the same thing.
        * Time permitting, implement a few network client applications.
    * Reading:
        * Read [Sections 4.1 and 4.2](http://www.sciencedirect.com/science/article/pii/B978012374957400013X)
          of the Saltzer &amp; Kaashoek textbook.  These two sections provide a wonderful discussion of the
          client-server model.
        * Read all of Chapter 11 of Bryant &amp; O'Hallaron (which is the third chapter of your reader).

# Implementing Servers
* Operative Metaphor
    * Colloquially, server-side applications wait by the phone (at a particular extension),
      praying that someone&mdash;no, anyone!!&mdash;will call.
    * Formally, server-side applications create a server socket that 
      listens to a particular port.  
        * The server socket is an integer identifier associated with a local IP
          address (think phone number) and port (think phone extension).
        * You should also think of the port number as a virtual process ID that
          the host associates with the actual pid of the server application.

# [Baby's First Server](http://3.bp.blogspot.com/-XUovTA5ae_0/TY_4BOMOYdI/AAAAAAAAFPk/7VjP57Izab8/s1600/photo-22.JPG)
* A Time Server
    * Full version of the server code (minus the <tt><b>createServerSocket</b></tt>
      [interface](http://www.stanford.edu/class/cs110/autumn-2017/examples/networking/server-socket.h)
      and [implementation](http://www.stanford.edu/class/cs110/autumn-2017/examples/networking/server-socket.cc)) can be found
      [right here](http://www.stanford.edu/class/cs110/autumn-2017/examples/networking/time-server-descriptors.cc).
    * Our time server accepts all incoming connection requests and
      quickly publishes the time without so much as listening to a word that
      the client has to say.

    ~~~{.cpp}
    static const short kDefaultPort = 12345;
    static const int kWrongArgumentCount = 1;
    static const int kServerStartFailure = 2;
    int main(int argc, char *argv[]) {
      if (argc > 1) {
        cerr << "Usage: " << argv[0] << endl;
        return kWrongArgumentCount;
      }

      int serverSocket = createServerSocket(kDefaultPort);
      if (serverSocket == kServerSocketFailure) {
        cerr << "Error: Could not start server on port " << kDefaultPort << "." << endl;
        cerr << "Aborting... " << endl;
        return kServerStartFailure;
      }

      cout << "Server listening on port " << kDefaultPort << "." << endl;
      while (true) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        publishTime(clientSocket);
      }

      return 0;
    }
    ~~~

# [Baby's First Server](http://3.bp.blogspot.com/-XUovTA5ae_0/TY_4BOMOYdI/AAAAAAAAFPk/7VjP57Izab8/s1600/photo-22.JPG) (continued)
* The implementation of <tt><b>publishTime</b></tt> is straightforward.
    * The implementation itself, however, isn't the focus.  Strictly
      speaking, it's generating dynamic content&mdash;uninteresting, but
      dynamic, nonetheless&mdash;and publishing it back to the client
      over the socket descriptor.

    ~~~{.cpp}
    static void publishTime(int clientSocket) {
      time_t rawtime;
      time(&rawtime);
      struct tm *ptm = gmtime(&rawtime);
      char timeString[128]; // more than big enough                                                                                                                                           
      /* size_t len = */ strftime(timeString, sizeof(timeString), "%c\n", ptm);

      size_t numBytesWritten = 0, numBytesToWrite = strlen(timeString);
      while (numBytesWritten < numBytesToWrite) {
        numBytesWritten += write(clientSocket,
                                 timeString + numBytesWritten,
                                 numBytesToWrite - numBytesWritten);
      }
      close(clientSocket);
    }
    ~~~

    * The first five lines here produce the full time string that should be published.
      Let these five lines represent more generally the server-side computation needed for
      the service to produce output.  Here's is the current time, but it could have been
      a static HTML page, a Google search result, an RSS XML document, an image, or a Netflix
      video.
    * The remaining lines publish the time string&mdash;we'll call it the payload&mdash;to the
      client socker using the raw, low-level I/O we've seen before.

# Server-Side Networking and Threading
* Networking and threading go together like peanut butter and jelly.
    * The work a server needs to do in order to meet the client's request
      might be time consuming&mdash;so time consuming that a sequential
      implementation might interfere with the server's ability to accept
      future requests.
    * One solution: as soon as <tt><b>accept</b></tt> returns a socket descriptor,
      spawn a child thread to get any intense, time consuming computation
      off of the main thread.  The child thread can make use of a second processor
      or a second core, and the main thread can quickly move on to its next
      <tt><b>accept</b></tt> call.

# Server-Side Networking and Threading (continued)
* Networking and threading belong together.
    * Here's the same time server example, save for its decision to use
      [<i>threading</i>](http://artsandcatsmovement.files.wordpress.com/2012/07/kitten-with-yarn.jpeg)
      to compute and publish the time back to the client.

    ~~~{.cpp}
    int main(int argc, char *argv[]) {
      if (argc > 1) {
        cerr << "Usage: " << argv[0] << endl;
        return kWrongArgumentCount;
      }

      int serverSocket = createServerSocket(kDefaultPort);
      if (serverSocket == kServerSocketFailure) {
        cerr << "Error: Could not start time server to listen to port " << kDefaultPort << "." << endl;
        cerr << "Aborting... " << endl;
        return kServerStartFailure;
      }

      cout << "Server listening on port " << kDefaultPort << "." << endl;  
      ThreadPool pool(4);
      while (true) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        pool.schedule([clientSocket] { publishTime(clientSocket); });
      }
      return 0;
    }
    ~~~

    * Note the use of a <tt><b>ThreadPool</b></tt> to get the server-side computation
      off of the the main thread.  This way, the main thread can rotate around and
      more immediately advance on to other <tt><b>accept</b></tt> requests.

# Server-Side Networking and Threading (continued)
* Networking and threading belong together like Pepper and Iron Man.
    * The implementation of <tt><b>publishTime</b></tt> <b>must</b> change if it's to be
      thread safe.  The change is simple but important: we need to call a reentrant,
      thread safe version of <tt><b>gmtime</b></tt> called <tt><b>gmtime_r</b></tt>.
        * <tt><b>gmtime</b></tt> returns a pointer to a single, statically allocated
          record of time information that's used by all calls to it.  If two
          threads make competing calls to it, then both threads race to pull
          time information from the shared, statically allocated record.
        * One solution is to use a <tt><b>mutex</b></tt> to ensure that a thread
          can call <tt><b>gmtime</b></tt> without competition and subsequently
          copy the data out of the static record into a local character buffer.
        * Another solution&mdash;one that doesn't require locking
          and one I think is better&mdash;makes use
          of a second version of the same function called <tt><b>gmtime_r</b></tt>.
          The second, reentrant version just requires that space for a
          dedicated return value be passed in.

    ~~~{.cpp}
    static void publishTime(int clientSocket) {
      time_t rawtime;
      time(&rawtime);
      struct tm tm;
      gmtime_r(&rawtime, &tm);
      char timeString[128]; // more than big enough
      /* size_t len = */ strftime(timeString, sizeof(timeString), "%c", &tm);
      sockbuf sb(clientSocket); // destructor closes socket
      iosockstream ss(&sb);
      ss << timeString << endl;
    }
    ~~~

# [Baby's Second Server](https://w8m8b4g9.ssl.hwcdn.net/media.easynews.com/social/EN_blog_ipodhack3.png)
* The <tt><b>while</b></tt> loop around the exposed <tt><b>write</b></tt> calls
  in the prior example were actually necessary this time.
    * The socket descriptor is bound to a network driver that has a limited amount of space.
    * In practice, it'd be common to see <tt><b>write</b></tt>'s return value be less than
      the value provided via the third argument.
    * Ideally, we'd rely on either C streams (e.g. the <tt><b>FILE</b></tt> <tt><b>*</b></tt>)
      or C++ streams (e.g. the <tt><b>iostream</b></tt> class hierarchy) to layer over data buffers
      and manage the <tt><b>while</b></tt> loop around exposed <tt><b>write</b></tt> calls for us.
    * Fortunately, we have access to a third-party library that provides this.  We're going to
      operate as if the third-party library is just part of standard C++.

    ~~~{.cpp}
    static void publishTime(int clientSocket) {
      time_t rawtime;
      time(&rawtime);
      struct tm *ptm = gmtime(&rawtime);
      char timeString[128]; // more than big enough
      /* size_t len = */ strftime(timeString, sizeof(timeString), "%c", ptm);
      sockbuf sb(clientSocket);
      iosockstream ss(&sb);
      ss << timeString << endl;
    } // the sockbuf closes the socket when it's destroyed
    ~~~

    * We rely on the same C library functions to generate the time string.
    * This time, however, we insert that string into an <tt><b>iosockstream</b></tt> that itself
      layers over the client socket.
        * Note that the intermediary <tt><b>sockbuf</b></tt> class takes
          ownership of the socket and closes it when its destructor is called.

# Our first network client!
* Intentionally easy!
    * The full program file is [right here](http://www.stanford.edu/class/cs110/autumn-2017/examples/networking/time-client.cc)
    * The protocol&mdash;informally, the set of rules both client and server must
      follow if they're to speak with one another&mdash;is simple.
    * The protocol here is...
        * The client connects (e.g. "rings" the service's phone at a particular
          "extension", and waits for the server to "pick up")
        * The client says nothing.
        * The server speaks by publishing the current time into its own
          end of the connection and then hangs up.
        * The client ingests the published text (understood, by protocol, to be just
          one line), publishes it to the console, and then itself hangs up.

    ~~~{.cpp}
    int main(int argc, char *argv[]) {
      int clientSocket = createClientSocket("myth7.stanford.edu", 12345);
      if (clientSocket == kClientSocketError) {
        cerr << "Time server could not be reached" << endl;
        cerr << "Aborting" << endl;
        return 1;
      }
      sockbuf sb(clientSocket);
      iosockstream ss(&sb);
      string timeline;
      getline(ss, timeline);
      cout << timeline << endl;
      return 0;
    }
    ~~~

    * We'll discuss the implementation of <b><tt>createClientSocket</tt></b> on Monday, but it's okay
      to just view it as a built-in that sets up a bidirectional pipe between the client and the
      server running on the specified host and port number.

# Emulation of <b><tt>wget</tt></b>
* <b><tt>wget</tt></b> is a command line utility that, given its URL,
  downloads a single document (HTML document, XML document, JPG, or whatever).
    * Without being concerned so much about error checking and
      robustness, we can write something very simple to emulate
      the <b><tt>wget</tt></b>'s core functionality.
    * Full program is [right here](http://www.stanford.edu/class/cs110/autumn-2017/examples/networking/web-get.cc).
    * It'll allow me to illustrate the most basic parts of the HTTP protocol, which are key to the assignment
      being released today.
    * I'll run <tt><b>/usr/bin/wget</b></tt> (the built-in) and <tt><b>web-get</b></tt> (which is what we'll write)
      side by side in lecture.
    * <tt><b>main</b></tt> entry point and implementation are presented here:

    ~~~{.cpp}
    static const string kProtocolPrefix = "http://";
    static const string kDefaultPath = "/";
    static pair<string, string> parseURL(string url) {
      if (startsWith(url, kProtocolPrefix)) // in "string-utils.h"
        url = url.substr(kProtocolPrefix.size());
      size_t found = url.find('/');
      if (found == string::npos)
        return make_pair(url, kDefaultPath); // defined in <utility>
      string host = url.substr(0, found);
      string path = url.substr(found);
      return make_pair(host, path);
    }
    
    int main(int argc, char *argv[]) {
      if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <url>" << endl;
        return kWrongArgumentCount;
      }
      pullContent(parseURL(argv[1]));
      return 0;
    }     
    ~~~

    * The <tt><b>parseURL</b></tt> function is a gesture to the
      programmatic split at the host-path border.  In practice,
      more protocols (<tt><b>https</b></tt>, for instance) might
      be supported as well.

# Emulation of <b><tt>wget</tt></b> (continued)
* Of course, the <tt><b>pullContent</b></tt> function needs to
  cover the networking.
    * We've already used <tt><b>createClientSocket</b></tt> function for our <b><tt>time-client</b></tt> program.
      Again, we'll conver its implementation and the implementation of <tt><b>createServerSocket</b></tt> on
      Wednesday during class.
    * There is, of course, <b><tt>web-get</tt></b>-specific work to be done:

    ~~~{.cpp}
    static void pullContent(const pair<string, string>& components) {
      int clientSocket = createClientSocket(components.first, 80);
      // error checking omitted    
      sockbuf sb(clientSocket);
      iosockstream ss(&sb);
      issueRequest(ss, components.first, components.second);
      skipHeader(ss);
      savePayload(ss, getFileName(components.second));
    }
	~~~
    * The implementations of <b><tt>issueRequest</tt></b>, <b><tt>skipHeader</tt></b>, and
      <b><tt>savePayload</tt></b> subdivide the client-server conversation into manageable
      chunks.
    * The best takeaway from the above is the fact that, for the second 
      time in two examples, we've layered an <b><tt>iosockstream</tt></b>
      on top of a <b><tt>sockbuf</tt></b>, which itself is layered on top
      of our bidirectional socket descriptor.
        * Be glad we have the <b><tt>socket++</tt></b> library, because
          without it, we'd need to do so much manual character array
          manipulation that coding would cease to be fun.
        * One very relevant piece of information about the <b><tt>sockbuf</tt></b>
          class: its destructor closes the file descriptor passed to it when it's
          constructed, so we shouldn't call <b><tt>close</tt></b> on 
          <b><tt>clientSocket</tt></b> ourselves.

# Emulation of <b><tt>wget</tt></b> (continued)
* The implementations of the helper functions are fairly straighforward:
    * Here's the implementation of <b><tt>issueRequest</tt></b>.  Notice
      that I manually construct the absolute minimum, two-line request 
      imaginable and send it over the wire to the server.
    * It's standard HTTP-protocol practice that each line, including the blank line
      that marks the end of the request, end in CRLF (short for carriage-return-
      line-feed), which is '\\r' following by '\\n'.
    * The <b><tt>flush</tt></b> call is necessary to ensure all character data
      is pressed over the wire and consumable at the other end.

    ~~~{.cpp}
    static void issueRequest(iosockstream& ss, const string& host, const string& path) {
      ss << "GET " << path << " HTTP/1.0\r\n";
      ss << "Host: " << host << "\r\n";
      ss << "\r\n";
      ss.flush();
    }
    ~~~

    * After the <b><tt>flush</tt></b>, the client transitions from 
      speak to listen mode.
        * The <tt><b>iosockstream</b></tt> is read/write, 
          because the socket descriptor backing it is bidirectional.
    * We read in all of the HTTP response header
      lines until we get to either a blank line or one that contains
      nothing other than a '\\r'.
    * The blank line is, indeed, supposed to be "\\r\\n", but some
      servers are sloppy, so we're supposed to treat the '\\r' as
      optional.  (Recall that <b><tt>getline</tt></b> chews up
      the '\\n', but it'll leave '\\r' at the end of the line).

    ~~~{.cpp}
    static void skipHeader(iosockstream& ss) {
      string line;
      do {
        getline(ss, line);
      } while (!line.empty() && line != "\r");
    }
    ~~~

    * This is a reasonably legitimate situation where the 
      <b><tt>do/while</tt></b> loop is the correct idiom. #yaydowhileloops

# Emulation of <b><tt>wget</tt></b> (continued)
* Of course, there's that payload part.
    * Everything beyond the response header and that
      blank line is considered payload&mdash;that's the
      file, the JSON, the HTML, the image, the cat video.
    * Every single byte that comes through should be replicated,
      in order, to a local copy.

    ~~~{.cpp}
    static string getFileName(const string& path) {
      if (path.empty() || path[path.size() - 1] == '/') {
        return "index.html"; // not always correct, but not the point
      }

      size_t found = path.rfind('/');
      return path.substr(found + 1);
    }
    
    static const size_t kBufferSize = 1024; // just a random, large size
    static void savePayload(iosockstream& ss, const string& filename) {
      ofstream output(filename, ios::binary); // don't assume it's text
      size_t totalBytes = 0;
      while (!ss.fail()) {
        char buffer[kBufferSize] = {'\0'};
        ss.read(buffer, sizeof(buffer));
        totalBytes += ss.gcount();
        output.write(buffer, ss.gcount());
      }
      cout << "Total number of bytes fetched: " << totalBytes << endl;
    }
    ~~~

    * The HTTP/1.0 protocol dictates that everything beyond that blank
      line is payload, and that once the server publishes each and every
      byte of the payload, it closes it's end of the connection.  That
      server-side close is the client-side's EOF, and we write everything
      we read.
        * Note that we open an <tt><b>ofstream</b></tt> in binary
          mode, predominantly so the <tt><b>ofstream</b></tt> doesn't
          do anything funky with byte characters that are <i>incidentally</i>
          newline characters.
        * <tt><b>gcount</b></tt> returns the number of bytes read by the
          most recent call to <tt><b>read</b></tt>.  (This I did not
          know until I wrote this example).
    * The HTTP/1.1 protocol allows for connections to remain open, even
      after the initial payload has come through.  I specifically avoid
      this here by going with HTTP/1.0, which doesn't allow this.


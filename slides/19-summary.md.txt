# Announcements
* Assignment 8
    * Assignment 8 today Wednesday, due next Thursday, December 7<sup>th</sup>.
    * Very generous late policy.
    * Assignments will be autograded without code reviews and made available by Sunday morning.

* Today's Lecture
    * Introduce nonblocking I/O.
        * Discuss what slow system calls are and why they're bad news for the threads
          of execution that need to make them.
        * Cover the implementation of <tt><b>non-blocking-alphabet-client</b></tt>.
        * Introduce the <tt><b>OutboundFile</b></tt> abstraction, which internally relies
          on nonblocking I/O to syndicate the contents of a file.
        * Without worrying about the implementation of the <tt><b>OutboundFile</b></tt>,
          we'll rely on them to implement a nonblocking web server capable of handling an
          incredibly large number of client connections in just one thread of execution.
        * Work through the implementation of the <tt><b>OutboundFile</b></tt> class.

# Case study: The <tt><b>slow-alphabet-server</b></tt>
* Consider the following server implementation:

    ~~~{.cpp}
    static const string kAlphabet = "abcdefghijklmnopqrstuvwxyz";
    static const useconds_t kDelay = 100000; // 100000 microseconds is 100 ms is 0.1 seconds
    static void handleRequest(int client) {
      sockbuf sb(client);
      iosockstream ss(&sb);
      for (size_t i = 0; i < kAlphabet.size(); i++) {
        ss << kAlphabet[i] << flush;
        usleep(kDelay);
      }
    }

    static const unsigned short kSlowAlphabetServerPort = 41411;
    int main(int argc, char *argv[]) {
      int server = createServerSocket(kSlowAlphabetServerPort);
      ThreadPool pool(128);
      while (true) {
        int client = accept(server, NULL, NULL);
        pool.schedule([client]() { handleRequest(client); });
      }
      return 0;
    }
    ~~~

# Case study: The <tt><b>slow-alphabet-server</b></tt>
  * Full implementation is [right here](http://cs110.stanford.edu/autumn-2017/examples/non-blocking-io/slow-alphabet-server.cc).
      * Operates much like the sequential <tt><b>time-server-concurrent</b></tt> we wrote a few lectures ago.
      * The protocol:
          * wait for an incoming connection, and
          * delegate responsibility to handle that connection to a worker within a <tt><b>ThreadPool</b></tt>, and
          * have worker very slowly spell out the English alphabet over 2.6 seconds, and
          * close connection (which happens because the <tt><b>sockbuf</b></tt> destructor is called.
      * There's nothing nonblocking about this server, but it is intentionally slow to emulate the
        time a genuine server-side computation might take to generate a full response.
      * Many servers, like the one above, will push out the partial responses to the client.

# Case study: The <tt><b>blocking-alphabet-client</b></tt>
* Presented here is a traditional (i.e. blocking) client of the <tt><b>slow-alphabet-server</b></tt>:

    ~~~{.cpp}
    static const unsigned short kSlowAlphabetServerPort = 41411;
    int main(int argc, char *argv[]) {
      int client = createClientSocket("localhost", kSlowAlphabetServerPort);
      size_t numSuccessfulReads = 0;
      size_t numBytes = 0;
      while (true) {
        char ch;
        ssize_t count = read(client, &ch, 1);
        assert(count != -1); // simple sanity check, assume more robust in practice
        if (count == 0) break; // we are truly done
        numSuccessfulReads++;
        numBytes += count;
        cout << ch << flush;
      }
      close(client);

      cout << endl;
      cout << "Alphabet Length: " << numBytes << " bytes." << endl;
      cout << "Num reads: " << numSuccessfulReads << endl;
      return 0;
    }
    ~~~~
  
# Case study: The <tt><b>blocking-alphabet-client</b></tt>
  * Full implementation is [right here](http://cs110.stanford.edu/autumn-2017/examples/non-blocking-io/blocking-alphabet-client.cc).
      * Relies on traditional client socket as returned by <tt><b>createClientSocket</b></tt>.
      * Character buffer passed to <tt><b>read</b></tt> is of size 1, thereby constraining the range of legitimate
        return values to tbe [-1, 1].
      * Provided the <tt><b>slow-alphabet-server</b></tt> is running, a client run on the 
        same machine would reliably behave as follows:

    ~~~{.sh}
    myth7> ./slow-alphabet-server &
    [1] 7516
    myth7> ./blocking-alphabet-client
    abcdefghijklmnopqrstuvwxyz
    Alphabet Length: 26 bytes.
    Num reads: 26
    myth7> time ./blocking-alphabet-client
    abcdefghijklmnopqrstuvwxyz
    Alphabet Length: 26 bytes.
    Num reads: 26
    0.000u 0.002s 0:02.60 0.0%      0+0k 0+8io 0pf+0w
    myth7> kill -KILL 7516
    [1]    Killed                        ./slow-alphabet-server
    ~~~

# Case study: The <tt><b>non-blocking-alphabet-client</b></tt>
* Presented here is client of the <tt><b>slow-alphabet-server</b></tt> that relies on
  nonblocking I/O:

    ~~~{.cpp}
    static const unsigned short kSlowAlphabetServerPort = 41411;
    int main(int argc, char *argv[]) {
      int client = createClientSocket("localhost", kSlowAlphabetServerPort);
      setAsNonBlocking(client);

      size_t numReads = 0;
      size_t numSuccessfulReads = 0;
      size_t numUnsuccessfulReads = 0;
      size_t numBytes = 0;
      while (true) {
        char ch;
        ssize_t count = read(client, &ch, 1);
        numReads++;
        if (count == 0) break; // we are truly done
        if (count > 0) {
          numSuccessfulReads++;
          numBytes += count;
          cout << ch << flush;
        } else {
          assert(errno == EWOULDBLOCK || errno == EAGAIN);
          numUnsuccessfulReads++;
        }
      }  
      close(client);

      cout << endl;
      cout << "Alphabet Length: " << numBytes << " bytes." << endl;
      cout << "Num reads: " << numReads << " (" << numSuccessfulReads << " successful, " << numUnsuccessfulReads << " unsuccessful)." << endl;
      return 0;
    }
    ~~~

# Case study: The <tt><b>non-blocking-alphabet-client</b></tt>
  * Full implementation is [right here](http://cs110.stanford.edu/autumn-2017/examples/non-blocking-io/non-blocking-alphabet-client.cc).
      * Relies on traditional client socket as returned by <tt><b>createClientSocket</b></tt> as the first version did,
        but immediately transitions it to be nonblocking.
          * Now, <tt><b>read</b></tt> as used above is incapable of blocking.
              * Data available? Expect ch to be updated and a return value of 1.
              * No data available, ever? Expect a return value of 0.
              * No data available right now, but possibly in the future? Expect a return value of
                -1 and <tt><b>errno</b></tt> to be set to <tt><b>EAGAIN</b></tt>.

# Case study: The <tt><b>non-blocking-alphabet-client</b></tt>
  * Look at the output of <tt><b>non-blocking-alphabet-client</b></tt>:
 
    ~~~{.sh}
    myth7> ./slow-alphabet-server &
    [1] 9801
    myth7> ./non-blocking-alphabet-client 
    abcdefghijklmnopqrstuvwxyz
    Alphabet Length: 26 bytes.
    Num reads: 11394590 (26 successful, 11394563 unsuccessful).
    myth7> time ./non-blocking-alphabet-client
    abcdefghijklmnopqrstuvwxyz
    Alphabet Length: 26 bytes.
    Num reads: 11268991 (26 successful, 11268964 unsuccessful).
    0.399u 2.202s 0:02.60 99.6%     0+0k 0+0io 0pf+0w
    myth7> kill -KILL 9801
    myth7> 
    [1]    Killed                        ./slow-alphabet-server
    myth7>
    ~~~

  * Look at how many calls to <tt><b>read</b></tt> punt because data just isn't ready yet.
  * Reasonable question: Why is this better?  Why would one rely on nonblocking I/O other
    than because we can?
      * This question is addressed in future examples.

# Overview of <tt><b>OutboundFile</b></tt> class implementation
* The full implementation
    * The [full implementation](http://cs110.stanford.edu/autumn-2017/examples/non-blocking-io/outbound-file.cc) 
      includes lots of spaghetti code.
    * In particular, true file descriptors and socket descriptors need to be treated differently
      in a few places&mdash;in particular, detecting when all data has been flushed out to the sink
      desciptor (which may be a local file, a console, or a remote client machine) isn't exactly pretty.
    * However, my implementation is decomposed well enough that I think many of the methods&mdash;the ones
      I'll show in lecture&mdash;are easy to follow and provide a clear narrative.  At the very least,
      I'll surely convince you that the <tt><b>OutboundFile</b></tt> implementation is accessible to
      someone just finishing up CS110.

# Overview of <tt><b>OutboundFile</b></tt> class implementation
* The full implementation
    * Here's is the condensed interface file for the <tt><b>OutboundFile</b></tt> class.

    ~~~{.cpp}
    class OutboundFile {
     public:
      OutboundFile();
      void initialize(const std::string& source, int sink);
      bool sendMoreData();

     private:
      int source, sink;
      static const size_t kBufferSize = 128;
      char buffer[kBufferSize];
      size_t numBytesAvailable;
      size_t numBytesSent;
      bool isSending;

      bool dataReadyToBeSent() const;
      void readMoreData();
      void writeMoreData();
      bool allDataFlushed();
    };
    ~~~
   
    * This was presented in a previous slide deck, except now I'm exposing the
      private data members.
        * <tt><b>source</b></tt> and <tt><b>sink</b></tt> are descriptors bound
          to the data source and the data recipient, and both are nonblocking.
        * <tt><b>buffer</b></tt> is a reasonably sized character array that helps
          shovel bytes lifted from the <tt><b>source</b></tt> via calls to <tt><b>read</b></tt>
          over to the <tt><b>sink</b></tt> via calls to <tt><b>write</b></tt>.  We
          shouldn't be surprised that <tt><b>read</b></tt> and <tt><b>write</b></tt>
          come in to play.
        * <tt><b>numBytesAvailable</b></tt> stores the number of meaningful characters residing
          in <tt><b>buffer</b></tt>.
        * <tt><b>numBytesSent</b></tt> tracks the number of bytes residing in <tt><b>buffer</b></tt>
          that have been written to the recipient.
            * When <tt><b>numBytesAvailable</b></tt> and <tt><b>numBytesSent</b></tt> are
              equal, we know that <tt><b>buffer</b></tt> is effectively empty, and that perhaps
              another call to <tt><b>read</b></tt> is in order.
        * <tt><b>isSending</b></tt> tracks whether all data has been pulled from the <tt><b>source</b></tt>
          and pushed to the recipient <tt><b>sink</b></tt>.

# Overview of <tt><b>OutboundFile</b></tt> class implementation
* The full implementation
    * Here's is enough of the <tt><b>OutboundFile</b></tt> implementation to make it clear
      how it works.

    ~~~{.cpp}
    OutboundFile::OutboundFile() : isSending(false) {}

    void OutboundFile::initialize(const string& source, int sink) {
      this->source = open(source.c_str(), O_RDONLY | O_NONBLOCK);
      this->sink = sink;
      setAsNonBlocking(this->sink);
      numBytesAvailable = numBytesSent = 0;
      isSending = true;
    }
    ~~~
    * The implementations of the constructor and <tt><b>initialize</b></tt> are complete.
        * The <tt><b>source</b></tt> is always a file descriptor bound to some local
          file.  
            * Note that the file is opened for reading (<tt><b>O_RDONLY</b></tt>), and
              the descriptor is configured to be nonblocking (<tt><b>O_NONBLOCK</b></tt>).
            * For reasons we discussed last time, it's not super important that the <tt><b>source</b></tt>
              be nonblocking, since it's bound to a local file.  But in the spirit of a nonblocking example,
              it's fine to make it nonblocking anyway.  We just should expect very many (if any) -1's to
              come back from the <tt><b>read</b></tt> calls.
        * The <tt><b>sink</b></tt> is explicitly marked as nonblocking, since we shouldn't require the
          client to convert it to nonblocking ahead of time.

# Overview of <tt><b>OutboundFile</b></tt> class implementation
* The full implementation
    * Here's is enough of the <tt><b>OutboundFile</b></tt> implementation to make it clear
      how it works.

    ~~~{.cpp}
    bool OutboundFile::sendMoreData() {
      if (!isSending) return !allDataFlushed();
      if (!dataReadyToBeSent()) {
        readMoreData();
        if (!dataReadyToBeSent()) return true;
      }
      writeMoreData();
      return true;
    }
    ~~~
    * The implementation of <tt><b>sendMoreData</b></tt> is incomplete, but it's enough
      that you can understand the full story.  (Again, full implementation is
      [right here](http://cs110.stanford.edu/winter-2017/examples/non-blocking-io/outbound-file.cc)).
        * Recall that <tt><b>sendMoreData</b></tt> returns <tt><b>false</b></tt> when no more
          calls to <tt><b>sendMoreData</b></tt> are needed, and <tt><b>true</b></tt> if it's
          unclear.
        * The first line detects the situation where all data has been read from the <tt><b>source</b></tt>
          and written to the <tt><b>sink</b></tt>, and it returns <tt><b>true</b></tt> unless
          it's able to further confirm that all of the data written to <tt><b>sink</b></tt> has
          actally arrived (i.e. we've confirmed that it's been flushed out to the final
          destination.)
        * The first call to <tt><b>dataReadyToBeSent</b></tt> checks to see if the <tt><b>buffer</b></tt>
          includes any characters that have yet to be pushed out.  If not, then it attempts to 
          <tt><b>readMoreData</b></tt>.  If after reading more data the buffer is still empty&mdash;that is, 
          the call to <tt><b>read</b></tt> resulted in a -1/<tt><b>EWOULDBLOCK</b></tt> pair, then we
          return <tt><b>true</b></tt> as a statement that there's no data to be written, no need to try
          writing, but come back later to see if that changes.
        * The call to <tt><b>writeMoreData</b></tt> is an opportunity to push data out to the <tt><b>sink</b></tt>.
        * We return <tt><b>true</b></tt> at the end, because we need to come back to, see if there's more data
          to be read in, or if we're done reading and writing and we've also managed to flush everything out
          to the ultimate destination.

# Case study: <tt><b>OutboundFile</b></tt> class
* The <tt><b>OutboundFile</b></tt> class is designed to read a local file and push its contents out over
  a supplied descriptor, and to do so without ever blocking.
    * Here's an abbreviated version of the interface file:

    ~~~{.cpp}
    class OutboundFile {
     public:
      OutboundFile();
      void initialize(const std::string& source, int sink);
      bool sendMoreData();

     private:
      // implementation details ommitted for the moment
    }
    ~~~

    * The constructor just defaultly constructs an instance of the <tt><b>OutboundFile</b></tt> class.  The
      <tt><b>initialize</b></tt> method identifies what local file should be used as a source of data and the
      descriptor where that data should be written verbatim.  The <tt><b>sendMoreData</b></tt> method pushes
      as much data as possible to the supplied sink, without blocking.  It returns <tt><b>true</b></tt> if 
      it's at all possible there's more data to be sent, and <tt><b>false</b></tt> if all data has been fully
      pushed out.  The full interface file (which leaks some implementation details, because you see the private
      section) is [right here](http://cs110.stanford.edu/autumn-2017/examples/non-blocking-io/outbound-file.h).

# Unit test for <tt><b>OutboundFile</b></tt>
* Here's a simple program I use to ensure that the <tt><b>OutboundFile</b></tt> class is working.
    * It's a simple program prints the source of the unit test to standard output. #meta
    * There's a copy of the code [right here](http://cs110.stanford.edu/autumn-2017/examples/non-blocking-io/outbound-file-test.cc).

    ~~~{.cpp}
    /**
     * File: outbound-file-test.cc
     * ---------------------------
     * Demonstrates how one should use the OutboundFile class
     * and can be used to confirm that it works properly.
     */
    #include "outbound-file.h"
    int main(int argc, char *argv[]) {
      OutboundFile obf;
      obf.initialize("outbound-file-test.cc", STDOUT_FILENO);
      while (obf.sendMoreData()) {;}
      return 0;
    }
    ~~~

# Static File Server
* Now consider the following server.
    * This is a program that implements a nonblocking server that happily serves up a copy of the server code itself to the client.
    * A full copy of the program is [right here](http://cs110.stanford.edu/autumn-2017/examples/non-blocking-io/expensive-server.cc).
    * And here's a copy of the program right here:

    ~~~{.cpp}
    static const unsigned short kDefaultPort = 12345;
    static const string kFileToServe("expensive-server.cc");
    int main(int argc, char *argv[]) {
      int serverSocket = createServerSocket(kDefaultPort);
      if (serverSocket == kServerSocketFailure) {
        cerr << "Could not start server.  Port " << kDefaultPort << " is probably in use." << endl;
        return 0;
      }

      setAsNonBlocking(serverSocket);
      cout << "Static file server listening on port " << kDefaultPort << "." << endl;
      list<OutboundFile> outboundFiles;
      size_t numConnections = 0;
      size_t numActiveConnections = 0;

      while (true) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
          assert(errno == EWOULDBLOCK);
        } else {
          OutboundFile obf;
          obf.initialize(kFileToServe, clientSocket);
          outboundFiles.push_back(obf);
          cout << "Connection #" << ++numConnections << endl;
          cout << "Queue size: " << ++numActiveConnections << endl;
        }

        auto iter = outboundFiles.begin();
        while (iter != outboundFiles.end()) {
          if (iter->sendMoreData()) {
            ++iter;
          } else {
            iter = outboundFiles.erase(iter);
            cout << "Queue size: " << --numActiveConnections << endl;
          }
        }
      }
    }
    ~~~

    * The implementation of <tt><b>setAsNonBlocking</b></tt> is UNIX gobbledygook, and it's right here:

    ~~~{.cpp}
    void setAsNonBlocking(int descriptor) {
      int flags = fcntl(descriptor, F_GETFL);
      if (flags == -1) flags = 0; // if first call to fcntl fails, just go with 0 
      fcntl(descriptor, F_SETFL, flags | O_NONBLOCK); // preserve other set flags
    }
    ~~~


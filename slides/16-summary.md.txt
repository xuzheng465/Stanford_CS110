# Announcements
* Assignments 6 and 7.
    * Assignment 6 is due Wednesday night.
    * Assignment 7, which requires that you build your very own HTTP proxy, goes out Wednesday
      and is due the Wednesday after we return from break.

* Today's Agenda
    * Introduce the <tt><b>struct hostent</b></tt> and the <tt><b>sockaddr</b></tt> hierarchy.
    * Work through the implementations of two functions that we've been taking for granted
      since we dipped into the networking segment of the quarter: <tt><b>createServerSocket</b></tt>
      and <tt><b>createClientSocket</b></tt>.
    * Work through one last networking example to illustrate a novel server-side
      architecture that's not at all far from how Facebook servers produce News Feeds,
      LinkedIn produces profiles, and Google produces search results.

# Scrabble API
* Imitation of [Lexical Word Finder](http://www.lexicalwordfinder.com/)
    * Assumes existence of standalone <tt><b>scrabble-word-finder</b></tt>.
    * Code contributing to <tt><b>scrabble-word-finder</b></tt>, which has no idea it might
      contribute to a server, is right [here](http://cs110.stanford.edu/autumn-2017/examples/networking/scrabble-word-finder.cc).
        * Implemented using straightforward procedural recursion with pruning.
        * Hardly optimized to be fast&mdash;no caching, makes use of only the most obvious pruning strategies.

* We want to implement a server to share what <tt><b>scrabble-word-finder</b></tt> is capable of.
    * Approach: allow URL to specify rack of letters.
    * <tt><b>http://myth4.stanford.edu:13133/ieclxal</b></tt> should produce all words
      that can be formed from <tt><b>ieclxal</b></tt>.

    ~~~{.js}
    {
      time: 0.223399,
      cached: false,
      possibilities: [
        'ace',
          // several words omitted
        'lex',
        'lexica',
        'lexical',
        'li',
        'lice',
        'lie',
        'lilac',
        'xi'
      ]
    }
    ~~~

# Scrabble API
* Computation relevant to server already exists
    * Reimplementing is bad, and reinventing the wheel is wasteful and time consuming
    * <tt><b>scrabble-word-finder</b></tt>, as an executable, already outputs the core of what
      we'd like to serve as plain text, as with:

    ~~~{.sh}
    myth4> ./scrabble-word-finder ieclxal
    ace
    lex
    lexica
    lexical
    li
    lice
    lie
    lilac
    xi
    myth4>
    ~~~

* Can we write a server that leverages existing functionality and packages it differently?
    * Of course we can, else I wouldn't be asking.
    * Leverage the implementation of <tt><b>subprocess</b></tt> from Assignment 3.

    ~~~{.cpp}
    struct subprocess_t {
      pid_t pid;
      int supplyfd;
      int ingestfd;
    };
    subprocess_t subprocess(char *argv[], 
                            bool supplyChildInput, 
                            bool ingestChildOutput) throw (SubprocessException);
    ~~~

# Scrabble API
* Each request is handled by a dedicated thread within a <tt><b>ThreadPool</b></tt>
    * Thread routine uses <tt><b>subprocess</b></tt> to marshal plain text output
      of <tt><b>scrabble-word-finder</b></tt> into JSON, and publishes that JSON
      as the payload of the HTTP response.
    * Here's the core of the server-side computation:

    ~~~{.cpp}
    static void publishScrabbleWords(int clientSocket) {
      sockbuf sb(clientSocket);
      iosockstream ss(&sb);
      string letters = getLetters(ss); // extracts tail of path from GET <path> <protocol>
      skipHeaders(ss); // skips everything else
      const char *command[] = {"./scrabble-word-finder", letters.c_str(), NULL};
      subprocess_t sp = subprocess(const_cast<char **>(command), false, true);
      vector<string> formableWords = pullFormableWords(sp.ingestfd);
      waitpid(sp.pid, NULL, 0);
      ostringstream payload;
      constructPayload(formableWords, payload); // posts JSON to payload
      sendResponse(ss, payload.str()); // publishes HTTP response to ss out of payload
    }
    ~~~

    * Helper functions, caching, and error checking are omitted from the above, but included as part of the
      [full code base](http://cs110.stanford.edu/autumn-2017/examples/networking/scrabble-word-finder-server.cc).


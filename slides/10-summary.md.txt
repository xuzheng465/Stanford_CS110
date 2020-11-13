# Announcements
* Today's lecture
    * Review the first multithreading example from this past Wednesday's lecture, highlighting similarities
      between multprocessing and multithreading, pointing out differences.
    * Work through two versions of a multithreaded program where the main thread of execution shares
      data with each of the baby threads it creates.  
        * Version 1 is intentionally broken to illustrate the most obvious of race conditions.
        * Version 2 provides a very simple fix so that the race condition is no longer present.
    * Motivate my decision to move from C to C++ for its more robust and error-resistant treatment of threads.
    * Work through two or three C++ threading examples.

* Assigned Reading
    * You're reading all of Section 12.1 and Sections 12.3 through Sections 12.8 right
      now, skipping those subsections that refer to networking and servers.
        * Chapter 12 is actually the fourth chapter of the reader.
        * Code examples are in C, but the concepts are largely the same regardless of language.
    * Once we learn networking, we'll come back and hit on some of these excluded sections.

# Announcements
* Other announcements
    * Assignment 3 is due tonight, just before midnight.
    * Assignment 4 is out as well, due a week from tomorrow (Tuesday) night.
    * Midterm is on Friday, November 3<sup>rd</sup> during normal class time in CEMEX Auditorium.

# Introverts Revisited
* C++ provides threading and synchronization directives as part of the language now.
    * Here's the <tt><b>introverts</b></tt> example we've seen before&mdash;this time in C++!
      (Full version of this program is online [here](http://cs110.stanford.edu/autumn-2017/examples/threads-cpp/introverts.cc)).

    ~~~{.cpp}
    #include <iostream>       // for cout, endl
    #include <thread>         // for C++ thread support
    #include "ostreamlock.h"  // for CS110 iomanipulators (oslock, osunlock) used to lock down streams
    using namespace std;
    
    static void recharge() {
      cout << oslock << "I recharge by spending time alone." << endl << osunlock;
    }
    
    static const size_t kNumIntroverts = 6;
    int main(int argc, char *argv[]) {
      cout << "Let's hear from " << kNumIntroverts << " introverts." << endl      
      thread introverts[kNumIntroverts]; // declare array of empty thread handles
      for (thread& introvert: introverts)
        introvert = thread(recharge);    // move anonymous threads into empty handles
      for (thread& introvert: introverts)
        introvert.join();    
      cout << "Everyone's recharged!" << endl;
      return 0;
    }    
    ~~~

# Introverts Revisited
* C++ provides threading and synchronization directives as part of the language.
    * Features:
        * We declare an array of empty (e.g. non-joinable) <tt><b>thread</b></tt> handles as we
          did in the C version.  (The <tt><b>thread</b></tt> is a class relatively new to C++, and the holy grail
          of <tt><b>thread</b></tt> documentation pages sits 
          [right here](http://en.cppreference.com/w/cpp/thread/thread).)
        * We install the <tt><b>recharge</b></tt> function into temporary threads that
          are then moved (via <tt><b>operator=(thread&& other)</b></tt>) into an until-then
          empty <tt><b>thread</b></tt> handle.
            * This is a new form of <tt><b>operator=</b></tt> that fully transplants the contents
              of the <tt><b>thread</b></tt> on the right-hand side into the <tt><b>thread</b></tt> on the left-hand
              side, leaving the <tt><b>thread</b></tt> on the right side as fully gutted, as if it were zero-argument
              constructed (e.g. an empty, non-joinable <tt><b>thread</b></tt> handle).
            * This is an important clarification, because a traditional <tt><b>operator=</b></tt>
              would produce a second working copy of the same <tt><b>thread</b></tt>, and we don't want that.  We
              instead want to initialize one of the <tt><b>thread</b></tt>s within the original array to be one that
              <i>now runs code</i>.  This is how we do that.
        * The <tt><b>join</b></tt> method, not surprisingly, is equivalent to the <tt><b>pthread_join</b></tt>
          function we've already learned about.
        * The prototype of the thread routine&mdash;in this case, <tt><b>recharge</b></tt>&mdash;can be
          anything (although the return type is always ignored, so it should be <tt><b>void</b></tt> unless the
          same function is being used in a non-<tt><b>thread</b></tt>ed context, and a return value is useful there).
        * Big point: <tt><b>operator&lt;&lt;</b></tt>, unlike <tt><b>printf</b></tt>, is <b><u>not</u></b> thread-safe.
            * Even worse: a series of daisy-chained calls to <tt><b>operator&lt;&lt;</b></tt> is certainly not
              guaranteed to be executed as one, phatty atomic transaction.  
            * Simple solution: I've constructed stream manipulators <tt><b>oslock</b></tt> and
              <tt><b>osunlock</b></tt> to lock down and subsequently release access to an <tt><b>ostream</b></tt>
              so that two independent threads are never trying to simultaneulously insert character data into
              a stream at the same time.  In essence, <tt><b>oslock</b></tt> and <tt><b>osunlock</b></tt> are
              bookending our first <i>critical region</i>, which is a block of code that must be executed
              in full before any other thread can enter the same region, lest we see evidence of synchronization issues.

# Thread routines can be configured in a type-safe manner
* Thread routines can take any number of arguments.
    * Variable argument lists&mdash;the equivalent of the ellipsis in C&mdash;are supported via a new 
      C++ feature called [variadic templates](http://www.cplusplus.com/articles/EhvU7k9E/).
    * That means we have a good amount of flexibility in how we prototype our thread routines
        * There's no question this is better than the unsafe <tt><b>void</b></tt> <tt><b>*</b></tt>
          funkiness that <tt><b>pthreads</b></tt> provides.
    * Here's a slightly more involed example where <tt><b>greet</b></tt> threads are configured to
      say hello a variable number of times. (Online version of the following program can be
      found right [here](http://cs110.stanford.edu/autumn-2017/examples/threads-cpp/greeters.cc)).

    ~~~{.cpp}
    static void greet(size_t id) {
      for (size_t i = 0; i < id; i++) {
        cout << oslock << "Greeter #" << id << " says 'Hello!'" << endl << osunlock;
        struct timespec ts = {
          0, random() % 1000000000
        };
        nanosleep(&ts, NULL);
      }    
      cout << oslock << "Greeter #" << id << " has issued all of his hellos, "
           << "so he goes home!" << endl << osunlock;
    }
    
    static const size_t kNumGreeters = 6;
    int main(int argc, char *argv[]) {
      srandom(time(NULL));
      cout << "Welcome to Greetland!" << endl;    
      thread greeters[kNumGreeters];
      for (size_t i = 0; i < kNumGreeters; i++)
        greeters[i] = thread(greet, i + 1);
      for (thread& greeter: greeters)
        greeter.join();
      cout << "Everyone's all greeted out!" << endl;
      return 0;
    }
    ~~~

# Ticket Agents And  Selling Airline Tickets
* Multiple threads are often spawned so they can subdivide and collectively solve a larger problem.
    * Consider the case where 10 ticket agents answer telephones at United Airlines to jointly
      sell 1000 airline tickets (looking for full program? 
      click [here](http://www.stanford.edu/class/cs110/autumn-2017/examples/threads-cpp/tickets.cc)):

    ~~~{.cpp}
    static const unsigned int kBaseIDNumber = 101;
    static const unsigned int kNumAgents = 10;
    static const unsigned int kNumTickets = 1000;
    static mutex ticketsLock;
    static unsigned int remainingTickets = kNumTickets;
    
    static void ticketAgent(size_t id) {
      while (true) {
        ticketsLock.lock();
        if (remainingTickets == 0) break;
        remainingTickets--;
        cout << oslock << "Agent #" << id << " sold a ticket! (" << remainingTickets 
             << " more to be sold)." << endl << osunlock;
        ticketsLock.unlock();
        if (shouldTakeBreak()) 
          takeBreak();
      }
      ticketsLock.unlock();
      cout << oslock << "Agent #" << id << " notices all tickets are sold, and goes home!" 
           << endl << osunlock;
    }
    
    int main(int argc, const char *argv[]) {
      thread agents[kNumAgents];
      for (size_t i = 0; i < kNumAgents; i++)
        agents[i] = thread(ticketAgent, kBaseIDNumber + i);
      for (thread& agent: agents)
        agent.join();
      cout << "End of Business Day!" << endl;
    }
    ~~~

    * Yes, we use global variables, but our defense here is that multiple threads all
      need access to a shared resource.  <tt><b>static</b></tt> global variables are
      not at all uncommon in multithreaded programs (although larger programs would
      probably package them in a dedicated library or module).
    * There's a significant critical region in this program, and we use a <tt><b>mutex</b></tt>
      object to mark the beginning and end of it.
        * We'll first write this program without the <tt><b>mutex</b></tt> to illustrate
          what the problems are.
        * We'll then insert the <tt><b>mutex</b></tt> to understand exactly how it solves it.


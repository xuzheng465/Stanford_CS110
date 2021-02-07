# Agenda
* Review the <tt><b>condition_variable_any</b></tt> and its contribution to the solution
  to the Dining Philosophers problem we worked through during the final 15 minutes of Wednesday's
  lecture.
    * In my opinion, the condition variable if the most difficult of the multithreading directives
      to understand.
    * I want to do my share to ensure you understand condition variables now instead of allowing
      you to wait until they're needed for later assignments.
* Introduce the <tt><b>semaphore</b></tt>
    * The most recent solution to the dining philosophers problem enlisted the services of
      a <tt><b>mutex</b></tt> and a <tt><b>condition_variable_any</b></tt>.
      They collectively provide what can be best described as an integer with atomic increment,
      atomic decrement, and the added restriction that the integer can never go negative.  Any 
      attempt to decrement a 0 prompts that thread to block until some other thread increments it.
    * The counting variable specific to dining philosophers represents a limited resource shared
      among many competing threads&mdash;in essence, a limited number of permits allowing philosophers
      to eat.
    * We can and will generalize the idea of a counting variable by defining a <tt><b>semaphore</b></tt>
      class that encapsulates an integer, provides atomic increment and decrement operations via
      <tt><b>signal</b></tt> and <tt><b>wait</b></tt> methods, and indefinitely blocks a thread trying
      to decrement a <tt><b>semaphore</b></tt> surrounding a 0.
        * Conceptually, the <tt><b>semaphore</b></tt> allows us to model a shared resource&mdash;a number
          of remaining permission slips, a number of remaining file descriptors, a number of remaining
          network connections, etc&mdash;while insulating us from the complexities that come with coding
          with <tt><b>condition_variable_any</b></tt>.  <tt><b>condition_variable_any</b></tt>s are more general
          than the <tt><b>semaphore</b></tt>s implemented in terms of them, but a large percentage of
          synchronization needs can be expressed in terms of the <tt><b>semaphore</b></tt>, which in
          my opinion is easier to understand.
        * Many modern languages provide native support for threading and synchronization.
            * Java, in particular, has supported threading and condition variable-style locking
              since the beginning.  It eventually added a <tt><b>Semaphore</b></tt> class in the
              early 2000's.
            * Honestly, I'm not sure why C++11 decided to exclude the <tt><b>semaphore</b></tt>,
              but I think it's easier to work with than 
              <tt><b>condition_variable_any</b></tt>s, so I'm going to introduce it so we can go forward
              pretending that it's just part of the C++ language.

# Semaphore API
* The semaphore API is very small.
    * We've already mentioned that increment and potentially-blocking decrement are called
      <tt><b>signal</b></tt> and <tt><b>wait</b></tt>.
    * Here's the reduced interface for our own <tt><b>semaphore</b></tt> class.

    ~~~{.cpp}
    class semaphore {
     public:
      semaphore(int value = 0);
      void wait();
      void signal();
    
     private:
      int value;
      std::mutex m;
      std::condition_variable_any cv;
    
      semaphore(const semaphore& orig) = delete;
      const semaphore& operator=(const semaphore& rhs) const = delete;
    };    
    ~~~
    
    * You can use the <tt><b>semaphore</b></tt> by including the <tt><b>semaphore.h</b></tt> file.
        * All <tt><b>Makefile</b></tt>s are configured so that you can include it and link against
          its implementation.  You can operate like it's a C++ built-in class.
        * You can look in <tt><b>/usr/class/cs110/local/include/semaphore.h</b></tt> to confirm it's
          really there. &#9786;

# Semaphore API (continued)
* Design decisions
    * By design, there's no <tt><b>getValue</b></tt>-like method!
        * Some <tt><b>semaphore</b></tt> designs provide such a method, but we do not.
        * Why omit it?  In between the time you call it and act on it, some other thread could
          very well change it.  Concurrency directives themselves shouldn't encourage anything that
          might lead to a race condition or a deadlock, so our version doesn't.
    * Notice the three private data members are akin to the three global variables we introduced to
      limit the number of philosophers grabbing forks in the last version.
    * I remove the copy constructor and assignment operator (using the <tt><b>delete</b></tt>
      keyword), because neither the <tt><b>mutex</b></tt> nor <tt><b>condition_variable_any</b></tt>
      are copy constructable, copy assignable, or even movable.
        * In a nutshell, this means you need to pass all instances of <tt><b>mutex</b></tt>,
          <tt><b>condition_variable_any</b></tt>, and <tt><b>semaphore</b></tt> around by reference
          or via its address.

# Semaphore Implementation
* The implementation is very short and very dense.
    * Here's the implementation of the constructor:

    ~~~{.cpp}
    semaphore::semaphore(int value) : value(value), {}
    ~~~

    * <tt><b>m</b></tt> and <tt><b>cv</b></tt> are zero-argument constructed.
    * Data members are constructed in the order they appear in the <tt><b>class</b></tt>
      definition (not necessarily the order they appear in the initialization list).
    * Here are the implementations of <tt><b>wait</b></tt> and <tt><b>signal</b></tt>, which
      look like our most recent implementations of <tt><b>waitForPermission</b></tt> and
      <tt><b>grantPermission</b></tt>:

    ~~~{.cpp}
    void semaphore::wait() {
      lock_guard<mutex> lg(m);
      cv.wait(m, [this]{ return value > 0; });
      value--;
    }
    
    void semaphore::signal() {
      lock_guard<mutex> lg(m);
      value++;
      if (value == 1) cv.notify_all();
    }
    ~~~

    * Note that <tt><b>this</b></tt> needs to be captured by the on-the-fly
      predicate function we pass to <tt><b>cv.wait</b></tt>.  We need access to
      the <tt><b>value</b></tt> data member, and capturing the address of the
      surrounding object allows this.
        * <tt><b>[&value]</b></tt> works with <tt><b>g++</b></tt>, but it's off
          C++11 specification and won't necessarily work with other compilers.

# Final Version of Dining Philosophers
* Using <tt><b>semaphore</b></tt>s, in my opinion, improves the narrative.
    * Strip out exposed <tt><b>int</b></tt>, <tt><b>mutex</b></tt>, and 
      <tt><b>condition_variable_any</b></tt> and replace with single <tt><b>semaphore</b></tt>.
    * No longer need separate <tt><b>waitForPermission</b></tt> and <tt><b>grantPermission</b></tt> 
      functions. [#like](http://assets.weddingwire.com/images/vendors/blogs/facebook_like_button.jpg)
    * One last time, with feeling (code is right [here](http://cs110.stanford.edu/autumn-2017/examples/threads-cpp/dining-philosophers-with-semaphore.cc)):

    ~~~{.cpp}
    static mutex forks[kNumForks];
    static semaphore numAllowed(kNumForks - 1);
    
    static void eat(unsigned int id) {
      unsigned int left = id;
      unsigned int right = (id + 1) % kNumForks;
      numAllowed.wait(); // atomic -- that blocks on attempt to decrement 0
      forks[left].lock();
      forks[right].lock();
      cout << oslock << id << " starts eating om nom nom nom." << endl << osunlock;
      sleep_for(getEatDuration());
      cout << oslock << id << " all done eating." << endl << osunlock;
      numAllowed.signal(); // atomic ++, never blocks, possibly unblocks other waiting threads
      forks[left].unlock();
      forks[right].unlock();
    }
    ~~~

    * Parting comments:
        * It's easy to understand the transactional <tt><b>++</b></tt> and <tt><b>--</b></tt> that comes with
          <tt><b>signal</b></tt> and <tt><b>wait</b></tt>.  
        * The thread yield that comes with a <tt><b>wait</b></tt> on a <tt><b>semaphore</b></tt> value of 0 
          is more difficult to understand.  Given that a <tt><b>semaphore</b></tt> represents a shared, limited
          resource, blocking and doing nothing until that resource becomes available is almost always the right
          thing to do.
        * Make sure you understand the many pros on this approach over the 
          busy waiting approach we initially used to avert the threat of deadlock.
        * Can you think of any situations when busy waiting (also called spin locking) might be
          the right approach? [#thoughtquestion](https://blog.udemy.com/wp-content/uploads/2014/06/shutterstock_137493509-300x300.jpg)

# Canonical Reader/Writer Example
* Thread Rendezvous
    * <tt><b>semaphore::wait()</b></tt> and <tt><b>semaphore::signal()</b></tt> can be exploited
      to provide a <i>different</i> form of thread communication: <i>rendezvous</i>.
    * Here's our first example (full program is [right here](http://cs110.stanford.edu/autumn-2017/examples/threads-cpp/reader-writer.cc)):

    ~~~{.cpp}
    static const unsigned int kNumBuffers = 30;
    static const unsigned int kNumCycles = 4;
    
    static char buffer[kNumBuffers];
    static semaphore emptyBuffers(kNumBuffers);
    static semaphore fullBuffers(0);
    
    static void writer() {
      cout << oslock << "Writer: ready to write." << endl << osunlock;
      for (unsigned int i = 0; i < kNumCycles * kNumBuffers; i++) {
        char ch = prepareData();
        emptyBuffers.wait();   // don't try to write to a slot unless you know it's empty
        buffer[i % kNumBuffers] = ch;
        fullBuffers.signal();  // signal reader there's more stuff to read 
        cout << oslock << "Writer: published data packet with character '"
             << ch << "'." << endl << osunlock;
      }
    }
    
    static void reader() {
      cout << oslock << "\t\tReader: ready to read." << endl << osunlock;
      for (unsigned int i = 0; i < kNumCycles * kNumBuffers; i++) {
        fullBuffers.wait();    // don't try to read from a slot unless you know it's full
        char ch = buffer[i % kNumBuffers];
        emptyBuffers.signal(); // signal writer there's a slot that can receive data
        processData(ch);
        cout << oslock << "\t\tReader: consumed data packet "
             << "with character '" << ch << "'." << endl << osunlock;
      }
    }
    
    int main(int argc, const char *argv[]) {
      thread w(writer);
      thread r(reader);
      w.join();
      r.join();
      return 0;
    }
    ~~~

    * Think of the <tt><b>writer</b></tt> thread as one that <b>serves</b> data to a network connection, and
      think of the <tt><b>reader</b></tt> thread as one that <b>consumes</b> it.
    * Two <tt><b>semaphore</b></tt>s are used to synchronize the two so that:
        * <tt><b>reader</b></tt> is never further along than <tt><b>writer</b></tt>, and
        * <tt><b>writer</b></tt> is never so far ahead of <tt><b>reader</b></tt> that it
          clobbers data that has yet to be consumed.

# Implementing <tt><b>myth-buster</b></tt>
* Core of sequential version of <tt><b>myth-buster</b></tt>
    * Sequentially connects to all ~30 <tt><b>myth</b></tt> machines and asks each for the
      total number of processes being run by CS110 students.
    * Networking details are abstracted away and packaged in a library routine with the following prototype:

    ~~~{.cpp}
    int getNumProcesses(unsigned short num, const unordered_set<string>& sunetIDs);
    ~~~
 
    * <tt><b>num</b></tt> is the myth machine number (e.g. 14 for <tt><b>myth14</b></tt>),
      and <tt><b>sunetIDs</b></tt> is a hashset housing the SUNet IDs of all students
      currently enrolled in CS110 according to our <tt><b>/usr/class/cs110/repos/assign3/</b></tt>
      directory.
    * Here is the sequential (and very slow) implementation of a <tt><b>compileCS110ProcessCountMap</b></tt>, which
      very brute force and CS106B-ish. (It assumes that <tt><b>sunetIDs</b></tt> has already been configured with
      the set of all CS110 student SUNet IDs, and it further assumes that <tt><b>counts</b></tt> refers to
      an initially empty map).
    * Full program is [right here](http://www.stanford.edu/class/cs110/autumn-2017/examples/threads-cpp/myth-buster-sequential.cc).

    ~~~{.cpp}
    static unsigned short kMinMythMachine = 1;
    static unsigned short kMaxMythMachine = 32;
    static void compileCS110ProcessCountMap(const unordered_set<string>& sunetIDs,
                                            map<unsigned short, unsigned short>& counts) {
      for (unsigned short num = kMinMythMachine; num <= kMaxMythMachine; num++) {
        int numProcesses = getNumProcesses(num, sunetIDs);
        if (numProcesses >= 0) { // -1 expresses networking failure
          counts[num] = numProcesses;
          cout << "myth" << num << " has this many CS110-student processes: " 
               << numProcesses << endl;
        }
      }
    }    
    ~~~

    * Each call to <tt><b>getNumProcesses</b></tt> is slow, and the accumulation of some 30
      sequential calls is painfully slow.  
    * Running the sequential version takes on the order of 40 seconds, even though 99% of that
      time is spent waiting for a network connections to be established.

# Introduce threading
* By introducing threading, we overlay the network stall times.
    * Multithreaded version of the program is [right here](http://www.stanford.edu/class/cs110/autumn-2017/examples/threads-cpp/myth-buster-concurrent.cc).
    * Move shared data structures and synchronization directives to global space

    ~~~{.cpp}
    static unordered_set<string> sunetIDs;
    static map<unsigned short, unsigned short> processCountMap;
    static mutex processCountMapLock;
    ~~~

    * Wrap a <tt><b>thread</b></tt> around the core of the sequential code, and use a <tt><b>semaphore</b></tt> to
      limit the number of threads doing active work to a reasonably small number (but not so small that
      the program becomes sequential again) so that the thread manager doesn't get overloaded with
      all that many threads.
    * Note we use an overloaded version of the <tt><b>signal</b></tt> method that accepts the
      <tt><b>on_thread_exit</b></tt> tag as its only argument.  Rather than signaling the <tt><b>semaphore</b></tt>
      right then and there, invoking this second version schedules the signal to be sent
      after the entire thread routine has exited, just as the thread is being destroyed.

    ~~~{.cpp}
    static void countCS110Processes(unsigned short num, semaphore& s) {
      int numProcesses = getNumProcesses(num, sunetIDs);
      if (numProcesses >= 0) {
        processCountMapLock.lock();
        processCountMap[num] = numProcesses;
        processCountMapLock.unlock();
        cout << oslock << "myth" << num << " has this many CS110-student processes: "
             << numProcesses << endl << osunlock;
      }
    
      s.signal(on_thread_exit);
    }
    
    static unsigned short kMinMythMachine = 1;
    static unsigned short kMaxMythMachine = 32;
    static int kMaxNumThreads = 8; // number of threads beyond main thread that are permitted to exist at any one moment
    static void compileCS110ProcessCountMap() {
      vector<thread> threads;
      semaphore numAllowed(kMaxNumThreads);
      for (unsigned short num = kMinMythMachine; num <= kMaxMythMachine; num++) {
        numAllowed.wait();
        threads.push_back(thread(countCS110Processes, num, ref(numAllowed)));
      }
    
      for (thread& t: threads) t.join();
    }
    ~~~


# Announcements
* Today: finish up discussion of multprocessing, launch into multithreading and concurrency.
    * Assignment 3 due on Monday night.
    * Assignment 4 out on Monday night as well, due eight days later.
        * Will likely post the assignment handout and starter repos 
          on Sunday night, so look for them online.
    * B &amp; O Reading: Chapter 12, skipping section 12.2.  (Chapter 12 is the 
      fourth of the four chapters in the reader.)
    * I'll spend 10 or 15 minutes reviewing virtual memory, pages, translation lookaside buffers,
      schedulers, context switches, process control blocks, and ready and blocked PCB queues.  I discuss
      all of these because:
        * I want you to understand how each process can operate as if it owns all of memory, even
          when there are hundreds of other processes that operate the same way.
        * I want you to understand how multiple processes can be executing at the same time, even
          though the machine you're on has a very small number of CPUs.
    * During the last hour or so, I'll teach you a small amount of threading using C and <tt><b>pthreads</b></tt>.
        * Textbook examples reference <tt><b>pthreads</b></tt>, which is why I want 
          you to see at least a little bit of it in lecture before we abandon it and move over to C++.
        * Concepts taught in B &amp; O Chapter 12 (which you are reading over
          the course of the next two weeks) are relevant in all languages,
          so don't go on thinking the reading won't be helpful.
        * I'll also illustrate the simplest of concurrency issues using <tt><b>pthreads</b></tt>
          so I can later argue why C++ threads, which is what we'll spend most of our time learning,
          make things much easier (or at least less difficult, since anything having to do with
          threading is very challenging).
    * Will start C++ threading once I get through three small <tt><b>pthreads</b></tt> examples.
    * Be sure to inspect code samples in <tt><b>/usr/class/cs110/lecture-examples/autumn-2017/threads-c</b></tt>
      to see and play with working versions of everything in the coming slide decks.

# What is a thread?
* A thread is an independent thread of execution within a single process.
    * OSes and/or programming languages allow processes to split themselves
      into two or more concurrently executing functions.
    * Allows for intra-process concurrency (and even true parallelism on multiprocessor 
      and/or multicore machines)
        * Stack segment is subdivided into multiple miniature stacks.
        * Thread manager time slices and switches between simultaneously running
          threads in much the same way that the kernel scheduler switches between
          processes.  (In fact, threads are often called <b>lightweight processes</t> [#srsly](http://www.lies.com/wp/images/2009/12/srsly.jpg)).
        * Primary difference: threads have independent stacks, but they all share access to
          the same text, data, and heap segments.
            * Pro: easier to support communication between threads, because address spaces
              accessible are largely the same.
            * Pro: Multiple threads can access the same global data and one copy of the code.
            * Pro: One thread can share its stack space (via pointers and references) with
              another thread.
            * Con: No protected memory, since address space is shared.  Memory errors are
              more common, and debugging is more difficult.
            * Con: Multiple threads can access the same global data and one copy of the code. 
              [#doubleedgedsword](http://www.max-logic.com/wp-content/uploads/2011/05/1110690_f496.jpg)
            * Con: One thread can share its stack space (via pointers and references) with
              another thread. [#mixedblessings](http://upload.wikimedia.org/wikipedia/en/a/a6/Mixed_Blessings_DVD_cover.jpg)

# ANSI C doesn't provide native support for threads.
* But POSIX threads (aka <b><tt>pthreads</tt></b>) comes with all standard
  UNIX and Linux installations of <b><tt>gcc</tt></b>.
    * Primary <b><tt>pthreads</tt></b> data type is the <b><tt>pthread_t</tt></b>, which
      is an opaque type that helps manage how a C function is executed in its own
      thread of execution.
    * Only <b><tt>pthreads</tt></b> functions we'll concern ourselves with (before moving
      on to C++ threads) are <b><tt>pthread_create</tt></b> and <b><tt>pthread_join</tt></b>.
    * Here's a very small sample program (online version is [right here](http://www.stanford.edu/class/cs110/autumn-2017/examples/threads-c/introverts.c)):

    ~~~{.c}
    #include <stdio.h>    // provides printf, which is thread-safe
    #include <pthread.h>  // provides pthread_t type, thread functions
    
    static void *recharge(void *args) {
      printf("I recharge by spending time alone.\n"); // printf is thread-safe
      return NULL;
    }
    
    static const size_t kNumIntroverts = 6;
    int main(int argc, char *argv[]) {
      printf("Let's hear from %zu introverts.\n", kNumIntroverts);
      pthread_t introverts[kNumIntroverts];
      for (size_t i = 0; i < kNumIntroverts; i++)
        pthread_create(&introverts[i], NULL, recharge, NULL);
      for (size_t i = 0; i < kNumIntroverts; i++)
        pthread_join(introverts[i], NULL);
      printf("Everyone's recharged!\n");
      return 0;
    }
    ~~~
    * Program above declares an array of six <b><tt>pthread_t</tt></b> <i>handles</i>.
    * Program then initializes each <b><tt>pthread_t</tt></b> (via <b><tt>pthread_create</tt></b>) 
      by installing the <b><tt>recharge</tt></b> as the routine the each of the threads 
      should follow while executing.
        * All thread functions must take a <b><tt>void</tt></b> <b><tt>*</tt></b> 
          and return one as well.  That's generic programming in C. 
          [#sadface](http://wallpapercave.com/wp/fdtebOR.jpg)
        * Second argument to <b><tt>pthread_create</tt></b> allows thread attibutes (thread priorities, etc.) 
          to be set.  We'll always pass in <b><tt>NULL</tt></b> to accept the defaults.
        * Fourth argument is passed verbatim to the thread routine once the thread is launched.  
          In this case, there are no arguments, so we elect to pass in <b><tt>NULL</tt></b>.
        * Each <b><tt>recharge</tt></b> thread is eligible for processor time the instant
          the surrounding <b><tt>pthread_t</tt></b> is fully configured.
    * Six threads compete for thread manager's attention, and we have very little control over what
      choices the thread manager makes.  
        * The 0<sup>th</sup> thread will <i>probably</i> get processor time first.
        * Probable and guaranteed are two different words.  
        * We have no official word on the permutation of possible schedules the thread manager will go with.
    * <b><tt>pthread_join</tt></b> is to threads what <b><tt>waitpid</tt></b> is to processes.
      The main thread blocks until the argument thread exits. (Key difference: the waiting thread 
      need not have spawned the one it blocks on).

# Race Conditions
* Here's a slightly more involved program!
    * Extroverts get their turn!
    * Online version is what's presented below is
      [right here](http://www.stanford.edu/class/cs110/autumn-2017/examples/threads-c/confused-extroverts.c).

    ~~~{.c}
    static const char *kExtroverts[] = {
      "Albert Chon",
      "John Carlo Buenaflor",
      "Jessica Guo",
      "Lucas Ege",
      "Sona Allahverdiyeva",
      "Yun Zhang",
      "Tagalong Introvert Jerry Cain"
    };
    static const size_t kNumExtroverts = sizeof(kExtroverts)/sizeof(kExtroverts[0]) - 1;
    
    static void *recharge(void *args) {
      const char *name = kExtroverts[*(size_t *)args];
      printf("Hey, I'm %s.  Empowered to meet you.\n", name);
      return NULL;
    }
    
    int main() {
      printf("Let's hear from %zu extroverts.\n", kNumExtroverts);
      pthread_t extroverts[kNumExtroverts];
      for (size_t i = 0; i < kNumExtroverts; i++)
        pthread_create(&extroverts[i], NULL, recharge, &i);  
      for (size_t j = 0; j < kNumExtroverts; j++)
        pthread_join(extroverts[j], NULL);
      printf("Everyone's recharged!\n");
      return 0;
    }
    ~~~

# Race Conditions (continued)
* Here are a few (clearly broken) sample runs:

    ~~~{.sh}
    poohbear@myth12:$ ./confused-extroverts 
    Let's hear from 6 extroverts.
    Hey, I'm John Carlo Buenaflor.  Empowered to meet you.
    Hey, I'm Jessica Guo.  Empowered to meet you.
    Hey, I'm Sona Allahverdiyeva.  Empowered to meet you.
    Hey, I'm Sona Allahverdiyeva.  Empowered to meet you.
    Hey, I'm Yun Zhang.  Empowered to meet you.
    Hey, I'm Tagalong Introvert Jerry Cain.  Empowered to meet you.
    Everyone's recharged!
    poohbear@myth12:$ ./confused-extroverts 
    Let's hear from 6 extroverts.
    Hey, I'm Sona Allahverdiyeva.  Empowered to meet you.
    Hey, I'm Tagalong Introvert Jerry Cain.  Empowered to meet you.
    Hey, I'm Tagalong Introvert Jerry Cain.  Empowered to meet you.
    Hey, I'm Tagalong Introvert Jerry Cain.  Empowered to meet you.
    Hey, I'm Tagalong Introvert Jerry Cain.  Empowered to meet you.
    Hey, I'm Tagalong Introvert Jerry Cain.  Empowered to meet you.
    Everyone's recharged!
    poohbear@myth12:$
    ~~~

# Race Conditions (continued)
* There's clearly something wrong, but what's the problem?
    * Note that <b><tt>recharge</tt></b> references its incoming parameter this time, and
      that <b><tt>pthread_create</tt></b> accepts the location of the surrounding loop's
      index variable via its fourth argument.  <b><tt>pthread_create</tt></b>'s fourth
      argument it always passed verbatim as the single argument to the thread routine.
    * The problem: The main thread advances <b><tt>i</tt></b> without regard for the fact
      that <b><tt>i</tt></b>'s address was shared with each of the six child threads.
    * At first glance, it's easy to assume that <b><tt>pthread_create</tt></b> captures
      not just the address of <b><tt>i</tt></b>, but the value of <b><tt>i</tt></b> itself.
      That assumption would be incorrect, as it copies the address and that's all.
    * The address of <b><tt>i</tt></b> (even after it goes out of scope) is constant, but
      its contents evolve in parallel with the execution of the six <b><tt>recharge</tt></b>
      threads.  <b><tt>*(size_t *)args</tt></b> takes a snapshot of whatever <b><tt>i</tt></b> just
      happens to be at the time it's evaluated.
    * This is a simple example of what's called a <i>race condition</i>.

# Preventing Race Conditions
* Fortunately, the fix here is straightforward.
    * Just pass the <b><tt>const char *</tt></b> instead.
    * Online version is what's presented below is
      [right here](http://www.stanford.edu/class/cs110/autumn-2017/examples/threads-c/extroverts.c).

    ~~~{.c}
    static const char *kExtroverts[] = {
      "Albert Chon",
      "John Carlo Buenaflor",
      "Jessica Guo",
      "Lucas Ege",
      "Sona Allahverdiyeva",
      "Yun Zhang",
      "Tagalong Introvert Jerry Cain"
    };
    static const size_t kNumExtroverts = sizeof(kExtroverts)/sizeof(kExtroverts[0]) - 1;
    
    static void *recharge(void *args) {
      const char *name = args;
      printf("Hey, I'm %s.  Empowered to meet you.\n", name);
      return NULL;
    }

    int main() {
      printf("Let's hear from %zu extroverts.\n", kNumExtroverts);
      pthread_t extroverts[kNumExtroverts];
      for (size_t i = 0; i < kNumExtroverts; i++)
        pthread_create(&extroverts[i], NULL, recharge, (void *) kExtroverts[i]);
      for (size_t i = 0; i < kNumExtroverts; i++)
        pthread_join(extroverts[i], NULL);
      printf("Everyone's recharged!\n");
      return 0;
    }
    ~~~

# Preventing Race Conditions (continued)
* Drama averted!
    * A different address is shared with each installation of <b><tt>recharge</tt></b>.
      A snapshot of the relevant C string is taken before the thread is 
      even created, not while it's executing.
    * Race conditions are often very complicated, and avoiding them won't always be this trivial.
    * Here are a few test runs just so you see that it's fixed (and that the output varies from run to run).

    ~~~{.sh}
    poohbear@myth12:$ ./extroverts 
    Let's hear from 6 extroverts.
    Hey, I'm Albert Chon.  Empowered to meet you.
    Hey, I'm Jessica Guo.  Empowered to meet you.
    Hey, I'm Sona Allahverdiyeva.  Empowered to meet you.
    Hey, I'm Yun Zhang.  Empowered to meet you.
    Hey, I'm Lucas Ege.  Empowered to meet you.
    Hey, I'm John Carlo Buenaflor.  Empowered to meet you.
    Everyone's recharged!
    poohbear@myth12:$ ./extroverts 
    Let's hear from 6 extroverts.
    Hey, I'm Albert Chon.  Empowered to meet you.
    Hey, I'm John Carlo Buenaflor.  Empowered to meet you.
    Hey, I'm Jessica Guo.  Empowered to meet you.
    Hey, I'm Lucas Ege.  Empowered to meet you.
    Hey, I'm Sona Allahverdiyeva.  Empowered to meet you.
    Hey, I'm Yun Zhang.  Empowered to meet you.
    Everyone's recharged!
    poohbear@myth12:$
    ~~~


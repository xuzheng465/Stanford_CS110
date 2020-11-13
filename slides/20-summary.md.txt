# Announcements
* Important Dates
    * Final Assignment 8 Due Thursday at 11:59pm
        * If you turn it in on Thursday, your score is multipled by 1.08.
        * If you turn it in by Friday night, then there's no penalty, but there's no bonus either.
        * If you turn it in by Saturday night, then your grade is capped at a 90%.
        * You can't turn Assignment 8 in after Saturday at 11:59pm.
    * Final Exam is Wednesday, December 13<sup>th</sup> at 3:30pm in being held in two locations
        * The first letter of your last name determines where you should take the exam:
            * Last names beginning with A, B, C, ...., or K: Skilling Auditorium
            * Last names beginning with L, M, N, ...., or Z: Gates B01
        * Will cover all material taught in Lectures 1 through 18 (through my systems principles lecture) in depth, and will
          expect surface understanding of the nonblocking I/O material I started last Wednesday and taught this past
          Monday and today.
        * Exam is closed notes, closed book, closed computer, but you can bring and refer to two 8.5" x 11"
          sheets of paper with as much as you can cram onto their four sides, front and back.

# Announcements
* Today's Lecture
    * Nonblocking I/O and event-driven programming (<tt><b>epoll</b></tt>, 
      <tt><b>kqueue</b></tt>, and <tt><b>libev</b></tt>/<tt><b>libuv</b></tt> packages), 
      cross-language compilation, the [Tornado](http://www.tornadoweb.org) web server, 
      [node.js](http://nodejs.org/) and Google's [V8](https://developers.google.com/v8/) engine.
        * Discuss the <tt><b>epoll</b></tt> suite of functions: <tt><b>epoll_create</b></tt>, <tt><b>epoll_ctl</b></tt>, and
          <tt><b>epoll_wait</b></tt>.
        * Discuss the difference between edge-triggered and level-triggered events.
        * Implement an event-driven HTML server using nonblocking I/O in one process and one thread of execution that
          makes efficent use of the CPU.

# I/O-Event Driven Programming
* Introducing the <tt><b>epoll</b></tt> I/O notification utilities!
    * <tt><b>epoll</b></tt> is a package of Linux routines that help nonblocking servers
      yield the processor until it knows there's work to be done with one or more of the
      open client connections.
    * There are three functions in the <tt><b>epoll</b></tt> suite that I'll briefly
      introduce in lecture, if not today, then on Wednesday.
        * <tt><b>epoll_create</b></tt>, which creates something called a watch set, which
          itself is a set of file descriptors which we'd like to monitor.  The return
          value is itself a file descriptor used to identify a watch set.  Because it's
          a file descriptor, watch sets can contain other watch sets. #deep
        * <tt><b>epoll_ctl</b></tt> is a control function that allows us to add descriptors
          to a watch set, remove descriptors from a watch set, and reconfigure file descriptors
          already in the watch set.
        * <tt><b>epoll_wait</b></tt> waits for I/O events, blocking the calling thread until one
          or more events are detected.
    * The example I'll work with in class is too large to put in the slide deck, so you should
      refer to an [online copy](http://cs110.stanford.edu/autumn-2017/examples/non-blocking-io/efficient-server.cc).
      I'll run the new server in class, explain what it does, and cover key parts of the overall design
      and how it uses the <tt><b>epoll</b></tt> suite to efficiently operate as a nonblocking web server
      capable of managing tens of thousands of open connections at once.


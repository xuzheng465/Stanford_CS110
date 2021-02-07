# Announcements
* Going Out This Wednesday: Assignment 8: MapReduce!
    * Assignment 8 requires you to code up your very own MapReduce implementation.
    * Assignment 8 goes out on Wednesday night, and is due the following Thursday at 11:59pm.
      To incentivize you to turn in the final assignment on time, I've changed the Assignment 8 late policy a bit.
        * If the assignment is turned in on time, then I'll multiply your overall score by 1.08, which has the
          potential to raise your entire homework average by one full percentage point.
        * If the assignment is turned in one day late, then you can still get 100% of the points.  That means there's
          no penalty for turning in the assignment on Friday, December 8<sup>th</sup>.
        * If the assignment is turned in two days late, then your grade is capped at 90%.
        * Assignment 8 may not be turned in more than than two days late.  That part still holds.

* Today's Agenda
    * Take a step back and provide a high-level view of all of the various design
      principles we've discussed this quarter, reviewing previously discussed
      examples and introducing new ones.
    * Time Permitting, introduce the idea of a slow system call and what that means.

* Wednesday's Agenda
    * Nonblocking I/O!

# Fast and Slow System Calls
* The first week of class, we learned about kernel-resident functions called <b>system calls</b>.
    * Fast system calls are those that return immediately, where &quot;immediately&quot; means
      they just need the processor and other local resources to get their work done.
        * By my own definitely, there's no hard limit on the time they're allowed to take.
        * Even if a system call were to take 60 seconds to complete, it'd be considered fast
          if all 60 seconds were spent executing code (i.e. no idle time blocking on external
          resources.)
    * Slow system calls are those that wait for an indefinite period of time for something
      to finish (e.g. <tt><b>waitpid</b></tt>), for something to become available (e.g. <tt><b>read</b></tt>
      from a client socket that's not seen any data recently), or for some external event (e.g. network
      connection request from client via <tt><b>accept</b></tt>.)
        * Calls to <tt><b>read</b></tt> are considered fast if they're reading from a local file, because there
          aren't really any external resources to prevent read from doing it's work.  It's true that
          some hardware needs to be accessed, but because that hardware is grafted into the machine, we can
          say with some confidence that the data being read from the local file will be available within
          a certain amount of time.
        * Calls to <tt><b>write</b></tt> are considered slow if data if being published
          to a socket and previously published data has congested internal buffers and not been pushed
          off the machine yet.
    * Slow system calls are the ones capable of <u>blocking a thread of execution indefinitely</u>, 
      rendering that thread inert until the system call is able to return.
        * We've relied on signals and signal handlers to take calls to <tt><b>waitpid</b></tt> off the normal
          flow of execution, and we've also relied on the <tt><b>WNOHANG</b></tt> flag to ensure that
          <tt><b>waitpid</b></tt> never actually blocks.  
            * That's an example of nonblocking.  
            * We just didn't call it that back then.
        * We've relied on multithreading to get calls to <tt><b>read</b></tt> and <tt><b>write</b></tt> off
          the main thread. (Note: remember that the <tt><b>iosocsktream</b></tt> class implementation layers
          over calls to <tt><b>read</b></tt> and <tt><b>write</b></tt>.)
            * Threading doesn't make the calls to <tt><b>read</b></tt> and <tt><b>write</b></tt> any faster,
              but it does parallelize the stall times, and it also frees up the main thread so that it can, if
              it chooses, focus on other computations.
            * You should be intimiately familiar with these ideas based on your work with 
              <tt><b>news-aggregator</b></tt> and <tt><b>proxy</b></tt>.
    * <tt><b>accept</b></tt> and <tt><b>read</b></tt> are the system calls that everyone
      always identifies as slow.

# Making Slow System Calls Fast
* Configuring a descriptor as nonblocking.
    * It's possible to configure a descriptor to be what's called <b>nonblocking</b>.  When nonblocking
      descriptors are passed to <tt><b>accept</b></tt>, <tt><b>read</b></tt>, or <tt><b>write</b></tt>, the
      function will always return as quickly as possible without waiting for anything external.
    * <tt><b>accept</b></tt>
        * If a client connection is available when <tt><b>accept</b></tt> is called, it'll return immediately with
          a socket connection to that client.
        * Provided the server socket is configured to be nonblocking, <tt><b>accept</b></tt> will return -1
          instead of blocking if there are no pending connection requests.  The -1 normally denotes that
          some error occurred, but if the <tt><b>errno</b></tt> global is set to <tt><b>EWOULDBLOCK</b></tt>,
          the -1 isn't ***really*** identifying an error, but instead saying that <tt><b>accept</b></tt> would have
          blocked had the server socket passed to it been a traditional (i.e. blocking) socket descriptor.
    * <tt><b>read</b></tt>
        * If one or more bytes of data are immediately available when <tt><b>read</b></tt> is called, then
          those bytes (or at least some of them) are written into the supplied character buffer, and the
          number of bytes placed is returned.
        * If no data is available (and the descriptor is still open and the other end of the descriptor 
          hasn't been shut down,) then <tt><b>read</b></tt> will return -1, provided the descriptor has been
          configured to be nonblocking.  Again, this -1 normally denotes that some error occurred, but in this
          case, the <tt><b>errno</b></tt> global is set to <tt><b>EWOULDBLOCK</b></tt>.  That's our clue that
          <tt><b>read</b></tt> didn't ***really*** fail.  The -1/<tt><b>EWOULDBLOCK</b></tt> combination is just saying
          that the call to <tt><b>read</b></tt> ***would have*** blocked had the descriptor been a traditional
          (i.e. blocking) one.

# Principles of System Design
* Let's take a step back and look at the big picture.
    * I'm all about implementation-driven lectures.  I prefer to rely on code to teach the material.
    * However, you also need to walk away from this course with an understanding
      of the basic principles guiding the design and implementation of large systems, be they
      file systems (Assignment 2), multithreaded RSS feed aggregators (Assignments 5 and 6),
      HTTP proxies (Assignment 7), or MapReduce frameworks (Assignment 8).
    * An understanding of and appreciation for these principles will help you make better design
      and implementation decisions should you take our more advanced systems courses.
        * [CS140: Operating Systems](http://cs140.stanford.edu), which has you design and <i>implement</i> processes, threads,
          virtual memory, and a much more robust filesystem than what you were charged with implementing
          for Assignment 2.
            * CS110 emphasizes the client use of processes, threads, concurrency 
              directives, and so forth.  CS140 is all about implementing them.
        * [CS143: Compiler Construction](http://cs143.stanford.edu), which has you implement 
          a pipeline of components that ultimately translates programs written in COOL (a language
          similar to C++) into an equivalent stream of assembly code instructions.
        * [CS144: Computer Networking](http://cs144.stanford.edu), where you study how computer 
          networks (the Internet in particular) are implemented.
            * CS110 emphasizes the client use of sockets and the socket API functions as a vehicle for
              building networked applications.  CS144 is all about understanding and, in some cases,
              implementing the various network layers that allow for those functions to work and work well.
    * <i>These slides are the result of a series of email exchanges and conversations with Mendel
      Rosenblum (CS110, CS140, and CS142 instructor) and Phil Levis (CS110E and CS144 instructor).</i> 

# Principles of System Design
* CS110 touches on seven such principles:
    * Abstraction
    * Modularity and Layering
    * Naming and Name Resolution
    * Caching
    * Virtualization
    * Concurrency
    * Client-server request-and-response

# Principles of System Design
* CS110 touches on seven such principles:
    * Abstraction
        * Separating behavior from implementation (trivial example: <tt><b>sort</b></tt> 
          has one interface, but a billion different implementations).
        * Defining of a clean interface to a component or subsystem that 
          makes using and interacting with a system much, much easier.
        * Examples of abstractions we've taken for granted (or will soon take for granted) this
          quarter in CS110:
            * <b>filesystems</b> (you've dealt with C <tt><b>FILE *</b></tt>s and C++ <tt><b>iostream</b></tt>s
              for a while now, and knew little of how they might work until we studied them this quarter).
              We did learn about file descriptors this quarter, and we'll soon leverage that abstraction
              to make other data sources (e.g. network connections) look and behave like files.
            * <b>processes</b> (you know how to fork off new processes now, even though you have no idea
              how <tt><b>fork</b></tt> and <tt><b>execvp</b></tt> work).
            * <b>signals</b> (you know they're used by the kernel to message a process that something 
              significant occurred, but you don't know how they're implemented).
            * <b>threads</b> (you know how to create C++11 <tt><b>thread</b></tt>s, but you don't <i>really</i>
              know how they're implemented).
            * <b>HTTP</b> (you're just now learning how to use the protocol that networked
              computers often use to exchange resources such as text documents, images, audio
              files, and so forth).
    * Modularity and Layering
    * Naming and Name Resolution
    * Caching
    * Virtualization
    * Concurrency
    * Client-server request-and-response

# Principles of System Design
* CS110 touches on seven such principles:
    * Abstraction
    * Modularity and Layering
        * Subdivision of a larger system into a collection of smaller
          subsystems, which themselves may be further subdivided into even
          smaller sub-subsysytems.
        * Example: filesystems, which use a form of modularity called <b>layering</b>, which
          is the organization of several modules that interact in some hierarchical
          manner, where each layer typically only opens its interface to the module
          above it.  Recall the layering scheme we subscribed to for Assignment 2:
            * symbolic link layer
            * absolute path name layer
            * path name layer
            * file name layer
            * inode number layer
            * file layer
            * block layer
    * Naming and Name Resolution
    * Caching
    * Virtualization
    * Concurrency
    * Client-server request-and-response

# Principles of System Design
* CS110 touches on seven such principles:
    * Abstraction
    * Modularity and Layering
        * Subdivision of a larger system into a collection of smaller
          subsystems, which themselves may be further subdivided into even
          smaller sub-subsysytems.
        * Example: filesystems
        * Example: <b><tt>g++</tt></b>, which chains together a series of components (which is, in a sense,
            another form of layering).
            * the <b>preprocessor</b>, which manages <tt><b>#include</b></tt>s, <tt><b>#define</b></tt>s,
              and other preprocessor directives to build a translation unit that is fed to...
            * the <b>lexer</b>, which reduces the translation unit down to a stream of tokens which
              are fed in sequence to...
            * the <b>parser</b>, which groups tokens into <i>syntactically</i> valid constructs, which
              are then semantically verified by...
            * the <b>semantic analyzer</b>, which confirms that the syntatically valid constructs make
              sense and respect the rules of the type system, so that x86 instructions can
              be emitted by...
            * the <b>code generator</b>, which translate your C++ code into equivalent machine code.
    * Naming and Name Resolution
    * Caching
    * Virtualization
    * Concurrency
    * Client-server request-and-response

# Principles of System Design
* CS110 touches on seven such principles:
    * Abstraction
    * Modularity and Layering
        * Subdivision of a larger system into a collection of smaller
          subsystems, which themselves may be further subdivided into even
          smaller subsubsysytems.
        * Example: filesystems
        * Example: <b><tt>g++</tt></b>
        * Example: computer networks, which rely on a programming
          model known as TCP/IP, so named because its two most important
          protocols (TCP for Transmission Control Protocol, IP for Internet Protocol)
          were the first to be included in the standard.
            * TCP/IP specifies how data should be packaged, transmitted, routed, and received.
            * The implementation is distributed across four different layers.
                * application layer (the highest layer in the stack)
                * transport layer
                * internet layer
                * link layer (the lowest layer in the stack)
            * We just learned the application-level API calls (<tt><b>socket</b></tt>, <tt><b>bind</b></tt>,
              <tt><b>connect</b></tt>, and so forth) needed to build networked applications
              (Assignments 7 and 8 have you build two of them)
            * [CS144](http://cs144.stanford.edu) teaches all four layers in detail and 
              how each layer interacts with the one below it.
    * Naming and Name Resolution
    * Caching
    * Virtualization
    * Concurrency
    * Client-server request-and-response

# Principles of System Design
* CS110 touches on seven such principles:
    * Abstraction
    * Modularity and Layering
        * Subdivision of a larger system into a collection of smaller
          subsystems, which themselves may be further subdivided into even
          smaller subsubsysytems.
        * Example: filesystems
        * Example: <b><tt>g++</tt></b>
        * Example: computer networks
        * Example: [<b><tt>Facebook</tt></b>](http://www.facebook.com), which includes so 
          many modules and subsystems I can't even identify all of them.  (Note that this
          is an example of a large system where some subsystems use layering [the caching
          layers come to mind] and others do not).  Here are just a few of the large components
          I worked on during my time at Facebook.
            * huge number of servers
            * many, many databases
            * several caching layers (C++ implementation of Apache's APC, specialized 
              [memcached](http://memcached.org/), [TAO](https://www.facebook.com/notes/facebook-engineering/tao-the-power-of-the-graph/10151525983993920))
            * dedicated photo and video upload servers
            * customized [photo](http://www.stanford.edu/class/cs240/readings/haystack.pdf) storage system
              optimized to hold most file metadata in main memory and to minimize disk seeks
            * Linux kernels hyperoptimized to be fast at things important to Facebook
            * homegrown [PHP-to-C++](http://en.wikipedia.org/wiki/HipHop_for_PHP) compiler, 
              so the servers are processes instead of PHP scripts
            * standalone PHP libraries of reusable UI components (buttons, scrollbars, menus, etc, 
              all with the Facebook look-and-feel)
            * dedicated grid of machines in place to provide real-time, machine-learning-driven story 
              selection for your News Feed
    * Naming and Name Resolution
    * Caching
    * Virtualization
    * Concurrency
    * Client-server request-and-response

# Principles of System Design
* CS110 touches on seven such principles:
    * Abstraction
    * Modularity and Layering
    * Naming and Name Resolution
        * Names provide a way to refer to system resources, and name resolution
          is a means for converting between human-readable names and machine-friendly ones.
        * We've already seen two examples:
            * Humans prefer absolute and relative pathnames to identify files, and
              computers work better with inode and block numbers.  You spent
              a good amount of energy with Assignment 2 managing the discovery
              of inode numbers and file block contents given a file's name.
            * Humans prefer domain names like [www.google.com](http://www.google.com), and computers
              work better with IP addresses like [74.125.239.51](http://74.125.239.51).
              We spent time in lecture understanding exactly how the human-readable domain name is
              converted to an IP address.
        * Other examples: the URL (a human-readable form of a resource location),
          the process ID (a computer-friendly way of referring to a process),
          and the file descriptor (a computer-friendly way of referring to a file [or something
          that behaves like a file&mdash;yay virtualization and abstraction!])
    * Caching
    * Virtualization
    * Concurrency
    * Client-server request-and-response

# Principles of System Design
* CS110 touches on seven such principles:
    * Abstraction
    * Modularity and Layering
    * Naming and Name Resolution
    * Caching
        * Simply stated, a cache is a component&mdash;possibly in hardware, possibly in software&mdash;
          that stores data so that future requests can be handled more quickly.
        * Examples of basic address-based caches (as taught in CS107)
            * L1-level instruction and data caches that serve as a staging area for CPU registers.
            * L2-level caches that serve as a staging area for L1 caches.
            * A portion of main memory&mdash;the portion not backing the virtual address
              spaces of active processes&mdash;used as a disk cache to store pages of files.
        * Examples of caches to store results of repeated (often expensive) calculations:
            * Web browsers that cache recently fetched documents, provided the server is clear
              that the documents can be cached, either indefinitely or for a period of time.
            * Web proxies that cache static resources so that <i>other</i> clients requesting
              the same data can be served more quickly.
            * DNS caches, which hold a mapping of recently resolved domain names to their IP addresses.
            * [memcached](http://www.memcached.org), which maintains a dictionary of objects
              frequently used to generate web content.
            * The four different caches in the [Solaris file system layers](http://www.princeton.edu/~unix/Solaris/troubleshoot/diskio.html#fscache)
                * The Directory Name Lookup Cache stores vnode-entry-to-path-directory-lookup information.
                * The inode cache collects and maintains file metadata information stored more permanently in inodes.
                * The rnode cache stores information about the many nodes contributing to an NFS filesystem.
                * The buffer cache stores inodes and indirect disk blocks.
    * Virtualization
    * Concurrency
    * Client-server request-and-response

# Principles of System Design
* CS110 touches on seven such principles:
    * Abstraction
    * Modularity and Layering
    * Naming and Name Resolution
    * Caching
    * Virtualization
        * Virtualization is an abstraction mechanism used to make
          many hardware resources look like one.  Examples include:
            * RAID, which aggregates many (often inexpensive) storage devices
              to behave as a single hard drive.
            * the <b>Andrew File System</b>, aka AFS, which grafts many 
              independent, networked file systems into one rooted at 
              <b><tt>/afs</tt></b>.  You descend into various subdirectories
              of <b><tt>/afs</tt></b> with, in principle, zero knowledge of
              where in the world those directories are stored.
            * a web server load balancer, where hundreds, thousands, or even 
              hundreds of thousands of servers are fronted by a much
              smaller set of machines that intercepts all requests and
              quickly forwards them to the server least loaded.
        * Virtualization is <i>also</i> an abstraction mechanism used to make
          a single hardware resource look like many.  Examples include:
            * virtual-to-physical memory mapping, which allows each process 
              to believe it owns the entire address space.  We covered virtual-to-physical
              memory mapping in lecture a few weeks ago.
            * threads, where the stack frame of a single process is subdivided
              into many stack frames so that multiple threads of execution, each
              with their own context, can be rotated through in much the same way
              the scheduler rotates through multiple processes. (Threads, in fact, are often
              called <b>virtual processes</b>.  A single process is made to look like many).
            * virtual machines, which are software implementations designed to execute
              programs as a physical machine would.  Virtual machines can do something
              as little as provide a runtime environment for, say, a Java or C# executable,
              or it can do as much as run several different operating systems (Mendel Rosenblum's
              VMware comes to mind) on an architecture that otherwise couldn't support them.
    * Concurrency
    * Client-server request-and-response

# Principles of System Design
* CS110 touches on seven such principles:
    * Abstraction
    * Modularity and Layering
    * Naming and Name Resolution
    * Caching
    * Virtualization
    * Concurrency
        * We have a good amount of experience with concurrency already:
            * Multiple processes running on a single processor, seemingly at the same time.
            * Multiple threads running within a single process, seemingly at the same time.
        * When multiple processors and/or multiple cores are available, processes can truly
          run in parallel, and threads within a single process can run in parallel.
        * Signal and interrupt handlers are also technically concurrent programs.  Program
          execution occasionally needs to be halted to receive information from the outside
          (the file system, the keyboard, the mouse, the Wacom tablet, the attached MIDI
          controller, or the network).
        * Some programming languages&mdash;[Erlang](http://http://www.erlang.org/) comes to
          mind&mdash; are so inherently concurrent that they adopt a programming model
          that makes race conditions virtually impossible.  (Other languages&mdash;I'm 
          thinking of pure JavaScript&mdash;adopt the view that concurrency, or at least threading, 
          is too complicated and error-prone to support).
        * Come Wednesday, you'll bathe in all things MapReduce as a programming model
          that employs multiple threads or processes across multiple processors across
          multiple machines to process huge amounts of information and whittle that
          information down into useful, aggregated data sets.
    * Client-server request-and-response

# Principles of System Design
* CS110 touches on seven such principles:
    * Abstraction
    * Modularity and Layering
    * Naming and Name Resolution
    * Caching
    * Virtualization
    * Concurrency
    * Client-server request-and-response
        * Request/response is a way to organize functionality into modules that
          have a clear set of responsibilities.
        * We've already had some experience with the request-and-response 
          aspect of this.
            * system calls (<b><tt>open</tt></b>, <b><tt>write</tt></b>, <b><tt>fork</tt></b>,
              <b><tt>sleep</tt></b>, <b><tt>bind</tt></b>, and so forth are all userland
              wrappers around a special type of call into the kernel... user space and kernel
              space are two separate modules with a very, very hard boundary between them).
            * HTTP, IMAP
            * NFS, AFS
            * DNS


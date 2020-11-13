# Multiprocessing Wrap
* Today's Agenda
    * I need to discuss how one can install a <tt><b>SIGCHLD</b></tt> handler to asynchronously
      monitor how child processes return, crash, stop, and restart.  In particular, I need
      to work through [these](http://web.stanford.edu/class/cs110/autumn-2017/examples/processes/five-children.c)
      [three](http://web.stanford.edu/class/cs110/autumn-2017/examples/processes/indistinguishable-pentuplets.c)
      [programs](http://web.stanford.edu/class/cs110/autumn-2017/examples/processes/pentuplets.c).
    * I still need to work through the synchronization examples I didn't get to on Wednesday either.
        * Buggy program is [here](http://web.stanford.edu/class/cs110/autumn-2017/examples/processes/job-list-synchronization.c).
        * Fixed program, which relies on signals masks, is right
          [here](http://web.stanford.edu/class/cs110/autumn-2017/examples/processes/job-list-synchronization-improved.c).
    * I want to introduce the <tt><b>kill</b></tt> system call, which allows one process to send an arbitrary
      signal to another.
        * The function is pretty easy to understand, but I want to work through a short programming puzzle
          just to make sure.
        * That programming puzzle was almost posted last Wednesday, and it can be found right
          [here](http://web.stanford.edu/class/cs110/autumn-2017/examples/processes/kill-puzzle.c).
    * I want to give you some high-level sense as to how the OS supports multiprocessing
      so that each process can operate as if it owns all of its 4GB (or 256TB, in a 64-bit world)
      virtual address space.  This part is all concept and whiteboard illustrations, but there's no code.
    * I also want you to understand how the scheduler can round-robin through all of the active
      processes so that each of them gets enough processor time to make progress.  Again, all 
      concept and whiteboard drawings, but no code.


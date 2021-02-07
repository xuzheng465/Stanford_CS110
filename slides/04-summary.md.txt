# Announcements
* Assignment 1 due on Wednesday night.
* CA office hours!
    * The full matrix of office hours is posted [right here](http://web.stanford.edu/class/cs110/autumn-2017/calendar.html).
    * Office hours are great for asking questions about the lecture material and the assignment specifications that can't
      be easily managed on Piazza.
* Reading for this week:
    * Do a close reading of the Bryant &amp; O'Hallaron reader, Chapter 2 (much of which you know about already:
      <b><tt>open</tt></b>, <b><tt>read</tt></b>, <b><tt>write</tt></b>, etc) and Chapter 1 (in that order).
      Read these two chapters slowly and carefully over the course of this week.
* Expect to finish up filesystems, naming, and layering today, and we'll begin multiprocessing and exceptional control flow if we finish a little early.
     * All multiprocessing and exceptional control flow examples can be found in <tt><b>/usr/class/cs110/lecture-examples/autumn-2017/processes</b></tt>.

# Multiprocessing: First Program
* New system call: <b><tt>fork</b></tt>
    * Here is the simplest of programs that knows how to create other 
      processes.  It uses a system call named <b><tt>fork</b></tt>.
    * Code is in lecture examples folder: <b><tt>processes/basic-fork.c</tt></b>. Code is also right
      [here](http://www.stanford.edu/class/cs110/autumn-2017/examples/processes/basic-fork.c).

	~~~{.c}
    static const int kForkFailed = 1;
    int main(int argc, char *argv[]) {
      printf("Greetings from process %d! (parent %d)\n", getpid(), getppid());
      pid_t pid = fork();
      exitIf(pid == -1, kForkFailed, stderr, "fork function failed.\n");
      printf("Bye-bye from process %d! (parent %d)\n", getpid(), getppid());
      return 0;
    }
	~~~
* <b><tt>fork</tt></b> is called once, but it returns twice.
    * <b><tt>fork</tt></b> knows how to clone the calling process, create a virtually
      identical copy of it, and schedule it as if the second copy of the original were
      running all along.  All segments (data, bss, init, stack, heap, text) are faithfully replicated, 
      and all open file descriptors from the first are duplicated and donated to the clone.
    * The only difference: <b><tt>fork</tt></b>'s return value in the new process (the <b>child</b>) is 0, and
      fork's return value in the spawning process (the <b>parent</b>) is the child's process id.  The return
      value can be used to dispatch each of the two processes in a different direction (although in this
      introductory example, we don't do that).
    * As a result, the output of the above program is really the output of two processes.  We should expect to
      see a single greeting but two separate bye-byes.
    * Key observation: Each of the bye-byes is inserted into the console by two different processes.  The order
      each line executes, in principle, cannot be predicted.  The system's scheduler dictates whether the child
      or the parent gets to print its bye-bye first (although on multiple core machines, child and parent
      could be running in parallel).

# Explosive <b><tt>fork</tt></b> graphs
* Understand the workflow, beware of <b><tt>fork</tt></b> bombing your system.
    * While you rarely have reason to use fork this way (and you shouldn't, or you'll begin to tax
      the shared system you're working on and/or hit your limit on the number of concurrent processes),
      it's instructive to trace through a short program where forked processes themselves call <b><tt>fork</tt></b>.
    * Code is in lecture examples folder: <b><tt>processes/fork-puzzle.c</tt></b>.  Code is also right
      [here](http://www.stanford.edu/class/cs110/autumn-2017/examples/processes/fork-puzzle.c).
    * Check this out:

	~~~{.c}
    static const char const *kTrail = "abcd";
    static const int kForkFail = 1;
    int main(int argc, char *argv[]) {
      size_t trailLength = strlen(kTrail);
      for (size_t i = 0; i < trailLength; i++) {
        printf("%c\n", kTrail[i]);
        pid_t pid = fork();
        exitIf(pid == -1, kForkFail, stderr, "Call to fork failed.");
      }
      return 0;
    }
	~~~
    * Reasonably obvious: One a is printed by the soon-to-be-great-grandaddy process.
    * Less obvious: The first child and the parent each return from fork and continue
      running in mirror processes, each with their own copy of the global "abcd" string,
      and each advancing to the i++ line within a loop that promotes a 0 to 1.
      It's hopefully clear now that two b's will be printed.
    * Key questions to answer:
        * How many c's get printed?
        * How many d's get printed?
        * Are both b's necessarily printed one after another?

# Synchronizing execution between parent and child
* <b><tt>waitpid</tt></b> instructs a process to block until another process exits.  
    * The first argument specifies the wait set, which for the moment is just the id of the child 
      process that needs to complete before <b><tt>waitpid</tt></b> can return. (There are more 
      possibilities, but we'll explore them later).
    * The second argument supplies the address of an integer where child termination status
      information can be placed (or we can pass in <b><tt>NULL</tt></b> if we don't care
      to get that information).
    * The third argument is a collection of bit flags we'll study later.  For the time being,
      we'll just go with 0 as the required parameter value, which means that <b><tt>waitpid</tt></b> should
      only return when a child exits.
    * The return value is the id of the child process that exited, or -1 if <b><tt>waitpid</tt></b> was called
      and there were no child processes to wait on.
    * Code for next small example is in lecture examples folder: <b><tt>processes/parent-child.c</tt></b>.
      Code is also right [here](http://www.stanford.edu/class/cs110/autumn-2017/examples/processes/parent-child.c).
    * Here's that example:

	~~~{.c}
    static const int kForkFailure = 1;
    int main(int argc, char *argv[]) {
      printf("I'm unique and just get printed once.\n");
      pid_t pid = fork(); // returns 0 within child, returns pid of child within fork
      exitIf(pid == -1, kForkFailure, stderr, "Call to fork failed... aborting.\n");
      bool parent = pid != 0;
      if ((random() % 2 == 0) == parent) sleep(1); // force exactly one of the two to sleep
      if (parent) waitpid(pid, NULL, 0); // parent shouldn't exit until it knows its child has finished
      printf("I get printed twice (this one is being printed from the %s).\n", parent  ? "parent" : "child");
      return 0;
    }
	~~~

    * The above example uses a coin flip to seduce one of the two processes to sleep for a second, which
      is normally all the time the other process needs to print what it needs to print first.
    * The parent thread elects to wait for the child thread to exit before it allows itself to
      exit.  It's a akin to a parent not being able to go to bed until he or she knows the child has, and
      it's emblematic of the types of synchronization directives we'll be seeing a lot of this quarter.
    * Understand that the final <b><tt>printf</tt></b> gets executed twice.  The child is always
      the first to execute it, however, because the parent is blocked in the <b><tt>waitpid</tt></b> call
      until the child executes <b>everything</b>.

# Parent and child processes normally diverge:
* The following program is a step toward how <tt><b>fork</b></tt> gets
  used in practice:
    * Check this out (code is in lecture examples folder <tt><b>processes/separate.c</b></tt>
      and can also be viewed right [here](http://cs110.stanford.edu/autumn-2017/examples/processes/separate.c))

    ~~~{.c}
    int main(int argc, char *argv[]) {
      printf("Before.\n");
      pid_t pid = fork();
      exitIf(pid == -1, kForkFailed, stderr, "Fork function failed.\n");
      printf("After.\n");
      if (pid == 0) {
        printf("I'm the child, and the parent will wait up for me.\n");
        return 110; // contrived exit status
      } else {
        int status;
        exitUnless(waitpid(pid, &status, 0) == pid, kWaitFailed, stderr, 
                   "Parent's wait for child process with pid %d failed.\n", pid);
        if (WIFEXITED(status)) {
          printf("Child exited with status %d.\n", WEXITSTATUS(status));
        } else {
          printf("Child terminated abnormally.\n");
        }
        return 0;
      }
    }
    ~~~

    * The above example directs the child process one way, the parent another.
    * The parent process correctly waits for the child to complete, and
      this example (as opposed to the last example from the last slide deck)
      actually does the right thing by asserting <tt><b>waitpid</b></tt>'s
      return value matches the process id of the child that exited.
    * The parent also lifts exit status information about the child process
      out of the <tt><b>waitpid</b></tt> call, and uses the <tt><b>WIFEXITED</b></tt>
      macro to examine some high-order bits of this <tt><b>status</b></tt> 
      argument to confirm the process exited normally, and it also uses the
      <tt><b>WEXITSTATUS</b></tt> macro to extract the lower eight bits
      from its argument to produce the child process's return value.
    * Check out the man page for <tt><b>waitpid</b></tt> for the good
      word on all of the different macros. (Your textbook covers them
      all really nicely as well).


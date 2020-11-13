# Announcements
* Assignments 2 and 3
    * Assignment 2 due tonight at 11:59pm.
        * We expect to crank on Assignment 2 submissions starting on Saturday morning (after
          the hard deadline has passed) and send out grade reports by next Saturday.
        * In general, we promise to get grade reports back 7 days after the hard deadline.
    * Assignment 3 goes out later tonight and falls due a week from Monday, on October 23<sup>rd</sup>.
        * Assigment 3 is the longest of the quarter, and that's why you have such a long time
          to complete it.  Please start soon.

* Readings:
    * Finish reading B&amp;O Chapter 2 (Chapter 10 of full textbook)
      so you can confirm that you know most of the material there,
      as I covered much of it during the first two weeks of lecture.
    * Finish reading B&amp;O Chapter 1 (Chapter 8 of the full textbook),
      focusing on Section 5, which covers process groups, signals,
      and signal handlers, all three of which will contribute to your
      Assignment 3 submission.

* Agenda? Still riding this multiprocessing train!
    * Last time I was just about to start up on
      [this](http://web.stanford.edu/class/cs110/autumn-2017/lectures/06-execvp-subprocess.html#(1)),
      but didn't quite get there.  Our discussion of the <tt><b>subprocess</b></tt> function relies on
      the notion of a pipe, the <tt><b>pipe</b></tt> and <tt><b>dup2</b></tt>
      system calls, and how they can be used to foster more sophisticated communication between
      multiple processes.
    * Once we work through <tt><b>subprocess</b></tt>, we'll start up on signals and signal handlers.

# Introduction to Signals
* Signals
    * A signal is a small message that notifies a process that an event of some type
      occurred.  Signals are often sent by the kernel, but they can be sent from other
      processes as well.
    * A signal handler is a function that executes in response to the 
      arrival and consumption of a signal.
    * You're already familiar with some signals, even if you've not referred to them by 
      that name before:
        * You haven't really programmed in C before unless you've dereferenced a <b><tt>NULL</tt></b>
          pointer.  When that happens, the kernel delivers a signal of type
          <tt><b>SIGSEGV</b></tt>, informally known as a segmentation fault (or a SEGmentation
          Violation, or <tt><b>SIGSEGV</b></tt> for short).  Unless you install a custom signal
	  handler to manage the signal differently, a <tt><b>SIGSEGV</b></tt> terminates
          the program and generates a core dump.
        * Whenever a process commits an integer-divide-by-zero (and, in some cases,
	  a floating-point divide by zero), the kernel hollers and issues a
          <tt><b>SIGFPE</b></tt> signal to the offending process. (By default, 
          the program terminates with a message of the <tt><b>SIGFPE</b></tt> and a core dump
          is produced).
        * When you type ctrl-c, the kernel issues a <tt><b>SIGINT</b></tt> 
          signal to the foreground process (and by default, the program is
          terminated).
        * When you type ctrl-z, the kernel issues a <tt><b>SIGTSTP</b></tt>
          signal to the foreground process (and by default, the process is
          suspended until a subsequent <tt><b>SIGCONT</b></tt> signal instructs
          it to resume).
        * Whenever a child process exits (either normally or abnormally), the kernel
          potentially delivers a <tt><b>SIGCHLD</b></tt> signal to the parent process.  
          By default, the signal is ignored (and in fact, by default the kernel doesn't
	  even deliver it unless the parent process registers an interest in receiving them).
          This particular signal type is instrumental to allowing forked child processes
          to run in the background while the parent process moves on to do its own
          work without blocking on a <tt><b>waitpid</b></tt> call.  The parent process,
          however, is not relieved of its obligation to reap child process zombies, so
          the parent process will typically register code to be invoked whenever a child
	  process terminates.  Doing so prompts the kernel to begin issuing
	  <tt><b>SIGCHLD</b></tt> signals so that the registered <tt><b>SIGCHLD</b></tt> 
	  handler can reap the zombies via <tt><b>waitpid</b></tt>.

# First Signal Handler Example
* Source code can install a signal handler to <i>catch</i> and handle a certain
  type of signal in a way that's different than the default.
    * Here's a carefully coded example that illustrates how to implement and
      install a <tt><b>SIGCHLD</b></tt> handler (over two slides).  The premise
      is that dad takes his five kids out to play.  Each of the five children
      plays for a different length of time.  When all five children are
      tired of playing, the six of them (five kids and their dad) all go home.
    * Understand that the parent process is modeling dad, and that the child processes
      are modeling his children. (Code can be found right
      [here](http://cs110.stanford.edu/autumn-2017/examples/processes/five-children.c)).
      
    ~~~{.c}
    static const size_t kNumChildren = 5;
    static size_t numChildrenDonePlaying = 0;

    static void reapChild(int sig) {
      exitIf(waitpid(-1, NULL, 0) == -1, kWaitFailed,
             stderr, "waitpid failed within reapChild sighandler.\n");
      numChildrenDonePlaying++;
      sleep(1); // represents time spent doing other useful work
    }
    
    int main(int argc, char *argv[]) {
      printf("Let my five children play while I take a nap.\n");
      exitIf(signal(SIGCHLD, reapChild) == SIG_ERR, kSignalFailed,
             stderr, "Failed to install SIGCHLD handler.\n");
      for (size_t kid = 1; kid <= 5; kid++) {
        pid_t pid = fork();
        exitIf(pid == -1, kForkFailed, stderr, "Child #%zu doesn't want to play.\n", kid);
        if (pid == 0) {
          sleep(3 * kid); // sleep emulates "play" time
          printf("Child #%zu tired... returns to dad.\n", kid);
          return 0;
        }
      }
    ~~~

# First Signal Handler Example (Continued)
* Here's the second half of the program from the last slide:
    * Notice the use of the <b><tt>snooze</tt></b> function, which
      is my own implementation of an uninterruptable <b><tt>sleep</tt></b>:

    ~~~{.c}
      while (numChildrenDonePlaying < kNumChildren) {
        printf("At least one child still playing, so dad nods off.\n");
        snooze(5); // implementation in /usr/class/cs110/local/include/sleep-utils.h
        printf("Dad wakes up! ");
      }

      printf("All children accounted for.  Good job, dad!\n");
      return 0;
    }
    ~~~

    * The above code is specifically crafted so each child process
      exits about 3 seconds apart.  <b><tt>reapChild</tt></b>, of course,
      catches and handles each of the <b><tt>SIGCHLD</tt></b> signals delivered
      by the kernel as each forked child process exits.  <b><tt>reapChild</tt></b> 
      is contrived to take about a second to execute to completion, so each
      <b><tt>reapChild</tt></b> finishes about two seconds before the next child
      process exits.
    * The <b><tt>signal</tt></b> function doesn't allow for state to be
      shared with the signal handler, so we have no choice but to use globals.
    * Here's the fairly predictable output of the above program.

    ~~~{.sh}
    myth22> ./five-children 
    Let my five children play while I take a nap.
    At least one child still playing, so dad nods off.
    Child #1 tired... returns to dad.
    Child #2 tired... returns to dad.
    Dad wakes up! At least one child still playing, so dad nods off.
    Child #3 tired... returns to dad.
    Child #4 tired... returns to dad.
    Dad wakes up! At least one child still playing, so dad nods off.
    Child #5 tired... returns to dad.
    Dad wakes up! All children accounted for.  Good job, dad!
    myth22>
    ~~~

# Variation on a Theme: Pentuplets
* Consider this next program:
    * This next program is largely the same as the prior one, except that each child process
      takes <i>the same amount of time to complete</i> (program is over two
      slides)
    * The code for this example can be found right
      [here](http://cs110.stanford.edu/autumn-2017/examples/processes/indistinguishable-pentuplets.c).

    ~~~{.c}
    static const size_t kNumChildren = 5;
    static size_t numChildrenDonePlaying = 0;
    
    static void reapChild(int sig) {
      exitIf(waitpid(-1, NULL, 0) == -1, kWaitFailed,
             stderr, "waitpid failed within reapChild sighandler.\n");
      numChildrenDonePlaying++;
      sleep(1); // represents time that useful work might be done
    }
    
    int main(int argc, char *argv[]) {
      printf("Let my five children play while I take a nap.\n");
      exitIf(signal(SIGCHLD, reapChild) == SIG_ERR, kSignalFailed,
             stderr, "Failed to install SIGCHLD handler.\n");
      for (size_t kid = 1; kid <= 5; kid++) {
        pid_t pid = fork();
        exitIf(pid == -1, kForkFailed, stderr, "Child #%zu doesn't want to play.\n", kid);
        if (pid == 0) {
          sleep(3); // all kids play together for three seconds
          printf("Kid #%zu done playing... runs back to dad.\n", kid);
          return 0;
        }
      }
    ~~~

# Variation on a Theme: Pentuplets (Continued)
* Here's the second part of the program:
    * (This part is precisely the same as it was before).

    ~~~{.c}
      while (numChildrenDonePlaying < kNumChildren) {
        printf("At least one child still playing, so dad nods off.\n");
        snooze(5);
        printf("Dad wakes up! ");
      }
    
      printf("All children accounted for.  Good job, dad!\n");
      return 0;
    }
    ~~~    
    * The primary difference here is that all child processes exit,
      more or less, at the same time.  And while it is the case that the kernel
      will issue five <b><tt>SIGCHLD</tt></b> signals, not all of them prompt
      a dedicated execution of <b><tt>reapChild</tt></b>.
    * Don't believe me?  Check out the reproducible test run of this second
      version (where all five children finish playing pretty much
      much simultaneously).

    ~~~{.sh}
    myth22> ./indistinguishable-pentuplets
    Let my five children play while I take a nap.
    At least one child still playing, so dad nods off.
    Kid #1 done playing... runs back to dad.
    Kid #3 done playing... runs back to dad.
    Kid #4 done playing... runs back to dad.
    Kid #2 done playing... runs back to dad.
    Kid #5 done playing... runs back to dad.
    Dad wakes up! At least one child still playing, so dad nods off.
    Dad wakes up! At least one child still playing, so dad nods off.
    Dad wakes up! At least one child still playing, so dad nods off.
    Dad wakes up! At least one child still playing, so dad nods off.
    Dad wakes up! At least one child still playing, so dad nods off.
    Dad wakes up! At least one child still playing, so dad nods off.
    Dad wakes up! At least one child still playing, so dad nods off.
    Dad wakes up! At least one child still playing, so dad nods off.
    Dad wakes up! At least one child still playing, so dad nods off.
    Dad wakes up! At least one child still playing, so dad nods off.
    Dad wakes up! At least one child still playing, so dad nods off.
    Dad wakes up! At least one child still playing, so dad nods off.
    <ctrl-c>
    myth22>
    ~~~

# Variation on a Theme (Continued)
* Here's what happens:
    * One of the child processes finishes before the other four, and the
      kernel sends a <b><tt>SIGCHLD</tt></b> signal to the parent on its
      behalf.
    * The <b><tt>SIGCHLD</tt></b> signal is caught, and <b><tt>reapChild</tt></b>
      executes (over the course of one second) to handle it.
    * During that one second, the second of the five child processes exits, and
      the corresponding <b><tt>SIGCHLD</tt></b> signal is recorded but blocked
      until the first call to <b><tt>reapChild</tt></b> exits.  The third, fourth, and
      fifth child processes all exit while the first <b><tt>reapChild</tt></b> is still running, but their
      <b><tt>SIGCHLD</tt></b> signals are discarded, as the kernel maintains not
      a <b>count</b> of pending <b><tt>SIGCHLD</tt></b> signals, but rather a single bit
      that records whether <b>one or more</b> signals have arrived.
    * When the first call to <b><tt>reapChild</tt></b> exits, the block on pending
      <b><tt>SIGCHLD</tt></b> signals is lifted.  The kernel soon detects
      the high <b><tt>SIGCHLD</tt></b> bit, unaware of how many <b><tt>SIGCHLD</tt></b> signals 
      were actually fired.  <b><tt>reapChild</tt></b> is invoked to handle
      all the outstanding <b><tt>SIGCHLD</tt></b> signals, so that <b><tt>reapChild</tt></b> 
      executes only one more time.
    * Redux: <tt><b>numChildrenDonePlaying</b></tt> global variable is only
      incremented twice, and the parent process (the process modeling daddy)
      repeatedly takes five second naps for all of eternity.

# The Solution...
* ... is simple, provided you understand how pending signals are recorded as bools and not counts.
    * <b><tt>reapChild</tt></b> must be implemented to account for the
      possibility that many <b><tt>SIGCHLD</tt></b> signals were fired, not
      just one (e.g. that many child processes finished during the prior
      <b><tt>reapChild</tt></b> call, during the window when <b><tt>SIGCHLD</tt></b>
      signals were received but blocked).
    * Each call to <b><tt>waitpid</tt></b> should include a third argument of <b><tt>WNOHANG</tt></b>, which is a flag
      instructing waitpid to not block on children that haven't exited yet.  When
      <b><tt>WNOHANG</tt></b> is used, we want to break on two different return values:
        * 0, which means that there are other child processes, but that none of them have
          transitioned to zombie status, and
        * -1, which means what it's meant before: that there are no children (which
          is confirmed by the fact that <b><tt>errno</tt></b> is set to 
          <b><tt>ECHILD</tt></b>).

# The Solution
* The solution makes use of <b><tt>WNOHANG</tt></b>.
    * Here is the <b>correct</b> implementation of <b><tt>reapChild</tt></b> 
      (look
      [here](http://cs110.stanford.edu/spring-2017/examples/processes/pentuplets.c)
      for the code).

    ~~~{.c}
    static void reapChild(int sig) {
      pid_t pid;
      while (true) {
        pid = waitpid(-1, NULL, WNOHANG);
        if (pid <= 0) break;
        numChildrenDonePlaying++;
      }
      exitUnless(pid == 0 || errno == ECHILD, kWaitFailed,
                 stderr, "waitpid failed within reapChild sighandler.\n");
      sleep(1); // represents time that useful work might be done     
    }
    ~~~

    * Here's the output of the repaired program:

    ~~~{.sh}
    myth22> ./pentuplets
    Let my five children play while I take a nap.
    At least one child still playing, so dad nods off.
    Kid #1 done playing... runs back to dad.
    Kid #2 done playing... runs back to dad.
    Kid #4 done playing... runs back to dad.
    Kid #3 done playing... runs back to dad.
    Kid #5 done playing... runs back to dad.
    Dad wakes up! All children accounted for.  Good job, dad!
    myth22>    
    ~~~

# Introduction to Synchronization Issues
* Synchronization, multi-processing, parallelism, and concurrency
    * All central themes of the course, and all very powerful features.
    * Very difficult to understand and get right, and all kinds of concurrency
      issues and race conditions can appear unless you code very carefully.
    * Consider the following program, which is a gesture to where your Assignment 4
      shell will end up when it's all done (code for entire program can be found right
      [here](http://cs110.stanford.edu/autumn-2017/examples/processes/job-list-synchronization.c)):

    ~~~{.c}
    static void reapChild(int sig) {
      pid_t pid;
      while (true) {
        pid = waitpid(-1, NULL, WNOHANG);
        if (pid <= 0) break;
        printf("Job %d removed from job list.\n", pid);
      }
      exitUnless(pid == 0 || errno == ECHILD, kWaitFailed,
                 stderr, "waitpid failed within reapChild sighandler.\n");
    }
    
    int main(int argc, char *argv[]) {
      exitIf(signal(SIGCHLD, reapChild) == SIG_ERR, kSignalFailed,
             stderr, "signal function failed.\n");
      for (size_t i = 0; i < 3; i++) {
        pid_t pid = fork();
        exitIf(pid == -1, kForkFailed,
               stderr, "fork function failed.\n");
        if (pid == 0) {
          char *listArguments[] = {"date", NULL};
          exitIf(execvp(listArguments[0], listArguments) == -1,
                 kExecFailed, stderr, "execvp function failed.\n");
        }
        snooze(1); // represents meaningful time spent
        printf("Job %d added to job list.\n", pid);
      }
    
      return 0;
    }
    
    ~~~

# Baby's First Concurrency Issue!
* Look at a test run of the previous program:
    * How wrong is this?!!

    ~~~{.sh}
    myth22> ./job-list-synchronization
    Wed Oct 11 9:45:41 PDT 2017
    Job 26874 removed from job list.
    Job 26874 added to job list.
    Wed Oct 11 9:45:42 PDT 2017
    Job 26875 removed from job list.
    Job 26875 added to job list.
    Wed Oct 11 9:45:43 PDT 2017
    Job 26876 removed from job list.
    Job 26876 added to job list.
    myth22>
    ~~~

    * The huge issue here is that each of the child processes (which quickly
      publish the date to the console via a <tt><b>fork</b></tt>/<tt><b>execvp</b></tt> pair)
      exits and prompts the kernel to fire a
      <tt><b>SIGCHLD</b></tt> signal at the parent before the parent makes it through
      its one second nap.  The <tt><b>reapChild</b></tt> function executes in full
      (and "removes" the job from some job list) before the parent advances to the
      point where it "adds" the job to the same job list.
    * That's messed up!  Welcome to the land of concurrent contexts operating on
      shared data structures.
    * The solution:
        * Informally, we need to programmatically block
          <tt><b>SIGCHLD</b></tt> signals from being handled until the parent manages to
          add the process to the job list.
        * Formally, we need to use <tt><b>sigset_t</b></tt> masks to temporarily block
	  <tt><b>SIGCHLD</b></tt> signals from being handled until the parent
          has gotten through the code that adds the job to the job list.

# Baby's First Concurrency Solution
* The concurrency issue is unacceptable.
    * We must take steps to programmatically suspend the handling of
      <tt><b>SIGCHLD</b></tt> siganls until the parent is prepared to handle them.
    * Enter the <tt><b>sigset_t</b></tt> type and the collection of functions that can
      be used to temporarily block the receipt of one or more signal types.
    * Here's the working <b><tt>main</tt></b> function (code for entire program can be found right
      [here](http://cs110.stanford.edu/autumn-2017/examples/processes/job-list-synchronization-improved.c)):

    ~~~{.c}
    int main(int argc, char *argv[]) {
      exitIf(signal(SIGCHLD, reapChild) == SIG_ERR, kSignalFailed,
             stderr, "signal function failed.\n");
      sigset_t mask;
      sigemptyset(&mask);
      sigaddset(&mask, SIGCHLD);
      for (size_t i = 0; i < 3; i++) {
        sigprocmask(SIG_BLOCK, &mask, NULL);
        pid_t pid = fork();
        exitIf(pid == -1, kForkFailed,
               stderr, "fork function failed.\n");
        if (pid == 0) {
          sigprocmask(SIG_UNBLOCK, &mask, NULL);
          char *listArguments[] = {"date", NULL};
          exitIf(execvp(listArguments[0], listArguments) == -1,
                 kExecFailed, stderr, "execvp function failed.\n");
        }
    
        snooze(1);
        printf("Job %d added to job list.\n", pid);
        sigprocmask(SIG_UNBLOCK, &mask, NULL); // begin handling SIGCHLD signals again
      }    
      return 0;
    }
    ~~~

    * The <tt><b>sigset_t</b></tt> figure manages the set of signal types to be blocked and
      unblocked via calls to <tt><b>sigprocmask</b></tt>.
        * Note the call to <tt><b>sigprocmask(SIG_BLOCK, &mask, NULL);</b></tt>, which informs the kernel
	  that the calling process isn't interested in handling any <tt><b>SIGCHLD</b></tt> events
          until after the spawned child process is added to the job list.  Note, also, the
          call to <tt><b>sigprocmask(SIG_UNBLOCK, &mask, NULL);</b></tt> appears after the child
          process pid has been added to the job list (gestured via <tt><b>printf</b></tt>).
        * As it turns out, the forked process inherits the blocked signals vector, so it too needs
          to lift the block via its own call to <tt><b>sigprocmask(SIG_UNBLOCK, &mask, NULL);</b></tt>.
          In this example, it doesn't matter, but it would matter if the forked child process itself
          forked off child processes of its own.

    ~~~{.sh}
    myth22> ./job-list-synchronization
    Wed Oct 11 9:49:23 PDT 2017
    Job 29619 added to job list.
    Job 29619 removed from job list.
    Wed Oct 11 9:49:24 PDT 2017
    Job 29620 added to job list.
    Job 29620 removed from job list.
    Wed Oct 11 9:49:25 PDT 2017
    Job 29621 added to job list.
    Job 29621 removed from job list.
    myth22>
    ~~~

# Interprocess communication
* Processes can message other processes using signals via the <b><tt>kill</tt></b> system call.
    * Prototype:

    ~~~{.c}
    int kill(pid_t pid, int signum);
    ~~~

    * Analogous to the <b><tt>/bin/kill</tt></b> shell command.
        * Unfortunately named, since kill implies <b><tt>SIGKILL</tt></b>, which implies death.
        * So named because the default action of most signals in early UNIX implementations
          was to just terminate the target process (identified by <b><tt>pid</tt></b>).
    * Returns -1 if the call fails (generally because the calling process doesn't have permission 
      to fire signals at target process), 0 if all is swell.
    * <b><tt>pid</tt></b> parameter is overloaded to provide many signalling strategies:
        * When <b><tt>pid</tt></b> is a positive number, the target is a process with that <b><tt>pid</tt></b>.
        * When <b><tt>pid</tt></b> is a negative number less than -1, the targets are all processes 
          within the process group <b><tt>abs(pid)</tt></b>.
        * <b><tt>pid</tt></b> can also be 0 or -1, but we don't need to worry about those (though it's
          documented in the B&O textbook if you're curious).

# The Kill Puzzle
* Here's a short puzzle to ensure you understand key ideas:
    * We'll first work through a small puzzle to confirm you all understand the workflow
      of the processes and understand how <b><tt>kill</tt></b> triggers various signal handlers to be
      executed. (Error checking is omitted from this example, since it's so small, and we'll
      assume, for simplicity, that nothing ever fails).
    * Putting it all together.  What're the possible outputs (plural!) of the
      [following program](http://cs110.stanford.edu/spring-2017/examples/processes/kill-puzzle.c)?

    ~~~{.c}
    static pid_t pid;
    static int counter = 0;

    static void parentHandler(int unused) {       static void childHandler(int unused) {
      counter++;                                    counter += 3;
      printf("counter = %d\n", counter);            printf("counter = %d\n", counter);
      kill(pid, SIGUSR1);                         }
    }



    int main(int argc, char *argv[]) {
      signal(SIGUSR1, parentHandler);
      if ((pid = fork()) == 0) {
        signal(SIGUSR1, childHandler);
        kill(getppid(), SIGUSR1);
        return 0;
      }

      waitpid(pid, NULL, 0);
      counter += 7;
      printf("counter = %d\n", counter);
      return 0;
    }
    ~~~

# The Kill Puzzle (continued)
* What's the expected output?
    * Make sure you understand why the program on the previous slide is capable of producing two
      different outputs, depending on processor scheduling.  (Hint: the child process may or may
      not exit before <tt><b>parentHandler</b></tt> has a chance to kill it!)

	~~~{.sh}
    myth22> ./kill-puzzle
    counter = 1
    counter = 8
    myth22> ./kill-puzzle
    counter = 1
    counter = 3
    counter = 8
    myth22>
	~~~


# Announcements
* Only a few announcements:
    * Assignment 1 is due tonight.
    * Assignment 1 will be graded by Sunday afternoon, grade reports emailed out soon thereafter.
    * Assignment 2 is slightly more intense than Assignment 1, so start earlier than later.
    * Discussion sections begin this week.  This week's discussion section handout is
      already posted, and section solutions will be posted over the course of the weekend.

* Topics for today:
    * Continue our discussion of <tt><b>fork</b></tt>, how to use it, and how it works.
        * Refer to this past Monday's slides for two of today's examples, 
          and hopefully everything goes swimmingly well with those and we can advance on to
          those posted today.
        * With any luck, you'll understand how a standard UNIX shell
          works by middle of next week.  Great stuff, this <tt><b>fork</b></tt>.
        * Sync up against <tt><b>/usr/class/cs110/lecture-examples/autumn-2017</b></tt> 
          to get the examples from this past Monday's and today's lectures.

# Spawning multiple processes
* Reaping multiple child processes:
    * Of course, the parent is allowed to call <tt><b>fork</b></tt> multiple
      times, provided it eventually reaps the child processes after they exit.
    * If we want to reap child processes as they exit without concern for the
      ordering in which they fork, then this does the trick:
    * Check this out (code is in lecture examples folder <tt><b>processes/reap-as-they-exit.c</b></tt>
      and can also be viewed right [here](http://cs110.stanford.edu/autumn-2017/examples/processes/reap-as-they-exit.c))

    ~~~{.c}
    int main(int argc, char *argv[]) {
      for (size_t i = 0; i < kNumChildren; i++) {
        pid_t pid = fork();
        exitIf(pid == -1, kForkFail, stderr, "Fork function failed.\n");
        if (pid == 0) exit(110 + i);
      }

      while (true) {
        int status;
        pid_t pid = waitpid(-1, &status, 0);
        if (pid == -1) break;
        if (WIFEXITED(status)) {
          printf("Child %d exited: status %d\n", pid, WEXITSTATUS(status));
        } else {
          printf("Child %d exited abnormally.\n", pid);
        }
      }

      exitUnless(errno == ECHILD, kWaitFail, stderr, "waitpid failed.\n");
      return 0;
    }
    ~~~
    * Note we feed a -1 as the first argument to <tt><b>waitpid</b></tt>.  That -1
      states we want to hear about **any** child as it exits.
    * Eventually, all children exit (normally or not) and waitpid **correctly**
      returns -1 to signal that all child processes have ended.  
      When <tt><b>waitpid</b></tt> returns -1, it sets a global variable called 
      <tt><b>errno</b></tt> to the constant <tt><b>ECHILD</b></tt> as a signal
      that -1 was returned because all child processes terminated.  Interestingly
      enough, that's the "error" we want.

# Spawning multiple processes, take II
* We can do the same thing, but reap in the order they are forked.
    * Look at this lovely program of octuplets (code is in lecture examples folder
      <tt><b>processes/reap-in-fork-order.c</b></tt> and can also be
      viewed right [here](http://cs110.stanford.edu/autumn-2017/examples/processes/reap-in-fork-order.c))

    ~~~{.c}
    int main(int argc, char *argv[]) {
      pid_t children[kNumChildren];
      for (size_t i = 0; i < kNumChildren; i++) {
        children[i] = fork();
        exitIf(children[i] == -1, kForkFail, stderr, "Fork function failed.\n");
        if (children[i] == 0) exit(110 + i);
      }

      for (size_t i = 0; i < kNumChildren; i++) {
        int status;
        exitUnless(waitpid(children[i], &status, 0) == children[i],
                   kWaitFail, stderr, "Intentional wait on child %d failed.\n", children[i]);
        exitUnless(WIFEXITED(status) && WEXITSTATUS(status) == 110 + i,
                   kExitFail, stderr, "Correct child %d exited abnormally.\n");
      }
    
      return 0;
    }
    ~~~

     * This version spawns and reaps child processes in some 
       first-spawned-first-reaped (let's invent an acronym: FSFR) manner.
     * Understand, of course, that the child processes aren't required to exit or otherwise
       terminate in FSFR order.  In theory, the first child thread could finish last, and
       the reap loop could be held up on its very first iteration until the first child
       actually finishes.  But the process zombies (as they're called) are certainly
       reaped in the order they were forked.

# Enter the <tt><b>execvp</b></tt> command
* <tt><b>execvp</b></tt> effectively reboots a process to
    run a different program from scratch.
    * <tt><b>execvp</b></tt> has many variants (<tt><b>execle</b></tt>,
      <tt><b>execlp</b></tt>, and so forth.  Type <tt><b>man</b></tt> <tt><b>execvp</b></tt>
      to see all of them).
    * Here is the prototype:

    ~~~{.c}
    int execvp(const char *path, char *argv[]);
    ~~~

    * Here's what the arguments and the return type mean:
        * <tt><b>path</b></tt> identifies the name of the executable that should
          be invoked.
        * <tt><b>argv</b></tt> is the argument vector that should be funneled
          through to the new executable's <tt><b>main</b></tt> function.  
        * Generally, at least for the purposes of CS110, <tt><b>path</b></tt> and
          <tt><b>argv[0]</b></tt> end up being the same exact string.
        * If <tt><b>execvp</b></tt> fails to consume the process and install
          a new process image within it, <tt><b>execvp</b></tt> will return -1.   

# Using <tt><b>execvp</b></tt>
* Core implementation of <tt><b>mysystem</b></tt> (to emulate the <tt><b>system</b></tt> builtin)
    * Here we present our own implementation of the <tt><b>mysystem</b></tt>
      function, which executes the provided <tt><b>command</b></tt> (guaranteed 
      to be a <tt><b>'\\0'</b></tt>-terminated C string) by calling 
      <tt><b>&quot;/bin/sh -c command&quot;</b></tt> and ultimately
      returning once the surrogate <tt><b>command</b></tt> has finished.  
    * If the execution of <tt><b>command</b></tt> exits normally (either via an <tt><b>exit</b></tt>
      system call, or via a normal return statement from <tt><b>main</b></tt>),
      then our <tt><b>mysystem</b></tt> implementation should return that exact 
      same exit status.  
    * If the execution exits abnormally (e.g. it segfaults), then we'll assume
      it aborted because some signal was ignored, and we'll return that negative
      of that signal number (e.g. -11 for <tt><b>SIGSEGV</b></tt>).
    * Here's the implementation (online right [here](http://cs110.stanford.edu/autumn-2017/examples/processes/mysystem.c))

    ~~~{.c}
    static int mysystem(const char *command) {
      pid_t pid = fork();
      if (pid == 0) {
        char *arguments[] = {"/bin/sh", "-c", (char *) command, NULL};
        execvp("/bin/sh", arguments);
        exitIf(true, kExecFailed, stderr, "execvp failed to invoke this: %s.\n", command);        
      }

      int status;
      waitpid(pid, &status, 0);
      if (WIFEXITED(status))
        return WEXITSTATUS(status);
      else
        return -WTERMSIG(status);
    }
    ~~~

    * Here's a trivial unit test that I'll run in lecture to prove this thing really works:

    ~~~{.c}
    static const size_t kMaxLine = 2048;
    int main(int argc, char *argv[]) {
      char buf[kMaxLine];
      while (true) {
        printf("> ");
        fgets(buf, kMaxLine, stdin);
        if (feof(stdin)) break;
        buf[strlen(buf) - 1] = '\0'; // overwrite '\n'
        printf("retcode = %d\n", mysystem(buf));
      }

      printf("\n");
      return 0;
    }
    ~~~


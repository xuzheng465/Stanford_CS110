# Announcements
* Assignment 1 grade reports were emailed out this morning.
    * Median grade was a perfect 56 out of 56.
    * Average style grade was halfway between a 'solid' and a 'minor-problems'.
    * Functionality counts five times as much as style in CS110.
        * I care about software engineering and code clarity, but..
        * I care about working code more.
        * Style grades translate as follows:
            * <tt><b>exceptional</b></tt>    -> 105%
            * <tt><b>solid</b></tt>          -> 95%
            * <tt><b>minor-problems</b></tt> -> 80%
            * <tt><b>major-problems</b></tt> -> 60%
* Assignment 2 falls due this Wednesday at 11:59 p.m.
* Assignment 3 goes out on Wednesday, not due for 11 days.
* Readings:
    * Finish reading B&amp;O Chapter 2 (Chapter 10 of full textbook) so you can confirm that you know most of 
      the material there, as I covered much of it during the first two weeks of lecture.
    * Finish reading B&amp;O Chapter 1 (Chapter 8 of the full textbook), focusing on Section 5, which covers
      process groups, signals, and signal handlers, all three of which are relevant
      to your next few assignments.

# Today
* Today's topics are all kinds of awesome.
    * We have a few lecture examples from last Wednesday
      to work through.  In particular, we want to implement a collection of miniature shells
      to illustrate how <tt><b>fork</b></tt>, <tt><b>waitpid</b></tt>, and <tt><b>execvp</b></tt> work.
    * I want to introduce the notion of a pipe, the <tt><b>pipe</b></tt> and <tt><b>dup2</b></tt>
      functions, and how they can be used to foster more sophisticated communication between
      the different processes.
    * Later today or first thing Wednesday, we'll begin our discussion of signals and signal handlers.

# Core implementation of <tt><b>simplesh</b></tt>, version 1.0
* This is the best example of <tt><b>execvp</b></tt> imaginable: a miniature
  shell not unlike those you've been using since the day you learned UNIX.
    * Relies on <tt><b>fork</b></tt>, <tt><b>waitpid</b></tt>, 
      and <tt><b>execvp</b></tt>.
    * This first version operates as a read-eval-print loop (keyword: repl),
      responding to many things we type in by forking off child processes.
        * Each child process is initially a deep clone of the shell.
        * Each child proceeds to replace its own process image with the new
          one we specify (e.g. <tt><b>ls</b></tt>, <tt><b>cp</b></tt>,
          our own CS110 <tt><b>search</b></tt> (which we wrote the second
          day of class), or even <tt><b>emacs</b></tt>.
        * A trailing ampersand (&amp;) (e.g. <tt><b>emacs</b></tt> <tt><b>&</b></tt>)
          is an instruction to execute in the background, without blocking access to the
          terminal.
    * Implementation of <tt><b>simplesh</b></tt> is presented over the next
      three slides.  Where helper functions don't rely on CS110 concepts, I omit
      their implementations (but will describe them in lecture).

# Core of <tt><b>simplesh</b></tt> implementation
* Implementation of <tt><b>main</b></tt>:
    * Here's the first half (full implementation right [here](http://cs110.stanford.edu/autumn-2017/examples/processes/simplesh.c))

    ~~~{.c}
    int main(int argc, char *argv[]) {
      while (true) {
        char command[kMaxCommandLength + 1];
        readCommand(command, sizeof(command) - 1);
        if (feof(stdin)) break;
        char *arguments[kMaxArgumentCount + 1];
        int count = parseCommandLine(command, arguments, 
                                     sizeof(arguments)/sizeof(arguments[0]));
        if (count == 0) continue;
        bool builtin = handleBuiltin(arguments);
        if (builtin) continue; // it's been handled, move on
        bool isBackgroundProcess = strcmp(arguments[count - 1], "&") == 0;
        if (isBackgroundProcess) arguments[--count] = NULL; // overwrite "&"
        pid_t pid = forkProcess();
    ~~~

# Core of <tt><b>simplesh</b></tt> implementation (continued)
* Implementation of <tt><b>main</b></tt>:
    * Here's the second half

    ~~~{.c}
        if (pid == 0) {
          if (execvp(arguments[0], arguments) < 0) {
            printf("%s: Command not found\n", arguments[0]);
            exit(0);
          }
        }

        if (!isBackgroundProcess) {
          waitForChildProcess(pid);
        } else {
          printf("%d %s\n", pid, command);
        }
      }

      printf("\n");
      return 0;
    }
    ~~~

# Core of <tt><b>simplesh</b></tt> implementation (continued)
* Helper routines
    * Here are a few helper routines that do rely on CS110 material:

    ~~~{.c}
    static bool handleBuiltin(char *arguments[]) {
      if (strcasecmp(arguments[0], "quit") == 0) exit(0);
      return strcmp(arguments[0], "&") == 0;
    }

    static pid_t forkProcess() {
      pid_t pid = fork();
      exitIf(pid == -1, kForkFailed, stderr, "fork function failed.\n");
      return pid;
    }

    static void waitForChildProcess(pid_t pid) {
      exitUnless(waitpid(pid, NULL, 0) == pid, kWaitFailed,
                 stderr, "Error waiting in foreground for process %d to exit", pid);
    }
    ~~~

# Implementing <tt><b>subprocess</b></tt> 
* Introducing <tt><b>pipe</b></tt>:
    * The <tt><b>pipe</b></tt> system call takes an uninitialized
      array of two integers (let’s call this array <tt><b>fds</b></tt>) and
      populates it with two file descriptors such that everything <i>written</i>
      to <tt><b>fds[1]</b></tt> can be <i>read</i> from <tt><b>fds[0]</b></tt>.

    ~~~{.c}
    int pipe(int fds[]); // fds array should be of length 2, return -1 on error, 0 otherwise
    ~~~

    * <tt><b>pipe</b></tt> is particularly useful for allowing parent processes to
      communicate with forked child processes.  (Recall that <tt><b>fork</b></tt> 
      clones the caller’s virtual address space <b>and</b> duplicates all open file descriptors as well).
    * Using <tt><b>pipe</b></tt>, <tt><b>fork</b></tt>, <tt><b>dup2</b></tt>,
      <tt><b>execvp</b></tt>, <tt><b>close</b></tt>, and <tt><b>waitpid</b></tt>,
      we can implement the <tt><b>subprocess</b></tt> function, which relies on the 
      following record definition and is implemented to the following prototype:
      
    ~~~{.c}
    typedef struct {
      pid_t pid;
      int infd;
    } subprocess_t;
    subprocess_t subprocess(const char *command);
    ~~~

    * The subprocess created by <tt><b>subprocess</tt></b> executes
      the provided command (guaranteed to be a <tt><b>'&#92;0'</b></tt>-terminated C string) by calling
      <tt><b>"/bin/sh -c command"</b></tt>.  Rather than waiting for <tt><b>command</b></tt> to finish,
      the implementation returns a <tt><b>subprocess_t</b></tt> with the <b><tt>command</tt></b>
      process’s pid and a single file descriptor.  In particular, arbitrary data can
      be published to the return value’s <tt><b>infd</b></tt> with the understanding
      that it’ll be read by <b><tt>command</tt></b>'s standard input.

# Implementing <tt><b>subprocess</b></tt> (continued)
* Sample client application, with output
    * The following client program and test run illustrate precisely how 
      <tt><b>subprocess</b></tt> should work:
      
    ~~~{.c}
    int main(int argc, char *argv[]) {
      subprocess_t sp = subprocess("/usr/bin/sort");
      const char *words[] = {
        "felicity", "umbrage", "susurration", "halcyon",
        "pulchritude", "ablution", "somnolent", "indefatigable"
      };      
      for (size_t i = 0; i < sizeof(words)/sizeof(words[0]); i++) { 
        dprintf(sp.infd, "%s\n", words[i]);
      }
      close(sp.infd); // effectively sends cntl-D to child process
      int status;
      pid_t pid = waitpid(sp.pid, &status, 0);
      return pid == sp.pid && WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }
    ~~~
    
    * The output of the above program, given a properly implemented <tt><b>subprocess</b></tt>
      routine, should look like so:

    ~~~{.sh}
    myth22> ./subprocess-test
    ablution
    felicity
    halcyon
    indefatigable
    pulchritude
    somnolent
    susurration
    umbrage
    ~~~

# Implementing <tt><b>subprocess</b></tt> (continued)
* Implementation is right here (warning: dense):
    * Here is the implementation of <tt><b>subprocess</b></tt> (where I omit error
      checking so that the meat of the implementation is clear):

    ~~~{.c}
    subprocess_t subprocess(const char *command) {
      int fds[2];
      pipe(fds);
      subprocess_t process = { fork(), fds[1] };
      if (process.pid == 0) {
        close(fds[1]);
        dup2(fds[0], STDIN_FILENO);
        close(fds[0]);
        char *argv[] = {"/bin/sh", "-c", (char *) command, NULL};
        execvp(argv[0], argv);
      }
      close(fds[0]);
      return process;
    }
    ~~~

    * Note that the write end of the pipe is embedded into the
      <tt><b>subprocess_t</b></tt>.  That way, the parent knows where
      to publish text so that it flows to the other end of the pipe,
      across the parent process/child process boundary.
    * Further note that the child process reassociates the read end of the
      pipe with its own standard input (using <tt><b>dup2</b></tt>).
    * Once the <b><tt>dup2</tt></b> reassociation has been made, the
      child process can close both ends of its copy of the pipe.
    * The parent doesn't need the read end of the pipe, so
      it too can close its own copy of <b><tt>fds[0]</tt></b>.
    * Full implementation is right [here](http://cs110.stanford.edu/autumn-2017/examples/processes/subprocess.c).


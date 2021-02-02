#include <stdlib.h>     // exit
#include <stdio.h>      // for printf
#include <stdbool.h>    // for bool, true, false
#include <string.h>     // for strchr, strcmp
#include <unistd.h>     // for fork, execve
#include <sys/wait.h>   // for waitpid
#include <errno.h>      // for errno
#include <signal.h>     // for signal
#include "exit-utils.h" // for exitIf, exitUnless
#include "simplesh-utils.h"

static pid_t fgpid = 0; // 0 means no foreground process
static void reapProcesses(int sig) {
  pid_t pid;
  while (true) {
    pid = waitpid(-1, NULL, WNOHANG);
    if (pid <= 0) break;
    if (pid == fgpid) fgpid = 0;
  }

  exitUnless(pid == 0 || errno == ECHILD, kWaitFailed, stderr, "waitpid function failed");
}

static void waitForForegroundProcess(pid_t pid) {
  fgpid = pid;
  unblockSIGCHLD();

  blockSIGCHLD();
  while (fgpid == pid) {
    unblockSIGCHLD();
    pause();
    blockSIGCHLD();
  }
  unblockSIGCHLD();
}

int main(int argc, char *argv[]) {
  signal(SIGCHLD, reapProcesses);
  while (true) {
    char command[kMaxCommandLength + 1];
    readCommand(command, sizeof(command) - 1);
    if (feof(stdin)) break;
    char *argv[kMaxArgumentCount + 1];
    int count = parseCommandLine(command, argv, sizeof(argv)/sizeof(argv[0]));
    if (count == 0) continue;
    bool builtin = handleBuiltin(argv);
    if (builtin) continue; // it's been handled, and backgrounding a builtin isn't an option
    bool isbg = strcmp(argv[count - 1], "&") == 0;
    if (isbg) argv[--count] = NULL; // overwrite "&"
    blockSIGCHLD();
    pid_t pid = forkProcess();
    if (pid == 0) {
      unblockSIGCHLD();
      execvp(argv[0], argv);
      printf("%s: Command not found\n", argv[0]);
      exit(0);
    }
    
    if (isbg) {
      printf("%d %s\n", pid, command);
      unblockSIGCHLD();
    } else {
      waitForForegroundProcess(pid);
    }
  }
  
  printf("\n");
  return 0;
}

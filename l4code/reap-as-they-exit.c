/**
 * File: reap-as-they-exit.c
 * -------------------------
 * The following program spawns off 8 children,
 * each of which returns a different exit status.
 */

#include <unistd.h>     // for fork
#include <stdio.h>      // for printf
#include <sys/wait.h>   // for waitpid
#include <errno.h>      // for errno, ECHILD
#include "exit-utils.h" 

static const int kNumChildren = 8;
static const int kForkFail = 1;
static const int kWaitFail = 2;

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

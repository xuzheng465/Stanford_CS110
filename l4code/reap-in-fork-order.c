/**
 * File: reap-in-fork-order.c
 * --------------------------
 * The following program spawns off 8 children,
 * each of which returns a different exit status.
 * The difference here is that we wait on them
 * in the order in which they are forked.
 */

#include <unistd.h>     // for fork
#include <stdio.h>      // for printf
#include <sys/wait.h>   // for waitpid
#include <errno.h>      // for errno, ECHILD
#include "exit-utils.h" 

static const int kNumChildren = 8;
static const int kForkFail = 1;
static const int kWaitFail = 2;
static const int kExitFail = 4;

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
    exitUnless(WIFEXITED(status) && (WEXITSTATUS(status) == (110 + i)),
	       kExitFail, stderr, "Correct child %d exited abnormally.\n");
    printf("Child with pid %d accounted for (return status of %d).\n", children[i], WEXITSTATUS(status));
  }

  return 0;
}

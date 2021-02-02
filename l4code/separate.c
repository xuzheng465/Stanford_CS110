/**
 * File: separate.c
 * ----------------
 * Here's is a program that's written in a style more conistent with
 * how fork is normally used.
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include "exit-utils.h"

static const int kForkFailed = 1;
static const int kWaitFailed = 2;

int main(int argc, char *argv[]) {
  printf("Before.\n");
  pid_t pid = fork();
  exitIf(pid == -1, kForkFailed, stderr, "Fork function failed.\n");
  printf("After.\n");
  if (pid == 0) {
    printf("I'm the child, and the parent will wait up for me.\n");
    return 110; // contrived exit status (not a bad number, though)
  } else {
    int status;
    exitUnless(waitpid(pid, &status, 0) == pid, kWaitFailed,
	       stderr, "Parent's wait for child process with pid %d failed.\n", pid);
    if (WIFEXITED(status)) {
      printf("Child exited with status %d.\n", WEXITSTATUS(status));
    } else {
      printf("Child terminated abnormally.\n");
    }
    return 0;
  }
}

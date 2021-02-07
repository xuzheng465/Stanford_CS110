/**
 * File: basic-fork.c
 * ------------------
 * Novelty program to illustrate the basics of fork.  It has the clear flaw
 * that the parent can finish before its child, and the child process isn't
 * reaped by the parent.
 */

#include <stdbool.h>      // for bool
#include <stdio.h>        // for printf
#include <unistd.h>       // for fork, getpid, getppid
#include "exit-utils.h"   // for our own exitIf

static const int kForkFailed = 1;
int main(int argc, char *argv[]) {
  printf("Greetings from process %d! (parent %d)\n", getpid(), getppid());
  pid_t pid = fork();
  exitIf(pid == -1, kForkFailed, stderr, "fork function failed.\n");
  printf("Bye-bye from process %d! (parent %d)\n", getpid(), getppid());
  return 0;
}

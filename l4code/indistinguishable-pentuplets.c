/**
 * File: indistinguishable-pentuplets.c
 * ------------------------------------
 * This program demonstrates the limitations of what happens
 * when multiple child processes exit simultaneously.
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include "exit-utils.h"  // exitIf, exitUnless
#include "sleep-utils.h" // for snooze

static const int kSignalFailed = 1;
static const int kForkFailed = 2;
static const int kWaitFailed = 3;

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
  
  while (numChildrenDonePlaying < kNumChildren) {
    printf("At least one child still playing, so dad nods off.\n");
    snooze(5);
    printf("Dad wakes up! ");
  }
  
  printf("All children accounted for.  Good job, dad!\n");
  return 0;
}

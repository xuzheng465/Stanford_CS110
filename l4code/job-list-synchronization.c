/**
 * File: job-list-syncrhonization.c
 * --------------------------------
 * Presents a simple emulation of a shell with
 * gestures to a job list, which you'll need to
 * support for Assignment 2.
 */

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include "exit-utils.h"
#include "sleep-utils.h"

static const int kForkFailed = 1;
static const int kExecFailed = 2;
static const int kWaitFailed = 4;
static const int kSignalFailed = 8;

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
    snooze(1);
    printf("Job %d added to job list.\n", pid);
  }
  
  return 0;
}

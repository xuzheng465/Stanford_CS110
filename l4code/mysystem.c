/**
 * File: mysystem.c
 * ----------------
 * Implements our own system function (which I call so that it doesn't compete with
 * the built-in system function).
 */

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include "string.h"
#include "exit-utils.h"

static const int kExecFailed = 1;
static int mysystem(const char *command) {
  pid_t pid = fork();
  if (pid == 0) {
    char *arguments[] = {"/bin/sh", "-c", (char *) command, NULL};
    execvp(arguments[0], arguments);
    exitIf(true, kExecFailed, stderr, "execvp failed to invoke this: %s.\n", command);
  }

  int status;
  waitpid(pid, &status, 0);
  if (WIFEXITED(status))
    return WEXITSTATUS(status);
  else
    return -WTERMSIG(status);
}

static const size_t kMaxLine = 2048;
int main(int argc, char *argv[]) {
  char command[kMaxLine];
  while (true) {
    printf("> ");
    fgets(command, kMaxLine, stdin);
    if (feof(stdin)) break; 
    command[strlen(command) - 1] = '\0'; // overwrite '\n'
    printf("retcode = %d\n", mysystem(command));
  }
  
  printf("\n");
  return 0;
}

/**
 * File: simplesh-utils.c
 * ----------------------
 * Presents the implementation of the the various services
 * exported by simplesh-utils.h
 */

#include "simplesh-utils.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "exit-utils.h"

static void pullRest() {
  while (getchar() != '\n');
}

void readCommand(char command[], size_t len) {
  char control[64] = {'\0'};
  command[0] = '\0';
  sprintf(control, "%%%zu[^\n]%%c", len);
  while (true) {
    printf("%s ", kCommandPrompt);
    char termch;
    if (scanf(control, command, &termch) < 2) { pullRest(); return; }
    if (termch == '\n') return;
    fprintf(stderr, "Command shouldn't exceed %hu characters.  Ignoring.\n", kMaxCommandLength);
    pullRest();
  }
}

static char *skipSpaces(const char *str) {
  while (*str == ' ') str++;
  return (char *) str;
}

int parseCommandLine(char *command, char *argv[], int len) {
  command = skipSpaces(command);
  int count = 0;
  while (count < len - 1 && *command != '\0') {
    argv[count++] = command;
    char *found = strchr(command, ' ');
    if (found == NULL) break;
    *found = '\0';
    command = found + 1;
    command = skipSpaces(command);
  }
  argv[count] = NULL;
  return count;
}

// left in for debugging purposes
/* static */ void printCommandLineArguments(char *argv[]) {
  int count = 0;
  while (*argv != NULL) {
    count++;
    printf("%2d: \"%s\"\n", count, *argv);
    argv++;
  }
}

bool handleBuiltin(char *argv[]) {
  if (strcasecmp(argv[0], "quit") == 0) exit(0);
  return strcmp(argv[0], "&") == 0;
}

pid_t forkProcess() {
  pid_t pid = fork();
  exitIf(pid == -1, kForkFailed, stderr, "fork function failed.\n");
  return pid;
}

static void toggleSIGCHLDBlock(int how) {
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGCHLD);
  sigprocmask(how, &mask, NULL);
}

void blockSIGCHLD() {
  toggleSIGCHLDBlock(SIG_BLOCK);
}

void unblockSIGCHLD() {
  toggleSIGCHLDBlock(SIG_UNBLOCK);
}

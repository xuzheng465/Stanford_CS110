/**
 * File: fork-puzzle.c
 * -------------------
 * Small example illustrating fork and the potential for
 * many possible outcomes that result from the concurrency
 * and the various ways all of the children, grandchildren,
 * etc. can be scheduled.
 */

#include <unistd.h>     // for fork
#include <stdio.h>      // for printf, etc
#include <string.h>     // for strlen
#include "exit-utils.h" // for exitIf, exitUnless

static const char const *kTrail = "abcd";
static const int kForkFail = 1;

int main(int argc, char *argv[]) {
  printf("Let the forking begin.\n");
  size_t trailLength = strlen(kTrail);
  for (size_t i = 0; i < trailLength; i++) {
    printf("%c\n", kTrail[i]);
    pid_t pid = fork();
    exitIf(pid == -1, kForkFail, stderr, "Call to fork failed.");
  }
  return 0;
}

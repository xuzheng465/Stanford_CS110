#include <stdlib.h>     // exit
#include <stdio.h>      // for printf
#include <stdbool.h>    // for bool, true, false
#include <string.h>     // for strchr, strcmp
#include <unistd.h>     // for fork, execve
#include <sys/wait.h>   // for waitpid
#include "exit-utils.h" // for exitIf, exitUnless, etc
#include "simplesh-utils.h"

int main(int argc, char *argv[]) {
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
    pid_t pid = forkProcess();
    if (pid == 0) {
      execvp(argv[0], argv);
      printf("%s: Command not found\n", argv[0]);
      exit(0);
    }
    
    if (isbg) {
      printf("%d %s\n", pid, command);
    } else {
      waitpid(pid, NULL, 0); // block inline
    }
  }

  printf("\n");
  return 0;
}

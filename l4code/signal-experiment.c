#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  *(int *)NULL = 0;
  printf("I never get here.\n");
  return 0;
}

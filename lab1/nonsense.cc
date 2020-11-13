/*
 * File: nonsense.cc
 * -----------------
 * Opens a collection of file descriptors and intentionally leaves
 * them open so that we can see how valgrind reports them.
 */

#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int fd1 = open("nonsense.cc", O_RDONLY);
  int fd2 = dup(fd1);
  close(fd1);
  int fd3 = dup(fd2);
  int fd4 = dup(fd3);
  dup(fd4);
  dup2(STDERR_FILENO, fd4);
  dup2(STDERR_FILENO, STDOUT_FILENO);
  close(STDIN_FILENO);
  open("nonsense.cc", O_RDONLY);
  close(STDERR_FILENO);
  return 0;
}

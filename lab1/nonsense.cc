/*
 * File: nonsense.cc
 * -----------------
 * Opens a collection of file descriptors and intentionally leaves
 * them open so that we can see how valgrind reports them.
 */

#include <fcntl.h>
#include <unistd.h>

/**
* Error version
*/
// int main(int argc, char *argv[]) {
//   int fd1 = open("nonsense.cc", O_RDONLY);
//   int fd2 = dup(fd1);
//   close(fd1);
//   int fd3 = dup(fd2);
//   int fd4 = dup(fd3);
//   dup(fd4);
//   dup2(STDERR_FILENO, fd4);
//   dup2(STDERR_FILENO, STDOUT_FILENO);
//   close(STDIN_FILENO);
//   open("nonsense.cc", O_RDONLY);
//   close(STDERR_FILENO);
//   return 0;
// }
int main(int argc, char *argv[]) {
  int fd1 = open("nonsense.cc", O_RDONLY);
  int fd2 = dup(fd1);
  close(fd1);
  int fd3 = dup(fd2);
  int fd4 = dup(fd3);
  int dupfd4 = dup(fd4);
  dup2(STDERR_FILENO, fd4);
  int dupstd = dup2(STDERR_FILENO, STDOUT_FILENO);
  close(STDIN_FILENO);
  int openafile = open("nonsense.cc", O_RDONLY);
  close(STDERR_FILENO);
  close(fd4);
  close(dupfd4);
  close(fd3);
  close(fd2);
  close(dupstd);
  close(openafile);
  return 0;
}

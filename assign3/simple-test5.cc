/**
 * File: simple-test5.cc
 * ---------------------
 * Presents the implementation of a short nonsense program that makes a small number of
 * system calls.  The program can be run standalone, but it's really designed to be fed as
 * an argument to the trace executable, as with:
 * 
 *    > ./trace ./simple-test5
 *
 * This test is useful for confirming that good and bad calls to 
 * open, write, and close get called just prior to exit.
 */

#include <fcntl.h>
#include <unistd.h>
#include <cassert>

int main(int argc, char *argv[]) {
  write(STDOUT_FILENO, "12345\n", 6);
  int fd = open(__FILE__, O_RDONLY);
  write(fd, "12345\n", 5);
  close(fd);
  read(fd, NULL, 64);
  close(/* bogusfd = */ 1000);
  return 0;
}

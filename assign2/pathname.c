
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
  // remove the placeholder implementation and replace with your own
  fprintf(stderr, "pathname_lookup(path=%s) unimplemented.  Returing -1.\n", pathname);
  return -1;
}

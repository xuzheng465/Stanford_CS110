#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

// remove the placeholder implementation and replace with your own
int directory_findname(struct unixfilesystem *fs, const char *name,
		       int dirinumber, struct direntv6 *dirEnt) {
  fprintf(stderr, "directory_lookupname(name=%s dirinumber=%d)\n", name, dirinumber); 
  return -1;
}

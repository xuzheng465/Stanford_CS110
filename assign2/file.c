#include <stdio.h>
#include <assert.h>

#include "file.h"
#include "inode.h"
#include "diskimg.h"

// remove the placeholder implementation and replace with your own
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
  fprintf(stderr, "file_getblock(inumber = %d, blockNum = %d) unimplemented. returning -1\n", inumber, blockNum);
  return -1;  
}

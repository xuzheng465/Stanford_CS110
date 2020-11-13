#include <stdio.h>
#include <assert.h>

#include "inode.h"
#include "diskimg.h"

// remove the placeholder implementation and replace with your own
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
  fprintf(stderr, "inode_get(inumber=%d) not implemented, returning -1\n", inumber);
  return -1;  
}

// remove the placeholder implementation and replace with your own
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
  fprintf(stderr, "inode_indexlookup(blockNum=%d) not implemented, returning -1\n", blockNum);
  return -1;
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}

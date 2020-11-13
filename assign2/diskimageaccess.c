#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>

#include "diskimg.h"
#include "unixfilesystem.h"
#include "inode.h"
#include "file.h"
#include "directory.h"
#include "pathname.h"
#include "chksumfile.h"

int quietFlag = 0; 
int idumpFlag = 0;
int pdumpFlag = 0;

static void PrintDirectory(struct unixfilesystem *fs,  char *pathname);
static void DumpInodeChecksum(struct unixfilesystem *fs, FILE *f);
static void DumpPathnameChecksum(struct unixfilesystem *fs, FILE *f);
static void PrintUsageAndExit(char *progname);
static int GetDirEntries(struct unixfilesystem *fs, int inumber, struct direntv6 *entries, int maxNumEntries);

int main(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "iqp")) != -1) {
    switch (opt) {
    case 'q':
      quietFlag = 1;
      break;
    case 'i':
      idumpFlag = 1;
      break;
    case 'p':
      pdumpFlag = 1;
      break;
    default: 
      PrintUsageAndExit(argv[0]);
    } 
  }

  if (optind != argc-1) {
    PrintUsageAndExit(argv[0]);
  }

  char *diskpath = argv[optind];
  int fd = diskimg_open(diskpath, 1);

  if (fd < 0) {
    fprintf(stderr, "Can't open diskimagePath %s\n", diskpath);
    exit(EXIT_FAILURE);
  }

  struct unixfilesystem *fs = unixfilesystem_init(fd);
  if (!fs) {
    fprintf(stderr, "Failed to initialize unix filesystem\n");
    exit(EXIT_FAILURE);
  }

  if (!quietFlag) {  
    int disksize = diskimg_getsize(fd);
    if (disksize < 0) {
      fprintf(stderr, "Error getting the size of %s\n", argv[1]);
      // Cast the result of diskimg_close to void so the compiler doesn't
      // complain that we're ignoring its return value.
      (void) diskimg_close(fd);
      free(fs);
      exit(EXIT_FAILURE);
    }
    printf("Disk %s is %d bytes (%d KB)\n", argv[1],  disksize, disksize/1024);
    printf("Superblock s_isize %d\n",(int)fs->superblock.s_isize);
    printf("Superblock s_fsize %d\n",(int)fs->superblock.s_fsize);
    printf("Superblock s_nfree %d\n",(int)fs->superblock.s_nfree);
    printf("Superblock s_ninode %d\n",(int)fs->superblock.s_ninode);
  }

  if (idumpFlag) DumpInodeChecksum(fs, stdout);
  if (pdumpFlag) DumpPathnameChecksum(fs, stdout);

  int err = diskimg_close(fd);
  if (err < 0) fprintf(stderr, "Error closing %s\n", argv[1]);
  free(fs);
  exit(EXIT_SUCCESS);
  return 0;
}

/**
 * Output to the specified file the checksum of all allocated inodes.
 *
 * This is used by the grading script, so be careful not to change its output
 * format.
 */
static void DumpInodeChecksum(struct unixfilesystem *fs, FILE *f) {
  for (int inumber = 1; inumber < fs->superblock.s_isize*16; inumber++) {
    struct inode in;
    if (inode_iget(fs, inumber, &in) < 0) {
      fprintf(stderr,"Can't read inode %d \n", inumber);
      return;
    }
    if ((in.i_mode & IALLOC) == 0) {
      // Skip this inode if it's not allocated.
      continue;
    }

    char chksum[CHKSUMFILE_SIZE];
    if (chksumfile_byinumber(fs, inumber, chksum) < 0) {
      fprintf(stderr, "Inode %d can't compute chksum\n", inumber);
      continue;
    }

    char chksumstring[CHKSUMFILE_STRINGSIZE];
    chksumfile_cvt2string(chksum, chksumstring);

    int size = inode_getsize(&in);
    fprintf(f, "Inode %d mode 0x%x size %d checksum %s\n",inumber,in.i_mode, size, chksumstring);
  }
}

/**
 * Output to the specified file the checksum of the specified pathname and
 * inode as well as all its children if it is a directory.
 *
 * This is used by the grading script, so be careful not to change its output
 * format.
 */
static void DumpPathAndChildren(struct unixfilesystem *fs, const char *pathname, int inumber, FILE *f) {
  struct inode in;
  if (inode_iget(fs, inumber, &in) < 0) {
    fprintf(stderr,"Can't read inode %d \n", inumber);
    return;
  }
  assert(in.i_mode & IALLOC);

  char chksum1[CHKSUMFILE_SIZE];
  if (chksumfile_byinumber(fs, inumber, chksum1) < 0) {
    fprintf(stderr,"Can't checksum inode %d path %s\n", inumber, pathname);
    return;
  }

  char chksum2[CHKSUMFILE_SIZE];
  if (chksumfile_bypathname(fs, pathname, chksum2) < 0) {
    fprintf(stderr,"Can't checksum inode %d path %s\n", inumber, pathname);
    return;
  }

  if (!chksumfile_compare(chksum1, chksum2)) {
    fprintf(stderr,"Pathname checksum of %s differs from inode %d\n", pathname, inumber);
    return;
  }

  char chksumstring[CHKSUMFILE_STRINGSIZE];
  chksumfile_cvt2string(chksum2, chksumstring);
  int size = inode_getsize(&in);
  fprintf(f, "Path %s %d mode 0x%x size %d checksum %s\n",pathname,inumber,in.i_mode, size, chksumstring);

  if (pathname[1] == 0) {
    /* pathame == "/" */
    pathname++; /* Delete extra / character */
  }

  if ((in.i_mode & IFMT) == IFDIR) { 
      const unsigned int MAXPATH = 1024;
      if (strlen(pathname) > MAXPATH-16) {
        fprintf(stderr, "Too deep of directories %s\n", pathname);
      }

      struct direntv6 direntries[10000];
      int numentries = GetDirEntries(fs, inumber, direntries, 10000);
      for (int i = 0; i < numentries; i++) {
        char *n =  direntries[i].d_name;
        if (n[0] == '.') {
          if ((n[1] == 0) || ((n[1] == '.') && (n[2] == 0))) {
            /* Skip over "." and ".." */
            continue;
          }
        }

        char nextpath[MAXPATH];
        sprintf(nextpath, "%s/%s",pathname, direntries[i].d_name);
        DumpPathAndChildren(fs, nextpath,  direntries[i].d_inumber, f);
      }
  }
}

/**
 * Output to the specified file the checksum of files on the disk by
 * tranversing the naming hierarcy. 
 * Note this is used by the grading script so don't alter output format. 
 */
static void DumpPathnameChecksum(struct unixfilesystem *fs, FILE *f) {
  DumpPathAndChildren(fs, "/", ROOT_INUMBER, f);
}

/**
 * Print all the entries in the specified directory. 
 */
static void PrintDirectory(struct unixfilesystem *fs,  char *pathname) {
  int inumber = pathname_lookup(fs, pathname);
  if (inumber < 0) {
    fprintf(stderr, "Can't find %s\n", pathname);
    return;
  }

  struct direntv6 direntries[10000];
  int numentries = GetDirEntries(fs, inumber, direntries, 10000);
  if (numentries < 0) {
    fprintf(stderr, "Can't read entries from %s\n", pathname);
    return;
  }
  
  for (int i = 0; i < numentries; i++) { 
    printf("Direntry %s Name %s Inumber %d\n", pathname, direntries[i].d_name, direntries[i].d_inumber);
  }
}

/**
 * Fetch as many entries from a directory that will fit in the specified array. Return the 
 * number of entries found. 
 */
static int GetDirEntries(struct unixfilesystem *fs, int inumber, struct direntv6 *entries, int maxNumEntries) {
  struct inode in;
  int err = inode_iget(fs, inumber, &in);
  if (err < 0) return err;
  
  if (!(in.i_mode & IALLOC) || ((in.i_mode & IFMT) != IFDIR)) {
    /* Not allocated or not a directory */
    return -1;
  }

  if (maxNumEntries < 1) return -1;
  int size = inode_getsize(&in);

  assert((size % sizeof(struct direntv6)) == 0);

  int count = 0;
  int numBlocks  = (size + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;
  char buf[DISKIMG_SECTOR_SIZE];
  struct direntv6 *dir = (struct direntv6 *) buf;
  for (int bno = 0; bno < numBlocks; bno++) {
    int bytesLeft, numEntriesInBlock, i;
    bytesLeft = file_getblock(fs, inumber,bno,dir);
    if (bytesLeft < 0) {
      fprintf(stderr, "Error reading directory\n");
      return -1;
    }
    numEntriesInBlock = bytesLeft/sizeof(struct direntv6); 
    for (i = 0; i <  numEntriesInBlock ; i++) { 
      entries[count] = dir[i];
      count++;
      if (count >= maxNumEntries) return count;
    }
  }
  return count;
}


static void PrintUsageAndExit(char *progname) {
  fprintf(stderr, "Usage: %s <options> diskimagePath\n", progname);
  fprintf(stderr, "where <options> can be:\n");
  fprintf(stderr, "-q     don't print extra info\n"); 
  fprintf(stderr, "-i     print all inode checksums\n"); 
  fprintf(stderr, "-p     print all pathname checksums\n");  
  exit(EXIT_FAILURE);
}

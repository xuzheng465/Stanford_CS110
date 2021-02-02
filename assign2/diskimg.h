#ifndef _DISKIMG_H_
#define _DISKIMG_H_

#include <stdint.h>

// Size of a disk sector (e.g. block) in bytes.
#define DISKIMG_SECTOR_SIZE 512

/**
 * Opens a disk image for I/O. Returns an open file descriptor, or -1 if
 * unsuccessful.  
 */
int diskimg_open(char *pathname, int readOnly);

/**
 * Returns the size of the disk imgage in bytes, or -1 if unsuccessful.
 */
int diskimg_getsize(int fd); 

/**
 * Reads the specified sector (e.g. block) from the disk.  Returns the number of bytes read,
 * or -1 on error.
 */
int diskimg_readsector(int fd, int sectorNum, void *buf); 

/**
 * Writes the specified sector from the disk.  Returns the number of bytes
 * written, or -1 on error.
 */
int diskimg_writesector(int fd, int sectorNum, void *buf); 

/**
 * Clean up from a previous diskimg_open() call.  Returns 0 on success, or -1 on
 * error.
 */
int diskimg_close(int fd);

#endif // _DISKIMG_H_

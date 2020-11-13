#ifndef _INO_H
#define  _INO_H

#include <stdint.h>

/**
 * This is taken from sys/ino.h of Unix Version 6.  The actual structure looked like:
 *
 *   int    i_mode;
 *   char   i_nlink;
 *   char   i_uid;
 *   char   i_gid;
 *   char   i_size0;
 *   char  *i_size1;
 *   int    i_addr[8];
 *   int    i_atime[2];
 *   int    i_mtime[2];
 * 
 * The machine this originally ran on had 16-bit ints and 16-bit pointers.
 * We've converted the structure to use types from stdint.h so that the widths
 * of the fields match those in the original structure.
 */

/**
 * The inode is the focus of all file activity in unix.  There is a unique
 * inode allocated for each active file, each current directory, each mounted-on
 * file, text file, and the root.  An inode is 'named' by its dev/inumber pair. (iget/iget.c)
 * Data, from mode on, is read in from permanent inode on volume.
 */

struct inode {
  uint16_t	i_mode;         // bit vector of file type and permissions
  uint8_t 	i_nlink;	// number of references to file
  uint8_t 	i_uid;		// owner
  uint8_t 	i_gid;		// group of owner
  uint8_t  	i_size0;	// most significant byte of size
  uint16_t	i_size1;	// lower two bytes of size (size is encoded in a three-byte number)
  uint16_t	i_addr[8];	// device addresses constituting file
  uint16_t	i_atime[2];     // access time
  uint16_t	i_mtime[2];     // modify time
};

/* modes */
#define	IALLOC	0100000		// file is used
#define	IFMT	060000		// type of file
#define	IFDIR	040000	        // directory
#define	IFCHR	020000	        // character special
#define	IFBLK	060000	        // block special, 0 is regular
#define	ILARG	010000		// large addressing algorithm
#define	ISUID	04000		// set user id on execution
#define	ISGID	02000		// set group id on execution
#define ISVTX	01000		// save swapped text even after use
#define	IREAD	0400		// read, write, execute permissions
#define	IWRITE	0200
#define	IEXEC	0100

#endif // _INO_H

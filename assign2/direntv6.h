#ifndef _DIRENTV6_H_
#define _DIRENTV6_H_

#include <stdint.h>

/**
 * The Unix Version 6 code didn't use a structure like this but this is 
 * structure does match for format of a directory entry.
 */
struct direntv6 {
  uint16_t d_inumber;   
  char     d_name[14];
};

#endif // _DIRENTV6_H_

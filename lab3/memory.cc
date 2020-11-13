/**
 * File: memory.cc
 * ---------------
 * Provides the implementation of those functions exported by memory.h.
 */

#include "memory.h"
#include <sys/mman.h>
#include "random-generator.h"
#include <iostream>
using namespace std;

int *createSharedArray(size_t length) {
  int *numbers =
    static_cast<int *>(mmap(NULL, length * sizeof(int), PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0));
  RandomGenerator rgen;
  for (size_t i = 0; i < length; i++) 
    numbers[i] = rgen.getNextInt(kMinValue, kMaxValue);
  return numbers;
}

void freeSharedArray(int *array, size_t length) {
  munmap(array, length * sizeof(int));
}

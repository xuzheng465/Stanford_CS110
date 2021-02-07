/**
 * File: memory.h
 * --------------
 * Exports a pair of functions needed to dynamically allocate
 * a block of memory in an anonymous segment that can be shared
 * processes.
 */

#pragma once
#include <cstddef>   // for size_t
#include <string>

/**
 * Constants: kMinValue, kMaxValue
 * -------------------------------
 * Defines the smallest and largest numbers that
 * might be randomly chosen to populate a shared array.
 */
static const unsigned int kMinValue = 100;
static const unsigned int kMaxValue = 999;

/**
 * Function: createSharedArray
 * ---------------------------
 * Dynamically allocates an integer array of the specified
 * length, populates each entry with a random number drawn
 * from [kMinValue, kMaxValue], and then returns the
 * base address of the array.
 *
 * Note the array isn't allocated in the heap, but in another
 * segment created specifically for the purposes of this library.
 * Furthermore, the array is set up so that one copy is shared
 * across all fork boundaries (in contrast to the deep copies
 * that normally come with fork).
 */
int *createSharedArray(size_t length);

/**
 * Function: freeSharedArray
 * -------------------------
 * Properly disposes of the array returned by createSharedArray.
 */
void freeSharedArray(int *array, size_t length);


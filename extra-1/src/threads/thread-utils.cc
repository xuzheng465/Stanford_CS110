/**
 * Presents the implementation of those
 * methods exported by the thread-utils.h
 * interface.
 */

#include "thread-utils.h"
#include <chrono> // for chrono::milliseconds
#include <thread> // for this_thread
using namespace std;

void sleep_for(size_t milliseconds) {
  chrono::milliseconds time(milliseconds);
  this_thread::sleep_for(time);
}

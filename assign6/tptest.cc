/**
 * File: tptest.cc
 * ---------------
 * Simple test in place to verify that the ThreadPool class works.
 */

#include <iostream>
#include "thread-pool.h"
#include "thread-utils.h"
#include "ostreamlock.h"
using namespace std;

static const size_t kNumThreads = 12;
static const size_t kNumFunctions = 1000;
static void simpleTest() {
  ThreadPool pool(kNumThreads);
  for (size_t id = 0; id < kNumFunctions; id++) {
    pool.schedule([id] {
      cout << oslock << "Thread (ID: " << id << ") has started." << endl << osunlock;
      size_t sleepTime = (id % 3) * 10;
      sleep_for(sleepTime);
      cout << oslock << "Thread (ID: " << id << ") has finished." << endl << osunlock;
    });
  }

  pool.wait();
}

int main(int argc, char *argv[]) {
  simpleTest();
  return 0;
}

/**
 * File: event-barrier-test.cc
 * ---------------------------
 * Minimal framework to exercise the EventBarrier class.
 */

#include "event-barrier.h"
#include <iostream>
#include <random>
#include <thread>
#include <unistd.h>
#include "ostreamlock.h"
using namespace std;

static void minstrel(const string& name, EventBarrier& eb) {
  cout << oslock << name << " walks toward the drawbridge." << endl << osunlock;
  sleep(random() % 3 + 3); // minstrels arrive at drawbridge at different times
  cout << oslock << name << " arrives at the drawbridge, must wait." << endl << osunlock;
  eb.wait(); // all minstrels wait until drawbridge has been raised
  cout << oslock << name << " detects drawbridge lifted, starts crossing." << endl << osunlock;
  sleep(random() % 3 + 2); // minstrels walk at different rates
  cout << oslock << name << " has crossed the bridge." << endl << osunlock;
  eb.past();
}

static void gatekeeper(EventBarrier& eb) {
  sleep(random() % 5 + 7);
  cout << oslock << "Gatekeeper raises the drawbridge." << endl << osunlock;
  eb.lift(); // lift the drawbridge
  cout << oslock << "Gatekeeper lowers drawbridge knowing all have crossed." << endl << osunlock;
}

static string kMinstrelNames[] = { 
  "Peter", "Paul", "Mary", "Manny", "Moe", "Jack" 
};
static const size_t kNumMinstrels = 6;
int main(int argc, char *argv[]) {
  EventBarrier drawbridge;
  thread minstrels[kNumMinstrels];
  for (size_t i = 0; i < kNumMinstrels; i++) 
    minstrels[i] = thread(minstrel, kMinstrelNames[i], ref(drawbridge));
  thread g(gatekeeper, ref(drawbridge));
  
  for (thread& c: minstrels) c.join();
  g.join();
  return 0;
}

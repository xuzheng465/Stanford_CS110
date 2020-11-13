#include "memory.h"
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <algorithm>
#include <iostream>
#include <iomanip>
using namespace std;

static bool shouldKeepMerging(size_t start, size_t reach, size_t length) {
  return start % reach == 0 && reach <= length;
}

static void repeatedlyMerge(int numbers[], size_t length, size_t start) {
  int *base = numbers + start;
  for (size_t reach = 2; shouldKeepMerging(start, reach, length); reach *= 2) {
    raise(SIGSTOP);
    inplace_merge(base, base + reach/2, base + reach);
  }
  exit(0);
}

static void createMergers(int numbers[], pid_t workers[], size_t length) {
  for (size_t start = 0; start < length; start++) {
    workers[start] = fork();
    if (workers[start] == 0) 
      repeatedlyMerge(numbers, length, start);
  }
}

static void orchestrateMergers(int numbers[], pid_t workers[], size_t length) {
  size_t step = 1;
  while (step <= length) {
    for (size_t start = 0; start < length; start += step) 
      waitpid(workers[start], NULL, WUNTRACED);
    step *= 2;
    for (size_t start = 0; start < length; start += step) 
      kill(workers[start], SIGCONT);
  }
}

static void mergesort(int numbers[], size_t length) {
  pid_t workers[length];
  createMergers(numbers, workers, length);
  orchestrateMergers(numbers, workers, length);
}

static const size_t kNumElements = 128;
int main(int argc, char *argv[]) {
  for (size_t trial = 1; trial <= 10000; trial++) {
    int *numbers = createSharedArray(kNumElements);    
    mergesort(numbers, kNumElements);
    bool sorted = is_sorted(numbers, numbers + kNumElements);
    cout << "\rTrial #" << setw(5) << setfill('0') << trial << ": " 
         << (sorted ? "\033[1;34mSUCCEEDED!\033[0m" : "\033[1;31mFAILED!   \033[0m") << flush;
    freeSharedArray(numbers, kNumElements);
    if (!sorted) { cout << endl << "mergesort is \033[1;31mBROKEN.... please fix!\033[0m" << flush; break; }
  }
  cout << endl;
  return 0;
}

#include "memory.h"
#include <cassert>
#include <algorithm>    // for is_sorted
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

static void foreverSwap(int &first, int& second) {
  while (true) {
    raise(SIGSTOP);
    if (first > second) swap(first, second);
  }
}

static void launchAllSwappers(int numbers[], pid_t pids[], size_t count) {
  for (size_t i = 0; i < count; i++) {
    pids[i] = fork();
    if (pids[i] == 0) 
      foreverSwap(numbers[i], numbers[i + 1]);
  }
  
  for (size_t i = 0; i < count; i++) 
    waitpid(pids[i], NULL, WUNTRACED);  
}

static void startAndWait(pid_t pids[], size_t start, size_t stop) {
  for (size_t i = start; i < stop; i += 2) kill(pids[i], SIGCONT);
  for (size_t i = start; i < stop; i += 2) waitpid(pids[i], NULL, WUNTRACED);
}

static void manipulateSwappers(pid_t pids[], size_t count) {
  for (size_t pass = 0; pass <= count/2; pass++) {
    startAndWait(pids, 0, count);
    startAndWait(pids, 1, count);
  }
  
  for (size_t i = 0; i < count; i++) kill(pids[i], SIGKILL);
  for (size_t i = 0; i < count; i++) waitpid(pids[i], NULL, 0);
}

static void bubblesort(int numbers[], size_t length) {
  size_t numProcesses = length - 1;
  pid_t pids[numProcesses];
  launchAllSwappers(numbers, pids, numProcesses);
  manipulateSwappers(pids, numProcesses);
}

const size_t kNumElements = 128;
int main(int argc, char *argv[]) {
  for (size_t trial = 1; trial <= 10000; trial++) {
    int *numbers = createSharedArray(kNumElements);    
    bubblesort(numbers, kNumElements);
    bool sorted = is_sorted(numbers, numbers + kNumElements);
    cout << "\rTrial #" << setw(5) << setfill('0') << trial << ": " 
         << (sorted ? "\033[1;34mSUCCEEDED!\033[0m" : "\033[1;31mFAILED!   \033[0m") << flush;
    freeSharedArray(numbers, kNumElements);
    if (!sorted) { cout << endl << "bubblesort is \033[1;31mBROKEN.... please fix!\033[0m" << flush; break; }
  }
  cout << endl;
  return 0;
}

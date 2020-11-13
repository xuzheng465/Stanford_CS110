/**
 * File: quicksort.cc
 * ------------------
 * Provides a working sequential version of quicksort, leaving
 * you with the task of implementing two different multithreaded
 * versions as descrived in the lab4 handout.
 */

#include "random-generator.h"  // for RandomGenerator
#include <vector>              // for vector
#include <algorithm>           // for is_sorted
#include <iostream>            // for cout, flush, endl
#include <iomanip>             // for setw, setfill
#include <cassert>             // for assert macro
#include <thread>              // for thread
using namespace std;

/**
 * Function: populateSequence
 * --------------------------
 * Self-explanatory.
 */
static const unsigned int kMinValue = 100;
static const unsigned int kMaxValue = 999;
static void populateSequence(vector<int>& numbers) {
  RandomGenerator rgen;
  for (int& n: numbers) 
    n = rgen.getNextInt(kMinValue, kMaxValue);
}

/**
 * Function: partition
 * -------------------
 * Partially sorts the range of integers in the supplied vector so that
 * all numbers less than or equal to numbers[start] appears before all numbers
 * strictly greater than numbers[start].
 * 
 * This version of partition is subscribed to as is by all three
 * versions of quicksort.
 */
static size_t partition(vector<int>& numbers, ssize_t start, ssize_t finish) {
  int pivot = numbers[start];
  ssize_t lh = start + 1;
  ssize_t rh = finish;
  while (true) {
    while (lh < rh && numbers[rh] >= pivot) rh--;
    while (lh < rh && numbers[lh] < pivot) lh++;
    if (lh == rh) break;
    swap(numbers[lh], numbers[rh]);
  }
  
  if (numbers[lh] >= pivot) return start;
  numbers[start] = numbers[lh];
  numbers[lh] = pivot;
  return lh;
}

static void conservativeQuicksort(vector<int>& numbers) {
  cerr << "conservativeQuicksort is not implemented yet." << endl;
  exit(EXIT_FAILURE);
}

static void aggressiveQuicksort(vector<int>& numbers) {
  cerr << "aggressiveQuicksort is not implemented yet."<< endl;
  exit(EXIT_FAILURE);
}

/**
 * Function: quicksort
 * -------------------
 * Implements the canonical version of quicksort.
 */
static void quicksort(vector<int>& numbers, ssize_t start, ssize_t finish) {
  if (start >= finish) return;
  ssize_t mid = partition(numbers, start, finish);
  quicksort(numbers, start, mid - 1);
  quicksort(numbers, mid + 1, finish);
}

/**
 * Function: quicksort
 * -------------------
 * Wraps around the call to the sequential version of quicksort.
 * The real work is done by the three-argument version of quicksort
 * above.
 */
static void quicksort(vector<int>& numbers) {
  quicksort(numbers, 0, numbers.size() - 1);
}

/**
 * Function: usage
 * ---------------
 * Called whenever there's an error, usage prints the supplied
 * message, and generic usage message, and then terminates the program.
 */
static void usage(const string& message) {
  cerr << "Error: " << message << endl;
  cerr << "Usage: ./quicksort [--aggressive | --conservative | --sequential]" << endl;
  exit(EXIT_FAILURE);
}

/**
 * Function: validateArgumentVector
 * --------------------------------
 * Validates the argument vector passed to main, and provided
 * everything looks correct, returns one of three strings to 
 * be clear which version of quicksort should be executed.
 */
static string validateArgumentVector(char *argv[], int argc) {
  if (argc == 1) return "sequential";
  if (argc > 2) usage("Invoked with too many arguments.");
  string flag = argv[1];
  if (flag == "--aggressive" || flag == "--conservative" || flag == "--sequential") 
    return flag.substr(2);
  usage("Second argument must be one of three different flags.");
  assert(false); // can't get here, but g++ can't tell that
}

static const size_t kNumElements = 128; // don't make the number bigger than this
int main(int argc, char *argv[]) {
  string version = validateArgumentVector(argv, argc);
  void (*qsfn)(vector<int>&) = quicksort;
  if (version == "aggressive") qsfn = aggressiveQuicksort;
  else if (version == "conservative") qsfn = conservativeQuicksort;
  for (size_t trial = 1; trial <= 1000; trial++) {
    vector<int> numbers(kNumElements);
    populateSequence(numbers);
    qsfn(numbers);
    bool sorted = is_sorted(numbers.cbegin(), numbers.cend());
    cout << "\rTrial #" << setw(4) << setfill('0') << trial << ": " 
         << (sorted ? "\033[1;34mSUCCEEDED!\033[0m" : "\033[1;31mFAILED!   \033[0m") << flush;
    if (!sorted) { cout << endl << "quicksort is \033[1;31mBROKEN.... please fix!\033[0m" << flush; break; }
  }
  cout << endl;
  return 0;
}

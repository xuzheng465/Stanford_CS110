/**
 * File: spin.cc
 * -------------
 * Presents an executable that sleeps for <n> seconds
 * in one-second bursts.
 */
#include <iostream>  // for cerr
#include <unistd.h>  // sleep
using namespace std;

static const int kWrongArgumentCount = 1;
int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <n>" << endl;
    return kWrongArgumentCount;
  }
  
  size_t secs = atoi(argv[1]);
  for (size_t i = 0; i < secs; i++) sleep(1);
  return 0;
}

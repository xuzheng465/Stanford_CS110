/**
 * int.cc
 * ------
 * Short program used to test stsh. int spins for <n>
 * seconds in one-second bursts, and then sends itself a
 * SIGINT.
 */
#include <iostream>       // for cerr
#include <unistd.h>       // for sleep, getpid
#include <sys/wait.h>     // for raise, SIGINT
using namespace std;

static const int kWrongArgumentCount = 1;
static const int kRaiseFailed = 2;
int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <n>" << endl;
    return kWrongArgumentCount;
  }

  size_t secs = atoi(argv[1]);
  for (size_t i = 0; i < secs; i++) sleep(1);

  if (raise(SIGINT) != 0) {
    cerr << "Problem interrupting process " << getpid() << "." << endl;
    return kRaiseFailed;
  }

  return 0;
}

/** 
 * File: split.cc
 * --------------
 * Short used to test stsh. split forks a child 
 * that spins for <n> seconds in one-second bursts.
 */
#include <iostream>    // for cerr
#include <unistd.h>    // for fork, sleep
#include <sys/wait.h>  // for wait
using namespace std;

static const int kWrongArgumentCount = 1;
static const int kForkFailed = 2;
int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <n>" << endl;
    return kWrongArgumentCount;
  }
  
  size_t secs = atoi(argv[1]);
  pid_t pid = fork();
  if (pid < 0) {
    cerr << "fork function failed." << endl;
    return kForkFailed;
  }

  if (pid == 0) {
    for (size_t i = 0; i < secs; i++) sleep(1);
    return 0;
  }

  wait(NULL);
  return 0;
}

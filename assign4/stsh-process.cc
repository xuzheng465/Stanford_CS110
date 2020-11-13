/**
 * File: stsh-process.cc
 * ---------------------
 * Presents the implementation of the STSHProcess class.
 */

#include "stsh-process.h"
#include <iomanip>  // for setw, left
using namespace std;

STSHProcess::STSHProcess(pid_t pid, const command& command, STSHProcessState state) : pid(pid), state(state) {
  tokens.push_back(command.command);
  for (char * const *tokenp = &command.tokens[0]; *tokenp != NULL; tokenp++)
    tokens.push_back(*tokenp);
}

static ostream& operator<<(ostream& os, STSHProcessState state) {
  const char *str = "Unknown";
  switch (state) {
    case kWaiting: str = "Waiting"; break;
    case kRunning: str = "Running"; break;
    case kStopped: str = "Stopped"; break;
    case kTerminated: str = "Terminated"; break;
  }

  return os << str;
}

ostream& operator<<(ostream& os, const STSHProcess& process) {
  os << setw(5) << process.pid << " " << setw(12) << left << process.state << right;
  for (const string& token: process.tokens) os << " " << token;
  return os;
}

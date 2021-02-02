/**
 * File: trace-system-calls-test.cc
 * --------------------------------
 * Exercises the functionality of the trace-system-calls module,
 * which is supposed to know precisely how to dig into the system include and Linux 
 * source directory trees to pull out system call numbers, names, and signatures.
 */

#include "trace-system-calls.h"
#include <iostream>
using namespace std;

static void printSignature(int systemCallNumber, const string& systemCallName, const systemCallSignature& signature) {
  cout << systemCallNumber << ": " << systemCallName << "(";
  for (size_t i = 0; i < signature.size(); i++) {
    cout << signature[i];
    if (i == signature.size() - 1) break;
    cout << ", ";
  }
  cout << ")"; 
}

static void printSystemCallData(const map<int, string>& systemCallNumbers, const map<string, systemCallSignature>& systemCallSignatures) {
  for (const pair<int, string>& numToNamePair: systemCallNumbers) {
    int systemCallNumber = numToNamePair.first;
    const string& systemCallName = numToNamePair.second;
    auto iter = systemCallSignatures.find(systemCallName);
    if (iter != systemCallSignatures.cend()) {
      printSignature(systemCallNumber, systemCallName, iter->second);
    } else {
      cout << systemCallNumber << ": " << systemCallName << " isn't implemented.";
    }
    cout << endl;
  }
}

int main(int argc, char *argv[]) {
  map<int, string> systemCallNumbers;
  map<string, int> systemCallNames;
  map<string, systemCallSignature> systemCallSignatures;
  compileSystemCallData(systemCallNumbers, systemCallNames, systemCallSignatures, /* rebuild = */ true);
  printSystemCallData(systemCallNumbers, systemCallSignatures);
  return 0;
}

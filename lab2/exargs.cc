/**
 * File: exargs.cc
 * ---------------
 * Provides a full version of the exargs executable, which simulates
 * a very constrained version of the xargs builtin.  Our exargs
 * tokens standard input around blanks and newlines, extends
 * the initial argument vector of its command to include
 * all of these extra tokens, executes the full command, waits
 * for the command to finish, and then returns 0 if everything
 * went smoothly and the command exited with code 0, and returns
 * 1 otherwise.
 *
 * There is nothing to be written here.  This is just included in the
 * lab2 folder so you have a working version of exargs.
 */

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include "unistd.h"
#include <sys/wait.h>
using namespace std;

static void pullAllTokens(istream& in, vector<string>& tokens) {
  while (true) {
    string line;
    getline(in, line);
    if (in.fail()) break;
    istringstream iss(line);
    while (true) {      
      string token;
      getline(iss, token, ' ');
      if (iss.fail()) break;
      tokens.push_back(token);
    }
  }
}

int main(int argc, char *argv[]) {
  vector<string> tokens;
  pullAllTokens(cin, tokens);
  pid_t pid = fork();
  if (pid == 0) {
    char *exargsv[argc + tokens.size()];
    memcpy(exargsv, argv + 1, (argc - 1) * sizeof(char *));
    transform(tokens.cbegin(), tokens.cend(), exargsv + argc - 1, 
              [](const string& str) { return const_cast<char *>(str.c_str()); });
    exargsv[argc + tokens.size() - 1] = NULL;
    execvp(exargsv[0], exargsv);
    cerr << exargsv[0] << ": command not found" << endl;
    exit(0);
  }
  
  int status;
  waitpid(pid, &status, 0);
  return status == 0 ? 0 : 1; // trivia: if all of status is 0, then child exited normally with code 0
}

/**
 * File: stsh-readline.cc
 * ----------------------
 * Presents the implementation of the readline function, which can be configured to use 
 * the GNU readline library.
 */

#include "stsh-readline.h"
#include <readline/readline.h>
#include <readline/history.h>

#include <iostream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <getopt.h>
#include "string-utils.h"
using namespace std;

static string prompt = "stsh> ";
static bool history = true;
static const int kIncorrectUsage = 1;
static void printUsage(const string& message, const string& executable) {
  cerr << "Error: " << message << endl;
  cerr << "Usage: ./" << executable << " [--suppress-prompt] [--no-history]" << endl;
  exit(kIncorrectUsage);
}

void rlinit(int argc, char *argv[]) {
  struct option options[] = {
    {"suppress-prompt", no_argument, NULL, 's'},
    {"no-history", no_argument, NULL, 'n'},
    {NULL, 0, NULL, 0},
  };

  while (true) {
    int ch = getopt_long(argc, argv, "sn", options, NULL);
    if (ch == -1) break;
    switch (ch) {
    case 's':
      prompt = "";
      break;
    case 'n':
      history = false;
      break;
    default:
      printUsage("Unrecognized flag.", argv[0]);
    }
  }

  argc -= optind;
  if (argc > 0) printUsage("Too many arguments.", argv[0]);
}

bool readline(string& line) {
  line.clear();
  if (!history) {
    cout << prompt;
    getline(cin, line);
    trim(line);
    return !cin.eof();
  }
  
  char *s = readline(prompt.c_str());
  if (s == NULL) return false;
  line = s;
  free(s);
  trim(line);
  if (!line.empty()) 
    add_history(line.c_str());
  return true;
}

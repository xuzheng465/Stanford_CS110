/**
 * File: tsh-parse.h
 * -----------------
 * Exports a function that knows how to parse an
 * arbitrary string into tokens and store them into 
 * an argument vector, all while extracting the names of
 * input and/or output redirection files.
 */

#ifndef _tsh_parse_
#define _tsh_parse_

#include <vector>
#include <string>
#include <iostream>

const size_t kMaxCommandLength = 32;
const size_t kMaxArguments = 32;

struct command {
  char command[kMaxCommandLength + 1]; // NULL terminated
  char *tokens[kMaxArguments + 1]; // array, C strings are all NULL terminated
};

struct pipeline {
  std::string input;   // empty if no input redirection file to first command
  std::string output;  // empty if no output redirection file from last command
  std::vector<command> commands;
  bool background;

/**
 * Accepts a command line and parses it to construct the pipeline.
 * The command line is parsed according to the following rules:
 *
 * The input is taken to represent either a single command, or a list of
 * commands all separated by the '|' character.
 * 
 * Each command is parsed as a list of tokens that are delimited by white space
 * except where double quotes indicate the whitespace is significant.
 * So the line:
 *
 *   ls -a my_dir
 *
 * is parsed as the 3 tokens "ls", "-a", and "my_dir"
 * 
 * The line:
 *
 *   echo "This string is a single token"
 * 
 * is parsed, unsurprisingly, as 2 tokens.
 *
 * For each command, the first token is taken to be the name of the command,
 * and the following tokens are the arguments.
 *
 * The first command in the list (or the only command if no pipes are found)
 * also allows for input redirection of the form "< input" where input is the
 * name of a file containing input. This option can come before or after the
 * command.
 *
 * Similarly the last command in the list (or the only command) allows for
 * output redirection by specifying an option of the form "> output" where
 * output is the name of a file to be created that will hold the results of the
 * final command.
 *
 * Note that in the case of a single command, the one command can have both
 * input and output redirection, and those options can be specified in any
 * order. That is: "< input" , "> output", and  "command [args...]" can be
 * written in any order.
 */
  pipeline(const std::string& str);

/**
 * It frees the dynamically allocated char *s containing the argument lists.
 */
  ~pipeline();
};

std::ostream& operator<<(std::ostream& os, const pipeline& p);

#endif // _tsh_parse_

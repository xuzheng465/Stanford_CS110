/**
 * File: stsh-process.h
 * --------------------
 * Defines the behavior of the STSHProcess, which
 * models the state of a single process running under
 * the jurisdiction of stsh.  The class is fairly atomic
 * in the sense that it is little more than an
 * object-oriented record that knows how to seriaize itself to an ostream.
 *
 * Read through the header comments in stsh-job.h and stsh-job-list.h to see how
 * the STSHProcess class (by itself, neither difficult nor all that interesting)
 * can be used within STSHJobs and the STSHJobList.
 */

#pragma once
#include "stsh-parser/stsh-parse.h" // for struct command
#include <vector>   // for vector
#include <string>   // for string
#include <iostream> // for ostream

/**
 * Enumerated Type: STSHProcessState
 * ------------------------------------------------
 * Defines the various states an individual process might be in at any one time.
 * Processes can be waiting (i.e. we've added it to the job list but haven't formally
 * started it yet), running, stopped (ala SIGTSTP), or terminated (for whatever reason, but
 * is still in the job list because sibling processes in the job list are still running).
 */
enum STSHProcessState { 
  kWaiting, kRunning, kStopped, kTerminated 
};

class STSHProcess {

/**
 * Function: operator<<
 * Usage: cout << joblist;
 * -----------------------
 * Inserts a nicely formatted serialization of the provided STSHProcess
 * into the provided ostream.
 */
  friend std::ostream& operator<<(std::ostream& os, const STSHProcess& process);

public:

/**
 * Constructor: STSHProcess
 * ------------------------
 * Default constructor, where the process id is set to 0 as a placeholder.
 */
  STSHProcess(): pid(0) {}

/**
 * Constructor: STSHProcess
 * ------------------------
 * Constructs the object to package the provided pid, command line, and process state
 * together.
 */
  STSHProcess(pid_t pid, const command& command, STSHProcessState state = kRunning);

/**
 * Method: getID
 * -------------
 * Returns the pid of the STSHProcess.
 */
  pid_t getID() const { return pid; }

/**
 * Method: getState
 * ----------------
 * Returns the state of the process, which should be one of the
 * legitimate STSHProcessState values.
 */
  STSHProcessState getState() const { return state; }

/**
 * Method: setState
 * ----------------
 * Sets the state of the process to be that provided.
 */
  void setState(STSHProcessState state) { this->state = state; }

private:
  pid_t pid;
  std::vector<std::string> tokens;
  STSHProcessState state;
};

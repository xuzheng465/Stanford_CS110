/**
 * File: stsh-job.h
 * ----------------
 * STSHJob manages all of the bookkeeping details associated
 * with an stsh job.  The following code snippet illustrates 
 * how an individual STSHJob is manipulated.
 * 
 *     static STSHJob createJob(size_t num, const vector<STSHProcess>& processes) {
 *       STSHJob job(num, kBackground);
 *       for (const STSHProcess& process: processes) job.addProcess(process);
 *     }
 *
 * In practice, STSHJobs are constructed by the JobList, which
 * maintains a list of all STSHJobs, as with this:
 *
 *     static size_t addJob(JobList& joblist, const vector<STSHProcess>& processes, STSHJobState state) {
 *       STSHJob& job = joblist.addJob(state);
 *       for (const STSHProcess& process) job.addProcess(process);
 *       return job.getNum(); // surface the job number the job was assigned
 *     }
 */

#pragma once
#include "stsh-process.h"
#include <cstddef>  // for size_t
#include <vector>   // for vector
#include <iostream> // for ostream

/**
 * Enumerated Type: STSHJobState
 * -----------------------------
 * Defines the two states a job might be in at any one time.  
 * We only track whether or not a job is running in the foreground
 * or background, not because it needs to be listed, but because it 
 * helps us identify which job should respond to forwarded SIGINTs 
 * and SIGTSTPs.
 */
enum STSHJobState { kForeground, kBackground };

class STSHJob {

/**
 * Function: operator<<
 * Usage: cout << joblist;
 * -----------------------
 * Inserts a nicely formatted serialization of the provided STSHJob
 * into the provided ostream.
 */
  friend std::ostream& operator<<(std::ostream& os, const STSHJob& job);
  
public:

/**
 * Constructor: STSHJob
 * --------------------
 * Default constructor, where the job number is just set to 0 (with the understanding
 * that all legitimate job numbers are actually supposed to be positive).
 */
  STSHJob(): num(0) {}

/**
 * Constructor: STSHJob
 * --------------------
 * Constructs an instance of STSHJob with the specified job number and state.
 */
  STSHJob(size_t num, STSHJobState state) : num(num), state(state) {}

/**
 * Method: STSHJob
 * ---------------
 * Retrieves the STSHJob's job number.
 */
  size_t getNum() const { return num; }

/**
 * Method: addProcess
 * ------------------
 * Appends the provided STSHProcess to be sequence of previously appended processes.
 */
  void addProcess(const STSHProcess& process) { processes.push_back(process); }

/**
 * Method: getProcesses
 * --------------------
 * Returns a reference to the sequence of STSHProcess instances making up
 * the job.
 */
  std::vector<STSHProcess>& getProcesses() { return processes; }
  const std::vector<STSHProcess>& getProcesses() const { return processes; }

/**
 * Method: containsProcess
 * -----------------------
 * Returns true if and only if the job contains a process with the specified pid.
 */
  bool containsProcess(pid_t pid) const;

/**
 * Method: getProcess
 * ------------------
 * Returns a reference (or const reference) to the STSHProcess maintained on
 * on behalf of the provided process id.  If no such process exists, then
 * a reference to the constant STSHJob::nprocess is returned as a sentinel.
 */
  STSHProcess& getProcess(pid_t pid);
  const STSHProcess& getProcess(pid_t pid) const;

/**
 * Method: getState
 * ----------------
 * Returns the state of the job (which is either kForeground or kBackground).
 */    
  STSHJobState getState() const { return state; }

/**
 * Method: setState
 * ----------------
 * Sets the job state (which must be either kForeground or kBackground).
 */
  void setState(STSHJobState state) { this->state = state; }

/**
 * Method: getGroupID
 * ------------------
 * Returns the process group id associated with the job, which should be the pid
 * of the leading process.  If the job is empty, then 0 is returned.
 */
  pid_t getGroupID() const { return processes.empty() ? 0 : processes[0].getID(); }

private:
  size_t num;
  std::vector<STSHProcess> processes;
  STSHJobState state;
  static STSHProcess nprocess;
};

/**
 * File: stsh-job-list.h
 * ---------------------
 * Defines the STSHJobList class and documents its behavior.
 * If you want to add a new process to a job list, you might 
 * do so this way:
 * 
 *    static void addToJobList(STSHJobList& jobList, const vector<pair<pid_t, string>>& children) {
 *      STSHJob& job = jobList.addJob(kBackground); //
 *      for (const pair<string, pid_t>& child: children) {
 *        pid_t pid = child.first;
 *        const string& command = child.second;
 *        job.addProcess(STSHProcess(pid, command)); // third argument defaults to kRunning     
 *      }
 *    
 *      cout << jobList;
 *    }
 *    
 * As you detect state changes in individual processes, you can update process states like this:
 * 
 *    static void updateJobList(STSHJobList& jobList, pid_t pid, STSHProcess state) {
 *      if (!jobList.containsProcess()) return;
 *      STSHJob& job = STSHJob.getJobWithProcess(pid);
 *      assert(job.containsProcess(pid));
 *      STSHProcess& process = job.getProcess(pid_t pid);
 *      process.setState(state);
 *      jobList.synchronize(job);
 *    }
 */

#pragma once
#include "stsh-parser/stsh-parse.h"
#include "stsh-job.h"
#include "stsh-process.h"
#include <cstddef>
#include <string>
#include <map>
#include <iostream>
#include <sys/types.h>

class STSHJobList {

/**
 * Overloaded version of operator<< so that an entire job list can just be
 * inserted into cout or some other ostream.  
 */
  friend std::ostream& operator<<(std::ostream& os, const STSHJobList& joblist);

public:

/**
 * Method: addJob
 * --------------
 * Inserts a new STSHJob into the job list.  The STSHJob doesn't
 * contain any processes, but it is given a job number and the state
 * of the job is set to be either kForeground or kBackground with the
 * understanding that it will almost certainly have one or more processes
 * inserted into it.  The method returns a reference to the STSHJob instance
 * held and owned by the STSHJobList.
 */
  STSHJob& addJob(const STSHJobState& state);

/**
 * Method: hasForegroundJob
 * ------------------------
 * Returns true if and only if the receiving STSHJobList has
 * a foreground job (of course, there can be at most one.)
 */
  bool hasForegroundJob() const;

/**
 * Method: getForegroundJob
 * ------------------------
 * Returns a reference to the foreground job. Typically, a call
 * this method should be guarded by a call to hasForegroundJob.
 * If this method is called when hasForegroundJob would have returned
 * false, the behavior is undefined.
 */  
  STSHJob& getForegroundJob();
  const STSHJob& getForegroundJob() const;

/**
 * Method: containsJob
 * -------------------
 * Returns true iff the job list contains a job with the
 * specified job number.
 */  
  bool containsJob(size_t num) const;

/**
 * Method: getJob
 * --------------
 * Returns a reference to the job with the specified job number.
 * If there is no such job, then the behavior is undefined.
 */
  STSHJob& getJob(size_t num);
  const STSHJob& getJob(size_t num) const;

/**
 * Method: containsProcess
 * -----------------------
 * Returns true iff some process within some
 * job within the job list has the specified pid.
 */
  bool containsProcess(pid_t pid) const;

/**
 * Method: getJobWithProcess
 * -------------------------
 * Returns a reference to the job that includes the process
 * identified by the specified pid.  Calls to this function
 * should be guarded by calls to containsProcess, because
 * when the specified pid doesn't exist, the behavior here
 * isn't defined.
 */
  STSHJob& getJobWithProcess(pid_t pid);
  const STSHJob& getJobWithProcess(pid_t pid) const;

/**
 * Method: synchronize
 * -------------------
 * Analyzes the provided job on the assumption that one
 * of its processes has recently changed state, and updates
 * the entire job around it to be consistent with those changes
 * (e.g. if all processes have terminated, the surrounding job is terminated, or
 * if none of the processes are running, then the job can't be considered
 * a foreground job).
 */  
  void synchronize(STSHJob& job);
  
private:
  size_t next = 1;
  std::map<size_t, STSHJob> jobs; // maps work, because we want to publish in order of job number
  static STSHJob njob;
};

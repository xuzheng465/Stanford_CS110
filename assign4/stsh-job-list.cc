/**
 * File: stsh-job-list.cc
 * ----------------------
 * Presents the implementation of the STSHJobList mutation methods.
 */

#include "stsh-job-list.h"
#include "stsh-exception.h"
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

STSHJob STSHJobList::njob; // njob stands for no-job

STSHJob& STSHJobList::addJob(const STSHJobState& state) {
  jobs[next] = STSHJob(next, state);
  return jobs[next++];
}

bool STSHJobList::hasForegroundJob() const {
  const STSHJob& job = getForegroundJob();
  return &job != &njob;
}

STSHJob& STSHJobList::getForegroundJob() {
  for (pair<const size_t, STSHJob>& p: jobs) {
    STSHJob& job = p.second;
    if (job.getState() == kForeground) {
      return job;
    }
  }

  return njob;
}

const STSHJob& STSHJobList::getForegroundJob() const { 
  return const_cast<STSHJobList *>(this)->getForegroundJob(); 
}

bool STSHJobList::containsJob(size_t num) const {
  return jobs.find(num) != jobs.cend();
}

STSHJob& STSHJobList::getJob(size_t num) {
  if (!containsJob(num)) return njob;
  return jobs[num];
}

const STSHJob& STSHJobList::getJob(size_t num) const {
  return const_cast<STSHJobList *>(this)->getJob(num);
}

bool STSHJobList::containsProcess(pid_t pid) const {
  const STSHJob& job = getJobWithProcess(pid);
  return &job != &njob;
}

STSHJob& STSHJobList::getJobWithProcess(pid_t pid) {
  for (pair<const size_t, STSHJob>& p: jobs) {
    STSHJob& job = p.second;    
    if (job.containsProcess(pid)) {
      return job;
    }
  }
  
  return njob;
}

const STSHJob& STSHJobList::getJobWithProcess(pid_t pid) const {
  return const_cast<STSHJobList *>(this)->getJobWithProcess(pid);
}

void STSHJobList::synchronize(STSHJob& job) {
  const vector<STSHProcess>& processes = job.getProcesses();
  bool somethingIsRunning = false;
  for (const STSHProcess& process: processes) {
    if (process.getState() == kRunning) {
      somethingIsRunning = true;
      break;
    }
  }
  
  if (!somethingIsRunning) {
    job.setState(kBackground); // make sure it's not categorized as foreground
  }
  
  for (const STSHProcess& process: processes) {
    if (process.getState() != kTerminated) {
      return;
    }
  }
  
  jobs.erase(job.getNum());
}

ostream& operator<<(ostream& os, const STSHJobList& joblist) {
  for (const pair<size_t, STSHJob>& p: joblist.jobs) 
    os << p.second << endl;
  return os;
}

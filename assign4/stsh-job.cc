/**
 * File: stsh-job.cc
 * -----------------
 * Provides the implementations of the STSHJob method set.
 */

#include "stsh-job.h"
#include <iomanip> // for setw
#include <sstream> // for ostringstream
using namespace std;

STSHProcess STSHJob::nprocess;

bool STSHJob::containsProcess(pid_t pid) const {
  const STSHProcess& process = getProcess(pid);
  return &process != &nprocess;
}

STSHProcess& STSHJob::getProcess(pid_t pid) {
  for (STSHProcess& process: processes) {
    if (process.getID() == pid) {
      return process;
    }
  }
  
  return nprocess;
}

const STSHProcess& STSHJob::getProcess(pid_t pid) const {
  return const_cast<STSHJob *>(this)->getProcess(pid);
}

ostream& operator<<(ostream& os, const STSHJob& job) {
  ostringstream oss;
  oss << "[" << job.num << "]";
  os << setw(oss.str().size()) << oss.str() << " ";
  if (job.processes.empty()) return os << "(job is empty, devoid of processes)";
  os << job.processes[0];
  for (size_t i = 1; i < job.processes.size(); i++) {
    os << " |" << endl;
    os << setw(oss.str().size()) << " " << " " << job.processes[i];
  }

  return os;
}

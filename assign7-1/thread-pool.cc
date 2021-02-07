/**
 * File: thread-pool.cc
 * --------------------
 * Presents the implementation of the ThreadPool class.
 * Copied my own solution from assignment 6.
 */

#include "thread-pool.h"
#include <algorithm>
using namespace std;

void ThreadPool::dispatcher() {
  while (true) {
    lock_guard<mutex> lgThunkQueue(mThunkQueue);
    cvThunkQueue.wait(mThunkQueue, [this]{
      return !thunkQueue.empty() || exit;
    });
    if (exit) return;
    mThunkQueue.unlock();
    lock_guard<mutex> lgWorkerStatus(mWorkerStatus);
    cvWorkerStatus.wait(mWorkerStatus, [this]{
      return find(wBusy.begin(), wBusy.end(), false) != wBusy.end();
    });
    for (size_t workerID = 0; workerID < wBusy.size(); workerID++) {
      if (!wBusy[workerID]) {
        wBusy[workerID] = true;
        mWorkerStatus.unlock();
        cvWorkerStatus.notify_all();
        mThunkQueue.lock();
        wThunks[workerID] = thunkQueue.front();
        thunkQueue.pop();
        mThunkQueue.unlock();
        cvThunkQueue.notify_all();
        sWorkers[workerID].signal();
        break;
      }
    }
  }
}

void ThreadPool::worker(size_t workerID) {
  while (true) {
    sWorkers[workerID].wait();
    if (exit) return;
    wThunks[workerID]();
    lock_guard<mutex> lgWorkerStatus(mWorkerStatus);
    wBusy[workerID] = false;
    mWorkerStatus.unlock();
    cvWorkerStatus.notify_all();
  }
}

ThreadPool::ThreadPool(size_t numThreads) : wts(numThreads), exit(false),
  wBusy(numThreads, false), wThunks(numThreads), sWorkers(numThreads) {
  dt = thread([this]() { dispatcher(); });
  for (size_t workerID = 0; workerID < numThreads; workerID++) {
    wts[workerID] = thread([this](size_t workerID) {
      worker(workerID);
    }, workerID);
  }
}

void ThreadPool::schedule(const function<void(void)>& thunk) {
  lock_guard<mutex> lgThunkQueue(mThunkQueue);
  thunkQueue.push(thunk);
  mThunkQueue.unlock();
  cvThunkQueue.notify_all();
}

void ThreadPool::wait() {
  lock_guard<mutex> lgThunkQueue(mThunkQueue);
  cvThunkQueue.wait(mThunkQueue, [this]{ return thunkQueue.empty(); });
  mThunkQueue.unlock();
  lock_guard<mutex> lgWorkerStatus(mWorkerStatus);
  cvWorkerStatus.wait(mWorkerStatus, [this]{
    return find(wBusy.begin(), wBusy.end(), true) == wBusy.end();
  });
  mWorkerStatus.unlock();
}

ThreadPool::~ThreadPool() {
  wait();
  exit = true;
  cvThunkQueue.notify_all();
  cvWorkerStatus.notify_all();
  for (semaphore& s : sWorkers) s.signal();
  dt.join();
  for (thread& t : wts) t.join();
}

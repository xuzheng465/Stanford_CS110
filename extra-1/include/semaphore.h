/**
 * Implements a semaphore class, which isn't part of the
 * C++11 specification, but is included in Java and in posix
 * threads, and is a common enough abstraction that we should
 * provide one.
 */

#ifndef _semaphore_
#define _semaphore_

#include <mutex>
#include <condition_variable>

/**
 * The on_thread_exit_t and the on_thread_exit constant
 * are used to differentiate between an immediate
 * signal and one that should be scheduled for when the
 * surrounding thread routine has completed and the thread
 * itself is being destroyed.
 */

struct on_thread_exit_t {};
constexpr on_thread_exit_t on_thread_exit {};

class semaphore {
 public:
  semaphore(int value = 0);
  void wait();
  void signal();
  void signal(on_thread_exit_t ote);
  
 private:
  int value;

  std::mutex m;
  std::condition_variable_any cv;

  semaphore(const semaphore& orig) = delete;
  const semaphore& operator=(const semaphore& rhs) const = delete;
};

#endif

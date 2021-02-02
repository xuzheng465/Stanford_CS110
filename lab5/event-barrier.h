/**
 * File: event-barrier.h
 * ---------------------
 * Defines the EventBarrier class as documented below.
 */

#pragma once
#include <mutex>
#include <condition_variable>

class EventBarrier {
 public:
  
  /**
   * Constructs: EventBarrier
   * ------------------------
   * Constructs an event barrier in place to block zero or more threads until the barrier
   * if lifted.
   * 
   * When that barrier is lifted, each of the waiting threads notifies the lifting thread that they've
   * advanced beyond the barrier.
   */
  EventBarrier();

  /**
   * Method: wait
   * ------------
   * Causes the calling thread to wait until the barrier it lifted.  If the barrier is
   * currently up, then wait returns immediately without blocking.
   */
  void wait();
  
  /**
   * Method: lift
   * ------------
   * Lifts the barrier and notifies all previously blocked thread that they can continue.  The thread
   * calling lift is then itself blocked until all waiters have advanced past the barrier.
   */
  void lift();

  /**
   * Method: past
   * ------------
   * Called by a thread previously blocked on a barrier (that has since been lifted) to inform
   * the lifting thread that they've moved past the the barrier (and that imposing the same barrier
   * again won't block it).
   */
  void past();

 private:
  // add your own data members right here.
  
  // in place to prevent EventBarrier objects from being passed by value
  EventBarrier(const EventBarrier& orig) = delete;
  const EventBarrier& operator=(const EventBarrier& rhs) = delete;
};

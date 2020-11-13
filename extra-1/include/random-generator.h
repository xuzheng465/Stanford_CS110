/**
 * Presents a random number generator that can be used
 * to uniformly generate random numbers from a provided inteval
 * and to simulate fair or even biased coin flips.
 *
 * This library is not-thread safe.
 */

#ifndef _random_generator_
#define _random_generator_

#include <random>

class RandomGenerator {
 public:
  int getNextInt(int low, int high);
  double getNextReal(double low, double high);
  bool getNextBool(double chance = 0.5);
  
 private:
  static std::random_device rd;
  static std::mt19937 gen;
};

#endif

/**
 * Presents the implementation of the RandomGenerator class.
 */

#include "random-generator.h"
#include <random>
#include <exception>
using namespace std;

random_device RandomGenerator::rd;
mt19937 RandomGenerator::gen(RandomGenerator::rd());

int RandomGenerator::getNextInt(int low, int high) {
  if (high < low) 
    throw 
      "RandomGenerator::getNextInt: first argument cannot "
      "be greater than the second.";
  uniform_int_distribution<> dis(low, high);
  return dis(gen);
}

double RandomGenerator::getNextReal(double low, double high) {
  if (high <= low)
    throw 
      "RandomGenerator::getNextReal: first argument "
      "must be strictly less than the second.";
  uniform_real_distribution<> dis(low, high);
  return dis(gen);
}

bool RandomGenerator::getNextBool(double chance) {
  if (chance < 0 || chance > 1)
    throw "RandomGenerator::getNextBool: argument must be in [0.0, 1.0]";
  return chance == 1 || getNextReal(0, 1) < chance;
}

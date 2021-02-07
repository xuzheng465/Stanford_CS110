#pragma once
#include <vector>
#include <string>
#include <iostream>

const std::string kIMDBDataDirectory("/usr/class/cs110/samples/assign1/");

/**
 * Convenience struct: film
 * ------------------------
 * Bundles the name of a film and the year it was made
 * into a single struct.  It is a true struct in that the 
 * client is free to access both fields and change them at will
 * without issue.  operator== and operator< are implemented
 * so that films can be stored in STL containers requiring 
 * such methods.
 */
struct film {
  
  std::string title;
  int year;
  
  /** 
   * Methods: operator==
   *          operator<
   * -------------------
   * Compares the two films for equality, where films are considered to be equal
   * if and only if they share the same name and appeared in the same year.  
   * film is considered to be less than another film if its title is 
   * lexicographically less than the second title, or if their titles are the same 
   * but the first's year is precedes the second's.
   *
   * @param rhs the film to which the receiving film is being compared.
   * @return boolean value which is true if and only if the required constraints
   *         between receiving object and argument are met.
   */

  bool operator==(const film& rhs) const { 
    return this->title == rhs.title && (this->year == rhs.year); 
  }
  
  bool operator<(const film& rhs) const { 
    return 
      (this->title < rhs.title) || 
      (this->title == rhs.title && this->year < rhs.year);
  }
};


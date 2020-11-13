/**
 * File: aggregate.cc
 * ------------------
 * Defines the entry point to the aggregate executable.
 * Note that the main function essentially passes the buck
 * to whatever instance is returned by the createNewsAggregator
 * factory method.  That instance is asked to build the 
 * index and then allow the user to search it.
 *
 * For the purposes of Assignment 5, the only type that can be
 * returned from NewsAggregator::createNewsAggregator(argc, argv)
 * is a NewsAggregator *.  For Assignment 6, when you'll revisit
 * this assignment, the function could very well return a pointer
 * to a subclass instance instead, and that subclass might have 
 * different versions of buildIndex and queryIndex.
 */

#include "news-aggregator.h"
#include <memory> // for unique_ptr
using std::unique_ptr;

int main(int argc, char *argv[]) {
  unique_ptr<NewsAggregator> aggregator(NewsAggregator::createNewsAggregator(argc, argv));
  aggregator->buildIndex();
  aggregator->queryIndex();
  return 0;
}

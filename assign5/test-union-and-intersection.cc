/**
 * File: test-union-and-intersection.cc
 * --------------------------
 * Small utility program to demonstrate how the STL set_union and set_intersection
 * functions work.  To fully understand this program, you'll need to read up on
 * sort, is_sorted, set_union, set_intersection, and back_inserter, all of which are
 * well described at http://en.cppreference.com (search for sort, is_sorted, etc.).
 *
 * You won't need to use set_union for your aggregate program, but you'll definitely
 * need to use all of the others.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
using namespace std;

static void printList(const string& header, const vector<string>& words) {
  cout << header << ":";
  if (words.empty())
    cout << " [nothing to print]";
  else
    for (const string& word: words)
      cout << " " << word;
  cout << endl;
}

static const char *kVocabListOne[] = {"indefatigable", "pulchritude", "ablution"};
static const char *kVocabListTwo[] = {"detritus", "susurration", "halcyon", "pulchritude"};
static const char *kVocabListThree[] = {"retrenchment", "tumultuous", "indefatigable", "sycophant", "abyss"};
int main(int argc, char *argv[]) {
  vector<string> one(kVocabListOne, kVocabListOne + 3);
  assert(!is_sorted(one.cbegin(), one.cend()));
  sort(one.begin(), one.end());
  assert(is_sorted(one.cbegin(), one.cend()));
  const vector<string>& largeList = one;
  printList("Words In First List", largeList);

  vector<string> two(kVocabListTwo, kVocabListTwo + 4);
  assert(!is_sorted(two.cbegin(), two.cend()));
  sort(two.begin(), two.end());
  assert(is_sorted(two.cbegin(), two.cend()));
  vector<string> largerList;
  set_union(largeList.cbegin(), largeList.cend(), two.cbegin(), two.cend(), back_inserter(largerList));
  printList("Words In Either", largerList);
  vector<string> smallerList;
  set_intersection(largeList.cbegin(), largeList.cend(), two.cbegin(), two.cend(), back_inserter(smallerList));
  printList("Words In Both", smallerList);

  vector<string> three(kVocabListThree, kVocabListThree + 5);
  assert(!is_sorted(three.cbegin(), three.cend()));
  sort(three.begin(), three.end());
  assert(is_sorted(three.cbegin(), three.cend()));
  vector<string> largestList;
  set_union(largerList.cbegin(), largerList.cend(), three.cbegin(), three.cend(), back_inserter(largestList));
  printList("Words In Any", largestList);
  vector<string> smallestList;
  set_intersection(smallerList.cbegin(), smallerList.cend(), three.cbegin(), three.cend(), back_inserter(smallestList));
  printList("Words In All Three", smallestList);

  return 0;
}

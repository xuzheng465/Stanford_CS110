/**
 * File: rss-index.cc
 * ------------------
 * Presents the implementation of the RSSIndex class, which is
 * little more than a glorified map.
 */

#include "rss-index.h"

#include <algorithm>

using namespace std;

void RSSIndex::add(const Article& article, const vector<string>& words) {
  for (const string& word : words) { // iteration via for keyword, yay C++11
    index[word][article]++;
  }
}

static const vector<pair<Article, int> > emptyResult;
vector<pair<Article, int> > RSSIndex::getMatchingArticles(const string& word) const {
  auto indexFound = index.find(word);
  if (indexFound == index.end()) return emptyResult;
  const map<Article, int>& matches = indexFound->second;
  vector<pair<Article, int> > v;
  for (const pair<Article, int>& match: matches) v.push_back(match);
  sort(v.begin(), v.end(), [](const pair<Article, int>& one, 
                              const pair<Article, int>& two) {
   return one.second > two.second || (one.second == two.second && one.first < two.first);
  });
  return v;
}

/**
 * File: rss-index.h
 * -----------------
 * Exports an RSSIndex type, which is a data structure that maps
 * words to vectors of document/frequency pairs (where the document frequency 
 * pairs are represented as pair<Article, int>s).
 */

#pragma once
#include <map>
#include <vector>
#include "article.h"

class RSSIndex {
 public:
/**
 * Zero-argument constructor, constructs an empty index.
 */
  RSSIndex() {}

/**
 * Notes that each of the words in the supplied vector appears within the
 * specified article.  The add operation is not thread-safe, so care must be taken
 * to externally lock the RSSIndex down if two racing threads might try to
 * add to the RSSIndex at the same time.
 */
  void add(const Article& article, const std::vector<std::string>& words);

/**
 * Returns a reference to the list of documents associated with the specified
 * word.  The list is a vector of URL/frequency pairs, sorted by frequency from
 * high to low (and alphabetically for those with the same frequence counts.)
 */
  std::vector<std::pair<Article, int> > getMatchingArticles(const std::string& word) const;
  
 private:
  std::map<std::string, std::map<Article, int> > index;

/**
 * RSSIndex instances can theoretically store a huge amount of data, so we
 * elect to delete the compiler-supplied implementations of the copy constructor
 * and operator= so that clients are forced to pass instances around by reference
 * or by address.
 */
  RSSIndex(const RSSIndex& other) = delete;
  void operator=(const RSSIndex& rhs) = delete;
};

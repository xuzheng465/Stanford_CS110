/**
 * File: rss-feed.h
 * ----------------
 * Defines the interface for the RSSFeed class, which given a URL
 * to an RSS feed document, pulls and parses the remote document to
 * produces a sequence of Articles (see article.h).
 */

#pragma once
#include <string>
#include <vector>
#include "article.h"
#include "rss-feed-exception.h"

class RSSFeed {
 public:

/**
 * Constructor: RSSFeed
 * Usage: RSSFeed feed("http://feeds.washingtonpost.com/news/world.rss");
 * ----------------------------------------------------------------------
 * Constructs an RSSFeed object around the provided URL.
 */
  RSSFeed(const std::string& url) : url(url) {}

/**
 * Method: parse
 * Usage: feed.parse();
 * --------------------
 * Pulls the RSS news feed from the encapsulated URL and processed it
 * so that getArticles can return a vector of Articles in constant time.
 *
 * If any problems are encountered, and RSSFeedException is thrown.
 */
  void parse() throw (RSSFeedException);

/**
 * Method: getArticles
 * Usage: const vector<Article>& articles = feed.getArticles();
 * ------------------------------------------------------------
 * Returns a const reference to the sequence of Articles embedded
 * within the already-parsed-and-processed RSSFeed object.
 */
  const std::vector<Article>& getArticles() const { return articles; }
  
 private:
  std::string url;
  std::vector<Article> articles;

  /**
   * The following two lines delete the default implementations you'd
   * otherwise get for the copy constructor and operator=.  Because the implementation
   * of parse() involved networking code, it's not clear what the semantics of a
   * deep copy really should be.  By deleting these two items, we force all clients
   * of the RSSFeed class to pass instances around by reference or by addresses.
   */
  RSSFeed(const RSSFeed& other) = delete;
  void operator=(const RSSFeed& rhs) = delete;
};

/**
 * File: news-aggregator.h
 * -----------------------
 * Defines the NewsAggregator class.  As opposed to your Assignment 5
 * version, this one relies on the services of two ThreadPools to
 * limit and *recycle* a small number of threads.
 */

#pragma once
#include <string>
#include "log.h"
#include "rss-index.h"
#include "thread-pool.h"

class NewsAggregator {
  
 public:
/**
 * Factory Method: createNewsAggregator
 * ------------------------------------
 * Static factory method that parses the command line
 * arguments to decide what RSS feed list should be downloaded
 * and parsed for its RSS feeds, which are themselves parsed for
 * their news articles, all in the pursuit of compiling one big, bad index.
 */
  static NewsAggregator *createNewsAggregator(int argc, char *argv[]);

/**
 * Method: buildIndex
 * ------------------
 * Pulls the embedded RSSFeedList, parses it, parses the
 * RSSFeeds, and finally parses the HTMLDocuments they
 * reference to actually build the index.
 */
  void buildIndex();

/**
 * Method: queryIndex
 * ------------------
 * Provides the read-query-print loop that allows the user to
 * query the index to list articles.
 */
  void queryIndex() const;
  
 private:
/**
 * Private Types: url, server, title
 * ---------------------------------
 * All synonyms for strings, but useful so
 * that something like pair<string, string> can
 * instead be declared as a pair<server, title>
 * so it's clear that each string is being used
 * to store.
 */
  typedef std::string url;
  typedef std::string server;
  typedef std::string title;
  
  NewsAggregatorLog log;
  std::string rssFeedListURI;
  RSSIndex index;
  bool built;
  
/**
 * Constructor: NewsAggregator
 * ---------------------------
 * Private constructor used exclusively by the createNewsAggregator function
 * (and no one else) to construct a NewsAggregator around the supplied URI.
 */
  NewsAggregator(const std::string& rssFeedListURI, bool verbose);

/**
 * Method: processAllFeeds
 * -----------------------
 * Downloads all of the feeds and news articles to build the index.
 * You need to implement this function using two ThreadPools instead
 * of an unbounded number of threads.
 */
  void processAllFeeds();

/**
 * Copy Constructor, Assignment Operator
 * -------------------------------------
 * Explicitly deleted so that one can only pass NewsAggregator objects
 * around by reference.  These two deletions are often in place to
 * forbid large objects from being copied.
 */
  NewsAggregator(const NewsAggregator& original) = delete;
  NewsAggregator& operator=(const NewsAggregator& rhs) = delete;
};

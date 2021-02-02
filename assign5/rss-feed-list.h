/**
 * File: rss-feed-list.h
 * ---------------------
 * Simple module designed to pull a custom XML file that collates
 * an unlimited number of RSS Feed Name/XML URL pairs.  The implementation
 * assumed that xmlInitParse has already been called.  It also assumes
 * the the RSS feed list is itself structured as an RSS feed, so that the
 * high-level structure of the remote XML document should be consistent with
 * this snippet.
 *
 * <?xml version="1.0" encoding="iso-8859-1" ?>
 * <rss version="2.0">
 *   <channel>
 *     <item>
 *       <title>SFGate: Page One Stories</title>
 *       <link>http://feeds.sfgate.com/sfgate/rss/feeds/news_pageone</link>
 *     </item>
 *     <item>
 *       <title>World: World News, International News, Foreign Reporting - The Washington Post</title>
 *       <link>http://feeds.washingtonpost.com/rss/world</link>
 *     </item>
 *   </channel>
 * </rss>
 */

#pragma once
#include <string>
#include <map>
#include "rss-feed-list-exception.h"

class RSSFeedList {
 public:

/**
 * Constructor: RSSFeedList
 * Usage: RSSFeedList rssFeedList("large-feed.xml");
 * -------------------------------------------------
 * Constructs an RSSFeedList around the provided URL (which may be
 * a local file).
 */
  RSSFeedList(const std::string& url) : url(url) {}

/**
 * Method: parse
 * Usage: list.parse();
 * --------------------
 * Pulls the content from the encapsulated URL so that getFeeds
 * is outfitted to immediately return a map of feed titles and
 * links (where the keys are the links, and the values are the titles).
 *
 * If any problems are encountered, an RSSFeedListException is thrown.
 */
  void parse() throw (RSSFeedListException);

/**
 * Method: getFeeds
 * Usage: const auto& feeds = list.getFeeds();
 * ------------------------------------
 * Returns an immutable reference to the map of url-title associations.
 * URLs are string like "http://feeds.washingtonpost.com/rss/world" and 
 * titles are strings like "World News: The Washington Post"
 */
  const std::map<std::string, std::string>& getFeeds() const { return feeds; }
  
 private:
  std::string url;
  std::map<std::string, std::string> feeds;

/**
 * The following two lines delete the default implementations you'd
 * otherwise get for the copy constructor and operator=.  Because the implementation
 * of parse() involves networking code, it's not clear what the semantics of a
 * deep copy really should be.  By deleting these two items, we force all clients
 * of the RSSFeedList class to pass instances around by reference or by address.
 */
  RSSFeedList(const RSSFeedList& other) = delete;
  void operator=(const RSSFeedList& rhs) = delete;
};

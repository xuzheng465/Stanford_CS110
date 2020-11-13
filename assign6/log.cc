/**
 * File: log.cc
 * ------------
 * Presents the (fairly straightforward) implementation of all of the 
 * NewsAggregatorLog methods so that everyone can agree on what text should be
 * used to communicate information, warnings, and errors at runtime.
 */

#include "log.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
#include "ostreamlock.h"
using namespace std;

static const int kIncorrectUsage = 1;
void NewsAggregatorLog::printUsage(const string& message, const string& executable) {
  cerr << "Error: " << message << endl;
  cerr << "Usage: ./" << executable << " [--verbose] [--quiet] [--conserve-threads] [--url <feed-file>]" << endl;
  exit(kIncorrectUsage);
}

static const int kBogusRSSFeedListName = 1;
void NewsAggregatorLog::noteFullRSSFeedListDownloadFailureAndExit(const string& rssFeedListURI) const {
  cerr << "Ran into trouble while pulling full RSS feed list from \""
       << rssFeedListURI << "\"." << endl; 
  cerr << "Aborting...." << endl;
  exit(kBogusRSSFeedListName);
}

void NewsAggregatorLog::noteFullRSSFeedListDownloadEnd() const {
  if (verbose) cout << oslock << "All RSS news feed documents have been downloaded!" << endl << osunlock;
}

void NewsAggregatorLog::noteSingleFeedDownloadBeginning(const string& feedURI) const {
  if (!verbose) return;
  cout << oslock << "Begin full download of feed URI: " << feedURI << endl << osunlock;
}

void NewsAggregatorLog::noteSingleFeedDownloadSkipped(const string& feedURI) const {
  if (!verbose) return;
  cout << oslock << "Skipped entire download of feed URI: " << feedURI << endl << osunlock;
}

void NewsAggregatorLog::noteSingleFeedDownloadEnd(const string& feedURI) const {
  if (!verbose) return;
  cout << oslock << "End full download of feed URI: " << feedURI << endl << osunlock;
}

void NewsAggregatorLog::noteSingleFeedDownloadFailure(const string& feedURI) const {
  cerr << oslock << "Ran into trouble while pulling RSS feed from \""
       << feedURI << "\"." << endl << "Ignoring...." << endl << osunlock;
}

void NewsAggregatorLog::noteAllRSSFeedsDownloadEnd() const {
  if (verbose) cout << oslock << "All news articles have been downloaded!" << endl << osunlock;  
}

void NewsAggregatorLog::noteSingleArticleDownloadBeginning(const Article& article) const {
  if (!verbose) return;
  string title = shouldTruncate(article.title) ? truncate(article.title) : article.title;
  string url = shouldTruncate(article.url) ? truncate(article.url) : article.url;
  cout << oslock;
  cout << "  Parsing \"" << title << "\"" << endl;
  cout << "      [at \"" << url << "\"]" << endl;
  cout << osunlock;
}

void NewsAggregatorLog::noteSingleArticleDownloadSkipped(const Article& article) const {
  if (!verbose) return;
  string title = shouldTruncate(article.title) ? truncate(article.title) : article.title;
  string url = shouldTruncate(article.url) ? truncate(article.url) : article.url;
  cout << oslock;
  cout << "  Skipped \"" << title << "\"" << endl;
  cout << "      [at \"" << url << "\"]" << endl;
  cout << osunlock;
}

void NewsAggregatorLog::noteSingleArticleDownloadFailure(const Article& article) const {
  cerr << oslock;
  cerr << "Ran into trouble while pulling HTML document from \"" << article.url << "\" Ignoring...." << endl;
  cerr << osunlock;
}

void NewsAggregatorLog::noteAllArticlesHaveBeenScheduled(const string& feedTitle) const {
  if (!verbose) return;
  cout << oslock << feedTitle << ": All articles have been scheduled." << endl << osunlock;
}

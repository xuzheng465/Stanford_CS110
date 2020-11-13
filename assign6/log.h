/**
 * File: log.h
 * -----------
 * Exports a class that's dedicated to printing out structured info, warning, and error messages.
 */

#pragma once
#include <string>
#include "article.h"

class NewsAggregatorLog {
 public:
  NewsAggregatorLog(bool verbose): verbose(verbose) {} 
  
  static void printUsage(const std::string& message, const std::string& executableName);
  
  void noteFullRSSFeedListDownloadFailureAndExit(const std::string& rssFeedListURI) const;
  void noteFullRSSFeedListDownloadEnd() const;
  
  void noteSingleFeedDownloadBeginning(const std::string& feedURI) const;
  void noteSingleFeedDownloadSkipped(const std::string& feedURI) const;
  void noteSingleFeedDownloadFailure(const std::string& feedURI) const;
  void noteSingleFeedDownloadEnd(const std::string& feedURI) const;
  void noteAllRSSFeedsDownloadEnd() const;
  
  void noteSingleArticleDownloadBeginning(const Article& article) const;
  void noteSingleArticleDownloadSkipped(const Article& article) const;
  void noteSingleArticleDownloadFailure(const Article& article) const;
  void noteAllArticlesHaveBeenScheduled(const std::string& feedTitle) const;
  
 private:
  bool verbose;
  NewsAggregatorLog(const NewsAggregatorLog& original) = delete;
  NewsAggregatorLog& operator=(const NewsAggregatorLog& rhs) = delete;
};

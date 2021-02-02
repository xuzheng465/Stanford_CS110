/**
 * File: article.h
 * ---------------
 * Presents a simple exposed Article type containing the title
 * and URL of a news article.  operator< is overloaded so that
 * Articles can easily key stored in STL sets and as the keys of
 * maps.
 */

#pragma once
#include <string>

/**
 * Type: Article
 * -------------
 * Defines a simple aggregated type bundling the two most
 * important components of a news article: its URL and its title.
 */
struct Article {
  std::string url;
  std::string title;
};

/**
 * Function: operator<
 * -------------------
 * Overloads infix < so that Articles can be stored in sets
 * and as the keys in maps.  It's reasonable to expect that
 * two articles are the same if their URLs are the same.
 */
inline bool operator<(const Article& one, const Article& two) {
  return one.url < two.url;
}

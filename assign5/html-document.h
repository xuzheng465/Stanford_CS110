/**
 * File: html-document.h
 * ---------------------
 * Encapsulates the information needed to represent
 * a single HTML document.  While it could be simpilified
 * quite a bit, it's optimized to pull and parse the content
 * of an HTML document's body tag.
 */

#pragma once
#include <string>
#include <vector>
#include "html-document-exception.h"

class HTMLDocument {
 public:

/**
 * Constructor: HTMLDocument
 * Usage: HTMLDocument document("http://www.facebook.com/jerry");
 * -------------------------
 * Constructs an HTMLDocument instance around the specified URL.
 */
  HTMLDocument(const std::string& url) : url(url) {}

/**
 * Method: parse
 * Usage: htmlDoc.parse();
 * -----------------------
 * Connects to the relevant server housing the document at the encapsulated
 * URL, pulls the document content, and tokenizes it so that getTokens() works
 * as expected.
 *
 * If any problems are encountered, an HTMLDocumentException is thrown.
 */
  void parse() throw (HTMLDocumentException);

/**
 * Method: getURL
 * cout << htmlDoc.getURL() << endl;
 * ---------------------------------
 * Returns a const reference to the encapsulated URL (expressed as a C++ string).
 */
  const std::string& getURL() const { return url; }

/**
 * Method: getTokens
 * const vector<string>& tokens = htmlDoc.getTokens();
 * ---------------------------------------------------
 * Returns a const reference to the encapsulated vector of tokens making
 * up the content of the document.
 */
  const std::vector<std::string>& getTokens() const { return tokens; }
  
 private:
  std::string url;
  std::vector<std::string> tokens;

/**
 * The following two lines delete the default implementations you'd
 * otherwise get for the copy constructor and operator=.  Because the implementation
 * of parse() involves networking code, it's not clear what the semantics of a
 * deep copy really should be.  By deleting these two items, we force all clients
 * of the HTMLDocument class to pass instances around by reference or by address.
 */
  HTMLDocument(const HTMLDocument& other) = delete;
  void operator=(const HTMLDocument& rhs) = delete;
};

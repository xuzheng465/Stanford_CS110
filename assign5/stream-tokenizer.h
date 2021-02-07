/**
 * File: stream-tokenizer.h
 * ------------------------
 * Provides a C++ equivalent to Java's StreamTokenizer, which allows
 * the client to tokenize a collection of characters according to the 
 * set of delimiters as specified at construction time.
 */

#pragma once
#include <istream>
#include <string>

class StreamTokenizer {
 public:
/**
 * Constructor: StreamTokenizer
 * ----------------------------
 * Constructs a StreamTokenizer to pull character
 * content through the provided istream, using the
 * provided string to represent the character delimiter
 * set.  By default, the delimiters are completely ignored,
 * but if skipDelimiters is set to false, then delimiter
 * characters are returned as single character strings.
 */
  StreamTokenizer(std::istream& is, 
                  const std::string& delimiters, 
                  bool skipDelimiters = true);  

/**
 * Function: hasMoreTokens
 * -----------------------
 * Returns true if and only if the StreamTokenizer
 * has at least one more token to be returned via
 * nextToken.
 */
  bool hasMoreTokens() const;

/**
 * Function: nextToken
 * -------------------
 * Returns the next token in the sequence of tokens to be
 * returned, or the empty string if there are no more tokens.
 */
  std::string nextToken();
  
 private:
  std::istream& is;
  std::string delimiters;
  mutable std::string savedChar;
  bool skipDelimiters;
  
  std::string getNextXMLChar() const;

/**
 * The following two lines delete the default implementations you'd
 * otherwise get for the copy constructor and operator=.  Because the implementation
 * involves std::istreams, it's not clear what the semantics of a
 * deep copy really should be.  By deleting these two items, we force all clients
 * of the StreamTokenizer class to pass instances around by reference or by address.
 */
  StreamTokenizer(const StreamTokenizer& orig) = delete;
  void operator=(const StreamTokenizer& other) = delete;
};

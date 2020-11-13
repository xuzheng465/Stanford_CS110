#pragma once
#include "imdb-utils.h"
#include <string>
#include <vector>

class imdb {
 public:
  
/**
 * Constructor: imdb
 * -----------------
 * Constructs an imdb instance to layer on top of raw memory representations
 * stored in the specified directory.  The understanding is that the specified
 * directory contains binary files carefully formatted to compactly store
 * all of the information about the movies and actors relevant to an IMDB
 * application (like six-degrees).
 *
 * @param directory the name of the directory housing the formatted information backing the imdb.
 */

  imdb(const std::string& directory);

/**
 * Predicate Method: good
 * ----------------------
 * Returns true if and only if the imdb opened without indicident.
 * imdb::good would typically return false if:
 *
 *     1.) either one or both of the data files supporting the imdb were missing
 *     2.) the directory passed to the constructor doesn't exist.
 *     3.) the directory and files all exist, but you don't have the permission to read them.
 */

  bool good() const;

/**
 * Method: getCredits
 * ------------------
 * Searches for an actor/actress's list of movie credits.  The list 
 * of credits is returned via the second argument, which you'll note
 * is a non-const vector<film> reference.  If the specified actor/actress
 * isn't in the database, then the films vector will be left empty.
 *
 * @param player the name of the actor or actresses being queried.
 * @param films a reference to the vector of films that should be updated
 *              with the list of the specified actor/actress's credits.
 * @return true if and only if the specified actor/actress appeared in the
 *              database, and false otherwise.
 */
  
  bool getCredits(const std::string& player, std::vector<film>& films) const;

/**
 * Method: getCast
 * ---------------
 * Searches the receiving imdb for the specified film and returns the cast
 * by populating the specified vector<string> with the list of actors and actresses
 * who star in it.  If the movie doesn't exist in the database, the players vector
 * is cleared and its size left at 0.
 *
 * @param movie the film (title and year) being queried
 * @param players a reference to the vector of strings to be updated with the
 *                the list of actors and actresses starring in the specified film.
 *                If the movie doesn't exist, then the players vector would be cleared
 *                of all contents and resized to be of length 0.
 * @return true if and only if the specified movie appeared in the
 *              database, and false otherwise.
 */

  bool getCast(const film& movie, std::vector<std::string>& players) const;

/**
 * Destructor: ~imdb
 * -----------------
 * Releases any resources associated with the imdb.
 * Self-explantory.
 */
  ~imdb();
  
 private:
  static const char *const kActorFileName;
  static const char *const kMovieFileName;
  const void *actorFile;
  const void *movieFile;
  
  // everything below here is complicated and needn't be touched.
  // you're free to investigate, but you're on your own.
  struct fileInfo {
    int fd;
    size_t fileSize;
    const void *fileMap;
  } actorInfo, movieInfo;
  
  static const void *acquireFileMap(const std::string& fileName, struct fileInfo& info);
  static void releaseFileMap(struct fileInfo& info);

  imdb(const imdb& original) = delete;
  imdb& operator=(const imdb& rhs) = delete;
  imdb& operator=(const imdb& rhs) const = delete;
};

#pragma once
#include "imdb-utils.h"
#include <string>
#include <vector>
#include <iostream>

/**
 * Convenience Class: path 
 * -----------------------
 * The path class is a container class designed to
 * to store a sequence of actors/actresses and movies.
 * It is by no means intelligent, as it'll allow any
 * sequence of movies and players and provide none
 * of the consistency checks one might want.  You're
 * free to change this code to include those consistency
 * checks, or you may leave it alone and use it as is.
 */

class path {
  
/**
 * Overloaded operator<< function
 * ------------------------------
 * Function designed to format and publish an instance of
 * a path class.  Because the friend keyword is used, operator<<
 * isn't a method, but instead a global function marked as a 'friend' 
 * of the path.  While functions and methods normally respect
 * the privacy of a class, interacting with it via its public
 * interface, friends of a class are privileged and *can* directly
 * access the private portions.  Friendship shouldn't be granted 
 * all that often--typically, it's granted only to functions 
 * that are being implemented on behalf of the class.  operator<<
 * is one of those rare functions.
 */
  friend std::ostream& operator<<(std::ostream& os, const path& p);
  
 public:
  /**
   * Constructor: path
   * -----------------
   * Initializes a path to contain the specified player
   * and no one else.  The path grows because the client
   * appends movie/actor pairs, and the path shrinks when
   * the client calls undoConnection.
   */
  path(const std::string& startPlayer);

  /**
   * Method: getLength
   * -----------------
   * Replies with the number of movies in place to
   * form the path from the first player to the last.
   *
   * @return the number of movies currently making up the
   *         path.
   */  
  size_t getLength() const { return links.size(); };

  /**
   * Method: addConnection
   * ---------------------
   * Blindly adds the specified movie and actor to the
   * path.  No integrity checking is done, so it's the
   * responsibility of the client to make sure that the
   * specified player really did appear in the movie, and it's
   * also the client's responsibility to ensure that the last
   * player prior to the addConnection message was also in the
   * movie.  In theory, there is no limit to the number of 
   * of movie-player connections that can be added.
   * 
   * The implementation makes a deep copy of the specified movie and
   * actor, so you needn't worry about the memory management issues that
   * come up here.
   *
   * @param movie a reference to the film record starring both the specified
   *              player and the last player in the path.
   * @param player a reference to the actor/actress appearing in the specified film.
   */ 
  void addConnection(const film& movie, const std::string& player);

  /**
   * Method: undoConnection
   * ----------------------
   * Pulls the last movie-player connection off the path, unless
   * the length of the path (as defined by our getLength() method) is 0,
   * in which case nothing happens.
   */  
  void undoConnection();
  
  /**
   * Method: getLastPlayer
   * ---------------------
   * Returns the address of the last player in the
   * path.  Self-explanatory.
   *
   * @return the address of a string storing the name of the
   *         last player.
   */  
  const std::string& getLastPlayer() const;

  /**
   * Method: reverse
   * ---------------
   * Reverses the receiving path.
   */
  void reverse();
  
 private:
  // private struct definition... no one else uses it, so I define it internally
  // if you think about it, the existence of this struct is really an implementation detail,
  // so its very definition should be private, right?

  struct connection {
    film movie;
    std::string player;
    
    // convenience struct with constructors.. 
    connection() {}
    connection(const film& movie, const std::string& player) : movie(movie), player(player) {}
  };
  
  std::string startPlayer;
  std::vector<connection> links;
};

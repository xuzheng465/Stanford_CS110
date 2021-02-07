/**
 * Provides a locking mechanism that allows us to lock down
 * access to cout, cerr, and other stream references so that
 * the full accumulation of daisy-chained data is inserted into
 * an ostream as one big insertion.
 *
 *   Thread safe: cout << oslock << 1 << 2 << 3 << 4 << endl << osunlock;
 *   Not thread safe: cout << 1 << 2 << 3 << 4 << endl;
 */

#include <ostream>

std::ostream& oslock(std::ostream& os);
std::ostream& osunlock(std::ostream& os);

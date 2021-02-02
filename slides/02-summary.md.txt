# CS110: API Programming Against the UNIX Filesystem
* Software layered over hardware, filesystem API calls
    * First off, we'll take a first pass at understanding how the physical hardware of a disk drive can be made to
      look like software to store traditional files.  I'll leave some details out, but will provide enough detail to
      be clear how regular files of wildly different sizes can be stored on disk and retreived via the sessions with those
      files managed by data types like <tt><b>FILE *</b></tt>, <tt><b>ifstream</b></tt>, and <tt><b>ofstream</b></tt>.
    * We'll learn how programmers can interact (either directly, or indirectly though the <tt><b>FILE *</b></tt> and 
      <tt><b>[io]stream</b></tt> implementations) with the file system via <b>system calls</b>, which are a collection of kernel-resident
      functions that user programs must go through in order to access and manipulate system resources.
      Requests to open a file, read from a file, extend the heap, etc, all eventually go through
      system calls, which are the only functions that can be trusted to touch the system.
    * Today's lecture examples reside in <b><tt>/usr/class/cs110/lecture-examples/spring-2017/filesystems</b></tt>.
    * The <b><tt>/usr/class/cs110/lecture-examples/spring-2017</tt></b> directory is a
      mercurial repository that will be updated with additional examples as
      the quarter progresses.
        * To get started, type <b><tt>hg clone /usr/class/cs110/lecture-examples/autumn-2017 cs110-lecture-examples</tt></b>
          at the command prompt to create a local copy of the master.
        * Each time I mention there are new examples, navigate into your local copy and type
          <b><tt>hg pull &amp;&amp; hg update</tt></b>.
    * More importantly, read [Sections 1 through 5](http://www.sciencedirect.com/science/article/pii/B9780123749574000116)
      of the Saltzer &amp; Kaashoek online textbook, paying special attention to the details in Section 5, which will
      help you with your first assignment (which goes out on Friday).

# Filesystem API: Implementing <b><tt>copy</tt></b> to emulate <b><tt>cp</tt></b>
* Implementation of <b><tt>copy</tt></b>
    * The implementation of <b><tt>copy</tt></b> (designed to mimic the behavior of 
      <b><tt>cp</tt></b>) illustrates how to use <b><tt>open</tt></b>, <b><tt>read</tt></b>, 
      <b><tt>write</tt></b>, <b><tt>close</tt></b>, <b><tt>stat</tt></b>.  It also introduces the
      notion of a file descriptor.
    * <b><tt>man</tt></b> pages exist for all of these functions 
      (e.g. <b><tt>man 2 open</tt></b>, <b><tt>man 2 read</tt></b>, etc.)
    * Full implementation of our own <tt><b>copy</b></tt> executable is 
      [right here](http://cs110.stanford.edu/autumn-2017/examples/filesystems/copy.c).

* Pros and cons of file desciptors over <tt><b>FILE</b></tt> pointers and C++ <tt><b>iostream</b></tt>s
    * The file descriptor abstraction provides direct, low level access to a stream of data without the fuss
      of data structures or objects.  It certainly can't be slower, and depending on what you're doing,
      it may even be faster.
    * <tt><b>FILE</b></tt> pointers and C++ <tt><b>iostream</b></tt>s work well when you know
      you're layering over standard output, standard input, and local files.  They are less useful 
      when the stream of bytes is associated with a network connection. (<tt><b>FILE</b></tt> 
      pointers and C++ <tt><b>iostream</b></tt>s assume they can rewind and move the file pointer
      back and forth freely, but that's not the case with file descriptors associated with network
      connections).
    * File descriptors, however, work with <tt><b>read</b></tt> and <tt><b>write</b></tt> and
      nothing else.  C <tt><b>FILE</b></tt> pointers and C++
      streams provide automatic buffering and more elaborate formatting options.

# Filesystem API (continued)
* Implementation of <b><tt>copy</tt></b>
     
    ~~~{.c}
    int main(int argc, char *argv[]) {
      if (argc != 3) {
        fprintf(stderr, "%s <source-file> <destination-file>.\n", argv[0]);
        return kWrongArgumentCount;
      }

      int fdin = open(argv[1], /* flags = */ O_RDONLY);
      if (fdin == -1) {
        fprintf(stderr, "%s: source file could not be opened.\n", argv[1]);
        return kSourceFileNonExistent;
      }

      int fdout = open(argv[2], /* flags = */ O_WRONLY | O_CREAT | O_EXCL, 0644);
      if (fdout == -1) {
        switch (errno) {
          case EEXIST:
            fprintf(stderr, "%s: destination file already exists.\n", argv[2]);
            break;
          default:
            fprintf(stderr, "%s: destination file could not be created.\n", argv[2]);
            break;
        }   
        return kDestinationFileOpenFailure;
      }
    ~~~

# Filesystem API (continued)
* Implementation of <b><tt>copy</tt></b>, continued

    ~~~{.c}
      char buffer[1024];
      while (true) {
        ssize_t bytesRead = read(fdin, buffer, sizeof(buffer));
        if (bytesRead == 0) break;
        if (bytesRead == -1) {
          fprintf(stderr, "%s: lost access to file while reading.\n", argv[1]);
          return kReadFailure;
        }

        size_t bytesWritten = 0;
        while (bytesWritten < bytesRead) {
          ssize_t count = write(fdout, buffer + bytesWritten, bytesRead - bytesWritten);
          if (count == -1) {
            fprintf(stderr, "%s: lost access to file while writing.\n", argv[2]);
            return kWriteFailure;
          }
          bytesWritten += count;
        }
      }

      if (close(fdin) == -1) fprintf(stderr, "%s: had trouble closing file.\n", argv[1]);
      if (close(fdout) == -1) fprintf(stderr, "%s: had trouble closing file.\n", argv[2]);
      return 0;
    }
    ~~~
# Filesystem API: Implement <b><tt>t</tt></b> to emulate <b><tt>t</tt></b>
* Overview of <b><tt>tee</tt></b>
    * The <b><tt>tee</tt></b> user program copies everything from standard input to standard output, making zero or 
      more extra copies in the named files supplied as user program arguments.  For example, if the file <b><tt>alphabet.txt</tt></b>
      contains 27 bytes&mdash;the 26 letters of the English alphabet followed by a newline character, then the following would print 
      the alphabet to standard output and to three files named <b><tt>one.txt</tt></b>, <b><tt>two.txt</tt></b>, and <b><tt>three.txt</tt></b>.

    ~~~{.sh}
    myth4> cat alphabet.txt | tee one.txt two.txt three.txt
    abcdefghijklmnopqrstuvwxyz
    myth4> cat one.txt 
    abcdefghijklmnopqrstuvwxyz
    myth4> cat two.txt
    abcdefghijklmnopqrstuvwxyz
    myth4> diff one.txt two.txt
    myth4> diff one.txt three.txt
    myth4>
    ~~~

    * If the file <b><tt>vowels.txt</tt></b> contains the five vowels and the 
      newline character, and <b><tt>tee</tt></b> is invoked as follows, <b><tt>one.txt</tt></b> would be 
      rewritten to contain only the English vowels, but <b><tt>two.txt</tt></b> and <b><tt>three.txt</tt></b> would be left alone.

    ~~~{.sh}
    myth4> more vowels.txt | tee one.txt
    aeiou
    myth4> more one.txt 
    aeiou
    myth4> more two.txt 
    abcdefghijklmnopqrstuvwxyz
    myth4>
    ~~~

    * Full implementation of our own <tt><b>t</b></tt> executable is
      [right here](http://cs110.stanford.edu/autumn-2017/examples/filesystems/t.c).
    * Implementation here replicates much of what <b><tt>copy.c</tt></b> does, but it illustrates
      how you can use low-level I/O to manage many sessions with multiple files.  The implementation
      here is low on error checking, because I want you to focus on the low-level I/O and how it succeeds,
      not on how it fails.

# Filesystem API (continued)
* Implementation of <b><tt>t</tt></b>

    ~~~{.c}
    static void writeall(int fd, const char buffer[], size_t len) {
      size_t numWritten = 0;
      while (numWritten < len) {
        numWritten += write(fd, buffer + numWritten, len - numWritten);
      }
    }

    int main(int argc, char *argv[]) {
      int fds[argc];
      fds[0] = STDOUT_FILENO;
      for (size_t i = 1; i < argc; i++)
        fds[i] = open(argv[i], O_WRONLY | O_CREAT | O_TRUNC, 0644);

      char buffer[2048];
      while (true) {
        ssize_t numRead = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (numRead == 0) break;
        for (size_t i = 0; i < argc; i++)
          writeall(fds[i], buffer, numRead);
      }

      for (size_t i = 1; i < argc; i++) close(fds[i]);
      return 0;
    }
    ~~~

    * Feature 1: Note that <tt><b>argc</b></tt> incidentally provides a count on the number of descriptors that need to
      be written to.
    * Feature 2: <tt><b>STDIN_FILENO</b></tt> is a built-in constant for the number 0, which is the descriptor
      normally attached to standard input.  <tt><b>STDOUT_FILENO</b></tt> is a constant for the number 1, which
      is the default descriptor bound to standard output.
    * Feature 3: I assume all system calls succeed here.  I'm not being lazy, I promise.  I'm just trying to
      keep the examples as clear and compact as possible.

# Filesystem API: Using <tt><b>stat</tt></b> and <tt><b>lstat</tt></b>
* <tt><b>stat</tt></b> and <tt><b>lstat</tt></b>
    * <tt><b>stat</tt></b> is a function that populates a <tt><b>struct stat</tt></b>
      with information about some named file (regular files, directories, links).
    * <tt><b>stat</tt></b> and <tt><b>lstat</tt></b> operate exactly the same way,
      except when the named file is a link,  <tt><b>stat</tt></b> returns information about the file the link
      references, and <tt><b>lstat</tt></b> returns information about the link itself.
    * Manual (<b><tt>man</tt></b>) pages exist for both of these functions
      (e.g. <b><tt>man 2 stat</tt></b>, <b><tt>man 2 lstat</tt></b>, etc.)
    * The <tt><b>struct stat</tt></b> contain the following fields ([source](http://pubs.opengroup.org/onlinepubs/7908799/xsh/sysstat.h.html))

    ~~~{.sh}
    dev_t     st_dev     ID of device containing file
    ino_t     st_ino     file serial number
    mode_t    st_mode    mode of file
    nlink_t   st_nlink   number of links to the file
    uid_t     st_uid     user ID of file
    gid_t     st_gid     group ID of file
    dev_t     st_rdev    device ID (if file is character or block special)
    off_t     st_size    file size in bytes (if file is a regular file)
    time_t    st_atime   time of last access
    time_t    st_mtime   time of last data modification
    time_t    st_ctime   time of last status change
    blksize_t st_blksize a filesystem-specific preferred I/O block size for
                         this object.  In some filesystem types, this may
                         vary from file to file
    blkcnt_t  st_blocks  number of blocks allocated for this object
    ~~~
    * The <tt><b>st_mode</tt></b> field isn't so much a single value as it is a collection of
      bits encoding multiple pieces of information about file type and permissions.
    * A collection of bit masks and macros can be used to extract information from the <tt><b>st_mode</tt></b>
      field.
    * The next two examples&mdash;presented in these 
      <a href="02-filesystems-search.html">two</a> <a href="02-filesystems-list.html">slide decks</a>&mdash;illustrate 
      how the <tt><b>stat</tt></b> and <tt><b>lstat</tt></b> functions can be used
      to navigate and otherwise manipulate a tree of files within the file system.
# Filesystem API: Implementing <b><tt>search</tt></b>
* Implementation of <b><tt>search</tt></b>
    * <b><tt>search</tt></b> is our own simplied implementation of the <b><tt>find</tt></b>
      built-in.
    * The following <tt><b>main</tt></b> relies on <tt><b>listMatches</tt></b>, which we'll
      implement a little later.  (The full program of interest is online
      [right here](http://cs110.stanford.edu/autumn-2017/examples/filesystems/search.c).)

    ~~~{.c}
    static void exitUnless(bool test, FILE *stream, int code, const char *control, ...) {
      if (test) return;
      va_list arglist;
      va_start(arglist, control);
      vfprintf(stream, control, arglist);
      va_end(arglist);
      exit(code);
    }

    int main(int argc, char *argv[]) {
      exitUnless(argc == 3, stderr, kWrongArgumentCount,
                 "Usage: %s <directory> <pattern>\n", argv[0]);
      struct stat st;
      const char *directory = argv[1];
      stat(directory, &st);
      exitUnless(S_ISDIR(st.st_mode), stderr, kDirectoryNeeded,
                 "<directory> must be an actual directory, %s is not", directory);
      size_t length = strlen(directory);
      if (length > kMaxPath) return 0;

      const char *pattern = argv[2];
      char path[kMaxPath + 1];
      strcpy(path, directory); // no buffer overflow because of above check                  
      listMatches(path, length, pattern);
      return 0;
    }
    ~~~

# Filesystem API: <b><tt>search</tt></b> (continued)
* Implementation details
    * Don't worry about the <tt><b>va_list</tt></b> trickery unless you're curious.
      I just wanted to unify error checking to a single helper function.  My <b><tt>exitUnless</tt></b> is
      basically a sexier version of the <b><tt>assert</tt></b> macro.
    * The first thing that's new to us is the call to <b><tt>stat</tt></b>, which lifts a bunch of information about the named 
      file off of the file system and populates <tt><b>st</tt></b> with it.
    * You'll also note the use of the <tt><b>S_ISDIR</tt></b> macro, which examines the upper four bits
      of the <tt><b>st_mode</tt></b> field to determine whether the named file is a directory (or a link
      to one).
    * <tt><b>S_ISDIR</tt></b> has a few cousins: <tt><b>S_ISREG</tt></b> decides whether
      a file is a regular file, and <tt><b>S_ISLNK</tt></b> decided whether the file is a link.  (We'll use
      all of these in our next example).
    * Most of what's interesting is managed by the <tt><b>listMatches</tt></b> function, which does a depth-first
      traversal of the file system to see what files just happen to contain a named <tt><b>pattern</tt></b>
      as a substring.

# Filesystem API: <b><tt>search</tt></b> (continued)
* Implementation of <b><tt>listMatches</tt></b>

    ~~~{.c}
    static void listMatches(char path[], size_t length, const char *pattern) {
      DIR *dir = opendir(path);
      if (dir == NULL) return; // path isn't a directory
      strcpy(path + length, "/");
      while (true) {
        struct dirent *de = readdir(dir);
        if (de == NULL) break;
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;
        if (length + strlen(de->d_name) + 1 > kMaxPath) continue;
        strcpy(path + length + 1, de->d_name);
        struct stat st;
        lstat(path, &st);
        if (S_ISREG(st.st_mode)) { 
          if (strstr(de->d_name, pattern) != NULL) {
            printf("%s\n", path);
          }
        } else if (S_ISDIR(st.st_mode)) {
          listMatches(path, length + 1 + strlen(de->d_name), pattern);
        }
      }
    
      closedir(dir);
    }
    ~~~

# Implementation of <b><tt>listMatches</tt></b> (continued)
* Implementation details
    * My implementation relies on <b><tt>opendir</tt></b>, which accepts what is presumably
      a directory.  It returns a pointer to an opaque iterable that surfaces a sequence
      of <b><tt>struct dirent</tt></b>s via a series of <b><tt>readdir</tt></b> calls.
    * If <b><tt>opendir</tt></b>'s parameter is something other than a directory, it'll 
      return <b><tt>NULL</tt></b>.
    * When the <b><tt>DIR</tt></b> has surfaced all of its entries, <b><tt>readdir</tt></b> 
      returns <b><tt>NULL</tt></b>.  A return value of <b><tt>NULL</tt></b> says it's all over.
    * The <b><tt>struct dirent</tt></b> is only <i>guaranteed</i> to contain a <b><tt>d_name</tt></b> field, which
      is a C string expression of the directory entry's name.  <b><tt>.</tt></b> and <b><tt>..</tt></b> are among the sequence
      of named entries, but I ignore them so I don't cycle through any single directory more than once.
    * I use <b><tt>lstat</tt></b> instead of <b><tt>stat</tt></b> so I know whether an entry is really a link.
    * If the status clearly identifies an entry as a regular file, then I print the entire path
      if and only if it contains the <tt><b>pattern</tt></b> of interest.
    * If the status identifies an entry to be a directory, then I recursively descend into it to see if any of its
      named entries match the pattern we're looking for.
    * <b><tt>opendir</tt></b> returns access to a record that eventually must be released via a call
      to <b><tt>closedir</tt></b>.  That's why my implementation ends with it.

# Filesystem API: <b><tt>list</tt></b> 
* <b><tt>list</tt></b> implementation details
    * I also present the implementation of <b><tt>list</tt></b>, which emulates the
      functionality of <b><tt>ls</tt></b> (in particular, <b><tt>ls -lUa</tt></b>).
    * The implementation of <b><tt>list</tt></b> and <b><tt>search</tt></b> have many things
      in common, but the implementation of <b><tt>list</tt></b> is much longer.
    * Full implementation of entire <tt><b>list</b></tt> executable is [right here](http://cs110.stanford.edu/autumn-2017/examples/filesystems/list.c).
    * Sample output (notice this is my own <b><tt>list</tt></b>, not <b><tt>ls</tt></b>!):

    ~~~{.sh}
    myth7> list /usr/class/cs110/WWW
    drwxr-xr-x  8    70296 root       2048 Sep 24 15:07 .
    drwxr-xr-x >9 root     root       2048 Sep 25 12:46 ..
    drwxr-xr-x  2    70296 root       2048 Sep 24 09:23 restricted
    drwx------  2 poohbear operator   2048 Sep 24 09:23 repos
    drwx------ >9 poohbear operator   2048 Sep 25 16:29 autumn-2017
    -rw-------  1 poohbear operator     89 Sep 24 15:03 index.html
    ~~~

    * I don't present the entire implementation.  I just show one key function: the one
      that knows how to print out the permissions information for an arbitrary entry.

# Filesystem API: <b><tt>list</tt></b> 

* Implementation of <b><tt>list</tt></b>'s <b><tt>listPermissions</tt></b>:

    ~~~{.c}
    static inline void updatePermissionsBit(bool flag, char permissions[], size_t column, char ch) {
      if (flag) permissions[column] = ch;
    }

    static const size_t kNumPermissionColumns = 10;
    static const char kPermissionChars[] = {'r', 'w', 'x'};
    static const size_t kNumPermissionChars = sizeof(kPermissionChars);
    static const mode_t kPermissionFlags[] = {
      S_IRUSR, S_IWUSR, S_IXUSR, // user flags
      S_IRGRP, S_IWGRP, S_IXGRP, // group flags
      S_IROTH, S_IWOTH, S_IXOTH  // everyone (other) flags
    };
    static const size_t kNumPermissionFlags = sizeof(kPermissionFlags)/sizeof(kPermissionFlags[0]);

    static void listPermissions(mode_t mode) {
      char permissions[kNumPermissionColumns + 1];
      memset(permissions, '-', sizeof(permissions));
      permissions[kNumPermissionColumns] = '\0';
      updatePermissionsBit(S_ISDIR(mode), permissions, 0, 'd');
      updatePermissionsBit(S_ISLNK(mode), permissions, 0, 'l');
      for (size_t i = 0; i < kNumPermissionFlags; i++) {
        updatePermissionsBit(mode & kPermissionFlags[i], permissions, i + 1,
                             kPermissionChars[i % kNumPermissionChars]);
      }
      printf("%s ", permissions);
    }
    ~~~

    * Full implementation of <b><tt>list</tt></b> is in [list.c](http://cs110.stanford.edu/autumn-2017/examples/filesystems/list.c).


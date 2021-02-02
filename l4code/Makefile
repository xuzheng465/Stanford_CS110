# CS110 Exceptional Control Flow Examples

CC = gcc

# The CFLAGS variable sets compile flags for g:
#  -g          compile with debug information
#  -Wall       give all diagnostic warnings
#  -pedantic   require compliance with ANSI standard
#  -O0         do not optimize generated code
CFLAGS = -g -Wall -pedantic -O0 -std=gnu99 -I/afs/ir.stanford.edu/class/cs110/local/include

# The LDFLAGS variable sets flags for linker
#  -lm       link in libm (math library)
LDFLAGS =

# In this section, you list the files that are part of the project.
# If you add/change names of header/source files, here is where you edit the Makefile.
SOURCES = simplesh-utils.c
HEADERS = $(SOURCES:.c=.h)
PROGRAMS = basic-fork.c fork-puzzle.c separate.c parent-child.c \
           reap-as-they-exit.c reap-in-fork-order.c \
	   mysystem.c segfault.c simplesh.c loop.c pipe-experiment.c subprocess.c \
           signal-experiment.c five-children.c indistinguishable-pentuplets.c pentuplets.c \
           job-list-synchronization.c job-list-synchronization-improved.c kill-puzzle.c \
	   simplesh-with-race-and-spin.c simplesh-with-spin.c simplesh-with-naps.c simplesh-with-pause.c simplesh-all-better.c
OBJECTS = $(PROGRAMS:.c=.o) $(SOURCES:.c=.o)
TARGETS = $(PROGRAMS:.c=)

default: $(TARGETS)

%:%.o simplesh-utils.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# In make's default rules, a .o automatically depends on its .cc file
# (so editing the .cc will cause recompilation into its .o file).
# The line below creates additional dependencies, most notably that it
# will cause the .cc to recompiled if any included .h file changes.

Makefile.dependencies:: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -MM $(SOURCES) $(PROGRAMS)> Makefile.dependencies

-include Makefile.dependencies

# Phony means not a "real" target, it doesn't build anything
# The phony target "clean" is used to remove all compiled object files.
# The phony target "spartan" is used to remove all compilation products and extra backup files. 

.PHONY: clean spartan

clean:
	@rm -f $(TARGETS) $(OBJECTS) core Makefile.dependencies

spartan: clean
	@rm -f *~

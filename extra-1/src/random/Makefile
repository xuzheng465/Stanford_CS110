# CS110 random number library Makefile 

CXX = /usr/bin/g++-5

# The CFLAGS variable sets compile flags for gcc:
#  -g          compile with debug information
#  -Wall       give all diagnostic warnings
#  -pedantic   require compliance with ANSI standard
#  -O0         do not optimize generated code
#  -std=c++0x  go with the c++0x experimental extensions for thread support (and other nifty things)
CPPFLAGS = -g -Wall -pedantic -O0 -std=c++0x -I../../include

# The LDFLAGS variable sets flags for linker
#  -lm       link in libm (math library)
#  -lpthread link in libpthread (thread library) to back C++11 extensions
LDFLAGS = -lm

# The ARFLAGS variable, if absent, defaults to rv, but I don't want a verbose printout
ARFLAGS = r

# In this section, you list the files that are part of the project.
# If you add/change names of header/source files, here is where you
# edit the Makefile.
HEADERS = random-generator.h
SOURCES = $(HEADERS:.h=.cc)
OBJECTS = $(HEADERS:.h=.o)
TARGETS = ../../lib/librand.a

default: $(TARGETS)

../../lib/librand.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $?

# In make's default rules, a .o automatically depends on its .cc file
# (so editing the .cc will cause recompilation into its .o file).
# The line below creates additional dependencies, most notably that it
# will cause the .cc to recompiled if any included .h file changes.

Makefile.dependencies:: $(SOURCES) $(HEADERS)
	$(CXX) $(CPPFLAGS) -MM $(SOURCES) > Makefile.dependencies

-include Makefile.dependencies

# Phony means not a "real" target, it doesn't build anything
# The phony target "clean" is used to remove all compiled object files.
# The phony target "spartan" is used to remove all compilation products and extra backup files. 

.PHONY: clean spartan

clean:
	@rm -f $(TARGETS) $(OBJECTS) core Makefile.dependencies

spartan: clean
	@rm -f *~

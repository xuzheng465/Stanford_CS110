# Makefile for proxy
# Based on Makefiles previously written by Julie Zelenski.

CXX = /usr/bin/g++-5
DEPS = -MMD -MF $(@:.o=.d)

# The CFLAGS variable sets compile flags for gcc:
#  -g                         compile with debug information
#  -Wall                      give all diagnostic warnings
#  -pedantic                  require compliance with ANSI standard
#  -O0                        do not optimize generated code
#  -std=c++14                 go with the c++14 extensions for thread support, unordered maps, etc
#  -D_GLIBCXX_USE_NANOSLEEP   included for this_thread::sleep_for and this_thread::sleep_until support
#  -D_GLIBCXX_USE_SCHED_YIELD included for this_thread::yield support
CXXFLAGS = -g -Wall -pedantic -O0 -std=c++14 -D_GLIBCXX_USE_NANOSLEEP -D_GLIBCXX_USE_SCHED_YIELD -I../extra/include -I../extra/include $(DEPS)

# The LDFLAGS variable sets flags for linker
#  -pthread   link in libpthread (thread library) to back C++11 extensions (note -pthread and not -lpthread)
#  -lthread   link to course-specific concurrency functions and classes
#  -lsocket++ link to open source socket++ library, which layers iostream objects over sockets
# Actually I did 'sudo apt-get install socket++' to make it work
LDFLAGS = -lpthread -L../extra/lib -lthreads -L../extra/lib/socket++ -lsocket++ -Wl,-rpath=../extra/lib/socket++

# The ARFLAGS variable, if absent, defaults to rv, but I don't want a verbose printout
ARFLAGS = r

# In this section, you list the files that are part of the project.
# If you add/change names of header/source files, here is where you
# edit the Makefile.

SOURCES = \
	main.cc \
	proxy.cc \
	proxy-options.cc \
	scheduler.cc \
	request-handler.cc \
	request.cc \
	response.cc \
	header.cc \
	payload.cc \
	cache.cc \
	blacklist.cc \
	client-socket.cc

HEADERS = $(SOURCES:.cc=.h)
OBJECTS = $(SOURCES:.cc=.o)
DEPENDENCIES = $(patsubst %.o,%.d,$(OBJECTS))
TARGET = proxy

default: $(TARGET)

proxy: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

-include $(SOURCES:.cc=.d)

# Phony means not a "real" target, it doesn't build anything
# The phony target "clean" is used to remove all compiled object files.
# The phony target "spartan" is used to remove all compiled object and backup files.
.PHONY: clean spartan

clean:
	@rm -f $(TARGET) $(OBJECTS) $(DEPENDENCIES) *.o core

spartan: clean
	@rm -f *~

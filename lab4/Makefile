# CS110 lab4 Solution Makefile Hooks
PROGS = quicksort
CXX = /usr/bin/g++

CXX_WARNINGS = -Wall -pedantic -Wno-vla
CXX_DEPS = -MMD -MF $(@:.o=.d)
CXX_DEFINES =
CXX_INCLUDES = -I/usr/class/cs110/local/include

CXXFLAGS = -g $(CXX_WARNINGS) -O0 -std=c++11 $(CXX_DEPS) $(CXX_DEFINES) $(CXX_INCLUDES)
LDFLAGS = -pthread -L/usr/class/cs110/local/lib -lrand

PROGS_SRC = $(patsubst %,%.cc,$(PROGS))
PROGS_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(PROGS_SRC)))
PROGS_DEP = $(patsubst %.o,%.d,$(PROGS_OBJ))

all:: $(PROGS)

$(PROGS): %:%.o
	$(CXX) $^ $(LDFLAGS) -o $@ 

clean::
	rm -f $(PROGS) $(PROGS_OBJ) $(PROGS_DEP)

spartan:: clean
	\rm -fr *~

.PHONY: all clean spartan

-include $(PROGS_DEP)

# --- Solution stuff
# Generic rule for executable, copy binky to binky_soln and strip
%_soln: %
	install -m 755 --strip $< $@

SOLN_PROG = $(PROGS:=_soln)
all:: $(SOLN_PROG)

clean::
	rm -f $(SOLN_PROG) VALGRIND-* callgrind.out.*

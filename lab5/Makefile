# CS110 lab5 Makefile Hooks
PROGS = ebtest
CXX = /usr/bin/g++-5

CXX_WARNINGS = -Wall -pedantic -Wno-vla
CXX_DEPS = -MMD -MF $(@:.o=.d)
CXX_DEFINES =
CXX_INCLUDES = -I/usr/class/cs110/local/include

CXXFLAGS = -g $(CXX_WARNINGS) -O0 -std=c++14 $(CXX_DEPS) $(CXX_DEFINES) $(CXX_INCLUDES)
LDFLAGS = -L/afs/ir/class/cs110/local/lib -lrand -lthreads -pthread

PROGS_SRC = $(patsubst %,%.cc,$(PROGS))
PROGS_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(PROGS_SRC)))
PROGS_DEP = $(patsubst %.o,%.d,$(PROGS_OBJ))

LIB_SRC = event-barrier.cc
LIB_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(LIB_SRC)))
LIB_DEP = $(patsubst %.o,%.d,$(LIB_OBJ))
LIB = libeb.a

all:: $(PROGS)

$(PROGS): %:%.o $(LIB)
	$(CXX) $^ $(LDFLAGS) -o $@ 

$(LIB): $(LIB_OBJ)
	rm -f $@
	ar r $@ $^
	ranlib $@

clean::
	rm -f $(PROGS) $(PROGS_OBJ) $(PROGS_DEP)
	rm -f $(LIB) $(LIB_OBJ) $(LIB_DEP)

spartan:: clean
	\rm -fr *~

.PHONY: all clean spartan

-include $(PROGS_DEP)

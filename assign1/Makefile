# CS110 search Makefile Hooks

PROGS = search imdbtest
CXX = /usr/bin/g++

CXX_WARNINGS = -Wall -pedantic -Wno-vla
CXX_DEPS = -MMD -MF $(@:.o=.d)
CXX_DEFINES =
CXX_INCLUDES = -I../extra/include

CXXFLAGS = -g $(CXX_WARNINGS) -O0 -std=c++0x $(CXX_DEPS) $(CXX_DEFINES) $(CXX_INCLUDES)
LDFLAGS = 

LIB_SRC = imdb.cc path.cc
LIB_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(LIB_SRC)))
LIB_DEP = $(patsubst %.o,%.d,$(LIB_OBJ))
LIB = libsearch.a

PROGS_SRC = $(patsubst %,%.cc,$(PROGS))
PROGS_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(PROGS_SRC)))
PROGS_DEP = $(patsubst %.o,%.d,$(PROGS_OBJ))

all:: $(PROGS)

$(PROGS) $(EXTRA_PROGS): %:%.o $(LIB)
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

-include $(PROGS_DEP) $(LIB_DEP) $(LIB_DEP)

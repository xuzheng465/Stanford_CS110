# CS110 Makefile Hooks: agreggate

PROGS = aggregate
EXTRA_PROGS = test-union-and-intersection
CXX = /usr/bin/g++-5

NA_LIB_SRC = news-aggregator.cc \
	     log.cc \
	     utils.cc \
	     stream-tokenizer.cc \
	     rss-feed.cc \
	     rss-feed-list.cc \
	     html-document.cc \
	     rss-index.cc 

WARNINGS = -Wall -pedantic
DEPS = -MMD -MF $(@:.o=.d)
DEFINES = -D_GLIBCXX_USE_NANOSLEEP -D_GLIBCXX_USE_SCHED_YIELD
INCLUDES = -I/afs/ir/class/cs110/local/include -I/usr/include/libxml2

CXXFLAGS = -g $(WARNINGS) -O0 -std=c++0x $(DEPS) $(DEFINES) $(INCLUDES)
LDFLAGS = -lm -lxml2 -L/afs/ir/class/cs110/local/lib -lrand -lthreads -pthread

NA_LIB_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(NA_LIB_SRC)))
NA_LIB_DEP = $(patsubst %.o,%.d,$(NA_LIB_OBJ))
NA_LIB = libna.a

PROGS_SRC = aggregate.cc
PROGS_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(PROGS_SRC)))
PROGS_DEP = $(patsubst %.o,%.d,$(PROGS_OBJ))

EXTRA_PROGS_SRC = test-union-and-intersection.cc
EXTRA_PROGS_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(EXTRA_PROGS_SRC)))
EXTRA_PROGS_DEP = $(patsubst %.o,%.d,$(EXTRA_PROGS_OBJ))

all: $(PROGS) $(EXTRA_PROGS)

$(PROGS): %:%.o $(NA_LIB)
	$(CXX) $^ $(LDFLAGS) -o $@

$(EXTRA_PROGS): %:%.o
	$(CXX) $^ $(LDFLAGS) -o $@

$(NA_LIB): $(NA_LIB_OBJ)
	rm -f $@
	ar r $@ $^
	ranlib $@

clean:
	rm -f $(PROGS) $(EXTRA_PROGS) $(PROGS_OBJ) $(EXTRA_PROGS_OBJ) $(PROGS_DEP) $(EXTRA_PROGS_DEP)
	rm -f $(NA_LIB) $(NA_LIB_DEP) $(NA_LIB_OBJ)

spartan: clean
	\rm -fr *~

.PHONY: all clean spartan

-include $(NA_LIB_DEP) $(PROGS_DEP) $(EXTRA_PROGS_DEP)


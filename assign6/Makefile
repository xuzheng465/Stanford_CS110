# CS110 Makefile Hooks: aggregate

PROGS = aggregate tptest
EXTRA_PROGS = tpcustomtest
CXX = /usr/bin/g++-5

NA_LIB_SRC = news-aggregator.cc \
	     log.cc \
	     utils.cc \
	     stream-tokenizer.cc \
	     rss-feed.cc \
	     rss-feed-list.cc \
	     html-document.cc \
	     rss-index.cc

TP_LIB_SRC = thread-pool.cc

WARNINGS = -Wall -pedantic
DEPS = -MMD -MF $(@:.o=.d)
DEFINES = -D_GLIBCXX_USE_NANOSLEEP -D_GLIBCXX_USE_SCHED_YIELD
INCLUDES = -I../extra/include -I/usr/include/libxml2

CXXFLAGS = -g $(WARNINGS) -O0 -std=c++14 $(DEPS) $(DEFINES) $(INCLUDES)
LDFLAGS = -lm -lxml2 -L../extra/lib -lrand -lthreads -pthread

NA_LIB_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(NA_LIB_SRC)))
NA_LIB_DEP = $(patsubst %.o,%.d,$(NA_LIB_OBJ))
NA_LIB = libna.a

TP_LIB_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(TP_LIB_SRC)))
TP_LIB_DEP = $(patsubst %.o,%.d,$(TP_LIB_OBJ))
TP_LIB = libthreadpool.a

PROGS_SRC = aggregate.cc
PROGS_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(PROGS_SRC)))
PROGS_DEP = $(patsubst %.o,%.d,$(PROGS_OBJ))

EXTRA_PROGS_SRC = tptest.cc tpcustomtest.cc
EXTRA_PROGS_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(EXTRA_PROGS_SRC)))
EXTRA_PROGS_DEP = $(patsubst %.o,%.d,$(EXTRA_PROGS_OBJ))

all: $(NA_LIB) $(TP_LIB) $(PROGS) $(EXTRA_PROGS)

$(PROGS): %:%.o $(NA_LIB) $(TP_LIB)
	$(CXX) $^ $(LDFLAGS) -o $@

$(EXTRA_PROGS): %:%.o $(TP_LIB)
	$(CXX) $^ $(LDFLAGS) -o $@

$(NA_LIB): $(NA_LIB_OBJ)
	rm -f $@
	ar r $@ $^
	ranlib $@

$(TP_LIB): $(TP_LIB_OBJ)
	rm -f $@
	ar r $@ $^
	ranlib $@

clean:
	rm -f $(PROGS) $(EXTRA_PROGS) $(PROGS_OBJ) $(EXTRA_PROGS_OBJ) $(PROGS_DEP) $(EXTRA_PROGS_DEP)
	rm -f $(NA_LIB) $(NA_LIB_DEP) $(NA_LIB_OBJ)
	rm -f $(TP_LIB) $(TP_LIB_DEP) $(TP_LIB_OBJ)

spartan: clean
	\rm -fr *~

.PHONY: all clean spartan

-include $(NA_LIB_DEP) $(TP_LIB_DEP) $(PROGS_DEP)

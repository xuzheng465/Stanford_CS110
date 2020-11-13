# CS110 Assignment 3 Makefile
PROGS = stsh
EXTRA_PROGS = spin split int tstp fpe conduit
CXX = g++

LIB_SRC = stsh-signal.cc stsh-job-list.cc stsh-job.cc stsh-process.cc stsh-parse-utils.cc \
          stsh-parser/scanner.cc stsh-parser/parser.cc stsh-parser/stsh-parse.cc stsh-parser/stsh-readline.cc

WARNINGS = -Wall -pedantic -Wno-unused-function -Wno-vla
DEPS = -MMD -MF $(@:.o=.d)
DEFINES = 
INCLUDES = -I/afs/ir/class/cs110/local/include

CXXFLAGS = -g $(WARNINGS) -O0 -std=c++0x $(DEFINES) $(INCLUDES)
LDFLAGS = -lreadline -ll

LIB_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(LIB_SRC)))
LIB_DEP = $(patsubst %.o,%.d,$(LIB_OBJ))
LIB = stsh.a

PROGS_SRC = $(patsubst %,%.cc,$(PROGS))
PROGS_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(PROGS_SRC)))
PROGS_DEP = $(patsubst %.o,%.d,$(PROGS_OBJ))

EXTRA_PROGS_SRC = $(patsubst %,%.cc,$(EXTRA_PROGS))
EXTRA_PROGS_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(EXTRA_PROGS_SRC)))
EXTRA_PROGS_DEP = $(patsubst %.o,%.d,$(EXTRA_PROGS_OBJ))

default: $(PROGS) $(EXTRA_PROGS)

stsh-parser/parser.cc stsh-parser/scanner.cc:
	make -C stsh-parser

stsh-parser/parser.o: stsh-parser/parser.cc
stsh-parser/scanner.o: stsh-parser/scanner.cc

stsh: %:%.o $(LIB)
	$(CXX) $^ $(LDFLAGS) -o $@

$(LIB): $(LIB_OBJ)
	rm -f $@
	ar r $@ $^
	ranlib $@

$(EXTRA_PROGS): %:%.o
	$(CXX) $^ $(LDFLAGS) -o $@

clean::
	make -C stsh-parser clean
	rm -f $(PROGS) $(PROGS_OBJ) $(PROGS_DEP)
	rm -f $(EXTRA_PROGS) $(EXTRA_PROGS_OBJ) $(EXTRA_PROGS_DEP)
	rm -f $(LIB) $(LIB_DEP) $(LIB_OBJ)

spartan:: clean
	make -C stsh-parser spartan
	\rm -fr *~

.PHONY: all clean spartan

-include $(LIB_DEP) $(PROGS_DEP) $(EXTRA_PROG_DEP)


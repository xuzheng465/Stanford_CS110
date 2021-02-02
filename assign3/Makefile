# CS110 trace Solution Makefile Hooks

C_PROGS = pipeline-test
CXX_PROGS = trace farm
PROGS = $(C_PROGS) $(CXX_PROGS)
EXTRA_C_PROGS = 
EXTRA_CXX_PROGS = simple-test1 simple-test2 simple-test3 simple-test4 simple-test5 subprocess-test trace-system-calls-test trace-error-constants-test
EXTRA_PROGS = $(EXTRA_C_PROGS) $(EXTRA_CXX_PROGS)
CC = gcc
CXX = /usr/bin/g++-5

C_WARNINGS = -Wall -pedantic
C_DEPS = -MMD -MF $(@:.o=.d)
C_DEFINES = 
C_INCLUDES = -I/afs/ir/class/cs110/local/include

CFLAGS = -g $(C_WARNINGS) $(C_DEPS) -std=gnu99 $(C_INCLUDE)

CXX_WARNINGS = -Wall -pedantic -Wno-vla
CXX_DEPS = -MMD -MF $(@:.o=.d)
CXX_DEFINES =
CXX_INCLUDES = -I/afs/ir/class/cs110/local/include

CXXFLAGS = -g $(CXX_WARNINGS) -O0 -std=c++0x $(CXX_DEPS) $(CXX_DEFINES) $(CXX_INCLUDES)
LDFLAGS = 

PIPELINE_LIB_SRC = pipeline.c
PIPELINE_LIB_OBJ = $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(PIPELINE_LIB_SRC)))
PIPELINE_LIB_DEP = $(patsubst %.o,%.d,$(PIPELINE_LIB_OBJ))
PIPELINE_LIB = libpipeline.a

TRACE_LIB_SRC = trace-options.cc trace-error-constants.cc trace-system-calls.cc subprocess.cc
TRACE_LIB_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(TRACE_LIB_SRC)))
TRACE_LIB_DEP = $(patsubst %.o,%.d,$(TRACE_LIB_OBJ))
TRACE_LIB = libtrace.a

C_PROGS_SRC = $(patsubst %,%.c,$(C_PROGS))
C_PROGS_OBJ = $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(C_PROGS_SRC)))
C_PROGS_DEP = $(patsubst %.o,%.d,$(C_PROGS_OBJ))

CXX_PROGS_SRC = $(patsubst %,%.cc,$(CXX_PROGS))
CXX_PROGS_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(CXX_PROGS_SRC)))
CXX_PROGS_DEP = $(patsubst %.o,%.d,$(CXX_PROGS_OBJ))

EXTRA_C_PROGS_SRC = $(patsubst %,%.c,$(EXTRA_C_PROGS))
EXTRA_C_PROGS_OBJ = $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(EXTRA_C_PROGS_SRC)))
EXTRA_C_PROGS_DEP = $(patsubst %.o,%.d,$(EXTRA_C_PROGS_OBJ))

EXTRA_CXX_PROGS_SRC = $(patsubst %,%.cc,$(EXTRA_CXX_PROGS))
EXTRA_CXX_PROGS_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(EXTRA_CXX_PROGS_SRC)))
EXTRA_CXX_PROGS_DEP = $(patsubst %.o,%.d,$(EXTRA_CXX_PROGS_OBJ))

default: $(PROGS) $(EXTRA_PROGS)

$(CXX_PROGS) $(EXTRA_CXX_PROGS): %:%.o $(TRACE_LIB)
	$(CXX) $^ $(LDFLAGS) -o $@

$(C_PROGS): %:%.o $(PIPELINE_LIB)
	$(CC) $^ $(LDFLAGS) -o $@

$(PIPELINE_LIB): $(PIPELINE_LIB_OBJ)
	rm -f $@
	ar r $@ $^
	ranlib $@

$(TRACE_LIB): $(TRACE_LIB_OBJ)
	rm -f $@
	ar r $@ $^
	ranlib $@

# The soln target makes solution versions of the program.
# For each program 'binky' in $(C_TEST_PROGRAMS) and $(CXX_TEST_PROGRAMS), 
# thess rulee specify how to build 'binky_soln' by linking binky.c[c] to the
# sample library versions of pipeline and subprocess, producing a binky_soln
# you can compare to binky
C_TEST_PROGRAMS = pipeline-test
CXX_TEST_PROGRAMS = subprocess-test
C_SOLN_PROGRAMS = $(C_TEST_PROGRAMS:%=%_soln)
CXX_SOLN_PROGRAMS = $(CXX_TEST_PROGRAMS:%=%_soln)
soln: $(C_SOLN_PROGRAMS) $(CXX_SOLN_PROGRAMS)

# Change linker flags to link soln library found in slink dir
$(C_SOLN_PROGRAMS) $(CXX_SOLN_PROGRAMS): LDFLAGS += -Lslink
$(C_SOLN_PROGRAMS) $(CXX_SOLN_PROGRAMS): LDLIBS = -lsubprocess_soln -lpipeline_soln
$(C_SOLN_PROGRAMS): %_soln:%.o
	$(LINK.o) $(filter %.o, $^) $(LDLIBS) -o $@
$(CXX_SOLN_PROGRAMS): %_soln:%.o
	$(LINK.cc) $(filter %.o, $^) $(LDLIBS) -o $@

clean::
	rm -fr $(C_PROGS) $(C_PROGS_OBJ) $(C_PROGS_DEP)
	rm -fr $(CXX_PROGS) $(CXX_PROGS_OBJ) $(CXX_PROGS_DEP)
	rm -fr $(EXTRA_C_PROGS) $(EXTRA_C_PROGS_OBJ) $(EXTRA_C_PROGS_DEP)
	rm -fr $(EXTRA_CXX_PROGS) $(EXTRA_CXX_PROGS_OBJ) $(EXTRA_CXX_PROGS_DEP)
	rm -fr $(PIPELINE_LIB) $(PIPELINE_LIB_OBJ) $(PIPELINE_LIB_DEP)
	rm -fr $(TRACE_LIB) $(TRACE_LIB_OBJ) $(TRACE_LIB_DEP)
	rm -fr $(C_SOLN_PROGRAMS) $(CXX_SOLN_PROGRAMS)

spartan:: clean
	rm -fr *~
	rm -fr .trace_signatures.txt
	rm -fr padvtest padvtest.*
	rm -fr simple-test6 simple-test6.*

.PHONY: all clean spartan

-include $(C_PROGS_DEP) $(CXX_PROGS_DEP) $(PIPELINE_LIB_DEP) $(TRACE_LIB_DEP) $(EXTRA_C_PROGS_DEP) $(EXTRA_CXX_PROGS_DEP)

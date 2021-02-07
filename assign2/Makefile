# CS110 Assignment 2 Makefile
CC = gcc
PROG =  diskimageaccess

LIB_SRC  = diskimg.c inode.c unixfilesystem.c directory.c pathname.c  chksumfile.c file.c 
DEPS = -MMD -MF $(@:.o=.d)
WARNINGS = -fstack-protector -Wall -W -Wcast-qual -Wwrite-strings -Wextra -Wno-unused -Wno-unused-parameter

CFLAGS += -g $(WARNINGS) $(DEPS) -std=gnu99

LIB_OBJ = $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(LIB_SRC)))
LIB_DEP = $(patsubst %.o,%.d,$(LIB_OBJ))
LIB = v6fslib.a 

PROG_SRC = diskimageaccess.c 
PROG_OBJ = $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(PROG_SRC)))
PROG_DEP = $(patsubst %.o,%.d,$(PROG_OBJ))

TMP_PATH := /usr/bin:$(PATH)
export PATH = $(TMP_PATH)

LIBS += -lssl -lcrypto

all: $(PROG)


$(PROG): $(PROG_OBJ) $(LIB)
	$(CC) $(LDFLAGS) $(PROG_OBJ) $(LIB) $(LIBS) -o $@

$(LIB): $(LIB_OBJ)
	rm -f $@
	ar r $@ $^
	ranlib $@

clean::
	rm -f $(PROG) $(PROG_OBJ) $(PROG_DEP)
	rm -f $(LIB) $(LIB_DEP) $(LIB_OBJ)

.PHONY: all clean 

-include $(LIB_DEP) $(PROG_DEP)

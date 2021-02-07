#!/usr/bin/python

"""
A script to spawn remote processes that can be killed with a single Ctrl-C.
Hacked together by Michael Chang (mchang). Don't blame him for bugs, though.
"""

import sys, os, pty, select, signal
from collections import namedtuple

DIR = os.path.normpath(os.path.realpath(__file__) + "/../")
CMD = "./proxy"
HOST_FMT = "myth%d.stanford.edu"

def comment_me_out_once_you_understand():
    print "Open up this python script and update the HOSTS variable with"
    print "the number (and just the number) of the myth machine where you'd"
    print "like to run your proxy (e.g. HOSTS = [22])."
    print
    print "Once you have proxy chaining working, you can extend the HOSTS list"
    print "to be a comma-delimited list of myth machine numbers."
    sys.exit(0)

comment_me_out_once_you_understand()
HOSTS = [xx]

Child = namedtuple("Child", ["name", "pid", "fd"])
children = []
for i in range(len(HOSTS)):
    hostnum = HOSTS[i]
    hostname = HOST_FMT % hostnum
    # We need ssh to allocate a terminal, and we want each process to have its own.
    pid, fd = pty.fork()
    if pid == 0: 
        signal.signal(signal.SIGPIPE, signal.SIG_DFL)
        command = "./proxy"
        if i < len(HOSTS) - 1:
            secondaryproxynum = HOSTS[i + 1]
            secondaryproxy = HOST_FMT % secondaryproxynum
            command += " --proxy-server %s" % secondaryproxy
        args = ["/bin/bash", "-c", "ssh -t %s \"cd %s && %s\"" % (hostname, DIR, command)]
        os.execv(args[0], args)
    else:
        nicename = hostname.split(".")[0] if "." in hostname else hostname
        children.append(Child(nicename, pid, fd))

def print_child_output(child):
    try: # If child exits, read raises an error, argh!
        output = os.read(child.fd, 1024).rstrip()
        for line in output.split("\n"):
            line = line.rstrip()
            if not line: continue
            print "%s: %s" % (child.name, line)
        return True
    except OSError:
        pid, status = os.waitpid(child.pid, os.WNOHANG)
        if pid == 0: raise # What? Child didn't exit? No idea what's going on now
        return False

while len(children) != 0:
    try:
        exited = []
        r, w, x = select.select([c.fd for c in children], [], [])
        for fd in r:
            child = next((c for c in children if c.fd == fd), None)
            if not print_child_output(child): exited.append(child)
        for c in exited: children.remove(c)
    except KeyboardInterrupt:
        # This is almost certainly not the right way to kill these processes...
        # If we don't catch the SIGINT at all, all processes will terminate gracefully, which is fine.
        # But then you don't see the "Shutting down" message, so that wasn't satisfying.
        # Sending SIGINT doesn't work correctly, so let's just write a Ctrl-C to the terminal...
        print "" # Newline after "^C"
        for c in children: os.write(c.fd, "\x03")

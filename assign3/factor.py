#!/usr/bin/env python
import sys, os, signal, math, time

# This line is equivalent to the C syscall: prctl(PR_SET_PDEATHSIG, SIGKILL);
# It causes the OS to send this process a SIGKILL when its parent process (most likely ./farm)
# terminates for any reason. This avoids leaving behind stopped factor.py processes.
import ctypes; ctypes.CDLL(None).prctl(1, 9)

def isPrime(num):
    if num <= 1: return False
    for factor in xrange(2, int(math.sqrt(num)) + 1):
        if num % factor == 0: return False
    return True

def factorization(num):
    if num == 1 or isPrime(num):
        return '%d = %d' % (num, num)

    factors = []
    original = num
    for factor in xrange(2, num):
        while num % factor == 0:
            factors.append(factor)
            num = num / factor
    factors.sort()
    factors = map(lambda num: str(num), factors)
    return '%d = %s' % (original, ' * '.join(factors))

self_halting = len(sys.argv) > 1 and sys.argv[1] == '--self-halting'
pid = os.getpid()
while True:
    if self_halting: os.kill(pid, signal.SIGSTOP)
    try: num = int(raw_input()) 
    except EOFError: break;
    start = time.time()
    response = factorization(num)
    stop = time.time()
    print '%s [pid: %d, time: %g seconds]' % (response, pid, stop - start)
    

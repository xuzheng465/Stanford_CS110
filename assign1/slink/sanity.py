#!/usr/bin/python
import string, re

def normalize(str):
    lines = str.split('\n')
    lines = filter(lambda x: not re.match(r'^\s*$', x), lines)
    if len(lines) == 0 or 'was in' not in lines[0]: return str
    if len(lines) == 1: return re.sub('was in "[^"]*" \([0-9]+\) with', 'was in "<movie>" (<year>) with', str)
    first = re.sub('was in .*', 'was in "<movie>" (<year>) with <connector>.', lines[0])
    lastactor = lines[-1][lines[-1].index(') with ') + 7:]
    last = '<connector> was in "<movie>" (<year>) with %s' % lastactor
    for i in range(len(lines)):
        lines[i] = '<connector> was in "<movie>" (<year>) with <connector>.'
    lines[0] = first
    lines[-1] = last
    lines = map(lambda x: " + " + x, lines)
    str = '\n'.join(['Path between two supplied actors is of length %d:' % len(lines)] + lines)
    return str


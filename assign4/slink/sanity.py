#!/usr/bin/python

import re
import getpass

# Filter out pids (CS110 assignment 4)
def filter_pids(str):
    lines = str.split('\n')
    for i in range(len(lines)):
        m = re.match(r'\[\d+\] +([0-9]{3,5}).*', lines[i])
        if m:
            pids = re.findall(r' [0-9]{3,5}', lines[i])
            for pid in pids:
                lines[i] = re.sub(pid, ' <pid>', lines[i])
        m = re.match(r' +([0-9]{3,5}).*', lines[i])
        if m:
            lines[i] = re.sub(m.group(1), '<pid>', lines[i])
        m = re.search(r'(Usage: [^\,]*),.*', lines[i])
        if m:
            lines[i] = m.group(1) + '.'
    return '\n'.join(lines)

def id(str):
    return str

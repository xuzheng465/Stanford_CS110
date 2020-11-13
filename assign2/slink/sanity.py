#!/usr/bin/python

import re

# Filter for inode check (CS110 assignment 1)
def extract_filesys_inodes(str):
    canonical = []
    for line in str.split('\n'):
        match = re.match('Inode (?P<inode>[0-9]*) ([^ ]* ){5}(?P<chksum>[^ ]*)', line)
        if match:
            canonical.append(match)
    sorted_pairs = sorted(canonical, key=lambda line: int(line.group('inode')))
    return '\n'.join([pair.expand('Inode \g<inode> checksum \g<chksum>') for pair in sorted_pairs])

# Filter for path check (CS110 assignment 1)
def extract_filesys_paths(str):
    canonical = []
    for line in str.split('\n'):
        match = re.match('Path [^ ]* (?P<path>[^ ]*) ([^ ]* ){5}(?P<chksum>[^ ]*)', line)
        if match:
            canonical.append(match.expand('Path \g<path> checksum \g<chksum>'))
    return '\n'.join(sorted(canonical))


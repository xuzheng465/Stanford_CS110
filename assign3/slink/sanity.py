#!/usr/bin/python
import re, string, random

# Normalize factor-farm output
def normalize_farm_output_lines(str):
    print str
    normalized_lines = []
    pids = {}
    for line in str.split('\n'):
        match = re.match(r'Worker (\d+) is set to run on CPU (\d+).', line)
        if match:
            pid = int(match.group(1))
            if pid not in pids: pids[pid] = 'pid-%d' % len(pids)
            normalized_pid = pids[pid]
            cpu = int(match.group(2))
            line = 'Worker %s is set to run on CPU %d.' % (normalized_pid, cpu)
        match = re.match(r'([^:]*): (\d+)([^:]*): ([0-9]+\.[0-9e\-]+) .*', line)
        if match:
            pid = int(match.group(2))
            normalized_pid = 'child-pid' if pid in pids else 'unrelated-pid'
            line = '%s: <%s>, time: <duration> seconds]' % (match.group(1), normalized_pid)
        normalized_lines.append(line)
    return '\n'.join(normalized_lines)

def handle_specific_system_call(line):
    match = re.search(r'arch_prctl.*', line)
    if match:
        return 'arch_prctl -> [signature-not-available]'
    match = re.search(r'syscall\(2\).*', line)
    if match:
        return 'syscall(2) = <fd>'
    match = re.search(r'syscall\(39\).*', line)
    if match:
        return 'syscall(39) = <pid>'
    match = re.search(r'syscall\(56\).*', line)
    if match:
        return 'syscall(56) = <tid>'
    match = re.search(r'syscall\(110\).*', line)
    if match:
        return 'syscall(110) = <pid>'
    match = re.search(r'syscall\(186\).*', line)
    if match:
        return 'syscall(186) = <tid>'
    match = re.search(r'syscall\(231\).*', line)
    if match:
        return 'syscall(231) = <no return>'
    match = re.search(r'getpid\(\).*', line)
    if match:
        return 'getpid() = <pid>'
    match = re.search(r'getppid\(\).*', line)
    if match:
        return 'getppid() = <pid>'
    match = re.search(r'gettid\(\).*', line)
    if match:
        return 'gettid() = <tid>'
    match = re.search(r'brk[^=]+ (= [xa-fA-F0-9]+)', line)
    if match:
        line = line.replace(match.group(1), '= <brk-return-address>')
    match = re.search(r'clone[^=]+ (= [0-9]+)', line)
    if match:
        line = line.replace(match.group(1), '= <tid>')
    match = re.search(r'open\([^)]+\) (= \d+)', line)
    if match:
        line = line.replace(match.group(1), '= <fd>')
    match = re.search(r'fstat(\(\d+,).*', line)
    if match:
        line = line.replace(match.group(1), '(<fd>,')
    match = re.search(r'read(\(\d+,).*', line)
    if match:
        line = line.replace(match.group(1), '(<fd>,')
    match = re.search(r'write(\(\d+,).*', line)
    if match:
        line = line.replace(match.group(1), '(<fd>,')
    match = re.search(r'close\((\d+)\).*', line)
    if match:
        line = line.replace(match.group(1), '<fd>')
    match = re.search(r'(mmap\([^,]+, [^,]+, [^,]+, [^,]+,)( [^,]+,)( [^)]+\).*)', line)
    if match:
        line = match.group(1) + ' <fd>,' + match.group(3)
    return line

def generalize_line(line):
    line = handle_specific_system_call(line)
    while True:
        match = re.search(r'0x[0-9a-fA-F]{5,16}', line)
        if not match: break
        line = line.replace(match.group(0), '<userspace-address>')
    while True:
        match = re.search(r'\-?\d{6,16}', line)
        if not match: break
        line = line.replace(match.group(0), '<large-number>')
    return line

def normalize_trace_output_lines(str):
    normalized_lines = []
    lines = str.split('\n')
    for line in lines:
        line = generalize_line(line)
        normalized_lines.append(line)
    return '\n'.join(normalized_lines)

def strip_all_but_syscall_name(str):
    normalized_lines = []
    lines = str.split('\n')
    for line in lines:
        match = re.search(r'([^(]+)\(.*', line)
        if match:
            line = match.group(1) + '(<ignoring-rest...'
        elif re.search(r' -> \[signature-not-available\]', line):
            match = re.match(r'([^ ]+) .*', line)
            line = match.group(1) + '(<ignoring-rest...'
        normalized_lines.append(line)
    return '\n'.join(normalized_lines)

def strip_all_but_return_value(str):
    normalized_lines = []
    lines = str.split('\n')
    for line in lines:
        match = re.search(r'[^=]+= ( .*)', line)
        if match:
            line = '<function-name>(<zero-or-more-args>) =' + match.group(1)
        line = generalize_line(line)
        normalized_lines.append(line)
    return '\n'.join(normalized_lines)

def show_only_trace_return_value(str):
    normalized_lines = []
    lines = str.split('\n')
    for line in lines:
        match = re.search(r'exit.*', line)
        if match: return line
    return '<no-return-value>'

def extract_strings_from_line(line):
    strings = re.findall(r'\"(.+?)\"', line)
    return ', '.join(map(lambda s: '"' + s + '"', strings))

def extract_strings(str):
    normalized_lines = []
    lines = str.split('\n')
    for line in lines:
        line = extract_strings_from_line(line)
        if len(line) > 0:
            normalized_lines.append(line)
    return '\n'.join(normalized_lines)

def both(str):
    return normalize_trace_output_lines(normalize_farm_output_lines(str))


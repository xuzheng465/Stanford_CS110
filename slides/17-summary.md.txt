# Announcements
* Today's Agenda
    * Work through the Scrabble API example posted this
      [past Monday](http://web.stanford.edu/class/cs110/autumn-2017/lectures/16-networking-scrabble-api.html).
    * Discuss the MapReduce Programming Model, what a mapper is, what a reducer is, and how
      they can be chained together in a single pipeline of processes to analyze and process
      large data sets.
        * We'll present the map and reduce executables associated with the most canonical
          of MapReduce jobs: word counts.  The slides present code in Python, since it's
          very short and easy to follow (even if you don't know Python).  My lecture won't
          focus on the code, but rather on the general idea (which is fairly straightforward, imo.)
        * We'll discuss how very large data sets can be partitioned into many, many chunk files
          and processed by a large number of simultaneously executing map and reduce jobs
          on hundreds or even thousands of machines.
        * We'll discuss the <tt><b>group-by-key</b></tt> algorithm (very straightforward, actually) which is
          run on the full accumulation of mapper output files to generate the full set of reducer
          input files.

# Map Reduce
* The mapper!
    * A mapper is a program that reads in an arbitrary data file
      and outputs a file of key-value pairs, one per line.
    * Here's an example of a Python program that reads in an arbitrary text file and
      outputs lines of the form "<word> 1".

    ~~~{.python}
    #!/usr/bin/env python
    import sys
    import re
    import string

    pattern = re.compile("^[a-z]+$") # matches purely alphabetic words
    for line in sys.stdin:
        line = line.strip()
        tokens = line.split()
        for token in tokens:
            lowercaseword = token.lower()
            if pattern.match(lowercaseword):
                print '%s 1' % lowercaseword
    ~~~

    * The above program can be invoked as follows:

    ~~~{.sh}
    myth22> cat anna-karenina.txt | ./word-count-mapper.py
    ~~~

    * Doing do will produce the following (condensed) output:

    ~~~{.sh}
anna 1
karenina 1
by 1
leo 1
tolstoy 1
...
i 1
have 1
the 1
power 1
to 1
put 1
into 1
    ~~~

# Code: Group By Key
* The <tt><b>group-by-key</b></tt> process's precondition
    * The group-by-key process is used in all map-reduce pipelines, not just this one.  This group-by-key
      process assumes the mapper's output has been sorted by key so that multiple copies of the same
      key are grouped together, as with this:

    ~~~{.sh}
myth22> cat anna-karenina.txt | ./word-count-mapper.py | sort
    ~~~

    * The above pipeline produces the following (condensed) output:

    ~~~{.sh}
a 1
a 1
a 1
a 1
a 1 // plus 6064 additional copies of this same line
...
zeal 1
zeal 1
zeal 1
zealously 1
zest 1
zhivahov 1
zigzag 1
zoological 1
zoological 1
zoology 1
zu 1
    ~~~

# Code: Group By Key (continued)
* The <tt><b>group-by-key</b></tt> process's postcondition
    * The following Python script is a short (but dense) program that reads from an incoming
      stream of key-value pairs, sorted by key, and outputs the same content, save for the fact
      that all lines with the same key have been collapsed to a single line, where all values
      themselves have been collapsed to a single vector-of-values presentation:

    ~~~{.python}
    #!/usr/bin/env python
    from itertools import groupby
    from operator import itemgetter
    import sys
 
    def read_mapper_output(file):
        for line in file:
            yield line.strip().split(' ')
 
    def main():
        data = read_mapper_output(sys.stdin)
        for key, keygroup in groupby(data, itemgetter(0)):
            values = ' '.join(sorted(v for k, v in keygroup))
            print "%s %s" % (key, values)
 
    if __name__ == "__main__":
        main()
    ~~~

    * The sorted output of the problem-specific mapper could be fed to the above script, as 
      with this:

    ~~~{.python}
    myth22> more anna-karenina.txt | ./word-count-mapper.py | sort | ./group-by-key.py
    ~~~

    * Doing so produces this:

    ~~~{.python}
    a 1 1 1 1 1 // plus 6064 more 1's on this same line
    abandon 1 1 1 1 1 1
    abandoned 1 1 1 1 1 1 1 1 1
    abandonment 1
    abashed 1 1
    abasing 1
    aber 1
    abilities 1
    ...
    zaraisky 1 1 1 1
    zeal 1 1 1
    zealously 1
    zest 1
    zhivahov 1
    zigzag 1
    zoological 1 1
    zoology 1
    zu 1
    ~~~

# Code: Reducer
* The reducer!
    * A reducer is a problem-specific program that expects a sorted input file, where each line
      is a key/vector-of-values pair as produced by the <tt><b>group-by-key</b></tt> script.
    * Consider the following:

    ~~~{.python}
    #!/usr/bin/env python
    import sys
 
    def read_mapper_output(file):
        for line in file:
            yield line.strip().split(' ')
 
    def main():
        data = read_mapper_output(sys.stdin)
        for vec in data:
            word = vec[0]
            count = sum(int(number) for number in vec[1:])
            print "%s %d" % (word, count)
 
    if __name__ == "__main__":
        main()
    ~~~

    * The above reducer could be fed the sorted, key-grouped output of the previously supplied
      mapper if this chain of piped executables is supplied on the command line:

    ~~~{.python}
    myth22> more anna-karenina.txt | ./word-count-mapper.py | sort | ./group-by-key.py | ./word-count-reducer.py
    ~~~

    * The above chain of piped exeucutables produces this:

    ~~~{.python}
    a 6069
    abandon 6
    abandoned 9
    abandonment 1
    abashed 2
    abasing 1
    aber 1
    abilities 1
    ...
    zaraisky 4
    zeal 3
    zealously 1
    zest 1
    zhivahov 1
    zigzag 1
    zoological 2
    zoology 1
    zu 1
    ~~~


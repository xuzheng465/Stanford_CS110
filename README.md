# Stanford_CS110

[Course website](https://web.stanford.edu/class/cs110/)

Get from [this Repo](https://github.com/sauronalexander/cs110-1), and extract the starter code.

- [ ] Assignment 1
- [ ] Assignment 2
- [ ] Assignment 3
- [ ] Assignment 4
- [ ] Assignment 5
- [ ] Assignment 6
- [ ] Assignment 7
- [ ] Assignment 8

## Keng (Problem) in Assignment 1

When I tried to run imdbtest_soln, it can not find data directory. 

The directory is defined in the `imdb-utils.h`. 

```c++
const std::string kIMDBDataDirectory("/usr/class/cs110/samples/assign1/");
```

Just create the directory and put the data into this directory. Everything will be fine.

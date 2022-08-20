# Network Algorithms Reference Implementations

## Dependencies

This is C++11 software and requires a recent verison of `g++`. We used 9.4.0. It also uses GNU's `make`. We used version 4.2.1.


## Get data
This software can run with real or synthetic data. Right now, it's hard-coded to run with exactly 10 parties. To generate synthetic data for 10 parties, run:
```
$ python3 data/geninput.py
```
This will delete any existing data files and create new ones.

To use real data from BIS, go to the BIS directory and get it formatted. `analyze.py` will place it into this directory.
```
$ cd ../BIS/
$ python3 analyze.py -n 10 -t 0
```

Regardless of the method you chose, you should now have data:
```
$ ls ../insecure/data
bank0.dat  bank2.dat  bank4.dat  bank6.dat  bank8.dat  geninput.py
bank1.dat  bank3.dat  bank5.dat  bank7.dat  bank9.dat  README.md
```

**Troubleshooting**: If you try to run the computation without any data in the `data` directory, the program will abort with a `BadInputError`. If you're getting weird insolvency behavior, you might have more than 10 bank files (so your system isn't consistent when only 10 are read in).

## Compile and run
There's a Makefile with several targets. To build and run a given `<target>, do
```
$ make <target>
$ ./<target>
```

The targets have the following behavior
* `econ`: Runs the Eisenburg and Noe algorithm.
* `naive`: Runs a naive algorithm to check for solvency, but I'm not sure which one.
* `test`: Runs tests on the matrix implementation. You probably don't need to run this one.
* `data`: Generate synthetic data. You could use this instead of the instructions above.
* `clean`: Removes the generated binaries (but not the generated data).
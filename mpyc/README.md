# MPyC

## Install extra dependencies
This component doesn't have a Docker container, so you'll need to download the more useful time command for benchmarking. 
This sequence will install it at `/usr/local/bin/time`:
```
$ wget https://ftp.gnu.org/gnu/time/time-1.9.tar.gz
$ tar xzvf time-1.9.tar.gz
$ rm time-1.9.tar.gz
$ cd time-1.9
$ ./configure && make && sudo make install
$ cd ..
$ rm time-1.9
```

Then, install MPyC and some Python dependencies:
```
$ pip3 install mpyc sklearn gmpy2
```

## Run MPyC

First, make a directory for the data to live in:
```
$ cd src/
$ mkdir data
```
Then, run the benchmark script. 
```
$ /bin/bash benchmark.sh
```

This will collect and format data from the BIS directory (see that section for details) and run the three MPYC algorithms (EN, EGJ, and EN with a computational improvement).
Results live in `results/` and are appended to each file after each run. This repository includes our benchmark results in that directory, so you may see our results followed by yours if you run the benchmarking.
By default, the benchmark script runs for 2-9 parties, but you can modify L13 if you need to decrease the number of parties. 

## Make plots

Note that the plots won't build if you ran benchmarks and left your results appended to ours. Try `git restore results/` if that's the case.

Make a directory to hold the parsed data and run visualization:
```
$ mkdir csv
$ python3 visualize_benchmarks.py
```

This will create `mpyc-ram.png` and `mpyc-time.png`, which are versions of Figure 6a and 6b. The data that these plots represent is parsed into `csv/mpyc_ram.csv` and `csv/mpyc_runtime.csv`. The plots in the text are actually generated using a GNUPlot script on this data to generate a LaTeX file.


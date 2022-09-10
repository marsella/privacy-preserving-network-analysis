# Privacy Preserving Network Analysis

This repository accompanies the paper [Privacy Preserving Network Analysis](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=3680000) 
by [Marcella Hastings](http://marcellahastings.com), 
[Brett Hemenway Falk](https://www.seas.upenn.edu/~fbrett/),
and [Gerry Tsoukalas](https://gerrytsoukalas.com/).

The contents of this repository include implementations of network analysis algorithms in four MPC libraries;
a reference implementation of the network analysis algorithms in C++;
and scripts to scrape and format the data on which we ran the algorithms.

Each directory has more detailed notes about how to build and run the software, generate data for plots and tables in the text, and any limitations we're aware of.
* `BIS`: scripts to scrape and format data from the [Bank of International Settlements](https://www.bis.org).
* `emp`: implementation of algorithms using the EMP-toolking MPC engine
* `insecure`: C++ implementation of all algorithms, does _not_ use any MPC techniques
* `mpyc`: implementation of algorithms using the MPyC engine
* `picco`: implementation of algorithms using the PICCO MPC engine
* `scalemamba`: implementation of algorithms using the SCALE-MAMBA MPC engine

## Dependencies

### Docker
Each code sample is paired with a Docker container that downloads all the necessary software dependencies.

[Download Docker here!](https://docs.docker.com/get-docker/)

Some general usage notes about Docker:
- Docker containers are supposed to be self-sufficient environments.
You _build_ a container image to get an isolated filesystem with all the configuration details and dependencies.
You _run_ a container image to get an instance, which is a sandboxed process that can support the software we provided.
[Learn more about Docker through their tutorial.](https://docs.docker.com/get-started/)

- We list `build` and `run` commands for each library in their READMEs. If you're on linux, you might have to run these with `sudo`.


### Python3
Many of the examples use Python3 to generate synthetic data and produce plots. This was tested with Python 3.8.10.

You'll need the following packages:
```
$ pip install networkx matplotlib numpy
```

### GNUPlot
Some plots are generated with GNUPlot.
```
$ sudo apt-get install gnuplot-x11
```

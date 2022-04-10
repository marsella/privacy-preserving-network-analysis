#!/bin/bash 

sed -i 's/const static int nP = [[:digit:]]\+;/const static int nP = 3;/' ~/emp-agmpc/test/bench_mpc.cpp

make

./bin/bench_mpc 1 12345 & ./bin/bench_mpc 2 12345 & ./bin/bench_mpc 3 12345 

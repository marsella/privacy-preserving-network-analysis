#!/bin/bash 

sed -i 's/const static int nP = [[:digit:]]\+;/const static int nP = 9;/' ~/emp-agmpc/test/bench_mpc.cpp

make

./bin/bench_mpc 1 12345 & ./bin/bench_mpc 2 12345 & ./bin/bench_mpc 3 12345 & ./bin/bench_mpc 4 12345 & ./bin/bench_mpc 5 12345 & ./bin/bench_mpc 6 12345 & ./bin/bench_mpc 7 12345 & ./bin/bench_mpc 8 12345 & ./bin/bench_mpc 9 12345

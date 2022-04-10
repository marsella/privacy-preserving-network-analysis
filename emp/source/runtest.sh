#!/bin/bash

n=5
b=32
d=8
v=100

echo "Running test with n = $n, b = $b, d = $d, v = $v"
python geninput.py -e clearing -n $n -b $b -v $v -d $d
pushd build 2>/dev/null
killall clearing 2>/dev/null
make
./bin/clearing 2 12345 $n > /dev/null 2>/dev/null &
./bin/clearing 1 12345 $n 
popd 2>/dev/null


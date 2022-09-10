#!/bin/bash

# this benchmarks the performance of the noe algorithm
# times compilation and runtime
# collects some stats from the compile output

TIME="/usr/local/bin/time -v"
PROG=noe

PYTHON="python3"
#PYTHON="python"

for BANK in `seq 2 9`
do

  # generate data
  $PYTHON egjdata.py $BANK
  pushd .
  cd ../../BIS/
  $PYTHON analyze.py -n $BANK -t 2
  popd

  # run two noe versions
  for ALG in iterative inversion
  do
  
    LOG="results/output$BANK.$ALG.dat"
  
    $TIME $PYTHON noe.py $ALG $BANK -M $BANK >> $LOG 2>&1
  done

  # run egj
  LOG="results/output$BANK.EGJ.dat"
  $TIME $PYTHON egj.py $BANK -M $BANK >> $LOG 2>&1

done


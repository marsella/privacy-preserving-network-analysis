#!/bin/bash

# this benchmarks the performance of the noe algorithm
# times compilation and runtime
# collects some stats from the compile output

cd SCALE-MAMBA/

TIME="/usr/local/bin/time -v"
PROG=noe

PYTHON="/usr/bin/python2"
RESULTDIR="/root/benchmarks"


if [ $# -ne 2 ]; then
  echo "wrong number of arguments (expects 2)"
  echo "usage: ./benchmark.sh <fake offline? 1/0> <fake sacrifice? 1/0>"
  exit 1
fi

# 0 means real data. 1 means fake data.
FAKE_OFFLINE=$1
FAKE_SACRIFICE=$2

if [ $FAKE_OFFLINE -ne 0 ] && [ $FAKE_OFFLINE -ne 1 ]; then
  echo "Bad value for FAKE_OFFLINE. Aborting"
  exit 1
fi

if [ $FAKE_SACRIFICE -ne 0 ] && [ $FAKE_SACRIFICE -ne 1 ]; then
  echo "Bad value for FAKE_SACRIFICE. Aborting"
  exit 1
fi

mkdir -p $RESULTDIR

for BANK in 2 4
do
  # write the correct number of banks into the program
  sed -i '110c'"NUM_BANKS = int($BANK)" Programs/$PROG/$PROG.mpc

  # generate setup files
  FILE="Data/NetworkData.txt"
  echo "RootCA.crt" > $FILE
  echo $BANK        >> $FILE
  for (( PLAY=0; PLAY<$BANK; PLAY++ )); do
    echo "$PLAY 127.0.0.1 Player$PLAY.crt player$PLAY@example.com" >> $FILE
  done
  echo $FAKE_OFFLINE   >> $FILE
  echo $FAKE_SACRIFICE >> $FILE

  echo "2 1 123" | ./Setup.x

  # add retries to try and avoid persistant mac fails
  for _ in [1,..,100]; do
    # generate data
    python geninput.py $BANK

    # compile the program, collect stats
    COMPILE_OUTPUT="$RESULTDIR/output$BANK.$PROG.dat"
    $TIME $PYTHON compile.py Programs/$PROG >> $COMPILE_OUTPUT 2>&1

    # execute the program
    for (( PLAY=0; PLAY<$BANK; PLAY++ ))
    do 
      RUNTIMEOUTPUT="$RESULTDIR/output$BANK.$PROG.p$PLAY.runtime.dat"
      >$RUNTIMEOUTPUT # delete old logs
      ($TIME ./Player.x $PLAY ./Programs/$PROG & ) >> $RUNTIMEOUTPUT 2>&1
    done

    # wait for players to finish executing
    # TODO maybe add a condition that if the thing outputs "End of prog" you should stop
    while (ps aux | grep "Player.x" | grep -v grep >/dev/null); do
      sleep 2
    done

    if grep -L mac_fail $RUNTIMEOUTPUT; then
      break
    fi
  done

done


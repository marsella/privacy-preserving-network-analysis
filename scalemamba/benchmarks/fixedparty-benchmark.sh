#!/bin/bash

# this benchmarks the performance of the noe algorithm
# times compilation and runtime
# collects some stats from the compile output

cd SCALE-MAMBA/

TIME="/usr/local/bin/time -v"
PROG=noe

PYTHON="/usr/bin/python2"
RESULTDIR="/home/marcella/banking_MPC/code/scalemamba/benchmarks/fixedparty-results"

# 0 means real data. 1 means fake data.
FAKE_OFFLINE=0
FAKE_SACRIFICE=0

# generate setup files for 3 computation parties
CP=3
FILE="Data/NetworkData.txt"
echo "RootCA.crt" > $FILE
echo $CP         >> $FILE
for (( PLAY=0; PLAY<$CP; PLAY++ )); do
  echo "$PLAY 127.0.0.1 Player$PLAY.crt player$PLAY@example.com" >> $FILE
done
echo $FAKE_OFFLINE   >> $FILE
echo $FAKE_SACRIFICE >> $FILE

# note: this is not a secure key generation
echo "2 1 123" | ./Setup.x

for BANK in 4 6 8 10 
do
  # write the correct number of banks into the program
  sed -i '110c'"NUM_BANKS = int($BANK)" Programs/$PROG/$PROG.mpc

  # add retries to try and avoid persistant mac fails
  for _ in [1,..,100]; do
    # generate data; ONEPARTYINPUT must be True
    python geninput.py $BANK

    # compile the program, collect stats
    COMPILE_OUTPUT="$RESULTDIR/fixedparty$BANK.compile.$PROG.dat"
    $TIME $PYTHON compile.py Programs/$PROG >> $COMPILE_OUTPUT 2>&1

    # execute the program
    for (( PLAY=0; PLAY<$CP; PLAY++ ))
    do 
      RUNTIMEOUTPUT="$RESULTDIR/fixedparty$BANK.$PROG.p$PLAY.runtime.dat"
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


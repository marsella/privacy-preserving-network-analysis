echo "hello we are testing on data now"

TIME="/usr/local/bin/time -v"
PROG=noe
PYTHON="/usr/bin/env python"
BANK=21

# generate setup files
cd SCALE-MAMBA
rm -r ./Data
mv ~/src/Data .
FILE="Data/NetworkData.txt"
echo "RootCA.crt" > $FILE
echo $BANK        >> $FILE
for (( PLAY=0; PLAY<$BANK; PLAY++ )); do
  echo "$PLAY 127.0.0.1 Player$PLAY.crt player$PLAY@example.com" >> $FILE
done
echo 0 >> $FILE # real offline data
echo 0 >> $FILE # real sacrifice data

# TODO update this for new setup scripts
echo "2 1 123" | ./Setup.x

# add retries to try and avoid persistant mac fails
for _ in [1,..,100]; do
  # compile the program, collect stats
  COMPILE_OUTPUT="/tmp/output$BANK.dat"
  $TIME $PYTHON compile.py Programs/$PROG >> $COMPILE_OUTPUT 2>&1

  # execute the program
  for (( PLAY=0; PLAY<$BANK; PLAY++ ))
  do 
    RUNTIMEOUTPUT="/tmp/output$BANK.p$PLAY.runtime.dat"
    >$RUNTIMEOUTPUT # delete old logs
    ($TIME ./Player.x $PLAY ./Programs/$PROG & ) >> $RUNTIMEOUTPUT 2>&1
  done

  # wait for players to finish executing
  # TODO maybe add a condition that if the thing outputs "End of prog" you should stop
  while (ps aux | grep "Player.x" | grep -v grep >/dev/null); do
    sleep 2
  done

  # stop if no mac fails
  if grep -L mac_fail $RUNTIMEOUTPUT; then
    break
  fi
done

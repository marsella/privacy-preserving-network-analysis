

# To do list for emp benchmarking
# 1. build circuit in emp-sh2pc 
# 2. copy circuit to shared directory
# 3. update bench_mpc
#    - change nP line
#    - change circuit line to use correct circuit (maybe just rewrite to use nP var)
# 4. make (is it already in the build file?)
# 5. execute all parties

TIME="/usr/local/bin/time -v"
cd /root/emp-agmpc/build
cmake ..

for BANKS in 3 ; do
  echo $BANKS banks
  ps aux | grep bench_mpc | awk '{print $2}' | xargs kill # remove old processes

  cd /root/emp-sh2pc/build
  ./bin/EGJ -m $BANKS
  mv $BANKS"EGJ.circuit.txt" /usr/local/include/emp-tool/circuits/files/

  cd /root/emp-agmpc
  sed -i "s/const static int nP = [[:digit:]]\+;/const static int nP = $BANKS;/" test/bench_mpc.cpp
  cd build
  make bench_mpc
  
  for (( PLAY=1; PLAY<=$BANKS; PLAY++ )); do
    OUTPUT="/root/results/output$BANKS.p$PLAY.dat"
    ($TIME ./bin/bench_mpc $PLAY 12345 &) >> $OUTPUT 2>&1
  done

  # wait for parties to finish
  # todo: add a cancel condition if they all stop trying for some reason
  while (ps aux | grep "bin/bench_mpc" | grep -v grep >/dev/null); do
    sleep 2
  done
done


#!/bin/bash

port=12345

for ((nP=2;nP<5;nP++))
do
	FILE="/usr/local/include/emp-tool/circuits/files/${nP}EGJ.circuit.txt"
	if [ ! -f $FILE ]
	then
		pushd /root/emp-sh2pc/build
		./bin/EGJ -m $nP
		mv *circuit.txt /usr/local/include/emp-tool/circuits/files/
		popd
	fi

	if [ ! -f $FILE ]
	then
		echo "Failed to create circuit!"
	fi

	sed -i "s/const int nP = [[:digit:]]\+/const int nP = $nP/" ../test/bench_EGJ.cpp 
	make

	for ((i=1;i < nP; i++ ))
	do
		./bin/bench_EGJ $i $port &
	done

	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes --track-origins=yes  ./bin/bench_EGJ $nP $port 2> mem_err.txt

	killall bench_EGJ
done

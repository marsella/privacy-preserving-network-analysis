#!/bin/bash

CIRCUITCSV="EGJ_circuits.csv"
if [ ! -f "$CIRCUITCSV" ]
then
	echo "n,num_ands,num_gates" > "$CIRCUITCSV"
fi

for ((n=3; n<20;n++))
do
	CIRCUITFILE="${n}.EGJ.circuit.txt"
	#sed -i "s/int FixedP::denombits = [[:digit:]]\+;/int FixedP::denombits = $DLEGJ;/" ../test/EGJ.cpp
	#sed -i "s/int FixedP::bitlen = [[:digit:]]\+;/int FixedP::bitlen = $BITLEGJ;/" ../test/EGJ.cpp

	make
	./bin/EGJ -m $n
	num_gates=`head -n 1 $CIRCUITFILE | awk '{print $1}'`
	num_ands=`grep AND $CIRCUITFILE | wc -l`
	rm $CIRCUITFILE

	echo "$n,$num_ands,$num_gates" >> "$CIRCUITCSV"
done

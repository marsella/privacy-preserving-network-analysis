#!/bin/bash

CIRCUITCSV="EN_circuits.csv"
if [ ! -f "$CIRCUITCSV" ]
then
	echo "n,num_ands,num_gates" > "$CIRCUITCSV"
fi

for ((n=3; n<20;n++))
do
	CIRCUITFILE="${n}.EN.circuit.txt"
	#sed -i "s/int FixedP::denombits = [[:digit:]]\+;/int FixedP::denombits = $DLEN;/" ../test/EN.cpp
	#sed -i "s/int FixedP::bitlen = [[:digit:]]\+;/int FixedP::bitlen = $BITLEN;/" ../test/EN.cpp

	make
	./bin/EN -m $n
	num_gates=`head -n 1 $CIRCUITFILE | awk '{print $1}'`
	num_ands=`grep AND $CIRCUITFILE | wc -l`

	echo "$n,$num_ands,$num_gates" >> "$CIRCUITCSV"
done

#!/bin/bash

DEBUG="OFF" #Change to ON to allow debugging

#Generate the clearing circuit
cd ~/emp-sh2pc/build
#for i in {5..15..5}
#do
	#
	#./bin/EN -m $i
	#./bin/EGJ -m $i
#done
#mv *.circuit.txt /usr/local/include/emp-tool/circuits/files/

pushd /root/emp-sh2pc/build
for ((nP=2;nP<5;nP++))
do
	python geninput.py -e EN -n $nP
	./bin/EN -m $nP
	FILE="/usr/local/include/emp-tool/circuits/files/${nP}EGJ.circuit.txt"
	if [ ! -f $FILE ]
	then
		./bin/EGJ -m $nP
		mv *circuit.txt /usr/local/include/emp-tool/circuits/files/
	fi

	if [ ! -f $FILE ]
	then
		echo "Failed to create circuit! $nP"
	fi
done
popd

if [ ${DEBUG} == "ON" ]
then
	mv ~/source/vbenchEGJ.sh ~/emp-agmpc/build/ #script with valgrind
else
	mv ~/source/benchEGJ.sh ~/emp-agmpc/build/ #Script without valgrind
fi

#Copy test circuits to circuit directory
cd ~/emp-agmpc/build
mv ../test/*.txt /usr/local/include/emp-tool/circuits/files/

if [ "${DEBUG}" == "ON" ]
then
	#libc debug
	apt-get install libc6-dbg
	#wget
	apt-get install wget

	#Install valgrind
	pushd ~
	wget https://sourceware.org/pub/valgrind/valgrind-3.15.0.tar.bz2
	tar jxf valgrind-3.15.0.tar.bz2
	cd valgrind-3.15.0
	#wget http://www.valgrind.org/downloads/valgrind-3.14.0.tar.bz2
	#tar jxf valgrind-3.14.0.tar.bz2
	#cd valgrind-3.14.0
	./configure
	make
	make install
	popd

	#DEBUG MODE
#	sed -i "s/CMAKE_BUILD_TYPE:STRING=.*/CMAKE_BUILD_TYPE:STRING=Debug/" ~/emp-tool/CMakeCache.txt
#	sed -i "s/CMAKE_BUILD_TYPE:STRING=.*/CMAKE_BUILD_TYPE:STRING=Debug/" ~/relic/CMakeCache.txt
#	sed -i "s/CMAKE_BUILD_TYPE:STRING=.*/CMAKE_BUILD_TYPE:STRING=Debug/" ~/emp-ot/CMakeCache.txt
#	sed -i "s/CMAKE_BUILD_TYPE:STRING=.*/CMAKE_BUILD_TYPE:STRING=Debug/" ~/emp-agmpc/build/CMakeCache.txt
#	sed -i "s/USE_RANDOM_DEVICE:BOOL=OFF/USE_RANDOM_DEVICE:BOOL=ON/" ~/emp-ot/CMakeCache.txt #Valgrind chokes when this is off
#	sed -i "s/USE_RANDOM_DEVICE:BOOL=OFF/USE_RANDOM_DEVICE:BOOL=ON/" ~/emp-tool/CMakeCache.txt
#	sed -i "s/USE_RANDOM_DEVICE:BOOL=OFF/USE_RANDOM_DEVICE:BOOL=ON/" ~/emp-agmpc/build/CMakeCache.txt
	sed -i "s/OPTION(USE_RANDOM_DEVICE \"Option description\" OFF)/OPTION(USE_RANDOM_DEVICE \"Option description\" ON)/" ~/emp-tool/cmake/source_of_randomness.cmake
#	sed -i "s/set(CMAKE_C_FLAGS \"-pthread -Wall -march=native -O3 -maes -mrdseed\")/set(CMAKE_C_FLAGS \"-pthread -Wall -march=native -O3 -mno-aes -mrdseed\")/" ~/emp-tool/cmake/common.cmake
	sed -i "s/set(CMAKE_C_FLAGS \"-pthread -Wall -march=native -O3 -maes -mrdseed\")/set(CMAKE_C_FLAGS \"-pthread -Wall -march=native -O3 -maes -mrdseed -mno-rdrnd\")/" ~/emp-tool/cmake/common.cmake
	sed -i "s/set(CMAKE_BUILD_TYPE Release)/set(CMAKE_BUILD_TYPE Debug)/" ~/emp-tool/cmake/common.cmake

	pushd ~/emp-tool/
	cmake .
	make
	make install
	popd

	pushd ~/emp-ot/
	cmake .
	make
	make install
	popd

	pushd ~/relic/
	make
	make install
	popd

	pushd ~/emp-agmpc/build
	cmake .
	make
	popd
fi


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

# get readme scripts
git clone https://github.com/emp-toolkit/emp-readme.git

#Add relic packages for ECDSA signatures
#sed -i 's/-DWITH="MD;DV;BN;FB;EB;EC"/-DWITH="MD;DV;BN;FB;EB;EC;FP;BC;CP"/' ~/emp-readme/scripts/install_relic.sh

# install tools
python3 ./emp-readme/scripts/install.py --tool --ot --sh2pc
#apt-get install -y libboost-{chrono,log,program-options,date-time,thread,system,filesystem,regex,test}1.58-dev

#bash ./emp-readme/scripts/install_relic.sh
#bash ./emp-readme/scripts/install_emp-tool.sh
#bash ./emp-readme/scripts/install_emp-ot.sh

# install semi-honest 2-party circuits
#git clone https://github.com/emp-toolkit/emp-sh2pc.git

# add our files to compile scripts
#cp source/* emp-sh2pc/test/
cd 
cp ~/source/geninput.py ~/emp-sh2pc/
cp ~/source/EGJ_circuit_sizes.sh ~/emp-sh2pc/
cp ~/source/EN_circuit_sizes.sh ~/emp-sh2pc/
cp ~/source/benchEGJ.sh ~/emp-sh2pc/
cp ~/source/EN.cpp ~/emp-sh2pc/test/
cp ~/source/fixedp.h ~/emp-sh2pc/test/
cp ~/source/matrix.h ~/emp-sh2pc/test/
cp ~/source/EGJ.cpp ~/emp-sh2pc/test/
cp ~/source/bench_EGJ.cpp ~/emp-sh2pc/test/

cd emp-sh2pc
echo "add_test (EN)" >> CMakeLists.txt
echo "add_test (EGJ)" >> CMakeLists.txt
echo "add_test (bench_EGJ)" >> CMakeLists.txt

# build project
mkdir build
cd build
cmake ..
make
#make install

pushd /root/emp-sh2pc/build
for ((nP=2;nP<5;nP++))
do
	python3 geninput.py -e EN -n $nP
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


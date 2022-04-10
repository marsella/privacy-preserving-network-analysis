# get readme scripts
git clone https://github.com/emp-toolkit/emp-readme.git

# install tools
bash ./emp-readme/scripts/install_packages.sh
apt-get install -y libboost-{chrono,log,program-options,date-time,thread,system,filesystem,regex,test}1.58-dev
apt-get install -y python3-pip
apt-get install -y gdb

apt upgrade --fix-missing -y && apt autoremove -y && apt autoclean -y

bash ./emp-readme/scripts/install_relic.sh
bash ./emp-readme/scripts/install_emp-tool.sh
bash ./emp-readme/scripts/install_emp-ot.sh

# install semi-honest 2-party circuits
git clone https://github.com/emp-toolkit/emp-sh2pc.git

# install numpy
pip install --upgrade pip
pip install numpy scipy

pip3 install --upgrade pip
pip3 install numpy scipy

# add our files to compile scripts
cd 
cp ~/source/EN.cpp ~/emp-sh2pc/test/
cp ~/source/EGJ.cpp ~/emp-sh2pc/test/
cp ~/source/matrix.h ~/emp-sh2pc/test/
cp ~/source/fixedp.h ~/emp-sh2pc/test/

cd emp-sh2pc
echo "add_test (EN)" >> CMakeLists.txt
echo "add_test (EGJ)" >> CMakeLists.txt
sed -i '/add_test (circuit_file)/d' CMakeLists.txt
sed -i '/add_test (example)/d' CMakeLists.txt
sed -i '/add_test (float)/d' CMakeLists.txt
sed -i '/add_test (int)/d' CMakeLists.txt
sed -i '/add_test (bit)/d' CMakeLists.txt

# build project
mkdir build
cd build
cmake ..
make

cp ~/source/geninput.py ~/emp-sh2pc/build/
cp ~/source/runtest.sh ~/emp-sh2pc/build/
python3 geninput.py -e EGJ -n 5
python3 geninput.py -e EN -n 5

# install malicious 2-party circuits
cd
git clone https://github.com/emp-toolkit/emp-ag2pc.git
#Fix space in error message
sed -i 's/{cout <<ands <<"no match GT!"<<endl<<flush;/{cout <<ands <<" no match GT!"<<endl<<flush;/' ~/emp-ag2pc/emp-ag2pc/2pc.h
cd emp-ag2pc
mkdir build
cd build
cmake ..
make 

# install multiparty malicious platform
cd 
git clone https://github.com/emp-toolkit/emp-agmpc.git

#Fix space in error message
sed -i 's/{cout <<ands <<"no match GT!"<<endl<<flush;/{cout <<ands <<" no match GT!"<<endl<<flush;/' ~/emp-agmpc/emp-agmpc/mpc.h
#Remove old tests from cmake
sed -i '/add_test (test_mpc)/d' ~/emp-agmpc/CMakeLists.txt
sed -i '/add_test (test_mpc_individual)/d' ~/emp-agmpc/CMakeLists.txt
sed -i '/add_test (aes)/d' ~/emp-agmpc/CMakeLists.txt
sed -i '/add_test (sha1)/d' ~/emp-agmpc/CMakeLists.txt
sed -i '/add_test (sha256)/d' ~/emp-agmpc/CMakeLists.txt
sed -i '/add_test (triple)/d' ~/emp-agmpc/CMakeLists.txt

# add our test to cmake
printf "add_test (bench_mpc)\n" >> ~/emp-agmpc/CMakeLists.txt
printf "add_test (bench_EGJ)\n" >> ~/emp-agmpc/CMakeLists.txt
mv ~/source/bench_mpc.cpp ~/emp-agmpc/test/
mv ~/source/bench_EGJ.cpp ~/emp-agmpc/test/

cd ~/emp-agmpc
mkdir build && cd build
cmake ..
make

mv ~/source/cmpc_config.h ~/emp-agmpc/emp-agmpc/ #IP addresses for the parties

#EC STRING SIZE
sed -i "s/FB_POLYN:STRING=283/FB_POLYN:STRING=251/" ~/relic/CMakeCache.txt

# install time command to get RAM usage
cd
wget https://ftp.gnu.org/gnu/time/time-1.9.tar.gz
tar xzvf time-1.9.tar.gz
rm time-1.9.tar.gz
cd time-1.9
./configure && make && make install

# add vimrc
cat <<EOT >> ~/.vimrc
syntax on
set nowrap
set showmatch
set tabstop=4
set shiftwidth=4
set autoindent
set smartindent
EOT

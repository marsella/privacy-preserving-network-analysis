apt-get upgrade --fix-missing -y && apt-get autoremove -y && apt-get autoclean -y

# install pip and numpy
#wget https://bootstrap.pypa.io/pip/3.5/get-pip.py
#python3 get-pip.py
pip3 install --upgrade pip
pip3 install numpy scipy

# get readme scripts
git clone https://github.com/emp-toolkit/emp-readme.git
# install tools
python3 ./emp-readme/scripts/install.py --deps --tool --ot --sh2pc

#pip3 install --upgrade pip
#pip3 install numpy scipy

# add our files to compile scripts
cd 
cp ~/source/EN.cpp ~/emp-sh2pc/test/
cp ~/source/EGJ.cpp ~/emp-sh2pc/test/
cp ~/source/matrix.h ~/emp-sh2pc/test/
cp ~/source/fixedp.h ~/emp-sh2pc/test/
cp ~/source/EN_circuit_sizes.sh ~/emp-sh2pc/

cd emp-sh2pc/test
echo "add_test_case(EN)" >> CMakeLists.txt
echo "add_test_case(EGJ)" >> CMakeLists.txt
sed -i '/add_test_case_with_run(repeat)/d' CMakeLists.txt
sed -i '/add_test_case_with_run(circuit_file)/d' CMakeLists.txt
sed -i '/add_test_case_with_run(example)/d' CMakeLists.txt
sed -i '/add_test_case_with_run(float)/d' CMakeLists.txt
sed -i '/add_test_case_with_run(int)/d' CMakeLists.txt
sed -i '/add_test_case_with_run(bit)/d' CMakeLists.txt

#Use emp-sh2pc to build the files in the test cases (in this case EN and EGJ)
cd ..
cmake test
cd test
make

cd ..
mkdir build
cd build

cp ~/source/geninput.py .
cp ~/source/runtest.sh .
cp ~/source/EN_circuit_sizes.sh .
cp ~/source/EGJ_circuit_sizes.sh .
chmod +x EN_circuit_sizes.sh
chmod +x EGJ_circuit_sizes.sh
python3 geninput.py -e EGJ -n 5
python3 geninput.py -e EN -n 5
cp ~/emp-sh2pc/bin/test_EN EN
cp ~/emp-sh2pc/bin/test_EGJ EGJ

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


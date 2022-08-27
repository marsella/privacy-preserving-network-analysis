# install python packages
pip install --upgrade pip
pip install numpy

# install supporting mpir library
wget http://mpir.org/mpir-3.0.0.tar.bz2
tar -xvf mpir-3.0.0.tar.bz2
rm mpir-3.0.0.tar.bz2
cd mpir-3.0.0
./configure --enable-cxx
make && make check && make install
cp .libs/* /usr/lib64/

# install openssl from source because apparently apt's version is not good
# enough
cd
wget https://www.openssl.org/source/openssl-1.1.0h.tar.gz
tar -xvf openssl-1.1.0h.tar.gz
rm openssl-1.1.0h.tar.gz
cd openssl-1.1.0h
./config
make &&  make install
ldconfig 

# install time command to get RAM usage
cd
wget https://ftp.gnu.org/gnu/time/time-1.9.tar.gz
tar xzvf time-1.9.tar.gz
rm time-1.9.tar.gz
cd time-1.9
./configure && make && make install

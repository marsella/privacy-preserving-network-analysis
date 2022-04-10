# download + update with our minor fixes
git clone https://github.com/PICCO-Team/picco.git
cd picco
git checkout ee85c91 
cp ../picco.patch picco/
patch -p1 -i picco.patch

# install 
cd compiler
mkdir bin
./compile.sh
cp bin/* /usr/bin

# generate keys
mkdir /root/keys
cd /root/keys
for ID in {1..5}
do
  openssl genrsa -out private$ID.pem
  openssl rsa -in private$ID.pem -outform PEM -pubout -out pubkey$ID.pem
done

# compile our tests

cd ~/source
mkdir data
mv geninput.py data/
mkdir secure_server
mv run_config secure_server/
# link to shared resources
ln -s /root/keys secure_server/keys 
ln -s /root/picco/compute/smc-compute smc-compute

picco noe.cpp smc_config noe.gen util_config
make noe

cd 
git clone https://github.com/marsella/vim.git
mv vim/.vimrc .


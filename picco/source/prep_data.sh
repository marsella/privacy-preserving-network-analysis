cd data
python geninput.py
cd ..
picco-utility -I 1 data/bank0.dat util_config secure_server/share1-
picco-utility -I 2 data/bank1.dat util_config secure_server/share2-
picco-utility -I 3 data/bank2.dat util_config secure_server/share3-
picco-utility -I 4 data/bank3.dat util_config secure_server/share4-
picco-utility -I 5 data/bank4.dat util_config secure_server/share5-

EVAL="$1"
[ -z $EVAL ] && EVAL="noe"
echo $EVAL

cd secure_server
./$EVAL 5 run_config keys/private5.pem 5 1 share1-5 share2-5 share3-5 share4-5 share5-5 output &
sleep 1
./$EVAL 4 run_config keys/private4.pem 5 1 share1-4 share2-4 share3-4 share4-4 share5-4 output &
sleep 2
./$EVAL 3 run_config keys/private3.pem 5 1 share1-3 share2-3 share3-3 share4-3 share5-3 output &
sleep 3
./$EVAL 2 run_config keys/private2.pem 5 1 share1-2 share2-2 share3-2 share4-2 share5-2 output &
sleep 4
./$EVAL 1 run_config keys/private1.pem 5 1 share1-1 share2-1 share3-1 share4-1 share5-1 output &
sleep 5

picco-seed run_config ../util_config

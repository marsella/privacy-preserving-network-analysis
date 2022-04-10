# set up 4 participants
echo "RootCA.crt
4
0 127.0.0.1 Player0.crt player0@example.com
1 127.0.0.1 Player1.crt player1@example.com
2 127.0.0.1 Player2.crt player2@example.com
3 127.0.0.1 Player3.crt player3@example.com
1
1 " > Data/NetworkData.txt

echo "2 1 123" | ./Setup.x

PROG=noe
./compile.py Programs/$PROG


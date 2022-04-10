#include "emp-sh2pc/emp-sh2pc.h"
#include "math.h"
using namespace emp;
using namespace std;

int BITLEN = 12;

void run_tests(){

	Integer a( BITLEN, 1, ALICE );
	Integer b( 2*BITLEN, 5, BOB );
	Integer c( 2*BITLEN, 3, PUBLIC );

	c = a + b;
	cout << "Testing adding ints of different bit lengths:" << c.reveal<int>() << endl;
	

	Integer d;
	Integer a2( 3, 5, ALICE );
	Integer a3( 4, 3, ALICE );
	d = a2*a3;
	cout << "product of different bitlengths: " << d.reveal<int>() << endl;
	cout << "d.size() = " << d.size() << endl;
	d = a3*a2;
	cout << "product of different bitlengths (other order): " << d.reveal<int>() << endl;
	cout << "d.size() = " << d.size() << endl;

	d = a*a;
	cout << "Testing squaring: " << d.reveal<int>() << endl;
	cout << "d.size() = " << d.size() << endl;

	d = -a;
	cout << "Testing negation: " << d.reveal<int>() << endl;

	Bit b_one(1,PUBLIC);
	Bit b_zero(0,PUBLIC);
	Integer z(BITLEN,0,PUBLIC);
	d = z.select(b_one,b);
	cout << "Testing bit(1)*int: " << d.reveal<int>() << endl;
	d = z.select(b_zero,b);
	cout << "Testing bit(0)*int: " << d.reveal<int>() << endl;

	d = Integer(b.size(),0,PUBLIC);	
	for( int i=0; i<b.size(); i++ ) {
		d[i] = b_zero&b[i];
	}
	cout << "Testing bit(0)&int: " << d.reveal<int>() << endl;

	d = Integer(b.size(),0,PUBLIC);	
	for( int i=0; i<b.size(); i++ ) {
		d[i] = b_one&b[i];
	}
	cout << "Testing bit(1)&int: " << d.reveal<int>() << endl;

	d = Integer(1,1,PUBLIC);
	cout << "Testing one-bit int: " << d.reveal<int>() << endl;

	d = Integer(2,1,PUBLIC);
	cout << "Testing two-bit int: " << d.reveal<int>() << endl;
		
	c = d*b;
	cout << "Testing multiplication wraparound: " << c.reveal<int>() << endl;
	cout << "c.size() = " << c.size() << endl;

	c = b*d;
	cout << "Testing multiplication wraparound: " << c.reveal<int>() << endl;
	cout << "c.size() = " << c.size() << endl;

	a = Integer(16,3);
	b = Integer(16,7);
	d = Integer(16,31);
	c = a.modExp(b,d);
	cout << "Testing modExp" << endl;
	cout << a.reveal<int>() << "^" << b.reveal<int>() << "(mod " << d.reveal<int>() << ") = " << c.reveal<int>() << endl; 

}

int main(int argc, char** argv) {
    
    // run computation with semi-honest model
    int port, party;
    parse_party_and_port(argv, &party, &port);
    NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

    setup_semi_honest(io, party);


    run_tests();
    delete io;
}


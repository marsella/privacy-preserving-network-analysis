#include "emp-sh2pc/emp-sh2pc.h"
#include "math.h"
using namespace emp;
using namespace std;

int BITLEN = 32;

void matmul( Integer A[], Integer B[], Integer C[], int n, int m, int p );


//Multiply (n x m) by (m x p) matrix
void matmul( Integer A[], Integer B[], Integer C[], int n, int m, int p ) {

	Integer t;
	Integer T[n*p];
	
	for( int i=0; i<n; i++ ) {
		for( int j=0; j<p; j++ ) {
			T[i*p+j] = Integer(BITLEN,0,PUBLIC);
			for( int k=0; k<m; k++ ) {
				t = A[i*m+k]*B[j+k*p];
				T[i*p+j] = T[i*p+j] + t;
			}
		}
	}
	for( int i=0; i<n*p; i++ ){
		C[i] = T[i];
	}
}

void plain_matmul( int A[], int B[], int C[], int n, int m, int p ) {

	int t;
	int T[n*p];
	
	for( int i=0; i<n; i++ ) {
		for( int j=0; j<p; j++ ) {
			T[i*p+j] = 0;
			for( int k=0; k<m; k++ ) {
				t = A[i*m+k]*B[j+k*p];
				T[i*p+j] = T[i*p+j] + t;
			}
		}
	}
	for( int i=0; i<n*p; i++ ){
		C[i] = T[i];
	}
}

void test_matmul(int party){
	
	int n,m,p;

	n = 5;
	m = 5;
	p = 5;

	int a[n*m];
	int b[m*p];
	int c[n*p];

	Integer A[n*m];
	Integer B[m*p];
	Integer C[n*p];

	for( int i=0; i < n*m; i++ ) {
		a[i] = rand();
		A[i] = Integer( BITLEN, a[i], ALICE );
	}
	for( int i=0; i < m*p; i++ ) {
		b[i] = rand();
		B[i] = Integer( BITLEN, b[i], BOB );
	}

	plain_matmul( a, b, c, n, m, p );
	matmul( A, B, C, n, m, p );
	plain_matmul( c, b, c, n, m, p );
	matmul( C, B, C, n, m, p );
	plain_matmul( c, b, c, n, m, p );
	matmul( C, B, C, n, m, p );


	cout << "Truth : [ ";
	for(int i=0; i<n*p; i++) {
		cout << c[i] << " " ;
	}
	cout << "]" << endl;
	cout << "Secure : [ ";
	for(int i=0; i<n*p; i++) {
		cout << C[i].reveal<int>() << " " ;
	}
	cout << "]" << endl;
}


int main(int argc, char** argv) {
    // run computation with semi-honest model
    int port, party;
    parse_party_and_port(argv, &party, &port);
    NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

    setup_semi_honest(io, party);

    test_matmul(party);
    delete io;
}


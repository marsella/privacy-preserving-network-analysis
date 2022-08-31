/*
Test suite for the matrix.h library that drives the matrix arithmetic in EN and EGJ
*/


#include "emp-sh2pc/emp-sh2pc.h"
#include "math.h"
#include "matrix.h"
using namespace emp;
using namespace std;

int FixedP::denombits = 16;
int FixedP::bitlen = 64;
int BITLEN = 32;

void test_diagonal(int party){
	int n,m;

	n = 5;
	m = 2;

	double a[n];
	double b[n*m];
	//int c[n*p];

	FixedP A[n];
	FixedP B[n*m];
	FixedP C[n*m];

	for( int i=0; i < n; i++ ) {
		a[i] = rand()%10;
		A[i] = FixedP( a[i], ALICE );
	}
	for( int i=0; i < n*m; i++ ) {
		b[i] = rand()%10;
		B[i] = FixedP( b[i], BOB );
	}

	Matrix MA;
	MA = diagonalmatrix( A, n );
	Matrix MB( B, n, m );

	Matrix MC;

	cout << "==================MA====================" << endl;
	MA.print();
	cout << "==================MA====================" << endl;

	cout << "==================MB====================" << endl;
	MB.print();
	cout << "==================MB====================" << endl;
	
	MC = MA + MB;
	cout << "==================MA+MB====================" << endl;
	MC.print();
	cout << "==================MA+MB====================" << endl;

	cout << "==================MA====================" << endl;
	MA.print();
	cout << "==================MA====================" << endl;

	cout << "==================MB====================" << endl;
	MB.print();
	cout << "==================MB====================" << endl;
	
	MC = MA * MB;
	cout << "==================MA*MB====================" << endl;
	MC.print();
	cout << "==================MA*MB====================" << endl;

	MC = MA * MB;
	cout << "==================MA*MB====================" << endl;
	MC.print();
	cout << "==================MA*MB====================" << endl;

}

void test_matadd(int party){
	
	int n,m;

	n = 5;
	m = 2;

	double a[n*m];
	double b[n*m];
	//int c[n*p];

	FixedP A[n*m];
	FixedP B[n*m];
	FixedP C[n*m];

	for( int i=0; i < n*m; i++ ) {
		a[i] = rand()%10;
		A[i] = FixedP( a[i], ALICE );
		b[i] = rand()%10;
		B[i] = FixedP( b[i], BOB );
	}

	Matrix MA( A, n, m );
	Matrix MB( B, n, m );

	Matrix MC;

	cout << "==================MA====================" << endl;
	MA.print();
	cout << "==================MA====================" << endl;

	cout << "==================MB====================" << endl;
	MB.print();
	cout << "==================MB====================" << endl;
	
	MC = MA + MB;
	cout << "==================MA+MB====================" << endl;
	MC.print();
	cout << "==================MA+MB====================" << endl;

	MC = MA + MA;
	cout << "==================MA+MA====================" << endl;
	MC.print();
	cout << "==================MA+MA====================" << endl;

	MC = MB + MB;
	cout << "==================MB+MB====================" << endl;
	MC.print();
	cout << "==================MB+MB====================" << endl;

	MC = MB + MA;
	cout << "==================MB+MA====================" << endl;
	MC.print();
	cout << "==================MB+MA====================" << endl;

	MC = MA + MB;
	cout << "==================MA+MB====================" << endl;
	MC.print();
	cout << "==================MA+MB====================" << endl;

}

void test_matmul(int party){
	
	int n,m,p;

	n = 5;
	m = 5;
	p = 5;

	double a[n*m];
	double b[m*p];
	//int c[n*p];

	FixedP A[n*m];
	FixedP B[m*p];
	FixedP C[n*p];

	for( int i=0; i < n*m; i++ ) {
		a[i] = rand()%10;
		A[i] = FixedP( a[i], ALICE );
	}
	for( int i=0; i < m*p; i++ ) {
		b[i] = rand()%10;
		B[i] = FixedP( b[i], BOB );
	}

	Matrix MA( A, n, m );
	Matrix MB( B, m, p );

	Matrix MC;

	cout << "==================MA====================" << endl;
	MA.print();
	cout << "==================MA====================" << endl;

	cout << "==================MB====================" << endl;
	MB.print();
	cout << "==================MB====================" << endl;
	
	MC = MA + MB;
	cout << "==================MA+MB====================" << endl;
	MC.print();
	cout << "==================MA+MB====================" << endl;

	MC = MA * MB;
	cout << "==================MA*MB====================" << endl;
	MC.print();
	cout << "==================MA*MB====================" << endl;

}

void test_det(int party){
	
	int n;

	n = 5;

	double a[n*n];

	FixedP A[n*n];

	for( int i=0; i < n*n; i++ ) {
		a[i] = rand()%10;
		A[i] = FixedP( a[i], ALICE );
	}

	Matrix MA( A, n, n );
	FixedP det;

	det = MA.determinant();

	cout << "==================MA====================" << endl;
	MA.print();
	cout << "==================MA====================" << endl;

	cout << "det(A) = ";
	det.print();
	cout << endl; 

}

void test_adjugate(int party){
	
	int n;

	n = 6;

	double a[n*n];

	FixedP A[n*n];

	for( int i=0; i < n*n; i++ ) {
		a[i] = rand()%10;
		A[i] = FixedP( a[i], ALICE );
	}

	Matrix MA( A, n, n );
	Matrix MB;

	MB = MA.adjugate();

	cout << "==================MA====================" << endl;
	MA.print();
	cout << "==================MA====================" << endl;
	cout << "==================MB====================" << endl;
	MB.print();
	cout << "==================MB====================" << endl;


}

void test_transpose(int party){
	
	int n;

	n = 3;

	double *a;
	FixedP *A;

	a = new double[n*n];
	A = new FixedP[n*n];

	for( int i=0; i < n*n; i++ ) {
		a[i] = rand()%10;
		A[i] = FixedP( a[i], ALICE );
	}

	Matrix MA( A, n, n );

	cout << "==================MA====================" << endl;
	MA.print();
	cout << "==================MA====================" << endl;
	Matrix MB;

	MB = MA.transpose();
	cout << "==================MB====================" << endl;
	MB.print();
	cout << "==================MB====================" << endl;

	delete a;
	delete A;

	a = new double[n];
	A = new FixedP[n];

	for( int i=0; i < n; i++ ) {
		a[i] = rand()%10;
		A[i] = FixedP( a[i], ALICE );
	}

	MA = diagonalmatrix( A, n );

	cout << "==================MA====================" << endl;
	MA.print();
	cout << "==================MA====================" << endl;

	MB = MA.transpose();
	cout << "==================MB====================" << endl;
	MB.print();
	cout << "==================MB====================" << endl;


}

int main(int argc, char** argv) {
    // run computation with semi-honest model
    int port, party;
    parse_party_and_port(argv, &party, &port);
    NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

    setup_semi_honest(io, party);
	//test_matmul(party);
	//test_diagonal(party);
	test_matadd(party);
	//test_det(party);
	//test_adjugate(party);
	//test_transpose(party);
	//test_minor(party);
    delete io;
}


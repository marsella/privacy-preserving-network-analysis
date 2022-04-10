#include "emp-sh2pc/emp-sh2pc.h"
#include "math.h"
#include "matrix.h"
using namespace emp;
using namespace std;

int FixedP::denombits = 8;
int FixedP::bitlen = 32;

Matrix calculateclearing( Matrix PiT, Matrix e, Matrix pbar );
void calculateclearing( int n );
Matrix updatelambda( Matrix Lambda, Matrix PiT, Matrix e, Matrix pbar, Matrix p );

//Lambda[i] = 1 if bank i has failed, and Lambda[i] = 0 otherwise
//We represent Lambda as a vector of length n instead of an nxn diagonal matrix
Matrix updatelambda( Matrix Lambda, Matrix PiT, Matrix e, Matrix pbar, Matrix p ) {
	Matrix T;

	T = PiT*p;
	T = T + e;
	FixedP one( (double) 1, PUBLIC );
	FixedP newD[T.nr];

	if( T.nc != 1 ) { cout << "updatelambda: ERROR T.nc = " << T.nc << endl; }

	Bit b;
	for( int i=0; i<T.nr; i ++ ) {
		b = T.A[i] < pbar.A[i];
		newD[i] = Lambda.A[i].select( b, one );
	}

	Matrix newLambda = diagonalmatrix( newD, T.nr );
	return( newLambda );
}

//Calculate the clearing vector, p for a given
//Proportional debt matrix Pi^T
//Asset vector e (e[i] = assets of bank i)
//Total debt vector pbar (pbar[i] = total debts of i)
Matrix calculateclearing( Matrix PiT, Matrix e, Matrix pbar ) {

	Matrix A;
	Matrix Lambda = zeromatrix(PiT.nr,PiT.nc); //The diagonal failure matrix (Lambda[i*n+i] = 1 if bank i fails)
	Matrix IDM = identitymatrix( PiT.nr );
	Matrix ILp; //IL*p
	Matrix p = Matrix(pbar); //Clearing vector, initialized to pbar
	Matrix LPiT; //Lambda*PiT
	Matrix B;

	for( int i = 0; i < PiT.nr; i++ ) {
		
		LPiT = Lambda*PiT;

		A = LPiT*Lambda;

		ILp = (IDM-Lambda)*pbar;
		B = LPiT*ILp;
		B = B + Lambda*e;
		B = B + ILp;

		//Find fixed point
		for( int i=0; i<10; i++ ) {
			p = A*p;
			p = p + B;
		}

		//Update Lambda with new failures based on current clearing vector p
		Lambda = updatelambda( Lambda, PiT, e, pbar, p );
	}

	return( p );

}

void test_clearing(int n, string inputs[]) {
	
	FixedP Ipbar[n]; //pbar[i] = total amount owed by i
	FixedP Ip[n]; //Clearing vector (initialized to pbar[i]
	FixedP Ie[n]; //Asset vector
	FixedP IPi[n*n]; //Proportional payout matrix
	FixedP IPiT[n*n]; //Transpose payout matrix

	//Read inputs
	//Each player has n+2 inputs
	//1st input is e[i]
	//2nd input is pbar[i]
	//Last n inputs are the row L[i][j] for j=1...n
	for (int i=0; i < n; i ++ ) {
		Ie[i] = FixedP(inputs[i*(n+2)], ALICE );
	}
	for (int i=0; i <n; i ++ ) {
		Ipbar[i] = FixedP(inputs[i*(n+2)+1], ALICE );
		Ip[i] = Ipbar[i];
	}
	for (int i=0; i< n; i ++) {
		for( int j=0; j < n; j++ ) {
			IPi[i*n+j] = FixedP(inputs[i*(n+2)+j+2],ALICE);
			IPiT[j*n+i] = FixedP(inputs[i*(n+2)+j+2],ALICE);
		}	
	}

	Matrix pbar( Ipbar, n, 1 ); //(nx1) pbar[i] = total amount owed by i
	Matrix p( Ip, n, 1 ); //(nx1) Clearing vector (initialized to pbar[i]
	Matrix e( Ie, n, 1 ); //(nx1) Asset vector
	Matrix Pi( IPi, n, n ); //(nxn) Proportional payout matrix
	Matrix PiT( IPiT, n, n ); //(nxn) Transpose payout matrix

	p = calculateclearing( PiT, e, pbar );

	/*
	cout << "Pi = " << endl;
	Pi.print();
	cout << endl << "=================" << endl;
	cout << "e = " << endl;
	e.print();
	cout << endl << "=================" << endl;
	cout << "pbar = " << endl;
	pbar.print();
	cout << endl << "=================" << endl;
	cout << "p = " << endl;
	p.print();
	*/

    char fname[90];
    sprintf(fname, "./data/EN/%d.truep.dat", n);
    ifstream trueoutputs(fname);
	string truep_str[n];
    if( trueoutputs.is_open() ) {
        for( int i=0; i<n; i++) {
            getline( trueoutputs, truep_str[i]);
        }
        trueoutputs.close();
    }

	double maxdiff=0;
	double calculatedp[n];
	for( int i=0; i < n; i++ ) {
		calculatedp[i] = (double) p.A[i].x.reveal<long>();
		calculatedp[i] = calculatedp[i] / pow(2,p.A[i].denombits);
		maxdiff = max( maxdiff, abs( calculatedp[i] - atof( truep_str[i].c_str() ) ) );
	}

	cout << "Max difference is " << maxdiff << endl;

	cout << "[ ";
	for( int i=0; i < n; i++ ) {
		printf( "%.2f ", calculatedp[i] );
	}
	cout << "] (Calculated)" << endl;
	cout << "[ ";
	for( int i=0; i < n; i++ ) {
		printf( "%.2f ", atof( truep_str[i].c_str() ) );
	}
	cout << "] (True)" << endl;

}

void setup_clearing( int n ) {
	FixedP Ipbar[n]; //pbar[i] = total amount owed by i
	FixedP Ip[n]; //Clearing vector (initialized to pbar[i]
	FixedP Ie[n]; //Asset vector
	FixedP IPi[n*n]; //Proportional payout matrix
	FixedP IPiT[n*n]; //Transpose payout matrix

	//Read inputs
	//Each player has n+2 inputs
	//1st input is e[i]
	//2nd input is pbar[i]
	//Last n inputs are the row L[i][j] for j=1...n
	for (int i=0; i < n; i ++ ) {
		Ie[i] = FixedP(0, ALICE );
	}
	for (int i=0; i <n; i ++ ) {
		Ipbar[i] = FixedP(0, ALICE );
	}
	for (int i=0; i< n; i ++) {
		for( int j=0; j < n; j++ ) {
			IPi[i*n+j] = FixedP(0,ALICE);
			IPiT[j*n+i] = FixedP(0,ALICE);
		}	
	}
	for (int i=0; i <n; i ++ ) {
		Ip[i] = Ipbar[i];
	}

	Matrix pbar( Ipbar, n, 1 ); //(nx1) pbar[i] = total amount owed by i
	Matrix p( Ip, n, 1 ); //(nx1) Clearing vector (initialized to pbar[i]
	Matrix e( Ie, n, 1 ); //(nx1) Asset vector
	Matrix Pi( IPi, n, n ); //(nxn) Proportional payout matrix
	Matrix PiT( IPiT, n, n ); //(nxn) Transpose payout matrix

	p = calculateclearing( PiT, e, pbar );

}

int main(int argc, char** argv) {
    int n;

    // generate circuit for use in malicious library
    if (argc == 2 && strcmp(argv[1], "-m") == 0 ) {
		n = 5;
		string circuitfilename=to_string(n)+".EN.circuit.txt";
		cout << "Writing to circuit file: " << circuitfilename << endl;
		setup_plain_prot(true,circuitfilename);
		setup_clearing(n);
		finalize_plain_prot();
		return 0;
    }

    // generate circuit for use in malicious library
    if (argc == 3 && strcmp(argv[1], "-m") == 0 ) {
		n = atoi(argv[2]);
		string circuitfilename=to_string(n)+".EN.circuit.txt";
		cout << "Writing to circuit file: " << circuitfilename << endl;
		setup_plain_prot(true,circuitfilename);
		setup_clearing(n);
		finalize_plain_prot();
		return 0;
    }
    
    // run computation with semi-honest model
    int port, party;
    parse_party_and_port(argv, &party, &port);
    NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

    setup_semi_honest(io, party);

    if (argc != 4) {
      cout << "Usage: ./EN <party> <port> <n>" << endl
           << "where <n> is the number of players"
           << endl;
      delete io;
      return 0;
    }

    n = atoi(argv[3]);
	cout << "n = " << n << endl;
    char fname[90];

    sprintf(fname, "./data/EN/%d.dat", n);

    ifstream infile(fname);

	int num_inputs = (n+2)*n;
    string inputs[num_inputs];

    if( infile.is_open() ) {
        for( int i=0; i<num_inputs; i++) {
            getline( infile, inputs[i]);
        }
        infile.close();
		cout << "Read " << num_inputs << " inputs" << endl;
    }
	else {
		cout << "Unable to open " << fname << endl;
		cout << "Using 0 inputs" << endl;
	}

	if( FixedP::bitlen <= 3*FixedP::denombits ) {
		cout << "ERROR! FixedP::bitlen is too small, expect errors" << endl;
		cout << "FixedP::bitlen = " << FixedP::bitlen << endl;
		cout << "FixedP::denombits = " << FixedP::denombits << endl;
	}
	if( FixedP::bitlen > 64 ) {
		cout << "FixedP::bitlen = " << FixedP::bitlen << " > 64" << endl;
		cout << "Outputs may not print correctly" << endl;
	}
    test_clearing(n,inputs);

	/*
    sprintf(fname, "../data/EN/%d.output.dat", n);
    ifstream trueoutputs(fname);
    if( trueoutputs.is_open() ) {
	cout << "GROUND TRUTH" << endl;
	cout << trueoutputs.rdbuf();
	trueoutputs.close();
    }
	*/

    delete io;
}


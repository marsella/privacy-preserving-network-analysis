#include "emp-sh2pc/emp-sh2pc.h"
#include "math.h"
#include "matrix.h"
#include <sys/stat.h>
using namespace emp;
using namespace std;

int FixedP::denombits = 16;
int FixedP::bitlen = 50;

Matrix calculatemarketvalues( Matrix S, Matrix C, Matrix e );
double test_marketvalues(int n, string inputs[]);
void setup_marketvalues( int n );

//Calculate the market-value vector, p for a given
//Cross-holding matrix C
//Asset vector e (e[i] = assets of bank i)
//Self-holding matrix S
Matrix calculatemarketvalues( Matrix S, Matrix C, Matrix e ) {

	Matrix A;
	Matrix IDM = identitymatrix( C.nr );
	A = IDM;
	int num_iterations = 20;
	for( int i=0; i < num_iterations; i ++ ) {
		A = C*A;
		A = A + IDM;
	}
	A = A*e;
	A = S*A;

	return( A );

}

double test_marketvalues(int n, string inputs[]) {
	
	FixedP Ie[n]; //Asset vector
	FixedP IS[n]; //Self-holding vector
	FixedP IC[n*n]; //Cross-holding matrix

	//Read inputs
	//Each player has n+1 inputs
	//1st input is e[i]
	//Last n inputs are the column C[j][i] for j=1...n
	for (int i=0; i < n; i ++ ) {
		Ie[i] = FixedP(inputs[i*(n+1)], ALICE );
	}
	for (int i=0; i< n; i ++) {
		for( int j=0; j < i; j++ ) {
			IC[j*n+i] = FixedP(inputs[i*(n+1)+j+1],ALICE);
		}	
		for( int j=i+1; j < n; j++ ) {
			IC[j*n+i] = FixedP(inputs[i*(n+1)+j+1],ALICE);
		}	
		IC[i*n+i] = FixedP(0,PUBLIC);
		IS[i] = FixedP(inputs[i*(n+1)+i+1],ALICE);
	}

	Matrix e( Ie, n, 1 );
	Matrix C( IC, n, n );
	Matrix S = diagonalmatrix( IS, n );
	Matrix p;

	p = calculatemarketvalues( S,C,e );

    char fname[90];
    sprintf(fname, "./data/EGJ/%d.truev.dat", n);
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

	/*	
	cout << "S = " << endl;
	S.print();
	cout << endl << "=================" << endl;
	cout << "e = " << endl;
	e.print();
	cout << endl << "=================" << endl;
	cout << "C = " << endl;
	C.print();
	cout << endl << "=================" << endl;
	cout << "p = " << endl;
	p.print();
	*/
	
	return( maxdiff );

}

void setup_marketvalues( int n ) {
	FixedP Ie[n]; //Asset vector
	FixedP IS[n]; //Self-holding vector
	FixedP IC[n*n]; //Cross-holding matrix

	//Read inputs
	//Each player has n+1 inputs
	//1st input is e[i]
	//Last n inputs are the column C[j][i] for j=1...n
	for (int i=0; i < n; i ++ ) {
		Ie[i] = FixedP(0, ALICE );
	}
	for (int i=0; i< n; i ++) {
		for( int j=0; j < i; j++ ) {
			IC[j*n+i] = FixedP(0,ALICE);
		}	
		for( int j=i+1; j < n; j++ ) {
			IC[j*n+i] = FixedP(0,ALICE);
		}	
		IS[i] = FixedP(0,ALICE);
	}

	for (int i=0; i< n; i ++) {
		IC[i*n+i] = FixedP(0,PUBLIC);
	}

	Matrix e( Ie, n, 1 );
	Matrix C( IC, n, n );
	Matrix S = diagonalmatrix( IS, n );
	Matrix p;

	p = calculatemarketvalues( S,C,e );

	for( int i=0; i<n; i++ ) {
		p.A[i].x.reveal<long>();
	}

}

int main(int argc, char** argv) {
    int n;

    // generate circuit for use in malicious library
    if (argc == 2 && strcmp(argv[1], "-m") == 0 ) {
		n = 5;
		string circuitfilename=to_string(n)+".EGJ.circuit.txt";
		cout << "Writing to circuit file: " << circuitfilename << endl;
		setup_plain_prot(true,circuitfilename);
		setup_marketvalues(n);
		finalize_plain_prot();
		return 0;
    }

    // generate circuit for use in malicious library
    if (argc == 3 && strcmp(argv[1], "-m") == 0 ) {
		n = atoi(argv[2]);
		string circuitfilename=to_string(n)+".EGJ.circuit.txt";
		cout << "Writing to circuit file: " << circuitfilename << endl;
		setup_plain_prot(true,circuitfilename);
		setup_marketvalues(n);
		finalize_plain_prot();
		return 0;
    }
    
    // run computation with semi-honest model
    int port, party;
    parse_party_and_port(argv, &party, &port);
    NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

    setup_semi_honest(io, party);

    if (argc != 4) {
      cout << "Usage: ./EGJ <party> <port> <n>" << endl
           << "where <n> is the number of players"
           << endl;
      delete io;
      return 0;
    }

    n = atoi(argv[3]);
	cout << "n = " << n << endl;
    char fname[90];

    sprintf(fname, "./data/EGJ/%d.dat", n);

    ifstream infile(fname);

	int num_inputs = (n+1)*n;
    string inputs[num_inputs];

    if( infile.is_open() ) {
        for( int i=0; i<num_inputs; i++) {
            getline( infile, inputs[i]);
        }
        infile.close();
		cout << "Read " << num_inputs << " inputs" << endl;
    }
	else {
		cout << "Couldn't read " << fname << endl;
		cout << "All inputs set to 0" << endl;
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

	double max_err;
    max_err = test_marketvalues(n,inputs);

	ofstream outfile;
	string outfilename = "EGJ_comm";
	struct stat info;

	if( party == ALICE ) {
		outfilename = outfilename + "A.csv";
		if( stat( outfilename.c_str(), &info ) != 0 ) {
			outfile.open (outfilename.c_str());
			outfile << "n, bytes, bitlen, denombits, max_err" << endl;
		}
		else {
			outfile.open (outfilename.c_str());
		}
	}
	if( party == BOB ) {
		outfilename = outfilename + "B.csv";
		if( stat( outfilename.c_str(), &info ) != 0 ) {
			outfile.open (outfilename.c_str());
			outfile << "n, bytes, bitlen, denombits, max_err" << endl;
		}
		else {
			outfile.open (outfilename.c_str());
		}
	}

	long run_count = 0;

	run_count = io->counter;

	outfile << n << ", " << run_count << ", " << FixedP::bitlen << ", " << FixedP::denombits << ", " << max_err << endl;

	outfile.close();

	/*
    sprintf(fname, "../data/EGJ/%d.output.dat", n);
    ifstream trueoutputs(fname);
    if( trueoutputs.is_open() ) {
	cout << "GROUND TRUTH" << endl;
	cout << trueoutputs.rdbuf();
	trueoutputs.close();
    }
	*/
    delete io;
}


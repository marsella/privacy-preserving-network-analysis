#include "emp-sh2pc/emp-sh2pc.h"
#include "math.h"
using namespace emp;
using namespace std;

int BITLEN = 32;
int DENOM = 8; //Use fixed point arithmetic with 2^DENOM as the implicit denominator
int PRINTOUTS = false;

void calculateclearing( Integer PiT[], Integer e[], Integer pbar[], Integer p[], int n );
void findfix( Integer A[], Integer b[], Integer p[], int n, int k);
void matadd( Integer A[], Integer B[], Integer C[], int n );
void matmul( Integer A[], Integer B[], Integer C[], int n, int m, int p );
void matmuldiagA( Integer A[], Integer B[], Integer C[], int n, int m );
void matmuldiagB( Integer A[], Integer B[], Integer C[], int n, int m );
void updatelambda( Integer Lambda[], Integer PiT[], Integer e[], Integer pbar[], Integer p[], int n );
void printvector( Integer v[], string vname, int n );
void printscalar( Integer v, string vname );

//Print the integer vector v, with output string vname.
//n = length(v)
void printvector( Integer v[], string vname, int n ) {
	if( PRINTOUTS ) {
		cout << vname << " = [ ";
		for( int i=0; i<n; i++ ) {
			cout << v[i].reveal<long>() << " ";
		}
		cout << "]" << endl;
	}
}

void printscalar( Integer v, string vname ) {
	if( PRINTOUTS ) {
		cout << vname << " = [ ";
			cout << v.reveal<long>() << " ";
		cout << "]" << endl;
	}
}

//Lambda[i] = 1 if bank i has failed, and Lambda[i] = 0 otherwise
//We represent Lambda as a vector of length n instead of an nxn diagonal matrix
void updatelambda( Integer Lambda[], Integer PiT[], Integer e[], Integer pbar[], Integer p[], int n ){
	Bit b;
	Integer one(BITLEN,1<<DENOM,PUBLIC);
	Integer t;
	for( int i=0; i<n; i++ ) { //If e[i] + sum_j p[j]*Pi[j][i] < pbar[i] then Lambda[i][i] = 1, otherwise Lambda[i][i] remains 0
		t = Integer(BITLEN,0,PUBLIC);
		for( int j=0; j<n; j++ ) {
			t = t + p[j]*PiT[i*n+j];
		}
		t = t>>DENOM;
		t = t + e[i];
		b = t < pbar[i];	
		printscalar( t, "t" );
		printscalar( pbar[i], "pbar[i]" );
		Lambda[i] = Lambda[i].select( b, one );
	}
	//printvector( Pi, "Pi", n*n );
	printvector( Lambda, "Lambda", n );
}

//Calculate the clearing vector, p for a given
//Proportional debt matrix Pi^T
//Asset vector e (e[i] = assets of bank i)
//Total debt vector pbar (pbar[i] = total debts of i)
void calculateclearing( Integer PiT[], Integer e[], Integer pbar[], Integer p[], int n ) {
	int n2 = n*n;

	Integer Lambda[n];
	Integer A[n2];
	for( int i=0; i<n; i++ ){
		p[i] = pbar[i];
	}
	for( int i=0; i<n; i++ ){ //The diagonal failure matrix (Lambdi[i*n+i] = 1 if bank i fails)
		Lambda[i] = Integer(BITLEN,0,PUBLIC);
	}

	Integer T1[n2];
	Integer T2[n2];
	Integer T3[n2];
	Integer IL[n]; //Identity - Lambda
	Integer Le[n]; //Lambda*e
	Integer ILp[n]; //IL*p
	Integer T[n];

	Integer one(BITLEN,1<<DENOM,PUBLIC);
	for( int i = 0; i < n; i++ ) {
		matmuldiagA( Lambda, PiT, T3, n, n ); // T3 = Lambda*PiT
		matmuldiagB( T3, Lambda, T1, n, n ); //T1 = T3*Lambda = Lambda *pi *Lambda	
		for( int j=0; j<n; j++ ){
			IL[j] = one-Lambda[j];
		}
		printvector( IL, "I-Lambda", n );
		printvector( pbar, "pbar", n );
		matmuldiagA( IL, pbar, ILp, n, 1 ); //ILp = IL*pbar
		printvector( ILp, "(I-Lambda)*pbar", n );

		matmul( T3, ILp, T, n, n, 1 ); //T2 = T3*ILp = Lambda*PiT*(I-Lambda)pbar

		matmuldiagA( Lambda, e, Le, n, 1 ); //Le = Lambda*e
		printvector( Le, "Lambda*e", n );
		matadd( T, Le, T, n ); //T = T+ Le
		matadd( T, ILp, T, n ); //T = T+ILp
		printvector( T1, "A", n*n );		
		printvector( T, "b", n );		
		findfix( T1, T, p, n, 10 );
		updatelambda(Lambda,PiT,e,pbar,p,n);
	}
	
	
}

//Find a fixed point of A*p + b = p
//Assume A = nxn
//b = n-vector
//p = n-vector
//k = number of steps in iterative procedure
void findfix( Integer A[], Integer b[], Integer p[], int n, int k){

	for( int i=0; i<k; i++ ) {
		matmul( A, p, p, n, n, 1 ); //p = A*p
		matadd( p, b, p, n ); //p = p + b

		printvector( p, "p", n );
	}

	

}

//Add two vectors/matrices of length n
void matadd( Integer A[], Integer B[], Integer C[], int n ){

	for(int i=0; i<n; i++ ) {
		C[i] = A[i]+B[i];
	}
}

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
		C[i] = T[i]>>DENOM; //Since we've done a multiplication, and both terms were carrying an implicit denominator, we need to shift back
	}
}

//Multiply n vector  by (nxm) matrix (vector viewed as an nxn diagonal
void matmuldiagA( Integer A[], Integer B[], Integer C[], int n, int m ) {

	for( int i=0; i<n; i++ ) {
		for( int j=0; j < m; j ++ ) {
			C[i*m+j] = A[i]*B[i*m+j];
			C[i*m+j] = C[i*m+j]>>DENOM;
		}
	}
}

//Multiply (n x m) by m vector (viewed as an mxm diagonal
void matmuldiagB( Integer A[], Integer B[], Integer C[], int n, int m ) {

	for( int i=0; i<n; i++ ) {
		for( int j=0; j < m; j ++ ) {
			C[i*m+j] = A[i*m+j]*B[j];
			C[i*m+j] = C[i*m+j]>>DENOM;
		}
	}
}

void test_clearing(int n, string inputs[]) {
	
	cout << "BITLEN = " << BITLEN << endl;
	cout << "DENOM = " << DENOM << endl;

	Integer pbar[n]; //pbar[i] = total amount owed by i
	Integer p[n]; //Clearing vector (initialized to pbar[i]
	Integer e[n]; //Asset vector
	Integer Pi[n*n]; //Proportional payout matrix
	Integer PiT[n*n]; //Transpose payout matrix


	//Read inputs
	//Each player has n+2 inputs
	//1st input is e[i]
	//2nd input is pbar[i]
	//Last n inputs are the row L[i][j] for j=1...n
	for (int i=0; i < n; i ++ ) {
		e[i] = Integer(BITLEN, inputs[i*(n+2)], ALICE );
	}
	for (int i=0; i <n; i ++ ) {
		pbar[i] = Integer(BITLEN, inputs[i*(n+2)+1], ALICE );
		p[i] = pbar[i];
	}
	for (int i=0; i< n; i ++) {
		for( int j=0; j < n; j++ ) {
			Pi[i*n+j] = Integer(BITLEN, inputs[i*(n+2)+j+2],ALICE);
			PiT[j*n+i] = Integer(BITLEN, inputs[i*(n+2)+j+2],ALICE);
		}	
	}

	calculateclearing( PiT, e, pbar, p, n );

	cout << "Pi = [ ";
	for(int i=0; i<n*n; i++) {
		cout << Pi[i].reveal<long>() << " " ;
	}
	cout << "]" << endl;
	cout << "e = [ ";
	for(int i=0; i<n; i++) {
		cout << e[i].reveal<long>() << " " ;
	}
	cout << "]" << endl;
	cout << "pbar = [ ";
	for(int i=0; i<n; i++) {
		cout << pbar[i].reveal<long>() << " " ;
	}
	cout << "]" << endl;
	cout << "p = [ ";
	for(int i=0; i<n; i++) {
		cout << p[i].reveal<long>() << " " ;
	}
	cout << "]" << endl;
}

//When we are outputting to a circuit, we need a simplified computation that keeps better track of input values
//The circuit format does not keep track of who owns wires, or even which wires are inputs
//The main purpose of this function is to make sure the inputs are declared in the right order, and to remove extraneous output statements (we don't want extra output wires)
void setup_clearing(int n) {

	Integer pbar[n]; //pbar[i] = total amount owed by i
	Integer p[n]; //Clearing vector (initialized to pbar[i]
	Integer e[n]; //Asset vector
	Integer Pi[n*n]; //Proportional payout matrix
	Integer PiT[n*n]; //Transpose payout matrix

	//Remember, for multiparty circuits, input order matters
	for (int i=0; i < n; i ++ ) {
		e[i] = Integer(BITLEN, 0, ALICE );
		pbar[i] = Integer(BITLEN, 0, ALICE );
		for( int j=0; j<n; j++ ) {
			Pi[i*n+j] = Integer(BITLEN, 0 ,ALICE);
		}
	}
	for (int i=0; i <n; i ++ ) {
		p[i] = pbar[i];
	}
	for (int i=0; i< n; i ++) {
		for( int j=0; j < n; j++ ) {
			PiT[j*n+i] = Pi[i*n+j];
		}	
	}

	calculateclearing( PiT, e, pbar, p, n );
	for(int i=0; i<n; i++) {
		p[i].reveal<string>();
	}
}


int main(int argc, char** argv) {
    int n;

    // generate circuit for use in malicious library
    if (argc == 2 && strcmp(argv[1], "-m") == 0 ) {
		n = 5;
		string circuitfilename=to_string(n)+"clearing.circuit.txt";
		cout << "Writing to circuit file: " << circuitfilename << endl;
		setup_plain_prot(true,circuitfilename);
		setup_clearing(n);
		finalize_plain_prot();
		return 0;
    }

    // generate circuit for use in malicious library
    if (argc == 3 && strcmp(argv[1], "-m") == 0 ) {
		n = atoi(argv[2]);
		string circuitfilename=to_string(n)+"clearing.circuit.txt";
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
      cout << "Usage: ./clearing <party> <port> <n>" << endl
           << "where <n> is the number of players"
           << endl;
      delete io;
      return 0;
    }

    n = atoi(argv[3]);
	cout << "n = " << n << endl;
    char fname[90];

    sprintf(fname, "../data/clearing/%d.dat", n);

    ifstream infile(fname);

	int num_inputs = (n+2)*n;
    string inputs[num_inputs];

	string bitlenstr;
	string denomstr;
    if( infile.is_open() ) {
            getline( infile, bitlenstr);
            getline( infile, denomstr);
        for( int i=0; i<num_inputs; i++) {
            getline( infile, inputs[i]);
        }
        infile.close();
    }

	BITLEN = stoi(bitlenstr);
	DENOM = stoi(denomstr);
	if( BITLEN <= 3*DENOM ) {
		cout << "ERROR! BITLEN is too small, expect errors" << endl;
		cout << "BITLEN = " << BITLEN << endl;
		cout << "DENOM = " << DENOM << endl;
	}
	if( BITLEN > 64 ) {
		cout << "BITLEN = " << BITLEN << " > 64" << endl;
		cout << "Outputs may not print correctly" << endl;
	}
    test_clearing(n,inputs);

    sprintf(fname, "../data/clearing/%d.output.dat", n);
    ifstream trueoutputs(fname);
    if( trueoutputs.is_open() ) {
	cout << "GROUND TRUTH" << endl;
	cout << trueoutputs.rdbuf();
	trueoutputs.close();
    }
    delete io;
}


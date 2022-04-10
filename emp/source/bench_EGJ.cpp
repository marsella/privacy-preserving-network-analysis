#include <emp-tool/emp-tool.h>
#include "emp-agmpc/emp-agmpc.h"
using namespace std;
using namespace emp;

const string circuit_file_location = macro_xstr(EMP_CIRCUIT_PATH);
const int nP = 2; 
int party, port;

void bench_once(NetIOMP<nP> * ios[2], ThreadPool * pool, string filename) {

	if(party == 1) {
		cout <<"CIRCUIT:\t"<<filename<<endl;
	}
	CircuitFile cf(filename.c_str());


	if(party == ALICE) {
		//string file = circuit_file_location+"/"+filename;
		//cout << "About to read circuit file" << endl;
		//cout << "Read circuit file: " << filename.c_str() << endl;
		cout << cf.n1+cf.n2 << " input gates" << endl;
		cout << cf.n3 << " output gates" << endl;
	}

	auto start = clock_start();
	CMPC<nP>* mpc = new CMPC<nP>(ios, pool, party, &cf);
	ios[0]->flush();
	ios[1]->flush();
	double st2 = time_from(start);
//	ios[0]->sync();
//	ios[1]->sync();
	cout <<"Setup: "<<party<<" "<< st2 <<"\n"<<flush;

	start = clock_start();
	mpc->function_independent();
	ios[0]->flush();
	ios[1]->flush();
	double it2 = time_from(start);
	cout <<"FUNC_IND: " <<party<< " "<<it2<<" \n"<<flush;

	start = clock_start();
	mpc->function_dependent();
	ios[0]->flush();
	ios[1]->flush();
	double dt2 = time_from(start);
	cout <<"FUNC_DEP: "<<party<<" "<<dt2<<" \n"<<flush;


	bool *in = new bool[cf.n1+cf.n2]; bool *out = new bool[cf.n3];
	memset(in, false, cf.n1+cf.n2);
	start = clock_start();
	mpc->online(in, out);
	ios[0]->flush();
	ios[1]->flush();
	double ot2 = time_from(start);
	cout <<"ONLINE: "<<party<<" "<<ot2<<" \n"<<flush;

	string ofilename = to_string(party) + "EGJ_stats.csv";
	ifstream f(ofilename.c_str());
	bool exists = f.good();
	f.close();
	ofstream outfile;
	outfile.open(ofilename.c_str(),ofstream::app);
	if( ! exists ) {
		outfile << "nP,setup,func_ind,func_dep,online,bytesa,bytesb" << endl;
	}
	outfile << nP << "," << st2 << "," << it2 << "," << ot2 << "," << dt2 << ios[0]->count() << "," << ios[1]->count() << endl;
	outfile.close();


	/*
	if( party == ALICE ) {
		string bitstr;
		string outintstr;
		cout << "[ ";
		if( cf.n3 % bitlen != 0 ) {
			cout << "Error!" << endl;
			cout << "bitlen = " << bitlen << endl;
			cout << "Number of output bits = " << cf.n3 << endl;
		}
		for( int i=0; i < cf.n3/bitlen;i++ ) { //There are cf.n3 output bits.  If we assume the outputs are supposed to be a string of integers, each of length bitlen, we can reformat them
			bitstr="";
			for( int j = bitlen-1; j >= 0; j-- ) {
				if( out[i*bitlen+j] ) {
					bitstr += "1";
				}
				else {
					bitstr += "0";
				}
				//cout << "out[" << i*bitlen+j << "] = " << out[i*bitlen+j] << endl;
			}	
			outintstr = bin_to_dec( bitstr ); //Convert bitstring into an integer (string)
			//cout << "Player " << party << " : " << bitstr << " " ;
			cout << bitstr << " " ; //Print raw bits
			cout << "(" << outintstr << ") "; //Print integer version of raw bits
			//cout << outint << " ";
		}
		cout << "]" << endl;
	}
	*/
	
	delete mpc;
}
int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	
    if (argc != 3) {
      cout << "Usage: ./bench_EGJ <party> <port>" << endl
           << endl;
      return 0;
    }
    cout << "nP = " << nP << endl;

	if(party > nP)return 0;
	NetIOMP<nP> io(party, port);
#ifdef LOCALHOST
	NetIOMP<nP> io2(party, port+2*(nP+1)*(nP+1)+1);
#else
	NetIOMP<nP> io2(party, port+2*(nP+1));
#endif
	NetIOMP<nP> *ios[2] = {&io, &io2};
	ThreadPool pool(2*(nP-1)+2);	

	string circuit_file = to_string(nP) + "EGJ.circuit.txt";

	bench_once( ios, &pool, circuit_file_location + circuit_file );

	return 0;
}

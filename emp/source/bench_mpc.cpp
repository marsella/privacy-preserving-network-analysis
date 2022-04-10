#include <emp-tool/emp-tool.h>
#include "emp-agmpc/emp-agmpc.h"
using namespace std;
using namespace emp;

const string circuit_file_location = macro_xstr(EMP_CIRCUIT_PATH);
int bitlen = 54;

const static int nP = 3;
int party, port;
void bench_once(NetIOMP<nP> * ios[2], ThreadPool * pool, string filename) {

	if(party == 1) {
		cout <<"CIRCUIT:\t"<<filename<<endl;
	}
	CircuitFile cf(filename.c_str());

	//cf->n1 = (n+2)*n*b
	//cf->n3 = n*b
	int t = cf.n3/(cf.n1/cf.n3 - 2);
	if( t != bitlen ){
		cout << "bitlen is probably set incorrectly" << endl;
		cout << "bitlen = " << bitlen << endl;
		cout << cf.n1 << " inputs" << endl;
		cout << cf.n3 << " outputs" << endl;
	}
	

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
	double t2 = time_from(start);
//	ios[0]->sync();
//	ios[1]->sync();
	if(party == 1)cout <<"Setup:\t"<<party<<"\t"<< t2 <<"\n"<<flush;

	start = clock_start();
	mpc->function_independent();
	ios[0]->flush();
	ios[1]->flush();
	t2 = time_from(start);
	if(party == 1)cout <<"FUNC_IND:\t"<<party<<"\t"<<t2<<" \n"<<flush;

	start = clock_start();
	mpc->function_dependent();
	ios[0]->flush();
	ios[1]->flush();
	t2 = time_from(start);
	if(party == 1)cout <<"FUNC_DEP:\t"<<party<<"\t"<<t2<<" \n"<<flush;

	bool *in = new bool[cf.n1+cf.n2]; bool *out = new bool[cf.n3];
	memset(in, false, cf.n1+cf.n2);
	start = clock_start();
	mpc->online(in, out);
	ios[0]->flush();
	ios[1]->flush();
	t2 = time_from(start);
//	uint64_t band2 = io.count();
//	if(party == 1)cout <<"bandwidth\t"<<party<<"\t"<<band2<<endl;
	if(party == ALICE)cout <<"ONLINE:\t"<<party<<"\t"<<t2<<" \n"<<flush;

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
	
	delete mpc;
}
int main(int argc, char** argv) {
	int func = 6;
	parse_party_and_port(argv, &party, &port);
	if(party > nP)return 0;
	NetIOMP<nP> io(party, port);
#ifdef LOCALHOST
	NetIOMP<nP> io2(party, port+2*(nP+1)*(nP+1)+1);
#else
	NetIOMP<nP> io2(party, port+2*(nP+1));
#endif
	NetIOMP<nP> *ios[2] = {&io, &io2};
	ThreadPool pool(2*(nP-1)+2);	

//	for(int i = 0; i < 10; ++i)	
	if(func == 0)	
		bench_once(ios, &pool, circuit_file_location+"AES-non-expanded.txt");
//	for(int i = 0; i < 10; ++i)
	if(func == 1)	
		bench_once(ios, &pool, circuit_file_location+"sha-1.txt");
//	for(int i = 0; i < 10; ++i)
	if(func == 2)	
		bench_once(ios, &pool, circuit_file_location+"sha-256.txt");
	if(func == 3)	
		bench_once(ios, &pool, circuit_file_location+"not.txt");
	if(func == 4)	
		bench_once(ios, &pool, circuit_file_location+"ands.txt");
	if(func == 5)	
		bench_once(ios, &pool, circuit_file_location+"adder_32bit.txt");
	if(func == 6)	
		bench_once(ios, &pool, circuit_file_location+std::to_string(nP)+"EGJ.circuit.txt");
	
//	bench_once(ios, &pool, "/home/wangxiao/git/emp-toolkit/constantmpc/circ.txt");
	return 0;
}

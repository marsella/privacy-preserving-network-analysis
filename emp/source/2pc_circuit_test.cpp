#include <emp-tool/emp-tool.h>
#include "emp-ag2pc/emp-ag2pc.h"
using namespace std;
using namespace emp;

const string circuit_file_location = macro_xstr(EMP_CIRCUIT_PATH);

int main(int argc, char** argv) {
	int port, party;
	parse_party_and_port(argv, &party, &port);

	NetIO* io = new NetIO(party==ALICE ? nullptr:IP, port);
	io->set_nodelay();
	string file = circuit_file_location+"/10clearing.circuit.txt";

	if( party == ALICE ){
		cout << "Running circuit: " << file << endl;
	}
	CircuitFile cf(file.c_str());
	if( party == ALICE ){
		cout << "Alice has " << cf.n1 << " inputs" << endl;
		cout << "Bob has " << cf.n2 << " inputs" << endl;
		cout << "Circuit has " << cf.n3 << " outputs" << endl;
	}

	
	auto t1 = clock_start();
	C2PC twopc(io, party, &cf);
	io->flush();
	cout << "one time:\t"<<party<<"\t" <<time_from(t1)<<endl;
	t1 = clock_start();
	twopc.function_independent();
	io->flush();
	cout << "inde:\t"<<party<<"\t"<<time_from(t1)<<endl;

	t1 = clock_start();
	twopc.function_dependent();
	io->flush();
	cout << "dep:\t"<<party<<"\t"<<time_from(t1)<<endl;


	bool in[cf.n1+cf.n2], out[cf.n3];
 	for(int i = 0; i < cf.n1+cf.n2; ++i)
		in[i] = false;
	t1 = clock_start();
	twopc.online(in, out);
	cout << "online:\t"<<party<<"\t"<<time_from(t1)<<endl;
	if(party == BOB){
		string res = "";
		for(int i = 0; i < cf.n3; ++i)
			res += (out[i]?"1":"0");
		cout << "Result " << res << endl;	
	}
	delete io;
	return 0;
}

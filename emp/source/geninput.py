from __future__ import print_function
import argparse, random, os, math
import numpy as np

def create_dirs(program):
	dirname = "data/"+program
	if not os.path.exists(dirname):
		try:
			os.makedirs(dirname)
		except OSError as e:
			if e.errno != errno.EEXIST: raise

def gen_EN_input(valmax,n) :
	L = np.random.uniform(0,valmax,size=(n,n))
	for i in range(n):
		L[i][i] = 0
	pbar = np.zeros(n,dtype=np.float64)
	for i in range(n):
		pbar[i] = sum( [L[i][j] for j in range(n)] )
	Pi = np.zeros((n,n),dtype=np.float64)
	for i in range(n):
		for j in range(n):
			Pi[i][j] = L[i][j]/pbar[i]
	e = np.random.uniform(0,valmax/5,size=n)	
	f = open("data/EN/%d.dat"%n, 'w')
	for i in range(n):
		f.write("%f\n"%e[i])
		f.write("%f\n"%pbar[i])
		for j in range(n):
			f.write("%f\n"%Pi[i][j])
	f.close()

	test_EN(Pi,e,pbar)
	#print("expected value: %d"%xs[0])

def test_EN(Pi,e,pbar):
	n = len(pbar)
	p = pbar
	Lambda = np.zeros((n,n),dtype=np.float64)
	I = np.identity(n,dtype=np.float64)
	PiT = Pi.transpose()
	for _ in range(n):
		A = np.matmul(Lambda,PiT)
		A = np.matmul(A,Lambda)
		B = np.matmul(Lambda,PiT)
		B = np.matmul(B,I-Lambda)
		B = np.matmul(B,pbar)
		B = B + np.matmul(Lambda,e) + np.matmul(I-Lambda,pbar)
		for i in range(10):
			p = np.matmul(A,p)+B
		for i in range(n):
			if e[i] + (sum( [p[j]*Pi[j][i] for j in range(n)] )) < pbar[i]:
				Lambda[i][i] = 1

	clearing = np.min( np.dot(PiT,p) + e, pbar )
	if not np.isclose( clearing, p ):
		print( "ERROR: test_EN did not produce a clearing vector" )
		for i in range(n):
			print( f"Clearing[i] = {clearing[i]}" )
			print( f"p[i] = {p[i]}" )
			print( "e[i] = " + str(e[i]) )
			print( "pbar[i] = " + str(pbar[i]) )

	#Write to output file
	f = open("data/EN/%d.output.dat"%n, 'w')
	f.write("Pi = \n")
	for i in range(n):
		f.write( "[ " )
		for j in range(n):
			f.write("%f "%Pi[i][j])
		f.write( " ]\n" )
	f.write("e = \n")
	for i in range(n):
		f.write( "[ " )
		f.write("%f "%e[i])
		f.write("]\n")	
	f.write("pbar = \n")
	for i in range(n):
		f.write( "[ " )
		f.write("%f "%pbar[i])
		f.write("]\n")	
	f.write("p = \n")
	for i in range(n):
		f.write( "[ " )
		f.write("%f "%p[i])
		f.write("]\n")	
	f.close()

	f = open("data/EN/%d.truep.dat"%n, 'w')
	for i in range(n):
		f.write("%f\n"%p[i])
	f.close()

	return p


def gen_EGJ_input(valmax,n) :
	A = np.random.uniform(0.0,1.0,size=(n,n))
	col_sums = A.sum(axis=0)
	C = A / col_sums[np.newaxis,:]
	e = np.random.uniform(0,valmax,size=n)	
	f = open("data/EGJ/%d.dat"%n, 'w')
	for i in range(n):
		f.write("%f\n"%e[i])
		for j in range(n):
			f.write("%f\n"%C[j][i])
	f.close()

	test_EGJ(C,e)
	#print("expected value: %d"%xs[0])

def test_EGJ(C,e):
	n = C.shape[1]
	S = np.zeros( C.shape )
	for i in range(n):
		S[i][i] = C[i][i]
		C[i][i] = 0

	k = 20

	I = np.identity(n,dtype=np.float64)
	A = I
	for i in range(k):	
		A = np.matmul( C, A )
		A = A + I
	
	V = np.matmul( A, e )
	V = np.matmul( S, V )
		
	#Write to output file
	f = open("data/EGJ/%d.output.dat"%n, 'w')
	f.write("e = \n")
	for i in range(n):
		f.write( "[ " )
		f.write("%f "%e[i])
		f.write("]\n")	
	f.write("S = \n")
	for i in range(n):
		f.write( "[ " )
		f.write("%f "%S[i][i])
		f.write("]\n")	
	f.write("C = \n")
	for i in range(n):
		f.write( "[ " )
		for j in range(n):
			f.write("%f "%C[i][j])
		f.write("]\n")	
	f.write("v = \n")
	for i in range(n):
		f.write( "[ " )
		f.write("%f "%V[i])
		f.write("]\n")	
	f.close()

	f = open("data/EGJ/%d.truev.dat"%n, 'w')
	for i in range(n):
		f.write("%f\n"%V[i])
	f.close()
	return V

if __name__ == "__main__":
	parser = argparse.ArgumentParser( description='generates input for emp-toolkit sample programs')
	parser.add_argument('-n', default=10, type=int, help="number of players")
	parser.add_argument('-v', default=100, type=int, help="maximum debt")
	programs = ["EN","EGJ"]
	parser.add_argument('-e', default="EN", choices = programs, help="program selection")
	args = parser.parse_args()

	create_dirs(args.e)

	if args.e == "EN":
		gen_EN_input(args.v,args.n)

	if args.e == "EGJ":
		gen_EGJ_input(args.v,args.n)




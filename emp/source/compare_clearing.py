import sys, getopt
import math

def clearing( inputs ):
	n2 = len( inputs )
	n = int(math.sqrt(n2))
	if( n**2 != n2 ):
		print "ERROR! n = " + str(n) + ", n^2 = " + str(n**2)
	
	outputs = [0 for _ in range(n2)]
	for i in range(n):
		for j in range(n):
			for k in range(n):
				outputs[i*n+j] += inputs[i*n+k]*inputs[k*n+j]
	return outputs

def main(argv):
	try:
		opts, args = getopt.getopt(argv,"hn:b:",["n=","b="])
	except getopt.GetoptError:
		print 'compare_clearing.py -n <num players> -b <bit len>'
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
			print 'compare_clearing.py -n <num players> -b <bit len>'
			sys.exit()
		elif opt in ("-n"):
			n = int(arg)
		elif opt in ("-b"):
			b = int(arg)
	print "n = " + str(n)
	print "b = " + str(b)

	fname1 = "data/clearing/" + str(n) + ".1.dat"
	fname2 = "data/clearing/" + str(n) + ".2.dat"

	inputs1 = []
	with open(fname1) as fp:
		for cnt, line in enumerate(fp):
			inputs1.append(int(line))

	inputs2 = []
	with open(fname2) as fp:
		for cnt, line in enumerate(fp):
			inputs2.append(int(line))

	if len( inputs1 ) < n**2:
		print "len(inputs1) = " + str(len(inputs1)) + " < " + str(n**2) + " = n^2"
	if len( inputs2 ) < n**2:
		print "len(inputs2) = " + str(len(inputs2)) + " < " + str(n**2) + " = n^2"

	inputs = [x[0]^x[1] for x in zip(inputs1,inputs2)]
	print "Input matrix : [ " + " ".join(map(str,inputs)) + " ]"

	outputs = clearing( inputs )
	print "Output matrix : [ " + " ".join(map(str,outputs)) + " ]"

if __name__ == "__main__":
	main(sys.argv[1:])


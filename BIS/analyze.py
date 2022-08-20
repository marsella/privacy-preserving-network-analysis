
import networkx as nx
import matplotlib.pyplot as plt
import numpy as np
import csv,argparse,math

# data labels
ID = 0
REPORTER = 1
RECEIVER = 2
LIAB = 3
RESERVE = 2

def get_reporters(reserves):
	return [d[REPORTER] for d in reserves]

def get_repid_dictionary(reserves):
	resv = {}
	for (rid,rep,_) in reserves:
		resv[rep] = rid
	return resv

# reads data from a file formatted by format.R
def format_data():
	# read liabilities from file
	liab = []
	with open('X2018.Q2.csv') as csvfile:
		reader = csv.reader(csvfile, delimiter=",")
		next(reader) # skip first row
		for row in reader:
			liab.append((int(row[ID]), row[REPORTER], row[RECEIVER], float(row[3])))
	
	# read equity reserves from file
	reserves = []
	with open('X2018.Q2.equity.csv') as csvfile:
		reader = csv.reader(csvfile, delimiter=",")
		next(reader) # skip first row
		for row in reader:
			reserves.append((int(row[ID]), row[REPORTER], float(row[RESERVE])))

	# limit liabilities to countries that reported some amount of equity
	reporters = get_reporters(reserves)
	liab = [l for l in liab if l[REPORTER] in reporters and l[RECEIVER] in reporters]
	return (reserves, liab)

# get a subset of the data with `parties` number of participants
def get_subset(reserves, liab, parties):
	totalct = len(reserves)
	if parties > totalct or parties < 2:
		print("Number of parties must be between 2 and {}", totalct)
	else:
		reserves = reserves[:parties]
		reporters = get_reporters(reserves)
		liab = [l for l in liab if l[REPORTER] in reporters and l[RECEIVER] in reporters]
	return (reserves, liab)


	

# format data files for many individual players
# todo: format scalemamba for fewer computation parties
#		(e.g. secret-share data between 3-5 parties)
def format_scalemamba(reserves,liab, home='docker/src/Data/'):
	# get a mapping from country names to IDs
	repid = get_repid_dictionary(reserves)

	for (rid, name, resv_amt) in reserves:
		# format liabs into the ordered list of ints expected by sm
		debts = [0] * len(reserves)
		liab_owed = [l for l in liab if l[RECEIVER] == name]
		for (_,rep,recv,amt) in liab_owed:
			debts[ repid[rep]-1 ] = int(round(amt,0))

		# print to file: reserves, total debts, individual debts
		with open(home+"Player%s_in.dat"%(rid-1), 'w') as f:
			f.write("%s\n"%(int(round(resv_amt,0))))
			f.write("%s\n"%sum(debts))
			f.write("{}\n".format('\n'.join(map(str,debts))))

# format data files as python variables
# int array reserves
# int array total debts
# int array individual debts
# int array proportional debts
def format_python(reserves,liab):
	N = len(reserves)
	with open("../mpyc/src/data/bisdata.py", 'w') as f:
		# print reserves array
		rsv = [int(round(ra,0)) for (_,_,ra) in reserves]
		#f.write("reserves = [{}]\n".format(','.join(rsv)))
		f.write("reserves = {}\n".format(str(rsv)))

		# arrange debts (might have recv/rep backwards...)
		repid = get_repid_dictionary(reserves)
		ind_debts = [[0 for _ in range(N)] for _ in range(N)]
		for (rid, name, resv_amt) in reserves:
			liab_owed = [l for l in liab if l[RECEIVER] == name]
			for (_,rep,recv,amt) in liab_owed:
				ind_debts[rid-1][ repid[rep]-1 ] = int(round(amt,0))
		f.write("individual_debts = {}\n".format(str(ind_debts)))

		# get sums
		totals = [sum(dbt) for dbt in ind_debts]
		f.write("total_debts = {}\n".format(str(totals)))

		# get proportions
		totals = [float(t) for t in totals]
		props = [[0 for _ in range(N)] for _ in range(N)]
		for i in range(N):
			for j in range(N):
				props[i][j] = ind_debts[i][j] / totals[i]
		f.write("props = {}\n".format(str(props)))
		
def format_numpy(reserves,liab):
	N = len(reserves)
	rsv = [int(round(ra,0)) for (_,_,ra) in reserves]

	repid = get_repid_dictionary(reserves)
	ind_debts = [[0 for _ in range(N)] for _ in range(N)]
	for (rid, name, resv_amt) in reserves:
		liab_owed = [l for l in liab if l[RECEIVER] == name]
		for (_,rep,recv,amt) in liab_owed:
			ind_debts[rid-1][ repid[rep]-1 ] = int(round(amt,0))

	return np.array(ind_debts), np.array(rsv)

# formats for the insecure version
def format_insecure(reserves, liab):
	# get a mapping from country names to IDs
	repid = get_repid_dictionary(reserves)

	for (rid, name, resv_amt) in reserves:
		with open("../insecure/data/bank%s.dat"%(rid-1), 'w') as f:
			f.write("i {}\n".format(rid-1))
			f.write("r {}\n".format(int(round(resv_amt,0))))

			# record debts owed by this bank
			debts = [l for l in liab if l[RECEIVER] == name]
			for (_,rep, _,amt) in debts:
				f.write("d {} {}\n".format(repid[rep]-1, int(round(amt,0))))

			# record credits owed to this bank
			credits = [l for l in liab if l[REPORTER] == name]
			for (_,_,recv,amt) in credits:
				f.write("c {} {}\n".format(repid[recv]-1, int(round(amt,0))))



# attractively plot liabilities relationships
# doesn't include labels because they get cut off at savefig.
# instead, writes them to file for a more _competent_ graphing program to use later
def prettyprint(reserves,liab):
	G = nx.DiGraph()
	fig, ax = plt.subplots()
	fig = plt.figure(figsize=(7,7))
	for (_,rep,amt) in reserves:
		G.add_node(rep, reserve=amt)
	
	# weights edge width by size of debt
	wmax = max([w for (_,_,_,w) in liab])
	for (_,fro,to,weight) in liab:
		G.add_edge(fro,to,weight=(10*weight/wmax))
	_, names, sizes = zip(*reserves)
	
	# customize node labels
	#label_handles = nx.draw_networkx_labels(G,pos=nx.circular_layout(G,scale=1))
	#[label.set_bbox(dict(facecolor='white', edgecolor='none')) for label in label_handles.values()]
	
	# plot nodes and edges
	pos = nx.circular_layout(G, scale=.85)
	sizes = np.array(sizes) / 500
	weights = [G[u][v]['weight'] for u,v in G.edges()]
	nx.draw(G, pos=pos, node_size=sizes,node_color='white', width=weights)
	plt.gca().collections[0].set_edgecolor('black')

	plt.savefig("bis_nolabel.png")

	# write labels to file
	with open('BISnames.csv', 'w') as f:
		f.write("NAME\n")
		for n in names:
			f.write("%s\n"%n)


if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument('-n','--parties',type=int, help=('number of parties'))
	parser.add_argument('-t','--outputtype', type=int, help=('output type. 0 = insecure; 1 = scalemamba; 2 = mpyc'))
	parser.add_argument('-p', '--print', action='store_true', help=('print the data?'))
	parser.set_defaults(parties=5,outputtype=0)
	args = parser.parse_args()
	
	reserves, liab = format_data()
	reserves, liab = get_subset(reserves, liab, args.parties)
	print("%d reporters, %d total edges"%(len(reserves), len(liab)))

	if args.parties:
		prettyprint(reserves, liab)

	if (args.outputtype == 0):
		format_insecure(reserves, liab)
	elif (args.outputtype == 1):
		format_scalemamba(reserves, liab, './')
	elif (args.outputtype == 2):
		format_python(reserves, liab)
	else:
		print("output type not supported")




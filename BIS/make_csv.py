import csv,argparse,math
from analyze import format_data, get_subset, format_numpy
import sys
sys.path.append("../python")
from debt_error import clearing
from debt_error import defaulters

reserves, liab = format_data()
L,e = format_numpy( reserves, liab )

num_banks = len(reserves)
#reserves, liab = get_subset(reserves, liab, args.parties)
v = clearing( L, e, approximate=False )
av = clearing( L, e, approximate=True, k=10 )

D = defaulters( L,e,v )
num_defaulters = sum(D)
print( f"Number of defaulters = {num_defaulters}" )
if num_defaulters > 0:
	for i in range(len(D)):
		if D[i] != 0:
			print( f"{reserves[i][1]} defaulted" )

outfilename = 'BIS_data.csv'

with open(outfilename, 'w', newline='') as csvfile:
	BISwriter = csv.writer(csvfile, delimiter=',', quotechar="\"", quoting=csv.QUOTE_MINIMAL)
	BISwriter.writerow( ['name','reserves','marketvalue','approximatemarketvalue'] )
	for i in range(num_banks):
		BISwriter.writerow( [reserves[i][1],int(round(float(reserves[i][2]))),int(round(float(v[i]))),int(round(float(av[i])))] )

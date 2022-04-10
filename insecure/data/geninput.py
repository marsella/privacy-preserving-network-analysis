##
# Generates data for a bankking network simulation
# 
# Data fits the following constraints
# - System is _consistent_: every debt has a matching credit
# - No reserve, debt, or credit is larger than MAX_CASH
# 
# This outputs a set of files, one for each bank in the network
# A bank has four types of data entries
# - i: id number, refers to a specific bank. Exactly 1 per file
# - r: reserves amount, refers to cash the bank has on hand. Exactly 1 per file
# - c: credit, money that is owed TO the bank. Up to n-1 per file
# - d: debt, money that is owed BY the bank. Up to n-1 per file
# These entries may be in any order. 
# The number of entries per file is between 2 and 2n
##

import os,sys,random

MAX_CASH = 1000

n = 10 
if len(sys.argv) == 2:
    n = int(sys.argv[1])

# clean up old files
try:
    [os.remove("data/bank%d.dat"%bid) for bid in range(n)]
except: 
    print "no files to remove :)"

for bid in range(n):
    cash = random.randrange(MAX_CASH)
    f = open("data/bank%d.dat"%bid, 'a')
    f.write("i %d\nr %d\n"%(bid,cash))
    for other_bank in range(n):
        form = random.randrange(2)
        if form and other_bank != bid:
            amount = random.randrange(MAX_CASH)
            f.write("d %d %d\n"%(other_bank, amount))
            with open("data/bank%d.dat"%other_bank, 'a') as of:
                of.write("c %d %d\n"%(bid, amount))
                
    f.close()


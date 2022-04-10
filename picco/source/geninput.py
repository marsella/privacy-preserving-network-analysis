#
# Each bank is assigned an ID between 0 and NUM_BANKS
# this will be a public value mutually decided upon
# 
# Each bank must input
# - the debt they owe to other banks
#   this is a list of length NUM_BANKS. 
#   entries should be nonnegative
# - their cash reserves
#   this should be nonnegtive
# 

import random

NUM_BANKS = 5 
MAX_CASH = 1000
for bank in range(NUM_BANKS):
    debts = [random.randrange(MAX_CASH) for _ in range(NUM_BANKS)]
    debts[bank] = 0 # don't owe yourself money
    reserves = random.randrange(MAX_CASH)

    totaldebt = 1.0 * sum(debts)
    proportions = [d / totaldebt for d in debts]

    with open('bank%d.dat'%bank, 'w') as f:
        f.write("reserves =%d\n"%reserves)
        f.write("debts%d = "%bank)
        f.write(','.join([str(x) for x in debts]))
        f.write("\ntotal_debts = %d" %totaldebt)
        f.write("\nproportions%d = "%bank)
        f.write(','.join(["{0:.3f}".format(p) for p in proportions]))
        f.write("\npayment_strategy = %d" %totaldebt)
        f.write('\n')







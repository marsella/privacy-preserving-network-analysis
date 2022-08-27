import sys,random

ONEPARTYINPUT = False

assert len(sys.argv) == 2
NUM_BANKS = int(sys.argv[1])
MAX_AMT = 1000

if not ONEPARTYINPUT:
    for bank in range(NUM_BANKS):
        with open("Data/Player%s_in.dat"%bank, 'w') as f:
            # generate individual debts
            debts = [random.randint(0,MAX_AMT) for _ in range(NUM_BANKS)]
            debts[bank] = 0; # don't owe yourself money
            # reserves
            f.write("%s\n"%random.randint(0,MAX_AMT))
            # total debt
            f.write("%s\n"%sum(debts))
            # individual debts and proportions (to 4 decimal places)
            props = [float(d) / sum(debts) for d in debts]
            for (d, p) in zip(debts, props):
                #f.write("{} {:.4}\n".format(d,p))
                f.write("{} \n".format(d))
    
else:
    # write all inputs to player 0's files.
    debts = [[random.randint(0,MAX_AMT) for _ in range(NUM_BANKS)] for _ in range(NUM_BANKS)]
    for i in range(NUM_BANKS):
        debts[i][i] = 0 #don't owe yourself money
    reserves = [random.randint(0,MAX_AMT) for _ in range(NUM_BANKS)]
    total_debts = [sum(ds) for ds in debts]
    with open("Data/Player0_in.dat", 'w') as f:
        f.write("%s\n"%" ".join([str(r) for r in reserves]))
        f.write("%s\n"%" ".join([str(d) for d in total_debts]))
        for ds in debts:
            f.write("%s\n"%" ".join([str(d) for d in ds]))

    for bank in [1,2]: # 3 computation parties
        with open("Data/Player%s_in.dat"%bank, 'w') as f:
            zzz = "0 " * NUM_BANKS
            for _ in range(NUM_BANKS + 2):
                f.write("%s\n"%zzz)
    


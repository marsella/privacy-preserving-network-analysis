import numpy as np
import sys

def getRandomData(n):
    """
    generates random synthetic data that looks correct for the egj algorithm
    TODO: describe it

    S: self-holdings; % of underlying assets an entity controls
    C: cross-holdings; % of underlying assets controlled by other entities.
       sum of column i in C + S[i] = 1
    D: underyling assets; we're doing values in [0,1) here but it could easily be larger numbers.
    """
    d = np.arange(n)

    #For EGJ analysis, we need to normalize debts
    S = np.zeros((n,n), np.float64 ) #Self holdings
    S[d,d] = [ np.random.rand() for _ in range(n) ] 

    # normalized cross-holdings
    C = np.random.randint(1,200000, (n,n))
    C[d,d] = 0 #Diagonals of cross-holding matrix are 0
    C = C/C.sum(axis=0) #Normalize columns
    C = C*(1 - S[d,d]) #Column sums are 1-S[i,i]

    #Underlying assets
    #D = np.random.randint(1, 1000, (n) )
    D = np.random.rand(n)

    return (S,C,D)

def writeRandomData(n):
    S,C,D = getRandomData(n)

    with open("data/input{}.dat".format(n), 'w') as f:
        f.write("%s\n" % ' '.join([str(d) for d in D]))
        for row in C:
            f.write("%s\n" % ' '.join([str(c) for c in row]))
        for row in S:
            f.write("%s\n" % ' '.join([str(s) for s in row]))


def readData(n):
    with open("data/input{}.dat".format(n), 'r') as f:
        data = f.readlines()
    
    assert len(data) == 2*n + 1

    D = np.array(data[0].split(' '),np.float64)
    C = np.array([np.array(row.split(' '), np.float64) for row in data[1:n+1]])
    S = np.array([np.array(row.split(' '), np.float64) for row in data[n+1:]])

    return (D,C,S)

def getEGJData(n=6):
    """
    Returns the data set described in EGJ'14
    """
    assert n == 6

    d = np.arange(n)

    S = np.zeros((n,n), np.float64 ) #Self holdings
    S[d,d] = [ float(2/3) for _ in range(n) ] #EGJ assumes everyone has 2/3 self-holdings

    #The cross-holdings matrix from: https://pubs.aeaweb.org/doi/pdfplus/10.1257/aer.104.10.3115
    C = np.array( [0,198304,39458,329550,21817,115162,174862,0,32977,133954,30208,146096,1960,2663,0,444,51,292,40311,227813,2302,0,3188,26939,6679,2271,8077,2108,0,21620,27015,54178,1001,29938,78005,0] ).reshape(n,n)

    C[d,d] = 0 #Diagonals of cross-holding matrix are 0
    C = C/C.sum(axis=0) #Normalize columns
    C = C*(1 - S[d,d]) #Column sums are 1-S[i,i]

    # assets matrix from same source
    D = np.array( [11.6,14.9,1.3,9.2,1.0,6.3] ).reshape(n,1)

    return (S,C,D)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("you need to tell me n")
    n = int(sys.argv[1])
    try:
        writeRandomData(n)
    except:
        print("make sure you: mkdir data; touch __init__.py")


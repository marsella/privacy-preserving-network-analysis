import egj, egjdata
import numpy as np
from mpyc.runtime import mpc
import sys, math

async def test_EGJ():
    """
    Runs an end-to-end test on the data from the EGJ'14 paper
    """
    n = 6

    S,C,D = egjdata.getEGJData()

    result = plainEGJ(S,C,D,n)
    scaled = scaledEGJ(S,C,D,n)

    await mpc.start()
    secure = secureEGJ(S,C,D,n)
    secure = await mpc.output(secure)
    await mpc.shutdown()

    tol = .01
    for (clr, scl, sec) in zip(result, scaled, secure):
        assert (abs(clr - scl) < tol)
        assert (abs(scl - float(sec)) < tol)

async def test_e2e(n, tol=.001, fname=None):
    """
    Runs an end-to-end test of the egj algorithm.
    Computes EGJ 3 ways:
    1. in the clear using the simplest algorithm (matrix inversion) (egj)
    2. in the clear using scaled integer computation (blom et al.)
    3. securely using scaled integer computation (blom et al.)
    
    throws error if difference is greater than the allowed tolerance
    (compares 1 v 2 and 2 v 3)
    """

    D,C,S = egjdata.readData(n)

    result = plainEGJ(S,C,D,n)
    print("clear :", result)

    scaled = scaledEGJ(S,C,D,n)
    print("scaled:", scaled)

    await mpc.start()
    secure = secureEGJ(S,C,D,n)
    secure = await mpc.output(secure)
    print("secure:", await mpc.output(secure))
    await mpc.shutdown()

    # write results
    if fname:
        with open(fname, 'a') as f:
            f.write("clr {}\n".format(" ".join(map(str,result))))
            f.write("scl {}\n".format(" ".join(map(str,scaled))))
            f.write("sec {}\n".format(" ".join(map(str,secure))))


    # compare
    for (clr, scl, sec) in zip(result, scaled, secure):
        assert (abs(clr - scl) < tol)
        assert (abs(scl - float(sec)) < tol)

###################################################
@mpc.coroutine
async def secureEGJ(S,C,D,n):
    secfxp = mpc.SecFxp(64,32)
    await mpc.returnType(secfxp, n)

    bigsecfxp = mpc.SecFxp(212,106)

    # interpret as secure integers
    S = [bigsecfxp(S[i,i].item()) for i in range(n)]
    D = [secfxp(dci.item()) for dci in D]
    C = [[secfxp(cci.item()) for cci in crow] for crow in C]

    result = egj.crossholdings(D,C,S,n,bigsecfxp)
    result = mpc.convert(result,secfxp)

    return result

# computes EGJ in the clear using the simplest algorithm
def plainEGJ(S,C,D,n):
    Id = np.identity( n, np.float64 )

    true_A = np.matmul( S, np.linalg.inv(Id-C) )
    true_V = np.matmul( true_A, D ) #True network valuations
    true_V = true_V.reshape(n)
    
    return true_V

# scales everything up by alpha bits, so all the operations can be integer arithmetic
# computes in terms of adjugate and determinant
def scaledEGJ(S,C,D,n):

    Id = np.identity( n, np.float64 )
    IC = Id-C

    # scale
    beta = 2 ** 15
    IC = [[int(beta * c.item()) for c in row] for row in IC]
    D = [int(beta * d.item()) for d in D]

    # take inverse
    x = np.linalg.solve(IC, D)
    det = np.linalg.det(IC)
    adjB = det * x

    # multiply by S
    V = np.matmul(S, adjB)
    V = V / det

    return V

###################################################


async def test_inverse(n, tol=.001):
    C = np.random.rand(n,n)
    d = np.arange(n)
    C[d,d] = 0 #Diagonals of cross-holding matrix are 0
    C = C/C.sum(axis=0) #Normalize columns
    C = C*(1 - float(2/3)) #Column sums are 1-S[i,i]

    cinv = np.linalg.inv(C)

    beta = 2 ** 30
    C = [[int(beta * c.item()) for c in crow] for crow in C]

    pinv = np.linalg.inv(C)
    pdet = np.linalg.det(C)

    padj = pinv * pdet

    factor = beta / pdet

    await mpc.start()

    secint = mpc.SecInt(128)
    Cs = [[secint(c) for c in crow] for crow in C]
    sadj = egj.invert(Cs)

    sdet = sadj[-1]
    sadj = np.reshape(np.array(sadj[:-1]), (n,n))
    sadj = [[n for n in row] for row in sadj]

    secfxp = mpc.SecFxp(256,128)
    factor = secfxp(beta) / mpc.convert(sdet,secfxp)
    sinv = [[factor * mpc.convert(sa,secfxp) for sa in row] for row in sadj]

    scinv = [await mpc.output(row) for row in sinv]
    for (ps,ss) in zip(cinv, scinv):
        for p,s in zip(ps,ss):
            assert (p - float(s) < tol)
    
    await mpc.shutdown()

    
if __name__ == "__main__":
    passes = 1
    try: mpc.run(test_EGJ())
    except: passes = 0
    print("Passed {} / 1 EGJ test".format(passes))

    fails = 0
    tests = 10
    n = 5 if len(sys.argv) == 1 else int(sys.argv[1])
    fname = "results/{}.dat".format(n)
    open(fname, 'w').close() # clear file contents
    for i in range(tests):
        try:
            mpc.run(test_e2e(n, tol=100,fname=fname))
        except:
            print("Large error in e2e!")
            fails += 1
    print("Passed {} / {} e2e tests".format(tests - fails, tests))

    fails = 0
    tests = 0
    for i in range(tests):
        try:
            mpc.run( test_inverse(5))
        except: 
            print("Large error in inversion!")
            fails += 1
    print("Passed {} / {} inversion tests".format(tests - fails, tests))


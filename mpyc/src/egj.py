from __future__ import print_function
from mpyc.runtime import mpc
import numpy as np
import math, sys

import ridgeregression
import egjdata
from noe import findFix_inversion

@mpc.coroutine
async def crossholdings(D,C,S,n, bigsecfxp):
    """
    Computes market valuations in equity / crossholdings model (EGJ)
    Uses the linear solver to compute them properly

    Types:
    D : SecFxp(64,32) [n]
    C : SecFxp(64,32) [n][n]
    S : SecFxp(128,64) [n]
    """

    secfxp = mpc.SecFxp(64,32)
    await mpc.returnType(secfxp, n)

    #bigsecfxp = mpc.SecFxp(148,74)

    # this output is > 92 bit integer!
    inv_result = findFix_inversion(C,D,n)

    Cadj = [mpc.convert(r, bigsecfxp) for r in inv_result[:-1]]
    Cdet = mpc.convert(inv_result[-1], bigsecfxp)
    #print("sec adjB", await mpc.output(Cadj))
    #print("sec det", await mpc.output(Cdet))

    result = mpc.schur_prod(S, Cadj)
    result = [ mpc.convert(x / Cdet, secfxp) for x in result ]

    return result


async def main(n, egj=False):
    
    if egj:
        # use data from EGJ dataset
        Sc, Cc, Dc = egjdata.getEGJData()
    else:
        # use synthetic data
        Dc,Cc,Sc = egjdata.readData(n)

    await mpc.start()

    # these bit sizes work for up to 9 parties
    secfxp = mpc.SecFxp(64, 32)
    bigsecfxp = mpc.SecFxp(212,106)

    # initialize inputs
    C = [[secfxp(c.item()) for c in row] for row in Cc] # holdings
    D = [secfxp(d.item()) for d in  Dc] #assets
    S = [bigsecfxp(Sc[i,i].item()) for i in range(n)]

    # compute crossholdings
    values = crossholdings(D,C,S,n,bigsecfxp)

    result = await mpc.output(values)
    
    print("result:", result)
    await mpc.shutdown()


if __name__ == '__main__':
    
    n = int(sys.argv[1])
    mpc.run(main(n))


'''
# no longer using this but it seems useful to keep around
@mpc.coroutine
async def invert(A):
    """
    takes inverse of a matrix 
    modified from mpyc ridgeregression demo
    this function is somewhat tested! (except for identity)
    """
    secfld = type(A[0][0])
    d = len(A)
    await mpc.returnType(secfld, d*d+1)

    R, detR = ridgeregression.random_matrix_determinant(secfld, d)
    RA = mpc.matrix_prod(R, A)
    RA = await mpc.output([a for row in RA for a in row])
    RA = np.reshape(RA, (d, d))
    R = await mpc.gather(R)  # NB: R is secret-shared
    
    invA, detRA = ridgeregression.bareiss(secfld.field, np.concatenate((RA, R), axis=1))
    detA = detRA / detR
    adjA = [da * detA for row in invA for da in row]
    return adjA + [detA]

'''

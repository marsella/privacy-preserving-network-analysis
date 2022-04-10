from mpyc.runtime import mpc
import ridgeregression
import data.bisdata as bisdata # generated automatically by BIS/analyze.py
import numpy as np
import sys

### MAIN
# initializes variables
# calls function
#
async def main(version, NUM_BANKS):
    await mpc.start()

    secint = mpc.SecInt(64)
    secfxp = mpc.SecFxp(64,32)

    # For now, we treat all inputs as fixed point. This avoids nasty conversion issues
    # probably at an efficiency trade-off, but there ya go.
    # fixed inputs
    individual_debts = [list(map(secfxp, bisdata.individual_debts[i])) for i in range(NUM_BANKS)]
    total_debts      = list(map(secfxp, bisdata.total_debts))
    reserves         = list(map(secfxp, bisdata.reserves))

    proportions      = [list(map(secfxp, row)) for row in bisdata.props]

    # updated inputs
    defaults         = list(map(secfxp, [0] * NUM_BANKS))
    payment_strategy = list(map(secfxp, bisdata.total_debts))

    final_payments = payment_strategy 
    if version == "iterative":
        print ("running ITERATIVE algorithm")
        final_payments = iterativeFailures(individual_debts, total_debts, reserves, proportions, defaults, payment_strategy, NUM_BANKS)

        fpay = await mpc.output(final_payments)
        w = list(map(float, fpay))

    elif version == "inversion":
        print ("running INVERSION algorithm")
        final_payments = inversionFailures(individual_debts, total_debts, reserves, proportions, defaults, NUM_BANKS)

        w = await mpc.output(final_payments)

    print ('final strategy: ', w)
    await mpc.shutdown()


### OPTIMIZED GAUSSIAN ALGORITHM
# calls the optimized linear solver from Blom et al.
# this doesn't give back a full inverted matrix, just the adjugate + determinant
# so there's some different logic to use these without resorting to divison
@mpc.coroutine
async def inversionFailures(individual_debts, total_debts,reserves,proportions,defaults, NUM_BANKS):
    divfxp = mpc.SecFxp(256,128)
    await mpc.returnType(divfxp, NUM_BANKS)
    secfxp = mpc.SecFxp(64,32)

    payment_strategy = [0]*(NUM_BANKS+1)
    for k in range(NUM_BANKS):
        A = [[0] * NUM_BANKS for _ in range(NUM_BANKS)] # fxp
        b = [0] * NUM_BANKS # fxp

        nondefault_totals = [0] * NUM_BANKS # fxp: (I-Lambda)*p_bar
        default_reserves = [0] * NUM_BANKS # fxp: Lambda*e
        failure_payouts = [[0] * NUM_BANKS for _ in range(NUM_BANKS)] # fxp: Lambda*Pi^T

        print("Round ", k, ":", await mpc.output(defaults));
        for i in range(NUM_BANKS):
            nondefault_totals[i] = (1 - defaults[i]) * total_debts[i]
            default_reserves[i] = defaults[i] * reserves[i]


            for j in range(NUM_BANKS):
                # innerproduct of diagonal matrix defaults[i][-] 
                # and proportions[j][-] (eg transpose(proportions)[-][j])
                failure_payouts[i][j] = defaults[i] * proportions[j][i]
                A[i][j] = failure_payouts[i][j]  * defaults[j] 

        for i in range(NUM_BANKS):
            b[i] = default_reserves[i] + nondefault_totals[i]
            for j in range(NUM_BANKS):
                b[i] += failure_payouts[i][j] * nondefault_totals[j]

        # find equilibrium
        payment_strategy = findFix_inversion(A,b,NUM_BANKS)
        detP = payment_strategy[-1]
        adjP = [mpc.convert(r, mpc.SecFxp(64,32)) for r in payment_strategy[:-1]]

        # update defaulters
        # you default if credits < debts which is equal to
        #     proportions * adjP < detP (debts - reserves) 
        # type hack here: comparison doesn't work well on fxps
        # so convert each side to ints, then compare, then convert back to fxp
        # note that the ints here have to be big enough for correct comparison

        secint = mpc.SecInt(128)
        de = mpc.vector_sub(total_debts, reserves) 

        for i in range(NUM_BANKS):
            pdei = detP * mpc.convert(de[i], secint)
            
            payable = mpc.SecFxp(64,32)(0)
            for j in range(NUM_BANKS):
                payable += proportions[j][i] * adjP[j]

            payi = mpc.convert(payable,secint) 

            defaults[i] = mpc.convert(pdei > payi, secfxp)

    payment_strategy = [mpc.convert(ps, divfxp) for ps in payment_strategy]
    det = payment_strategy[-1]
    payment_strategy = [ps / det for ps in payment_strategy[:-1]]

    return payment_strategy

# finds a fixed point by solving (I-A)x = b for x.
# returns [adj (I-A); det (I-A)] all in one list
@mpc.coroutine
async def findFix_inversion(A,b, n):
    await (mpc.returnType(mpc.SecInt(128), n + 1))
    secint = mpc.SecInt(128)

    I = np.identity(n, int)
    IA = [[0 for i in range(n)] for j in range(n)]

    # compute I-A
    # scale to integers (todo: discuss alpha selection)
    alpha = 15
    beta = mpc.SecFxp(64,32)(2**alpha)

    # scale A, convert to integers
    for i in range(n):
        for j in range(n):
            IA[i][j] = mpc.convert(beta * (I[i][j] - A[i][j]), secint)

    # convert b to nx1 matrix, scale to integers
    B = [[mpc.convert(beta * bi, secint)] for bi in b]

    # solve for payment strategy 
    result = ridgeregression.linear_solve(IA,B)

    return result



### NAIVE SOLUTION
# uses an iterative algorithm to find fixed point
@mpc.coroutine
async def iterativeFailures(individual_debts, total_debts,reserves,proportions,defaults,payment_strategy, NUM_BANKS):
    await mpc.returnType(mpc.SecFxp(64,32), NUM_BANKS)
    secfxp = mpc.SecFxp(64,32)

    for k in range(NUM_BANKS):
        A = [[0] * NUM_BANKS for _ in range(NUM_BANKS)] # fxp
        b = [0] * NUM_BANKS # fxp

        nondefault_totals = [0] * NUM_BANKS # fxp: (I-Lambda)*p_bar
        default_reserves = [0] * NUM_BANKS # fxp: Lambda*e
        failure_payouts = [[0] * NUM_BANKS for _ in range(NUM_BANKS)] # fxp: Lambda*Pi^T

        print("Round ", k, ":", await mpc.output(defaults));
        for i in range(NUM_BANKS):
            nondefault_totals[i] = (1 - defaults[i]) * total_debts[i]
            default_reserves[i] = defaults[i] * reserves[i]


            for j in range(NUM_BANKS):
                # innerproduct of diagonal matrix defaults[i][-] 
                # and proportions[j][-] (eg transpose(proportions)[-][j])
                failure_payouts[i][j] = defaults[i] * proportions[j][i]
                A[i][j] = failure_payouts[i][j]  * defaults[j] 

        for i in range(NUM_BANKS):
            b[i] = default_reserves[i] + nondefault_totals[i]
            for j in range(NUM_BANKS):
                b[i] += failure_payouts[i][j] * nondefault_totals[j]

        # find equilibrium
        payment_strategy = findFix(A,b,payment_strategy,NUM_BANKS)

        # update defaulters
        for i in range(NUM_BANKS):
            actual_credits = reserves[i]
            for j in range(NUM_BANKS):
                actual_credits += proportions[j][i] * payment_strategy[j]

            # sad type hack: comparison doesn't work on fxps
            # so convert debts/credits to ints before comparison
            # then convert the result back to a fxp :(
            secint = mpc.SecInt(64)
            cmpr = mpc.convert(total_debts[i],secint) > mpc.convert(actual_credits,secint) 
            defaults[i] = mpc.convert(cmpr, secfxp)

    return payment_strategy

@mpc.coroutine
async def findFix(A, b, payment_strategy,NUM_BANKS):
    await mpc.returnType(mpc.SecFxp(64,32), NUM_BANKS)
    # k shouldn't really have an upper bound but we need to put something in.
    new_strategy = [0] * NUM_BANKS # fxp
    for k in range(NUM_BANKS):
      for i in range(NUM_BANKS):
        new_strategy[i] = b[i]
        for j in range(NUM_BANKS):
          new_strategy[i] += A[i][j] * payment_strategy[j]

      payment_strategy = new_strategy

    return new_strategy 

if __name__ == '__main__':
    
    version = "inversion"
    n = 4
    if len(sys.argv) == 3:
        version = sys.argv[1] 
        n = int(sys.argv[2])

    if version not in ["inversion", "iterative"]:
        print ("strategy not implemented. try [inversion] or [iterative]")
    else:
        mpc.run(main(version,n))


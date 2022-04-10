/**
 * Goal: implement an algorithm to balance banks
 * As given in Eisenberg and Noe, Systematic Risk in Financial Systems
 *
 **/

#include<iostream>
#include<fstream>
#include<string>
#include<vector>

#include "bank.h"
#include "matrix_utils.h"

using namespace std;



vector< vector<float> > buildLiabilitiesMatrix(vector<Bank> banks) {
    float pi;
    vector< vector<float> > liabilities(banks.size(), vector<float>(banks.size(),0));

    for (uint i=0; i<banks.size(); i++) {
        pi = banks[i].getTotalDebt();
        for (uint j=0; j<banks.size(); j++) {
            liabilities[i][j] = (pi > 0) ? banks[i].getDebt(j) / pi : 0;
        }
    }

    return liabilities;
}


vector<float> findFixedPoint(vector< vector<float> > liabilities, 
    vector< vector<float> > defaults, vector<float> payments, vector<Bank> banks) {

    vector<float> reserves(banks.size(), 0);
    vector<float> debts(banks.size(), 0);
    for(uint i=0; i<banks.size(); i++) {
        reserves[i] = banks[i].reserve;
        debts[i] = banks[i].getTotalDebt();
    }

    vector< vector<float> > A;
    /* A = /\* Pi^T * /\*/
    A = mMultiply(defaults, mTranspose(liabilities));
    A = mMultiply(A, defaults);

    vector<float> b;
    /* updated version, with I-/\p corrected?? */
    vector<float> nondefault_totals = mVProduct(mIdSubtract(defaults),debts);

    b = mVProduct(mTranspose(liabilities), nondefault_totals);
    b = mVProduct(defaults, vSum(b, reserves));
    b = vSum(b, nondefault_totals);

    vector<float> fnew = payments;
    vector<float> fprev;
    float THRESHOLD = 0;  // todo: experiment with larger values 
    //float delta = 1;
    int delta_rounds = 0;

    cout << "A" << endl;
    printMatrix(A);

    cout << "b:\t";
    printVector(b);
    
    //cout << "ps: ";
    //printVector(payments);
    //while(delta > THRESHOLD) {
    while (delta_rounds < banks.size()) {
        fprev = fnew;
        fnew = vSum(mVProduct(A, fprev), b);

        //cout << "ps" << delta_rounds << ": ";
        //printVector(fnew);

        //delta = vDifference(fnew, fprev);
        delta_rounds++;
    }
    //cout << delta_rounds << " rounds to reduce delta" << endl;

    return fnew;
}

int computePayableCredits(vector< vector<float> > liabilities, 
    vector<float> payments, int receiving_bank) {
    int sum = 0;
    for(uint i=0; i<payments.size(); i++) {
        sum += payments[i] * liabilities[i][receiving_bank];
    }
    return sum;
}

// todo: compute new defaults (?)
vector< vector<float> > findDefaults(vector<Bank> banks, 
    vector< vector<float> > defaults, 
    vector< vector<float> > liabilities, 
    vector<float> payments) {
    int debt, payable_credits, income;

    for(uint i=0; i<banks.size(); i++) {
        debt = banks[i].getTotalDebt();
        payable_credits = computePayableCredits(liabilities, payments, i);
        income = banks[i].reserve + payable_credits;
        // default if your total income under this payment scheme can't cover
        // your debt
        defaults[i][i] = (income >= debt)? 0 : 1;    
    }

    /* compute using alternate approach */
    cout << "de: [";
    for(uint i=0; i < banks.size(); i++) {
      int de = banks[i].getTotalDebt() - banks[i].reserve;
      cout << de << ", ";

      int payable = 0;
      for (uint j=0; j < banks.size(); j++) {
        payable += liabilities[j][i] * payments[j];
      }
      //cout << (de > payable?1:0) << ", ";
    }
    cout << "]" << endl;

    return defaults;
}

bool isSolvent(vector<Bank> banks, vector< vector<float> > defaulted, 
        vector<float> payments, vector< vector<float> > liabilities) {
    // todo: how do we verify this?
    bool solvent = true;
    for( uint i=0; i<banks.size(); i++) {
        // check whether resesrves + portion paid to this bank  
        // is enough to cover their portion of their liabilities
        float debt = banks[i].getTotalDebt();
        float payable_credits = computePayableCredits(liabilities, payments, i);
        float income = banks[i].reserve + payable_credits;


            string safe = defaulted[i][i] ? " defaulted" : " didn't default!" ;
            cout << "bank " << i << safe << endl;

            cout << "\tincome: " << income << "(" << banks[i].reserve <<
                 "r + " << payable_credits << "c)" << endl;
            cout<< "\tdebt: " << debt << endl;
            cout << "\tpayments: " << payments[i] << endl;

            // you should pay as much as possible
            if (defaulted[i][i]) {
              solvent = solvent and (income - payments[i] < 10); 
            } else {
                // if it's not defaulted and this isn't true, big problem
                assert (payments[i] >= debt);
                solvent = solvent and payments[i] >= debt ;
            }
    }

    cout << "payment strategy: ";
    printVector(payments);

    return solvent;
}


void fixedPointSolvency(vector<Bank> banks) {
    vector< vector<float> > liabilities = buildLiabilitiesMatrix(banks);
    vector< vector<float> > defaulted(banks.size(), vector<float>(banks.size(),0));
    vector<float> payments(banks.size());
    for(uint i=0; i<banks.size(); i++) {
        payments[i] = banks[i].getTotalDebt();
    }

    for(uint i=0; i<banks.size(); i++) {
        cout << "Round " << i << ":\t";
        for (uint i=0; i<defaulted.size(); i++){
          cout << defaulted[i][i] << '\t' ; 
        }
        cout << endl;

        payments = findFixedPoint(liabilities, defaulted, payments, banks);
        cout << "new strat: " ;
        printVector(payments);
        defaulted = findDefaults(banks, defaulted, liabilities, payments);
    }

    // todo: check correctness
    if (isSolvent(banks, defaulted, payments, liabilities)) {
        cout << "solvent!" << endl;
    } else {
        cout << "not solvent :( " << endl;
    }
}


int main() {
    int LEN = 6;
    vector<Bank> banks;
    for(int i=0; i<LEN; i++) {
        banks.push_back(Bank("data/bank" + to_string(i) + ".dat"));
    }
    fixedPointSolvency(banks);
    return 0;
}




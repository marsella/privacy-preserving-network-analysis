#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<cassert>

#include "bank.h"

using namespace std;
/**
 * given a vector of lambdas (percentages) and a list of creditors, computes amount you expect
 * to be paid
 */
float computePaidCredits(vector<double> portion_paid, vector<Debt> credits) {
	float amount_paid = 0;
	for(uint i=0; i<credits.size(); i++) {
		amount_paid += portion_paid[credits[i].id] * credits[i].amount;
	}
	return amount_paid;
}


/**
 * check to see if a solution is solvent
 * e.g. everyone can pay their debts like they said
 */
int isSolvent(vector<Bank> banks, vector<double> portion_paid, vector<bool> defaulted) {
    uint num_banks = banks.size();
	assert(num_banks == portion_paid.size());
	assert(num_banks == defaulted.size());

	int insolvent = 0;

	for(uint i=0; i<num_banks; i++) {
	    // check that each bank can pay the debts they've committed to
		assert( (defaulted[i] and portion_paid[i] < 1) or 
			    (not defaulted[i] and portion_paid[i] == 1));
		float payable_debt = banks[i].getTotalDebt() * portion_paid[i];
		float input = banks[i].reserve + computePaidCredits(portion_paid, banks[i].credits);
		if (payable_debt > input) {
		  insolvent++;
		}
	}
	return insolvent;
}

/**
 * check to see whether any banks go out of business
 * if they all have to pay their debts
 */
void naiveSolvency(vector<Bank> banks) {
	bool unstable = true;
	vector<double> portion_paid(banks.size(), 1);
	vector<bool> defaulted(banks.size(), false);
	int rounds = 0;
	while (unstable) {
		unstable = false;
		for (uint b=0; b<banks.size(); b++) {
			float paid_credits  = computePaidCredits(portion_paid, banks[b].credits);
			float unpaid_debt = portion_paid[banks[b].id] * banks[b].getTotalDebt(); 
			float total_resources = banks[b].reserve + paid_credits - unpaid_debt;
			//cout << banks[b].id << " has total resources of $" << total_resources << endl;

			if (total_resources < 0) {
				// comment the next line out to test general instability 
				unstable = true;
				if (not defaulted[banks[b].id]) {
					unstable = true;
					defaulted[banks[b].id] = true;
				}
				float new_portion = (banks[b].reserve + paid_credits) / banks[b].getTotalDebt();
				portion_paid[banks[b].id] = (1 < new_portion)?1:new_portion;
			}
		}
		rounds ++;
	}

	// check results
	int num_failed = 0;
	int shortfall = 0;

	int num_insolvent = isSolvent(banks, portion_paid, defaulted);

	for(uint b = 0; b<banks.size(); b++) {
		if (portion_paid[banks[b].id] < 1) {
			num_failed++;
			shortfall += (1 - portion_paid[banks[b].id]) * banks[b].getTotalDebt();
		}
	}
	cout << "failed banks: " << num_failed 
		 << " (" << num_insolvent << " still insolvent)" << endl;
	cout << "shortfall: " << shortfall << endl;
	cout << "total rounds: " << rounds << endl;
}

int main() {
	int LEN = 10;
	vector<Bank> banks;
	for(int i=0; i<LEN; i++) {
		banks.push_back(Bank("data/bank" + to_string(i) + ".dat"));
	}

	naiveSolvency(banks);
	return 0;
}


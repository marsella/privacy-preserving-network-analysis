#include<fstream>
#include<string>
#include<vector>

using namespace std;

#ifndef __BANK_H
#define __BANK_H

typedef struct Debt {
    int id;
    uint amount;
} Debt;

class Bank {
    private:
        int total_debt;
    public:

        int id;
        vector<Debt> debts;
        vector<Debt> credits;
        int reserve;

        Bank() {
            id = -1;
            reserve = 0;
            total_debt = 0;
        }

        Bank(string filename) {
            ifstream input;
            input.open(filename);
            cout << "reading data from " << filename << endl;
            Debt c;
            char type;
            total_debt = 0;

            while (! input.eof()) {
                input >> type;
                if (input.eof()) break;
                switch(type) {
                    case 'c':
                        input >> c.id >> c.amount;
                        credits.push_back(c);
                        break;
                    case 'd':
                        input >> c.id >> c.amount;
                        debts.push_back(c);
                        total_debt += c.amount;
                        break;
                    case 'i':
                        input >> this->id;
                        break;
                    case 'r':
                        input >> this->reserve;
                        break;
                    default:
                        cout << "error: bad input type: " << type  << endl;
                }
            }
        }


        int getTotalDebt() {
            return total_debt;
        }

        /* this is very inefficient. we probably want some constant time
         * representation
         * although that might be void when we switch to mpc anyway
         */
        int getDebt(int j_id) {
            for(uint i=0; i<debts.size(); i++) {
                if (debts[i].id == j_id) {
                    return debts[i].amount;
                }
            }
            return 0;
        }

};

#endif

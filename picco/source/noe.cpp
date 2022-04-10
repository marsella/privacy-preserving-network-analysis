public int NUM_BANKS = 5;

void initializeData(
    int individual_debts[NUM_BANKS][NUM_BANKS],
    int total_debts[NUM_BANKS],
    int reserves[NUM_BANKS],
    float proportions[NUM_BANKS][NUM_BANKS],
    float defaults[NUM_BANKS][NUM_BANKS],
    float payment_strategy[NUM_BANKS]) {

  // temporary holding pens 
  private int debts1[NUM_BANKS], debts2[NUM_BANKS],debts3[NUM_BANKS], debts4[NUM_BANKS], debts5[NUM_BANKS];
  private float proportions1[NUM_BANKS], proportions2[NUM_BANKS],proportions3[NUM_BANKS], proportions4[NUM_BANKS], proportions5[NUM_BANKS];

  public int i;

  // read in reserve values 
  private int res;
  smcinput(reserves[0],1);
  smcinput(reserves[1],2);
  smcinput(reserves[2],3);
  smcinput(reserves[3],4);
  smcinput(reserves[4],5);

  // read in debts
  smcinput(debts1, 1, NUM_BANKS);
  smcinput(debts2, 2, NUM_BANKS);
  smcinput(debts3, 3, NUM_BANKS);
  smcinput(debts4, 4, NUM_BANKS);
  smcinput(debts5, 5, NUM_BANKS);

  // read in sum of debts for each bank
  smcinput(total_debts[0],1);
  smcinput(total_debts[1],2);
  smcinput(total_debts[2],3);
  smcinput(total_debts[3],4);
  smcinput(total_debts[4],5);

  // read in debt proportions
  smcinput(proportions1, 1, NUM_BANKS);
  smcinput(proportions2, 2, NUM_BANKS);
  smcinput(proportions3, 3, NUM_BANKS);
  smcinput(proportions4, 4, NUM_BANKS);
  smcinput(proportions5, 5, NUM_BANKS);

  // fill in individual debt and proportion matrix
  // opt: can we parallelize this?
  for(i=0; i<NUM_BANKS; i++) {

    individual_debts[0][i] = debts1[i];
    individual_debts[1][i] = debts2[i];
    individual_debts[2][i] = debts3[i];
    individual_debts[3][i] = debts4[i];
    individual_debts[4][i] = debts5[i];

    proportions[0][i] = proportions1[i];
    proportions[1][i] = proportions2[i];
    proportions[2][i] = proportions3[i];
    proportions[3][i] = proportions4[i];
    proportions[4][i] = proportions5[i];
  }

  // initialize payment strategy with no defaults
  // nb: casting from private int to private float doesnt seem to work
  // so we'll just make banks input the same value twice ugh
  smcinput(payment_strategy[0],1);
  smcinput(payment_strategy[1],2);
  smcinput(payment_strategy[2],3);
  smcinput(payment_strategy[3],4);
  smcinput(payment_strategy[4],5);

  // initialize: no banks have defaulted
  for(i=0; i<NUM_BANKS; i++) {
    for(public int j=0; j<NUM_BANKS; j++) {
      defaults[i][j] = 0.0;
    }
  }
}


void iterativeFailures(
    int individual_debts[NUM_BANKS][NUM_BANKS],
    int total_debts[NUM_BANKS],
    int reserves[NUM_BANKS],
    float proportions[NUM_BANKS][NUM_BANKS],
    float defaults[NUM_BANKS][NUM_BANKS],
    float payment_strategy[NUM_BANKS]) {

  public int i; public int j; public int k;
  for(i=0; i<NUM_BANKS; i++) {
    private float A[NUM_BANKS][NUM_BANKS];
    private float b[NUM_BANKS];

    private float failure_payouts[NUM_BANKS][NUM_BANKS]; // Lambda*Pi^T
    for(i=0; i<NUM_BANKS; i++) {
      for(j=0; j<NUM_BANKS; j++) {
        // inner product of defaults[i][-] and proportions[j][-]
        // not implemented for floats
        //failure_payouts[i][j] = defaults[i] @ proportions[j];
        for(k=0; k<NUM_BANKS; k++) {
          // this fails with an rsa encrypt error
          failure_payouts[i][j] += defaults[i][k] * proportions[j][k];
        }
      }
    }

    private float non_default_totals; // (I-Lambda)*p_bar


    
    //findFix(A,b,payment_strategy);
  }

}

public int main() {
    // FIXED DATA
    // debt matrix. element (i,j) refers to the debt from i to j.
    private int individual_debts[NUM_BANKS][NUM_BANKS];
    // total debt. element (i) is the sum of the ith dimension of the debt matrix.
    private int total_debts[NUM_BANKS]; 
    // cash reserves for each bank.
    private int reserves[NUM_BANKS];
    // transpose of the payout proportion matrix. 
    // element (j,i) is the proportion of i's debt owed to j
    private float proportions[NUM_BANKS][NUM_BANKS];

    // UPDATED STRUCTURES
    // default matrix. element (i,i) = 1 if bank i has defaulted
    private float defaults[NUM_BANKS][NUM_BANKS];
    // payment strategy. Adjusted payments, given a set of defaulted banks
    private float payment_strategy[NUM_BANKS];

    //readData(individual_debts, reserves);
    initializeData(individual_debts, total_debts, reserves, 
                   proportions, defaults, payment_strategy);

    iterativeFailures(individual_debts, total_debts, reserves, 
                      proportions, defaults, payment_strategy);

    return 0;

}


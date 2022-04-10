#include "matrix_utils.h"

int main() {
    vector <vector <float> > A(2, vector <float>(2,0));
    A[0][1] = 1;
    printMatrix(A);

    A = mTranspose(A);
    printMatrix(A);
    A = mTranspose(A);
    printMatrix(A);


    vector <vector <float> > B(2, vector <float>(2,0));
    B[1][0] = 1;
    printMatrix(B);

    printMatrix(mTranspose(B));

    vector <vector <float> > result = mMultiply(A,B);
    printMatrix(result);

    result = mMultiply(B,A);
    printMatrix(result);

    vector<float> v(2,2);
    vector<float> vresult = mVProduct(A,v);
    printVector(vresult);

    cout << "vdiff: " << vDifference(v, vresult) << endl;

    vresult = mVProduct(B,v);
    printVector(vresult);

    vresult = vSum(vresult, v);
    printVector(vresult);
    
    return 0;
}


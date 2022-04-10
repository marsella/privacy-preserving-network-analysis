#include<iostream>
#include<string>
#include<vector> 
#include<cmath>
#include<assert.h>

using namespace std;

// TODO: none of these check dimensions very carefully

void printVector(vector <float> v) {
    for(uint i=0; i<v.size(); i++) {
      cout << v[i] << '\t';
    }
    cout << endl; // << "=========" << endl;;
}


void printMatrix(vector <vector <float> > A) {
  for(uint i=0; i<A.size(); i++) {
    for(uint j=0; j<A[i].size(); j++) {
      cout << A[i][j] << "\t";
    }
    cout << endl;
  }
  //cout << "=========" << endl;
}

vector <vector <float> > mMultiply(vector <vector <float> > A, 
                                  vector <vector <float> > B) {
    uint N = A.size();
    uint M = B.size();
    uint P = B[0].size();
    uint bsize = B.size();
    vector <vector <float> > result(N, vector<float>(P,0));

    for(uint n=0; n<N; n++) {
        assert(A[n].size() == bsize );
        for(uint p=0; p<P; p++) {
            for(uint m=0; m<M; m++) {
                result[n][p] += A[n][m] * B[m][p];
            }
        }
    }
    return result;
}


vector <float> mVProduct(vector <vector <float> > A, vector <float> v) {
    vector <float> result(A[0].size(), 0);

    for(uint i=0; i<A.size(); i++) {
        for(uint j=0; j<A[i].size(); j++) {
            result[i] += A[i][j] * v[j];
        }
    }
    return result;
}

vector<float> vSum(vector<float> v, vector<float> w) {
    assert(v.size() == w.size());
    vector <float> result(v.size(), 0);
    for(uint i=0; i<v.size(); i++) {
        result[i] = v[i] + w[i];
    }
    return result;
}

vector<float> vSub(vector<float> v, vector<float> w) {
    assert(v.size() == w.size());
    vector <float> result(v.size(), 0);
    for(uint i=0; i<v.size(); i++) {
        result[i] = v[i] - w[i];
    }
    return result;
}

/* computes the euclidean distance between v,w */
float vDifference(vector<float> v, vector<float> w) {
    float sum = 0;
    assert( v.size() == w.size() );
    for (uint i=0; i < v.size(); i++) {
        sum += (v[i] - w[i]) * (v[i] - w[i]);
    }

    return sqrt(sum);


}

vector< vector<float> > mTranspose(vector <vector<float> >A) {
    uint innersize = A[0].size();
    vector< vector<float> > result(innersize, vector<float>(A.size(),0));
    for(uint i=0; i<A.size(); i++) {
        assert(A[i].size() == innersize);
        for( uint j=0; j<innersize; j++) {
            result[j][i] = A[i][j];
        }
    }
    return result;
}


vector< vector<float> > mIdSubtract(vector <vector<float> >A) {
    uint innersize = A[0].size();
    vector< vector<float> > result(A.size(), vector<float>(innersize,0));
    for(uint i=0; i<A.size(); i++) {
      assert(A[i].size() == innersize);
      for( uint j=0; j<innersize; j++) {
        result[i][j] = (i==j?1:0) - A[i][j];
      }
    }
    return result;
}

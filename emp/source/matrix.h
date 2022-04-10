#ifndef MATRIX_H__
#define MATRIX_H__

using namespace emp;
using namespace std;
#include "emp-tool/circuits/bit.h"
#include "emp-tool/circuits/integer.h"
#include "math.h"
#include "fixedp.h"


class Matrix{
public: 
	int nr, nc;
	bool isdiagonal = false;
	bool isUT = false;
	bool isLT = false;

	//This will store the actual values of the matrix in row-major format
	FixedP *A;

	Matrix( );
	Matrix( const Matrix& rhs );
	Matrix( FixedP xx[], int nr, int nc );
	
	FixedP determinant();
	Matrix transpose();
	Matrix zeromatrix( int nr, int nc );
	Matrix identitymatrix( int nr );
	Matrix diagonalmatrix( FixedP xx[], int nr);
	Matrix ijminor( int i, int j );
	Matrix adjugate();
	void print();
	Matrix operator+( const Matrix & rhs ) const;
	Matrix operator-( const Matrix & rhs ) const;
	Matrix operator*( const Matrix & rhs ) const;

};

Matrix::Matrix() {
}

Matrix::Matrix( FixedP xx[], int numrows, int numcols ) {
	nr = numrows;
	nc = numcols;
	A = new FixedP[nr*nc];
	for( int i=0; i < nr; i++ ) {
		for( int j = 0; j < nc; j++ ) {
			A[i*nc+j] = xx[i*nc+j];
		}
	}
}

Matrix::Matrix( const Matrix& rhs ) {
	nr = rhs.nr;
	nc = rhs.nc;
	isdiagonal = rhs.isdiagonal;
	isUT = rhs.isUT;
	isLT = rhs.isLT;
	
	A = rhs.A;
}

Matrix zeromatrix( int nr, int nc ){
	
	FixedP zs[nr*nc];
	for( int i=0; i < nr*nc; i++ ) {
		zs[i] = FixedP( (double) 0, PUBLIC);	
	}
	Matrix zz( zs, nr, nc );
	zz.isdiagonal = true;
	return( zz );
}

Matrix identitymatrix( int nr ) {
	FixedP id[nr*nr];
	for( int i=0; i < nr; i++ ) {
		for( int j=0; j < nr; j++ ) {
			if( i == j ) {
				id[i*nr+j] = FixedP( (double) 1, PUBLIC);	
			}
			else {
				id[i*nr+j] = FixedP( (double) 0, PUBLIC );	
			}
		}
	}
	Matrix idm( id, nr, nr );
	idm.isdiagonal=true;
	return( idm );
}

Matrix diagonalmatrix( FixedP xx[], int nr ) {
	FixedP id[nr*nr];
	for( int i=0; i < nr; i++ ) {
		for( int j=0; j < nr; j++ ) {
			if( i == j ) {
				id[i*nr+j] = xx[i];
			}
			else {
				id[i*nr+j] = FixedP( 0, PUBLIC );	
			}
		}
	}
	Matrix idm( id, nr, nr );
	idm.isdiagonal=true;
	return( idm );
}

void Matrix::print() {
	for( int i=0; i < nr; i++ ) {
		cout << "[ ";
		for( int j = 0; j < nc; j++ ) {
			A[i*nc+j].print();
			cout << " ";
		}
		cout << "]\n";
	}
}

Matrix Matrix::ijminor( int i, int j ) {
	FixedP T[(this->nr-1)*(this->nc-1)];

	int rs,cs;	
	for( int rd = 0; rd < this->nr-1; rd ++ ) {
		if( rd >= i ) {
			rs = rd+1;
		}
		else {
			rs = rd;
		}
		for( int cd= 0; cd < this->nc-1; cd ++ ) {
			if( cd >= j ) {
				cs = cd+1;
			}
			else {
				cs = cd;
			}
			T[rd*(this->nr-1)+cd] = this->A[rs*this->nr+cs];
		}
	}
	Matrix res( T, this->nr-1, this->nc -1 );
	return( res );
}

FixedP Matrix::determinant() {
	if( this->nr != this->nc ) {
		cout << "ERROR in determinant." << endl;
		cout << "this->nr != this->nc" << endl;
		cout << this->nr << " != " << this->nc << endl;
	}

	if( this->nr == 1 ) {
		return( this->A[0] );
	}
	if( this->nr == 2 ) {
		return( this->A[0]*this->A[3] - this->A[1]*this->A[2] );
	}
	if( this->nr == 3 ) {
		return( this->A[0]*(this->A[5]*this->A[8] - this->A[5]*this->A[7]) - this->A[1]*(this->A[3]*this->A[8]-this->A[5]*this->A[6]) + this->A[2]*(this->A[3]*this->A[7]-this->A[4]*this->A[6]) );
	}

	Matrix M;
	FixedP det( (double) 0, PUBLIC);
	FixedP mdet;
	for( int i=0; i<this->nr; i++ ) {
		int j = 0;
		M = this->ijminor(i,j);
		mdet = M.determinant();
		if( (i+j)%2 == 0 ) {
			det = det + this->A[i*this->nr+j]*mdet;
		}	
		else {
			det = det - A[i*this->nr+j]*mdet;
		}
	}	
	return( det );
}

Matrix Matrix::adjugate() {
	if( this->nr != this->nc ) {
		cout << "ERROR in adjugate." << endl;
		cout << "this->nr != this->nc" << endl;
		cout << this->nr << " != " << this->nc << endl;
	}
	Matrix M;
	FixedP T[this->nr*this->nc];
	FixedP mdet;
	for( int i=0; i<this->nr; i++ ) {
		for( int j=0; j<this->nc; j++ ) {
			M = this->ijminor(i,j);
			mdet = M.determinant();
			if( (i+j)%2 == 0 ) {
				T[i*this->nr+j] = mdet;
			}
			else {
				T[i*this->nr+j] = -mdet;
			}
		}
	}	
	Matrix res( T, this->nr, this->nc );
	return( res );
}

Matrix Matrix::operator+( const Matrix & rhs ) const {
	if( this->nr != rhs.nr ) {
		cout << "Number of rows don't match!" << endl;
		cout << "lhs.nr = " << this->nr << endl;
		cout << "rhs.nr = " << rhs.nr << endl;
		return( rhs );
	}
	if( this->nc != rhs.nc ) {
		cout << "Number of columns don't match!" << endl;
		cout << "lhs.nc = " << this->nc << endl;
		cout << "rhs.nc = " << rhs.nc << endl;
		return( rhs );
	}

	if( this->isdiagonal ) {
		if( this->nr != this->nc ) { cout << "ERROR: a non-square matrix claims to be diagonal" << endl; }
		Matrix res(rhs.A, rhs.nr, rhs.nc );
		for( int i=0; i < this->nr; i ++ ) {
			res.A[i*nr+i] = res.A[i*nr+i] + this->A[i*nr+i];
		}
		return( res );
	}

	if( rhs.isdiagonal ) {
		if( rhs.nr != rhs.nc ) { cout << "ERROR: a non-square matrix claims to be diagonal" << endl; }
		Matrix res(this->A,this->nr, this->nc);
		for( int i=0; i < this->nr; i ++ ) {
			res.A[i*nr+i] = res.A[i*nr+i] + rhs.A[i*nr+i];
		}
		return( res );
	}

	FixedP tt[this->nr*this->nc];
	for( int i=0; i < this->nr; i ++ ) {
		for( int j = 0; j < this->nc; j ++ ) {
			tt[i*nc+j] = this->A[i*nc+j]+rhs.A[i*nc+j];
		}
	}

	Matrix res( tt, this->nr, this->nc );
	return( res );

}

Matrix Matrix::operator-( const Matrix & rhs ) const {
	if( this->nr != rhs.nr ) {
		cout << "Number of rows don't match!" << endl;
		cout << "lhs.nr = " << this->nr << endl;
		cout << "rhs.nr = " << rhs.nr << endl;
		return( rhs );
	}
	if( this->nc != rhs.nc ) {
		cout << "Number of columns don't match!" << endl;
		cout << "lhs.nc = " << this->nc << endl;
		cout << "rhs.nc = " << rhs.nc << endl;
		return( rhs );
	}

	if( this->isdiagonal ) {
		if( this->nr != this->nc ) { cout << "ERROR: a non-square matrix claims to be diagonal" << endl; }
		Matrix res(rhs.A, rhs.nr, rhs.nc );
		for( int i=0; i < this->nr; i ++ ) {
			res.A[i*nr+i] = -res.A[i*nr+i] + this->A[i*nr+i];
		}
		return( res );
	}

	if( rhs.isdiagonal ) {
		if( rhs.nr != rhs.nc ) { cout << "ERROR: a non-square matrix claims to be diagonal" << endl; }
		Matrix res(this->A,this->nr, this->nc);
		for( int i=0; i < this->nr; i ++ ) {
			res.A[i*nr+i] = res.A[i*nr+i] - rhs.A[i*nr+i];
		}
		return( res );
	}

	FixedP tt[this->nr*this->nc];
	for( int i=0; i < this->nr; i ++ ) {
		for( int j = 0; j < this->nc; j ++ ) {
			tt[i*nc+j] = this->A[i*nc+j]-rhs.A[i*nc+j];
		}
	}

	Matrix res( tt, this->nr, this->nc );
	return( res );

}

Matrix Matrix::transpose() {
	if( this->isdiagonal ) {
		return( *this );
	}

	FixedP tt[this->nr*this->nc];
	for( int i=0; i < this->nr; i ++ ) {
		for( int j=0; j < this->nc; j++ ) {
			tt[j*this->nr+i] = this->A[i*this->nc + j];
		}
	}
	Matrix res( tt, this->nc, this->nr );
	return( res );
}

Matrix Matrix::operator*( const Matrix & rhs ) const {
	if( this->nc != rhs.nr ) {
		cout << "Number of rows don't match!" << endl;
		cout << "lhs.nc = " << this->nc << endl;
		cout << "rhs.nr = " << rhs.nr << endl;
		return( rhs );
	}
	FixedP tt[this->nr*rhs.nc];
	FixedP t;

	if( this->isdiagonal ) {
		if( this->nr != this->nc ) { cout << "ERROR: a non-square matrix claims to be diagonal" << endl; }
		Matrix res(rhs.A,rhs.nr,rhs.nc);
		for( int i=0; i < res.nr; i++ ) {
			for( int j =0; j < res.nc; j++ ) {
				res.A[i*res.nc+j] = res.A[i*res.nc+j]*this->A[i*this->nr+i];
			}
		}
		return( res );
	}

	if( rhs.isdiagonal ) {
		if( rhs.nr != rhs.nc ) { cout << "ERROR: a non-square matrix claims to be diagonal" << endl; }
		Matrix res(this->A,this->nr,this->nc);
		for( int i=0; i < this->nr; i++ ) {
			for( int j = 0; j < this->nc; j++ ) {
				res.A[i*res.nc+j] = res.A[i*res.nc+j]*rhs.A[j*rhs.nc+j];
			}
		}
		return( res );
	}

	for( int i=0; i < this->nr; i ++ ) {
		for( int j = 0; j < rhs.nc; j ++ ) {
			tt[i*rhs.nc+j] = FixedP( (double) 0,PUBLIC);
			for( int k = 0; k < this->nc; k++ ) {
				t = this->A[i*this->nc + k]*rhs.A[j+k*rhs.nc];
				tt[i*rhs.nc+j] = tt[i*rhs.nc+j] + t;
			}
		}
	}

	Matrix res( tt, this->nr, rhs.nc );
	return( res );
}


#endif //MATRIX_H__

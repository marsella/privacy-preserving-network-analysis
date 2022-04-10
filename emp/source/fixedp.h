#ifndef FIXEDP_H__
#define FIXEDP_H__

using namespace emp;
using namespace std;
#include "emp-tool/circuits/bit.h"
#include "emp-tool/circuits/integer.h"
#include "math.h"

class FixedP{
public: 
	Integer x;
	static int denombits;
	static int bitlen;

	FixedP( );
	FixedP( Integer xx );
	FixedP( double xx, int party=PUBLIC );
	FixedP( string xx, int party=PUBLIC );

	void print();
	void setPrecision(int pbits);
	FixedP operator+( const FixedP & rhs ) const;
	FixedP operator-( const FixedP & rhs ) const;
	FixedP operator*( const FixedP & rhs ) const;
	FixedP operator/( const FixedP & rhs ) const;
	FixedP operator-() const;
	Bit operator<( const FixedP & rhs ) const;
	Bit operator>( const FixedP & rhs ) const;
	FixedP operator>>( int shamt ) const;
	FixedP operator<<( int shamt ) const;
	FixedP sqrt();
	double reveal();
	inline FixedP select(const Bit & select, const FixedP & a) const;

};

FixedP::FixedP() {
}

FixedP::FixedP( Integer xx ) {
	x = xx;
}

FixedP::FixedP( double xx, int party ) {
	long long yy;
	yy = (long long) pow(2,denombits)*xx;
	x = Integer( bitlen, yy, party );
}

FixedP::FixedP( string xx, int party ) {
	long long yy;
	yy = (long long) pow(2,denombits)*atof(xx.c_str());
	x = Integer( bitlen, yy, party );
}

void FixedP::print() {

	try {
		long num;
		num = this->x.reveal<long>();
		long long fulldenom = pow(2,denombits);
		
		if( num % fulldenom == 0 ) {
			cout << (num >> denombits); 
		}
		else {
			cout << ((double) num)/pow(2,denombits);
		}
	}	
	catch(...){
		cout << this->x.reveal<string>();
		cout << "/2^(" << denombits << ")" << endl;
		return;	
	}

	/*	
	if( this->x.length < (int) sizeof(long) ) {
		double yy;
		yy = (double) this->x.reveal<long>();
		yy /= pow( 2, denombits );		
		cout << yy;
	}
	else {
		cout << this->x.reveal<string>();
		cout << "/2^(" << denombits << ")" << endl;
	}
	*/
}


FixedP FixedP::operator+( const FixedP & rhs ) const {
	Integer sum;
	sum = this->x + rhs.x;
	FixedP res( sum );
	return( res );
}

FixedP FixedP::operator-( const FixedP & rhs ) const {
	Integer diff;
	diff = this->x - rhs.x;
	FixedP res( diff );
	return( res );
}

FixedP FixedP::operator-() const {
	Integer zero( this->x.length, 0, PUBLIC );
	FixedP res( zero-this->x );
	return( res );
}

FixedP FixedP::operator*( const FixedP & rhs ) const {
	Integer prod;
	prod = this->x * rhs.x;
	prod = prod >> denombits;
	FixedP res( prod );
	return( res );
}

FixedP FixedP::operator/( const FixedP & rhs ) const {
	Integer quo;
	quo = this->x << denombits;
	quo = quo/rhs.x;
	FixedP res( quo );
	return( res );
}

Bit FixedP::operator<( const FixedP & rhs ) const {
	Bit res = this->x < rhs.x;
	return( res );
}

Bit FixedP::operator>( const FixedP & rhs ) const {
	Bit res = this->x > rhs.x;
	return( res );
}

FixedP FixedP::operator>>( int shamt ) const {
	FixedP res( this->x >> shamt );
	return( res );
}

FixedP FixedP::operator<<( int shamt ) const {
	FixedP res( this->x << shamt );
	return( res );
}

inline FixedP FixedP::select(const Bit & select, const FixedP & a) const{
	FixedP res(*this);
	res.x = this->x.select( select, a.x );
	return res;
}

FixedP FixedP::sqrt() {
	FixedP t((double) 1);
	for( int i=0; i < 10; i ++ ) {
		t = t + *this/t;
		t = t >> 1;
	}
	return( t );
}

double FixedP::reveal() {
	double d = pow(2,this->denombits);
	long n = this->x.reveal<long>();

	return( ((double) n)/d );
}




#endif //FIXEDP_H__

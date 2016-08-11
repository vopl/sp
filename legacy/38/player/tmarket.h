#ifndef _tmarket_h_
#define _tmarket_h_

#include "hel/complex.h"
using namespace hel;

class TMarket
{
	real _t;
	real _lastx;

	real _buyAmount;
	real _balance;

	TVReal _prevBalance;
	real _buyAmount2;
	real _balance2;
public:
	TMarket();
	void setup(real t);
	void step(TVTimedValue &serie, real x, Complex v);
	real getEquity(TVTimedValue &serie, real x);
	real getBalance();
	real getEquity2(TVTimedValue &serie, real x);
	real getBalance2();

private:
	void oper(TVTimedValue &serie, real x, real lot);
};
#endif

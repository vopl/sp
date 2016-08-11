#include "stdafx.h"
#include "tmarket.h"

//////////////////////////////////////////////////////////////////////////
TMarket::TMarket()
	: _lastx(0)
	, _buyAmount(0)
	, _balance(0)
	, _buyAmount2(0)
	, _balance2(0)
{

}

//////////////////////////////////////////////////////////////////////////
void TMarket::setup(real t)
{
	_t = t;
}

//////////////////////////////////////////////////////////////////////////
void TMarket::step(TVTimedValue &serie, real x, Complex v)
{
	if(x-_lastx < _t/4)
	{
		return;
	}

	real cp = cos(v.p());

	if(cp < -0.99)
	{
		oper(serie, x, v.a());
		_lastx = x;
	}
	else if(cp > 0.99)
	{
		oper(serie, x, -v.a());
		_lastx = x;
	}
}

//////////////////////////////////////////////////////////////////////////
real TMarket::getEquity(TVTimedValue &serie, real x)
{
	size_t openIdx = std::lower_bound(serie.begin(), serie.end(), x) - serie.begin();
	if(openIdx >= serie.size())
	{
		openIdx = serie.size()-1;
	}

	return _balance + _buyAmount*serie[openIdx]._value;
}

//////////////////////////////////////////////////////////////////////////
real TMarket::getBalance()
{
	return _balance;
}

//////////////////////////////////////////////////////////////////////////
real TMarket::getEquity2(TVTimedValue &serie, real x)
{
	size_t openIdx = std::lower_bound(serie.begin(), serie.end(), x) - serie.begin();
	if(openIdx >= serie.size())
	{
		openIdx = serie.size()-1;
	}

	return _balance2 + _buyAmount2*serie[openIdx]._value;
}

//////////////////////////////////////////////////////////////////////////
real TMarket::getBalance2()
{
	return _balance2;
}


//////////////////////////////////////////////////////////////////////////
void TMarket::oper(TVTimedValue &serie, real x, real lot)
{
	if(_buyAmount > 10 && lot>0 ||
		_buyAmount < -10 && lot<0)
	{
		return;
	}

	size_t openIdx = std::lower_bound(serie.begin(), serie.end(), x) - serie.begin();
	if(openIdx >= serie.size())
	{
		openIdx = serie.size()-1;
	}

	size_t closeIdx = std::lower_bound(serie.begin(), serie.end(), x+_t/2) - serie.begin();
	if(closeIdx >= serie.size())
	{
		closeIdx = serie.size()-1;
	}

	real dv = serie[closeIdx]._value - serie[openIdx]._value;

	_balance -= lot*serie[openIdx]._value;
	_buyAmount += lot;

	//_prevBalance.push_back(getEquity(serie, x));
	_prevBalance.push_back(dv*lot);

	if(_prevBalance.size() >= 10)
	{
		while(_prevBalance.size()>10)
		{
			_prevBalance.erase(_prevBalance.begin());
		}

		real k,b;
		llsq(NULL, &_prevBalance[0], _prevBalance.size()-1, k, b);

		lot *= k>0?1:-1;
		_balance2 -= lot*serie[openIdx]._value;
		_buyAmount2 += lot;
	}

}

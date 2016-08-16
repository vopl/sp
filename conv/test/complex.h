#ifndef _hel_complex_h_
#define _hel_complex_h_

#include "types.h"
#include "math.h"
#include <cassert>

namespace hel
{
	enum EComplexConstructType
	{
		CREIM=0,
		CAP=1,
	};

	class Complex
	{
	public:
//		Complex();
		Complex(real v1=0, real v2=0, EComplexConstructType ecct = CREIM);
		Complex(const Complex &z);

		void setAP(real a, real p);
		void set(real re, real im);

		real &re();
		real &im();
		const real &re() const;
		const real &im() const;

		real a() const;
		real p() const;

		Complex conj() const;
		Complex sqr() const;
		Complex ln() const;

		Complex rotate(real p) const;

		Complex &operator=(const Complex &z);
		Complex &operator=(const real &re);

		Complex &operator+=(const real &re);
		Complex &operator-=(const real &re);
		Complex &operator*=(const real &re);
		Complex &operator/=(const real &re);

		Complex &operator+=(const Complex &z);
		Complex &operator-=(const Complex &z);
		Complex &operator*=(const Complex &z);
		Complex &operator/=(const Complex &z);

	private:
		real _re;
		real _im;
	};



	//////////////////////////////////////////////////////////////////////////
	typedef std::vector<Complex> TVComplex;


	//////////////////////////////////////////////////////////////////////////
	inline Complex::Complex(real v1, real v2, EComplexConstructType ecct)
		: _re(v1), _im(v2)
	{
		switch(ecct)
		{
		case CAP:
			setAP(v1, v2);
			break;
		case CREIM:
			//set(v1, v2);
			break;
		default:
			assert(0);
			_re = _im = 0;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	inline Complex::Complex(const Complex &z)
		: _re(z._re)
		, _im(z._im)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	inline void Complex::setAP(real a, real p)
	{
		_re = a*cos(p);
		_im = a*sin(p);
	}

	//////////////////////////////////////////////////////////////////////////
	inline void Complex::set(real re, real im)
	{
		_re = re;
		_im = im;
	}

	//////////////////////////////////////////////////////////////////////////
	inline real &Complex::re()
	{
		return _re;
	}

	//////////////////////////////////////////////////////////////////////////
	inline real &Complex::im()
	{
		return _im;
	}

	//////////////////////////////////////////////////////////////////////////
	inline const real &Complex::re() const
	{
		return _re;
	}

	//////////////////////////////////////////////////////////////////////////
	inline const real &Complex::im() const
	{
		return _im;
	}

	//////////////////////////////////////////////////////////////////////////
	inline real Complex::a() const
	{
		return sqrt(hel::sqr(_re) + hel::sqr(_im));
	}

	//////////////////////////////////////////////////////////////////////////
	inline real Complex::p() const
	{
		return atan2(_im, _re);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Complex Complex::conj() const
	{
		return Complex(_re, -_im);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Complex Complex::sqr() const
	{
		return Complex(hel::sqr(_re) - hel::sqr(_im), 2*_re*_im);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Complex Complex::ln() const
	{
		return Complex(0.5*log(_re*_re+_im*_im), p());
	}

	//////////////////////////////////////////////////////////////////////////
	inline 	Complex Complex::rotate(real p) const
	{
		real cosVal = cos(p);
		real sinVal = sin(p);
		return Complex(_re*cosVal - _im*sinVal, _re*sinVal + _im*cosVal);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Complex &Complex::operator=(const Complex &z)
	{
		_re = z._re;
		_im = z._im;
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Complex &Complex::operator=(const real &re)
	{
		_re = re;
		_im = 0;
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Complex &Complex::operator+=(const real &re)
	{
		_re += re;
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Complex &Complex::operator-=(const real &re)
	{
		_re -= re;
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Complex &Complex::operator*=(const real &re)
	{
		_re *= re;
		_im *= re;
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Complex &Complex::operator/=(const real &re)
	{
		_re /= re;
		_im /= re;
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Complex &Complex::operator+=(const Complex &z)
	{
		_re += z._re;
		_im += z._im;
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Complex &Complex::operator-=(const Complex &z)
	{
		_re -= z._re;
		_im -= z._im;
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Complex &Complex::operator*=(const Complex &z)
	{
		real t = _re*z._re - _im*z._im;
		_im = _re*z._im + _im*z._re;
		_re = t;
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Complex &Complex::operator/=(const Complex &z)
	{
		if( fabs(z._im)<fabs(z._re) )
		{
			real e = z._im/z._re;
			real f = z._re+z._im*e;
			real t = (_re+_im*e)/f;
			_im = (_im-_re*e)/f;
			_re = t;
		}
		else
		{
			real e = z._re/z._im;
			real f = z._im+z._re*e;
			real t = (_im+_re*e)/f;
			_im = (-_re+_im*e)/f;
			_re = t;
		}
		return *this;
	}










	//////////////////////////////////////////////////////////////////////////
	inline const bool operator==(const Complex &lhs, const Complex &rhs)
	{
		return lhs.re() == rhs.re() && lhs.im() == rhs.im();
	}

	inline const bool operator==(const real &lhs, const Complex &rhs)
	{
		return lhs == rhs.re() && 0 == rhs.im();
	}

	inline const bool operator==(const Complex &lhs, const real &rhs)
	{
		return lhs.re() == rhs && lhs.im() == 0;
	}

	inline const bool operator!=(const Complex &lhs, const Complex &rhs)
	{
		return !operator==(lhs, rhs);
	}

	inline const bool operator!=(const real &lhs, const Complex &rhs)
	{
		return !operator==(lhs, rhs);
	}

	inline const bool operator!=(const Complex &lhs, const real &rhs)
	{
		return !operator==(lhs, rhs);
	}

	//////////////////////////////////////////////////////////////////////////
	inline const Complex operator+(const Complex &rhs)
	{
		return rhs;
	}

	inline const Complex operator-(const Complex &rhs)
	{
		return Complex(-rhs.re(), -rhs.im());
	}

	//////////////////////////////////////////////////////////////////////////
	inline const Complex operator+(const Complex &lhs, const Complex &rhs)
	{
		return Complex(lhs.re()+rhs.re(), lhs.im()+rhs.im());
	}

	inline const Complex operator+(const real &lhs, const Complex &rhs)
	{
		return Complex(lhs+rhs.re(), 0+rhs.im());
	}

	inline const Complex operator+(const Complex &lhs, const real &rhs)
	{
		return Complex(lhs.re()+rhs, lhs.im()+0);
	}

	//////////////////////////////////////////////////////////////////////////
	inline const Complex operator-(const Complex &lhs, const Complex &rhs)
	{
		return Complex(lhs.re()-rhs.re(), lhs.im()-rhs.im());
	}

	inline const Complex operator-(const real &lhs, const Complex &rhs)
	{
		return Complex(lhs-rhs.re(), 0-rhs.im());
	}

	inline const Complex operator-(const Complex &lhs, const real &rhs)
	{
		return Complex(lhs.re()-rhs, lhs.im()-0);
	}

	//////////////////////////////////////////////////////////////////////////
	inline const Complex operator*(const Complex &lhs, const Complex &rhs)
	{
		return Complex(lhs.re()*rhs.re() - lhs.im()*rhs.im(),  lhs.re()*rhs.im() + lhs.im()*rhs.re());
	}

	inline const Complex operator*(const Complex &lhs, const real &rhs)
	{
		return Complex(lhs.re()*rhs,  lhs.im()*rhs);
	}
	
	inline const Complex operator*(const real &lhs, const Complex &rhs)
	{
		return Complex(lhs*rhs.re(),  lhs*rhs.im());
	}

	//////////////////////////////////////////////////////////////////////////
	inline const Complex operator/(const Complex &lhs, const Complex &rhs)
	{
		return Complex(lhs) /= rhs;
	}
	
	inline const Complex operator/(const real &lhs, const Complex &rhs)
	{
		return Complex(lhs) /= rhs;
	}
	
	inline const Complex operator/(const Complex &lhs, const real &rhs)
	{
		return Complex(lhs.re()/rhs, lhs.im()/rhs);
	}
}

#endif

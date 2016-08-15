#pragma once

#include "types.hpp"
#include <cmath>
#include <cassert>

namespace sp
{
    enum EComplexConstructType
    {
        CREIM=0,
        CAP=1,
    };

    class complex
    {
    public:
//        complex();
        complex(real v1=0, real v2=0, EComplexConstructType ecct = CREIM);
        complex(const complex &z);

        void setAP(real a, real p);
        void set(real re, real im);

        real &re();
        real &im();
        const real &re() const;
        const real &im() const;

        real a() const;
        real p() const;

        complex conj() const;
        complex sqr() const;
        complex ln() const;

        complex rotate(real p) const;

        complex &operator=(const complex &z);
        complex &operator=(const real &re);

        complex &operator+=(const real &re);
        complex &operator-=(const real &re);
        complex &operator*=(const real &re);
        complex &operator/=(const real &re);

        complex &operator+=(const complex &z);
        complex &operator-=(const complex &z);
        complex &operator*=(const complex &z);
        complex &operator/=(const complex &z);

    private:
        real _re;
        real _im;
    };



    //////////////////////////////////////////////////////////////////////////
    typedef std::vector<complex> TVComplex;


    //////////////////////////////////////////////////////////////////////////
    inline complex::complex(real v1, real v2, EComplexConstructType ecct)
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
    inline complex::complex(const complex &z)
        : _re(z._re)
        , _im(z._im)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    inline void complex::setAP(real a, real p)
    {
        _re = a*cos(p);
        _im = a*sin(p);
    }

    //////////////////////////////////////////////////////////////////////////
    inline void complex::set(real re, real im)
    {
        _re = re;
        _im = im;
    }

    //////////////////////////////////////////////////////////////////////////
    inline real &complex::re()
    {
        return _re;
    }

    //////////////////////////////////////////////////////////////////////////
    inline real &complex::im()
    {
        return _im;
    }

    //////////////////////////////////////////////////////////////////////////
    inline const real &complex::re() const
    {
        return _re;
    }

    //////////////////////////////////////////////////////////////////////////
    inline const real &complex::im() const
    {
        return _im;
    }

    //////////////////////////////////////////////////////////////////////////
    inline real complex::a() const
    {
        return sqrt(sp::sqr(_re) + sp::sqr(_im));
    }

    //////////////////////////////////////////////////////////////////////////
    inline real complex::p() const
    {
        return atan2(_im, _re);
    }

    //////////////////////////////////////////////////////////////////////////
    inline complex complex::conj() const
    {
        return complex(_re, -_im);
    }

    //////////////////////////////////////////////////////////////////////////
    inline complex complex::sqr() const
    {
        return complex(sp::sqr(_re) - sp::sqr(_im), 2*_re*_im);
    }

    //////////////////////////////////////////////////////////////////////////
    inline complex complex::ln() const
    {
        return complex(0.5*log(_re*_re+_im*_im), p());
    }

    //////////////////////////////////////////////////////////////////////////
    inline     complex complex::rotate(real p) const
    {
        real cosVal = cos(p);
        real sinVal = sin(p);
        return complex(_re*cosVal - _im*sinVal, _re*sinVal + _im*cosVal);
    }

    //////////////////////////////////////////////////////////////////////////
    inline complex &complex::operator=(const complex &z)
    {
        _re = z._re;
        _im = z._im;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    inline complex &complex::operator=(const real &re)
    {
        _re = re;
        _im = 0;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    inline complex &complex::operator+=(const real &re)
    {
        _re += re;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    inline complex &complex::operator-=(const real &re)
    {
        _re -= re;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    inline complex &complex::operator*=(const real &re)
    {
        _re *= re;
        _im *= re;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    inline complex &complex::operator/=(const real &re)
    {
        _re /= re;
        _im /= re;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    inline complex &complex::operator+=(const complex &z)
    {
        _re += z._re;
        _im += z._im;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    inline complex &complex::operator-=(const complex &z)
    {
        _re -= z._re;
        _im -= z._im;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    inline complex &complex::operator*=(const complex &z)
    {
        real t = _re*z._re - _im*z._im;
        _im = _re*z._im + _im*z._re;
        _re = t;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    inline complex &complex::operator/=(const complex &z)
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
    inline const bool operator==(const complex &lhs, const complex &rhs)
    {
        return lhs.re() == rhs.re() && lhs.im() == rhs.im();
    }

    inline const bool operator==(const real &lhs, const complex &rhs)
    {
        return lhs == rhs.re() && 0 == rhs.im();
    }

    inline const bool operator==(const complex &lhs, const real &rhs)
    {
        return lhs.re() == rhs && lhs.im() == 0;
    }

    inline const bool operator!=(const complex &lhs, const complex &rhs)
    {
        return !operator==(lhs, rhs);
    }

    inline const bool operator!=(const real &lhs, const complex &rhs)
    {
        return !operator==(lhs, rhs);
    }

    inline const bool operator!=(const complex &lhs, const real &rhs)
    {
        return !operator==(lhs, rhs);
    }

    //////////////////////////////////////////////////////////////////////////
    inline const complex operator+(const complex &rhs)
    {
        return rhs;
    }

    inline const complex operator-(const complex &rhs)
    {
        return complex(-rhs.re(), -rhs.im());
    }

    //////////////////////////////////////////////////////////////////////////
    inline const complex operator+(const complex &lhs, const complex &rhs)
    {
        return complex(lhs.re()+rhs.re(), lhs.im()+rhs.im());
    }

    inline const complex operator+(const real &lhs, const complex &rhs)
    {
        return complex(lhs+rhs.re(), 0+rhs.im());
    }

    inline const complex operator+(const complex &lhs, const real &rhs)
    {
        return complex(lhs.re()+rhs, lhs.im()+0);
    }

    //////////////////////////////////////////////////////////////////////////
    inline const complex operator-(const complex &lhs, const complex &rhs)
    {
        return complex(lhs.re()-rhs.re(), lhs.im()-rhs.im());
    }

    inline const complex operator-(const real &lhs, const complex &rhs)
    {
        return complex(lhs-rhs.re(), 0-rhs.im());
    }

    inline const complex operator-(const complex &lhs, const real &rhs)
    {
        return complex(lhs.re()-rhs, lhs.im()-0);
    }

    //////////////////////////////////////////////////////////////////////////
    inline const complex operator*(const complex &lhs, const complex &rhs)
    {
        return complex(lhs.re()*rhs.re() - lhs.im()*rhs.im(),  lhs.re()*rhs.im() + lhs.im()*rhs.re());
    }

    inline const complex operator*(const complex &lhs, const real &rhs)
    {
        return complex(lhs.re()*rhs,  lhs.im()*rhs);
    }
    
    inline const complex operator*(const real &lhs, const complex &rhs)
    {
        return complex(lhs*rhs.re(),  lhs*rhs.im());
    }

    //////////////////////////////////////////////////////////////////////////
    inline const complex operator/(const complex &lhs, const complex &rhs)
    {
        return complex(lhs) /= rhs;
    }
    
    inline const complex operator/(const real &lhs, const complex &rhs)
    {
        return complex(lhs) /= rhs;
    }
    
    inline const complex operator/(const complex &lhs, const real &rhs)
    {
        return complex(lhs.re()/rhs, lhs.im()/rhs);
    }
}

#pragma once

#include <cmath>
#include <cassert>

namespace sp
{
    enum EComplexConstructType
    {
        CREIM=0,
        CAP=1,
    };

    template <class real>
    class complex_tmpl
    {
    public:
//        complex_tmpl();
        complex_tmpl(real v1=real(), real v2=real(), EComplexConstructType ecct = CREIM);
        complex_tmpl(const complex_tmpl &z);

        void setAP(real a, real p);
        void set(real re, real im);

        real &re();
        real &im();
        real &operator[](std::size_t idx);

        const real &re() const;
        const real &im() const;
        const real &operator[](std::size_t idx) const;

        real a() const;
        real p() const;

        complex_tmpl conj() const;
        complex_tmpl sqr() const;
        complex_tmpl ln() const;

        complex_tmpl rotate(real p) const;

        complex_tmpl &operator=(const complex_tmpl &z);
        complex_tmpl &operator=(const real &re);

        complex_tmpl &operator+=(const real &re);
        complex_tmpl &operator-=(const real &re);
        complex_tmpl &operator*=(const real &re);
        complex_tmpl &operator/=(const real &re);

        complex_tmpl &operator+=(const complex_tmpl &z);
        complex_tmpl &operator-=(const complex_tmpl &z);
        complex_tmpl &operator*=(const complex_tmpl &z);
        complex_tmpl &operator/=(const complex_tmpl &z);

    private:
        real _re;
        real _im;
    };





    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real>::complex_tmpl(real v1, real v2, EComplexConstructType ecct)
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
            _re = _im = real();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real>::complex_tmpl(const complex_tmpl &z)
        : _re(z._re)
        , _im(z._im)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    void complex_tmpl<real>::setAP(real a, real p)
    {
        _re = a*cos(p);
        _im = a*sin(p);
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    void complex_tmpl<real>::set(real re, real im)
    {
        _re = re;
        _im = im;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    real &complex_tmpl<real>::re()
    {
        return _re;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    real &complex_tmpl<real>::im()
    {
        return _im;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    real &complex_tmpl<real>::operator[](std::size_t idx)
    {
        assert(idx<2);
        return idx ? _im : _re;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    const real &complex_tmpl<real>::re() const
    {
        return _re;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    const real &complex_tmpl<real>::im() const
    {
        return _im;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    const real &complex_tmpl<real>::operator[](std::size_t idx) const
    {
        assert(idx<2);
        return idx ? _im : _re;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    real complex_tmpl<real>::a() const
    {
        return sqrt(_re*_re + _im*_im);
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    real complex_tmpl<real>::p() const
    {
        return atan2(_im, _re);
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real> complex_tmpl<real>::conj() const
    {
        return complex_tmpl<real>(_re, -_im);
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real> complex_tmpl<real>::sqr() const
    {
        return complex_tmpl<real>(_re*_re - _im*_im, 2*_re*_im);
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real> complex_tmpl<real>::ln() const
    {
        return complex_tmpl(0.5*log(_re*_re+_im*_im), p());
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real> complex_tmpl<real>::rotate(real p) const
    {
        real cosVal = cos(p);
        real sinVal = sin(p);
        return complex_tmpl(_re*cosVal - _im*sinVal, _re*sinVal + _im*cosVal);
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real> &complex_tmpl<real>::operator=(const complex_tmpl &z)
    {
        _re = z._re;
        _im = z._im;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real> &complex_tmpl<real>::operator=(const real &re)
    {
        _re = re;
        _im = 0;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real> &complex_tmpl<real>::operator+=(const real &re)
    {
        _re += re;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real> &complex_tmpl<real>::operator-=(const real &re)
    {
        _re -= re;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real> &complex_tmpl<real>::operator*=(const real &re)
    {
        _re *= re;
        _im *= re;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real> &complex_tmpl<real>::operator/=(const real &re)
    {
        _re /= re;
        _im /= re;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real> &complex_tmpl<real>::operator+=(const complex_tmpl &z)
    {
        _re += z._re;
        _im += z._im;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real> &complex_tmpl<real>::operator-=(const complex_tmpl &z)
    {
        _re -= z._re;
        _im -= z._im;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real> &complex_tmpl<real>::operator*=(const complex_tmpl &z)
    {
        real t = _re*z._re - _im*z._im;
        _im = _re*z._im + _im*z._re;
        _re = t;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    complex_tmpl<real> &complex_tmpl<real>::operator/=(const complex_tmpl &z)
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
    template <class real>
    bool operator==(const complex_tmpl<real> &lhs, const complex_tmpl<real> &rhs)
    {
        return lhs.re() == rhs.re() && lhs.im() == rhs.im();
    }

    template <class real>
    bool operator==(const real &lhs, const complex_tmpl<real> &rhs)
    {
        return lhs == rhs.re() && 0 == rhs.im();
    }

    template <class real>
    bool operator==(const complex_tmpl<real> &lhs, const real &rhs)
    {
        return lhs.re() == rhs && lhs.im() == 0;
    }

    template <class real>
    bool operator!=(const complex_tmpl<real> &lhs, const complex_tmpl<real> &rhs)
    {
        return !operator==(lhs, rhs);
    }

    template <class real>
    bool operator!=(const real &lhs, const complex_tmpl<real> &rhs)
    {
        return !operator==(lhs, rhs);
    }

    template <class real>
    bool operator!=(const complex_tmpl<real> &lhs, const real &rhs)
    {
        return !operator==(lhs, rhs);
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    const complex_tmpl<real> operator+(const complex_tmpl<real> &rhs)
    {
        return rhs;
    }

    template <class real>
    const complex_tmpl<real> operator-(const complex_tmpl<real> &rhs)
    {
        return complex_tmpl<real>(-rhs.re(), -rhs.im());
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    const complex_tmpl<real> operator+(const complex_tmpl<real> &lhs, const complex_tmpl<real> &rhs)
    {
        return complex_tmpl<real>(lhs.re()+rhs.re(), lhs.im()+rhs.im());
    }

    template <class real>
    const complex_tmpl<real> operator+(const real &lhs, const complex_tmpl<real> &rhs)
    {
        return complex_tmpl<real>(lhs+rhs.re(), 0+rhs.im());
    }

    template <class real>
    const complex_tmpl<real> operator+(const complex_tmpl<real> &lhs, const real &rhs)
    {
        return complex_tmpl<real>(lhs.re()+rhs, lhs.im()+0);
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    const complex_tmpl<real> operator-(const complex_tmpl<real> &lhs, const complex_tmpl<real> &rhs)
    {
        return complex_tmpl<real>(lhs.re()-rhs.re(), lhs.im()-rhs.im());
    }

    template <class real>
    const complex_tmpl<real> operator-(const real &lhs, const complex_tmpl<real> &rhs)
    {
        return complex_tmpl<real>(lhs-rhs.re(), 0-rhs.im());
    }

    template <class real>
    const complex_tmpl<real> operator-(const complex_tmpl<real> &lhs, const real &rhs)
    {
        return complex_tmpl<real>(lhs.re()-rhs, lhs.im()-0);
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    const complex_tmpl<real> operator*(const complex_tmpl<real> &lhs, const complex_tmpl<real> &rhs)
    {
        return complex_tmpl<real>(lhs.re()*rhs.re() - lhs.im()*rhs.im(),  lhs.re()*rhs.im() + lhs.im()*rhs.re());
    }

    template <class real>
    const complex_tmpl<real> operator*(const complex_tmpl<real> &lhs, const real &rhs)
    {
        return complex_tmpl<real>(lhs.re()*rhs,  lhs.im()*rhs);
    }

    template <class real>
    const complex_tmpl<real> operator*(const real &lhs, const complex_tmpl<real> &rhs)
    {
        return complex_tmpl<real>(lhs*rhs.re(),  lhs*rhs.im());
    }

    //////////////////////////////////////////////////////////////////////////
    template <class real>
    const complex_tmpl<real> operator/(const complex_tmpl<real> &lhs, const complex_tmpl<real> &rhs)
    {
        return complex_tmpl<real>(lhs) /= rhs;
    }

    template <class real>
    const complex_tmpl<real> operator/(const real &lhs, const complex_tmpl<real> &rhs)
    {
        return complex_tmpl<real>(lhs) /= rhs;
    }

    template <class real>
    const complex_tmpl<real> operator/(const complex_tmpl<real> &lhs, const real &rhs)
    {
        return complex_tmpl<real>(lhs.re()/rhs, lhs.im()/rhs);
    }
}

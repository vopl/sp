#pragma once

#include <vector>
#include <cmath>
#include <quadmath.h>
#include <iostream>

#include <boost/multiprecision/float128.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

namespace sp
{
    //using real = float;
    //using real = double;
    using real = long double;
    //using real = boost::multiprecision::float128;
    //using real = boost::multiprecision::cpp_bin_float_quad;

    using TVReal = std::vector<real>;

    /////////-/////////-/////////-/////////-/////////-/////////-/////////-/////////-
    inline double fabs(double x) {return ::fabs(x);}
    inline double sqrt(double x) {return ::sqrt(x);}

    inline double cos(double x) {return ::cos(x);}
    inline double sin(double x) {return ::sin(x);}

    inline double exp(double x) {return ::exp(x);}
    inline double log(double x) {return ::log(x);}

    inline double atan2(double x, double y) {return ::atan2(x, y);}

    /////////-/////////-/////////-/////////-/////////-/////////-/////////-/////////-
    inline long double fabs(long double x) {return ::fabsl(x);}
    inline long double sqrt(long double x) {return ::sqrtl(x);}

    inline long double cos(long double x) {return ::cosl(x);}
    inline long double sin(long double x) {return ::sinl(x);}

    inline long double exp(long double x) {return ::expl(x);}
    inline long double log(long double x) {return ::logl(x);}

    inline long double atan2(long double x, long double y) {return ::atan2l(x, y);}

    /////////-/////////-/////////-/////////-/////////-/////////-/////////-/////////-
    inline boost::multiprecision::float128 fabs(boost::multiprecision::float128 x) {return boost::multiprecision::fabs(x);}
    inline boost::multiprecision::float128 sqrt(boost::multiprecision::float128 x) {return boost::multiprecision::sqrt(x);}

    inline boost::multiprecision::float128 cos(boost::multiprecision::float128 x) {return boost::multiprecision::cos(x);}
    inline boost::multiprecision::float128 sin(boost::multiprecision::float128 x) {return boost::multiprecision::sin(x);}

    inline boost::multiprecision::float128 exp(boost::multiprecision::float128 x) {return boost::multiprecision::exp(x);}
    inline boost::multiprecision::float128 log(boost::multiprecision::float128 x) {return boost::multiprecision::log(x);}

    inline boost::multiprecision::float128 atan2(boost::multiprecision::float128 x, long double y) {return boost::multiprecision::atan2(x, y);}

    /////////-/////////-/////////-/////////-/////////-/////////-/////////-/////////-
    template <class T>
    T sqr(T x)
    {
        return x*x;
    }
}





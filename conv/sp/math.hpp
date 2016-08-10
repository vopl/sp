#pragma once

#include <complex>
#include <cmath>
#include <limits>
#include <vector>

namespace sp
{
    using real = long double;
    using complex = std::complex<real>;


    using TVReal = std::vector<real>;
    using TVComplex = std::vector<complex>;


    static const real g_pi = M_PIl;
    static const real g_2pi = 2*g_pi;


    inline long double cos(long double x) {return ::cosl(x);}
    inline long double sin(long double x) {return ::sinl(x);}

    inline long double exp(long double x) {return ::expl(x);}
    inline long double log(long double x) {return ::logl(x);}
}

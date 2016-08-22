#pragma once

#include <vector>
#include <cmath>

namespace sp
{
    using real = long double;

    using TVReal = std::vector<real>;




    inline long double fabs(long double x) {return ::fabsl(x);}
    inline long double sqrt(long double x) {return ::sqrtl(x);}

    inline long double cos(long double x) {return ::cosl(x);}
    inline long double sin(long double x) {return ::sinl(x);}

    inline long double exp(long double x) {return ::expl(x);}
    inline long double log(long double x) {return ::logl(x);}

    inline long double atan2(long double x, long double y) {return ::atan2l(x, y);}


    template <class T>
    T sqr(T x)
    {
        return x*x;
    }

}

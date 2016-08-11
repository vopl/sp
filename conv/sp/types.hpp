#pragma once

#include <vector>
#include <cmath>

namespace sp
{
    using real = double;

    using TVReal = std::vector<real>;





    inline long double cos(long double x) {return ::cosl(x);}
    inline long double sin(long double x) {return ::sinl(x);}

    inline long double exp(long double x) {return ::expl(x);}
    inline long double log(long double x) {return ::logl(x);}


    template <class T>
    T sqr(T x)
    {
        return x*x;
    }

}

#pragma once

#include "math.hpp"
#include <cstring>


namespace sp
{
    //////////////////////////////////////////////////////////////////////////
    class Kernel
    {
    public:
        Kernel(real ppw);
        ~Kernel();

        complex eval(real t, real st, const complex &sv);

        int deconvolve(
            size_t esize, const real *et, const complex *ev,//отклик
            size_t ssize, const real *st,       complex *sv,//спектр
            size_t itMax,//макс итераций
            std::vector<double> &work);

    private:
        real _ppw;
    };
}

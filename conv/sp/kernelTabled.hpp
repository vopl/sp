#pragma once

#include "sp/types.hpp"
#include "sp/complex.hpp"
#include <string>
#include <map>

namespace sp
{
    class KernelTabled
    {
    public:
        KernelTabled(real pow);
        ~KernelTabled();


        complex eval(real t, real st, const complex &sv);

        int deconvolve(
            size_t esize, const real *et, const complex *ev,//отклик
            size_t ssize, const real *st,       complex *sv,//спектр
            size_t itMax,//макс итераций
            std::vector<double> &work);

    public:
        void evalKernel(real t, real &rr, real &ri, real &ir, real &ii);

    public:
        std::string stateFileName();
        bool load();
        bool save();

    private:
        real _pow;

    private:
        struct Value
        {
            complex _re;
            complex _im;
        };

        using ValuesByPeriod = std::map<real, Value>;
        ValuesByPeriod _valuesByPeriod;
        std::size_t _addedValuesAmount = 0;
    };
}

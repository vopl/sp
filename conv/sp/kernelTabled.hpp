#pragma once

#include "sp/types.hpp"
#include "sp/complex.hpp"
#include "sp/signalConvolver.hpp"
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

        void deconvolve(
            size_t esize, const real *et, const complex *ev,//отклик
            size_t ssize, const real *st,       complex *sv,//спектр
            size_t &iters,//макс итераций
            real &error,
            std::vector<double> &work);

    public:
        void evalKernel(real t, real &rr, real &ri, real &ir, real &ii);

    private:
        void buildValue(const real &period, complex &re, complex &im);

    private:
        std::string stateFileName();
        bool load();
        bool save();

        SignalConvolver &getSignalConvolver();

    private:
        real _pow;

        using SignalConvolverPtr = std::unique_ptr<SignalConvolver>;
        SignalConvolverPtr _scp;

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

#pragma once

#include "sp/types.hpp"
#include "sp/complex.hpp"
#include <string>

namespace sp
{
    class KernelTabled
    {
    public:
        KernelTabled();
        ~KernelTabled();

        void setup(real pow, real periodMin=0.1, real periodMax=10, std::size_t periodSteps=1000);

        complex eval(real t, real st, const complex &sv);

        int deconvolve(
            size_t esize, const real *et, const complex *ev,//отклик
            size_t ssize, const real *st,       complex *sv,//спектр
            size_t itMax,//макс итераций
            TVReal &work);

    public:
        void evalKernel(real t, real &rr, real &ri, real &ir, real &ii);

    private:
        std::string stateFileName();
        bool load();
        bool save();
        void build();

    private:
        real _pow;
        real _periodMin;
        real _periodMax;
        std::size_t _periodSteps;
        real _periodStep;

    private:
        TVComplex _kre;
        TVComplex _kim;

        //эдакие виртуальные производные, для каждой точки периода. При построении аппроксимации между двумя точками - их координаты трактуются как 0 и 1
        TVComplex _kdre;
        TVComplex _kdim;

    };
}

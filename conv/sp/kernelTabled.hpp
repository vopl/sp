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

    private://01
        real _min01;
        real _max01;
        real _step01;
        std::size_t _steps01;

        TVComplex _kre01;
        TVComplex _kim01;

        //эдакие виртуальные производные, для каждой точки периода. При построении аппроксимации между двумя точками - их координаты трактуются как 0 и 1
        TVComplex _kdre01;
        TVComplex _kdim01;

    private://1inf
        real _min1inf;
        real _max1inf;
        real _step1inf;
        std::size_t _steps1inf;

        TVComplex _kre1inf;
        TVComplex _kim1inf;
        TVComplex _kdre1inf;
        TVComplex _kdim1inf;
    };
}

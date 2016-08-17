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

        void setup(real pow, real periodSmallMult=0.1, real periodBigMult=10, std::size_t periodSteps=1000);

        complex eval(real t, real st, const complex &sv);

    private:
        void evalKernel(real t, real &rr, real &ri, real &ir, real &ii);

    private:
        std::string stateFileName();
        bool load();
        bool save();
        void build();

    private:
        real _pow;
        real _periodSmallMult;
        real _periodBigMult;
        std::size_t _periodSteps;

    private:
        TVComplex _kre;
        TVComplex _kim;

        //эдакие виртуальные производные, для каждой точки периода. При построении аппроксимации между двумя точками - их координаты трактуются как 0 и 1
        TVComplex _kdre;
        TVComplex _kdim;

    };
}

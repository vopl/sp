#pragma once

#include "sp/math.hpp"
#include "sp/conv/linearSolver.hpp"
#include "sp/conv/basis.hpp"
#include "sp/conv/point.hpp"

namespace sp { namespace conv
{
    class Transformer
    {
    public:
        Transformer();
        ~Transformer();

        void setup(
                real ppw,
                const TVReal &periods,
                real signalSampleStep,
                std::size_t signalBucketSize);

        void pushSignal(const real *signalBucketSamples);

        void update();

        const TVPoint &points();

    private:
        std::size_t signalSize4Period(real period);

    private:
        std::string d2FileName();
        bool d2Load();
        bool d2Save();


    private:
        real                            _ppw = 0;
        TVReal                          _periods;
        real                            _signalSampleStep = 0;
        std::size_t                     _signalSamplesPushed = 0;
        std::size_t                     _signalBucketSize = 0;
        TVReal                          _signal;

        Basis   _basis;
        TVPoint _points;
        TVReal  _coveredSignal;

        Matrix      _linearSolverKernT;
        LinearSolverPtr _linearSolver;
    };

}}

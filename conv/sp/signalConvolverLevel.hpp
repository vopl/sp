#pragma once

#include "sp/types.hpp"
#include "sp/complex.hpp"
#include "sp/signalConvolver.hpp"

namespace sp
{
    class SignalConvolverLevel
    {
    public:
        SignalConvolverLevel(real pow, real period, real signalSampleStep, std::size_t samplesPerPeriod);
        ~SignalConvolverLevel();

        void update(const real *signal, std::size_t signalSize, SignalApproxType sat);
        void filtrate(const std::vector<std::vector<real> > &firs);
        complex convolve();

    private:
        real updateOneLinear(const real *signal, std::size_t signalSize, real startTime, real stopTime, std::size_t signalStartIdx, std::size_t signalStopIdx);
        real updateOnePoly(const real *signal, std::size_t signalSize, real startTime, real stopTime, std::size_t signalStartIdx, std::size_t signalStopIdx);

    private:
        real    _pow;
        real    _period;
        real    _signalSampleStep;
        real    _sampleStep;
        TVReal  _values;
        TVReal  _valuesFiltered;

    };

}

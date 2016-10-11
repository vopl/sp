#pragma once

#include "sp/math.hpp"
#include "sp/conv/signalConvolver.hpp"

namespace sp { namespace conv
{
    class SignalConvolverLevel
    {
    public:
        SignalConvolverLevel(real ppw, real period, real signalSampleStep, std::size_t samplesPerPeriod);
        ~SignalConvolverLevel();

        EchoPoint convolve(const real *signal, std::size_t signalSize, SignalApproxType sat, TVReal &valuesWorkArray);
        EchoPoint convolveIdentity(real period, real phase);

    private:
        real        _ppw;
        real        _period;
        real        _signalSampleStep;
        std::size_t _samplesPerPeriod;
        real        _sampleStep;

    private:
        real updateOneLinear(const real *signal, std::size_t signalSize, real startTime, real stopTime, std::size_t signalStartIdx, std::size_t signalStopIdx);
        real updateOnePoly(const real *signal, std::size_t signalSize, real startTime, real stopTime, std::size_t signalStartIdx, std::size_t signalStopIdx);

    private:
        void update(TVReal &values, const real *signal, std::size_t signalSize, SignalApproxType sat);
        void updateIdentity(TVReal &values, real period, real phase);

        EchoPoint convolve(const TVReal &values);
    };

}}

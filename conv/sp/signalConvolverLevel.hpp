#pragma once

#include "sp/math.hpp"
#include "sp/signalConvolver.hpp"

namespace sp
{
    class SignalConvolverLevel
    {
    public:
        SignalConvolverLevel(real ppw, real period, real signalSampleStep, std::size_t samplesPerPeriod, std::size_t polyOrder);
        ~SignalConvolverLevel();

        complex convolve(const real *signal, std::size_t signalSize, SignalApproxType sat);
        complex convolveIdentity(real period, real phase);

    private:
        real        _ppw;
        real        _period;
        real        _signalSampleStep;
        std::size_t _samplesPerPeriod;
        real        _sampleStep;
        std::size_t _polyOrder;
        //TVReal      _values;
        //TVReal      _valuesFiltered;

        struct Serie
        {
            real    _dp = real();
            TVReal  _points;
        };

        Serie int_(const Serie &src);
        Serie dif(const Serie &src);
        std::size_t /*samples*/ finalize(Serie &src);

        //std::vector<Serie> _series;

    private:
        real updateOneLinear(const real *signal, std::size_t signalSize, real startTime, real stopTime, std::size_t signalStartIdx, std::size_t signalStopIdx);
        real updateOnePoly(const real *signal, std::size_t signalSize, real startTime, real stopTime, std::size_t signalStartIdx, std::size_t signalStopIdx);

    private:
        void update(TVReal &values, const real *signal, std::size_t signalSize, SignalApproxType sat);
        void updateIdentity(TVReal &values, real period, real phase);
        void filtrate(const TVReal &values, std::vector<Serie> &series);

        complex convolve(const std::vector<Serie> &_series);
    };

}

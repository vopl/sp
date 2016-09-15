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

        void update(const real *signal, std::size_t signalSize, SignalApproxType sat);
        void updateIdentity(real period, real phase);
        void filtrate(const TVReal &halfFir);
        complex convolve();

    private:
        //void filtrate_fir(const TVReal &halfFir);
        //std::vector<Summator<real>> filtrate_int(const std::vector<Summator<real>> &src);
        //std::vector<Summator<real>> filtrate_dif(const std::vector<Summator<real>> &src);


    private:
        real updateOneLinear(const real *signal, std::size_t signalSize, real startTime, real stopTime, std::size_t signalStartIdx, std::size_t signalStopIdx);
        real updateOnePoly(const real *signal, std::size_t signalSize, real startTime, real stopTime, std::size_t signalStartIdx, std::size_t signalStopIdx);

    private:
        real        _ppw;
        real        _period;
        real        _signalSampleStep;
        std::size_t _samplesPerPeriod;
        real        _sampleStep;
        std::size_t _polyOrder;
        TVReal      _values;
        //TVReal      _valuesFiltered;

        struct Serie
        {
            real    _dp = real();
            TVReal  _points;
        };

        Serie int_(const Serie &src);
        Serie dif(const Serie &src);
        std::size_t /*samples*/ finalize(Serie &src);

        std::vector<Serie> _series;
    };

}

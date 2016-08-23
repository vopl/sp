#pragma once

#include "types.hpp"
#include <boost/circular_buffer.hpp>

namespace sp
{
    class SignalConvolverLevel
    {
    public:
        SignalConvolverLevel(real pow, real period, real signalSampleStep, std::size_t samplesPerPeriod);
        ~SignalConvolverLevel();

        void update(const real *signal, std::size_t signalSize);

    private:
        real    _pow;
        real    _period;
        real    _signalSampleStep;
        real    _sampleStep;
        TVReal  _values;

    };

}

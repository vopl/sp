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

        void update(
                const real *first, std::size_t firstSize,
                const real *second, std::size_t secondSize);

    private:
        real    _pow;
        real    _period;
        real    _signalSampleStep;
        real    _sampleStep;
        TVReal  _values;

    };

}

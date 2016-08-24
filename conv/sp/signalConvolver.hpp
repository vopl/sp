#pragma once

#include "periodGrid.hpp"
#include "complex.hpp"

#include <boost/circular_buffer.hpp>
#include <memory>

namespace sp
{
    class SignalConvolverLevel;
    class SignalConvolver
    {
    public:
        SignalConvolver();
        ~SignalConvolver();


        void setup(real pow, const PeriodGrid &periodGrid, real sampleStep, std::size_t samplesPerPeriod);

        void pushSignal(const real *samples, std::size_t amount);

        real getTime() const;

        TVComplex /*echo*/ convolve();

    private:
        void prepareValues();

    private:
        real                            _sampleStep = 0;
        std::size_t                     _samplesPushed = 0;
        boost::circular_buffer<real>    _signal;
        bool                            _dirty = true;

        using LevelPtr = std::unique_ptr<SignalConvolverLevel>;
        std::vector<LevelPtr> _levels;

        std::vector<TVReal>             _firs;

    };

}

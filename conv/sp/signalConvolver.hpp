#pragma once

#include "sp/periodGrid.hpp"
#include "sp/complex.hpp"

#include <memory>

namespace sp
{
    class SignalConvolverLevel;
    class SignalConvolver
    {
    public:
        SignalConvolver();
        ~SignalConvolver();


        void setupFirs(real pow, std::size_t samplesPerPeriod);
        void setupSignal(real sampleStep, real maxPeriod);
        void setup(real pow, const PeriodGrid &periodGrid, real sampleStep, std::size_t samplesPerPeriod);

        void pushSignal(const real *samples, std::size_t amount);

        real getTime() const;

        TVComplex /*echo*/ convolve();
        complex /*echo*/ convolve(real period);

    private:
        void prepareValues();

    private:
        real                            _pow = 0;
        real                            _signalSampleStep = 0;
        std::size_t                     _signalSamplesPushed = 0;
        std::size_t                     _samplesPerPeriod = 0;
        TVReal                          _signal;
        bool                            _dirty = true;

        using LevelPtr = std::unique_ptr<SignalConvolverLevel>;
        std::vector<LevelPtr> _levels;

        std::vector<TVReal>             _halfFirs;

    };

}

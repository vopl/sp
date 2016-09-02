#pragma once

#include "sp/periodGrid.hpp"
#include "sp/complex.hpp"

#include <memory>

namespace sp
{
    enum class SignalApproxType
    {
        linear,
        poly6p5o32x, //require 2points before, 4points after
    };

    class SignalConvolverLevel;
    class SignalConvolver
    {
    public:
        SignalConvolver();
        ~SignalConvolver();


        void setupFirs(real ppw, std::size_t samplesPerPeriod);
        void setupSignal(real sampleStep, real maxPeriod, SignalApproxType sat = SignalApproxType::linear);
        void setup(real ppw, const TVReal &periods, real sampleStep, std::size_t samplesPerPeriod, SignalApproxType sat = SignalApproxType::linear);

        void pushSignal(const real *samples, std::size_t amount);

        real getTime() const;

        TVComplex /*echo*/ convolve();
        complex /*echo*/ convolve(real period);

    private:
        void prepareValues();

    private:
        real                            _ppw = 0;
        SignalApproxType                _sat = SignalApproxType::linear;
        real                            _signalSampleStep = 0;
        std::size_t                     _signalSamplesPushed = 0;
        std::size_t                     _samplesPerPeriod = 0;
        TVReal                          _signal;
        bool                            _dirty = true;

        using LevelPtr = std::unique_ptr<SignalConvolverLevel>;
        std::vector<LevelPtr> _levels;

        std::vector<std::vector<real>> _halfFirs;

    };

}

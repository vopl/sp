#pragma once

#include "math.hpp"

namespace sp
{
    class Convolver
    {
    public:
        Convolver(real pow, real periodMin, real periodMax, std::size_t periodSteps);
        ~Convolver();

        void execute(real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime, TVComplex &echo);

    private:
        real    _pow;
        TVReal  _periodGrid;
    };
}

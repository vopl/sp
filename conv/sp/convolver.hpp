#pragma once

#include "math.hpp"
#include "periodGrid.hpp"

namespace sp
{
    class Convolver
    {
    public:
        Convolver(real pow);
        ~Convolver();

        void execute(const PeriodGrid &periodGrid, real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime, TVComplex &echo);
        complex /*echo*/ execute(const real &period, real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime);

    private:
        real    _pow;
    };
}

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

        void execute(PeriodGrid &periodGrid, real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime, TVComplex &echo);

    private:
        real    _pow;
    };
}

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

        real /*signalStartTime*/ prepareSignal(real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime, real pow, real maxT, TVReal &preparedSignal);

        void execute(const PeriodGrid &periodGrid, real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime, TVComplex &echo);
        complex /*echo*/ execute(const real &period, real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime);

        void executeOnPrepared(const PeriodGrid &periodGrid, real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime, TVComplex &echo);
        complex /*echo*/ executeOnPrepared(const real &period, real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime);

    private:
        real    _pow;
    };
}

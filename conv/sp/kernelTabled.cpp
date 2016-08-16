#include "sp/kernelTabled.hpp"
#include "sp/convolver.hpp"

namespace sp
{
    KernelTabled::KernelTabled()
    {

    }

    KernelTabled::~KernelTabled()
    {

    }

    void KernelTabled::setup(real pow, real periodSmallMult, real periodBigMult, std::size_t periodSteps)
    {
        Convolver c(pow, periodSmallMult, periodBigMult, periodSteps);

        real targetX = periodBigMult*pow;
        const real sampleStep = periodSmallMult/1000;//1000 сэмплов на минимальный период
        TVReal signal(targetX/sampleStep+1000);

        assert(0);


        TVComplex r, i;

        const std::size_t phasesAmount = 10;
        for(std::size_t phaseIndex(0); phaseIndex<phasesAmount; ++phaseIndex)
        {

        }


        assert(0);
    }
}

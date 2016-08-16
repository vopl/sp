#pragma once

#include "sp/types.hpp"

namespace sp
{
    class KernelTabled
    {
    public:
        KernelTabled();
        ~KernelTabled();

        void setup(real pow, real periodSmallMult=0.1, real periodBigMult=10, std::size_t periodSteps=1000);


    };
}

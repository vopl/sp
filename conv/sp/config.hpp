#pragma once

#include "math.hpp"

namespace sp
{

    //частота в герцах
    static const real g_sampleStep = 1.0L/192000;

    static const real g_periodMax = 1.0L/16;
    static const real g_periodMin = 1.0L/16000;
    static const long g_periodSteps = 2000;//размерность сетки
}

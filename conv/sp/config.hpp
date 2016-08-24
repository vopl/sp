#pragma once

#include "math.hpp"

namespace sp
{

    //частота в герцах
    static const real g_sampleStep = 1.0L/88000;

    static const real g_periodMax = 1.0L/40;
    static const real g_periodMin = 1.0L/8000;
    static const long g_periodSteps = 500;//размерность сетки
}

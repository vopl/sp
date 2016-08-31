#pragma once

#include "math.hpp"

namespace sp
{

    //частота в герцах
    static const real g_sampleStep = 1.0L/364000/4;

    static const real g_periodMax = 1.0L/4;
    static const real g_periodMin = 1.0L/8000;
    static const long g_periodSteps = 1000;//размерность сетки
}

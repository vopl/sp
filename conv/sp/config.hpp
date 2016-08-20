#pragma once

#include "math.hpp"

namespace sp
{

    //частота в герцах
    static const real g_sampleStep = 1.0/22000;

    static const real g_periodMax = 1.0/20;//период для 20 герц
    static const real g_periodMin = 1.0/8000;//период для 2 килогерц
    static const long g_periodSteps = 500;//размерность сетки
}

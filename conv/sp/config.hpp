#pragma once

#include "math.hpp"

namespace sp
{

    //частота в герцах
    static const real g_sampleStep = 1.0/8000;

    static const real g_periodMax = 1.0/20;//период для 250 герц
    static const real g_periodMin = 1.0/2000;//период для 2 килогерц
    static const long g_periodSteps = 500;//размерность сетки

    //сетка периода, от максимума с шагом до минимума
    static const TVReal g_periodGrid = ([](){
        TVReal res;

        real minTLog = log(g_periodMin);
        real maxTLog = log(g_periodMax);
        real logtStep = (maxTLog - minTLog) / (g_periodSteps-1);
        res.resize(g_periodSteps);
        for(size_t k(0); k<g_periodSteps; k++)
        {
            res[k] = exp(minTLog+k*logtStep);
        }

        return res;
    })();
}

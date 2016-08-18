#pragma once

#include "math.hpp"

namespace sp
{

    //частота в герцах
    static const real g_sampleStep = 1.0/22000;

    static const real g_periodMax = 1.0/20;//период для 250 герц
    static const real g_periodMin = 1.0/2000;//период для 2 килогерц
    static const long g_periodSteps = 500;//размерность сетки

    //сетка периода, от максимума с шагом до минимума
    static const TVReal g_periodGrid = ([](){
        TVReal res;

        real periodStep = (g_periodMax - g_periodMin) / (g_periodSteps-1);
        res.resize(g_periodSteps);
        for(std::size_t k(0); k<g_periodSteps; k++)
        {
            res[k] = g_periodMin + k*periodStep;
        }

        return res;
    })();
}

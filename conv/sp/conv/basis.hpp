#pragma once

#include "sp/math.hpp"
#include <array>

namespace sp { namespace conv
{
    class Point;


    class Basis
    {
    public:
        static constexpr std::size_t _amplitudeKAmount = 1;

        static constexpr std::size_t _periodKAmount = 5;
        static constexpr real _periodKStep = 0.05 ;

    public:
        Basis();

        real eval(const Point &p, real period, real x, real wx);

    public:
        real _amplitudeK0[_amplitudeKAmount];
        real _amplitudeK1[_amplitudeKAmount];
        real _periodK0[_periodKAmount];
        real _periodK1[_periodKAmount];
    };

}}

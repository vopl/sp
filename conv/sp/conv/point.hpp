#pragma once

#include "sp/math.hpp"
#include "sp/conv/basis.hpp"

namespace sp { namespace conv
{
    struct Point
    {
        static constexpr std::size_t _amplitudeKAmount = Basis::_amplitudeKAmount;
        static constexpr std::size_t _periodKAmount = Basis::_periodKAmount;
        static constexpr real _periodKStep = Basis::_periodKStep;

        complex _v[_amplitudeKAmount][_periodKAmount];

        real *data();
        static std::size_t dataSize();
    };

    using TVPoint = std::vector<Point>;

    std::ostream &operator<<(std::ostream &out, const Point &p);

}}

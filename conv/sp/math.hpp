#pragma once

#include "types.hpp"
#include "complex.hpp"

#include <cmath>
#include <limits>

namespace sp
{

    static const real g_pi = M_PIl;
    static const real g_2pi = 2*g_pi;

    static const real g_2piPow2 = g_2pi*g_2pi;

    template <class Value>
    class Summator
    {
    public:
        Summator(Value init=0)
            : _sum(init)
        {
        }

#if defined(__GNUC__)
#  pragma GCC push_options
#  pragma GCC optimize ("O1")
#endif
        void operator+=(Value v)
        {
            Value Y = v - _error;
            Value T = _sum + Y;
            _error = (T - _sum) - Y;
            _sum = T;
        }
#if defined(__GNUC__)
#  pragma GCC pop_options
#endif

        operator Value() const
        {
            return _sum;
        }

    private:
        Value _sum = 0;
        Value _error = 0;
    };
}

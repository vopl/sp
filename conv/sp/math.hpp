#pragma once

#include "types.hpp"
#include "complex.hpp"
#include <boost/math/constants/constants.hpp>

#include <cmath>
#include <limits>

namespace sp
{

    static const real g_pi = boost::math::constants::pi<real>();
    static const real g_2pi = boost::math::constants::two_pi<real>();

    static const real g_2piPow2 = g_2pi*g_2pi;

    template <class Value>
    class Summator
    {
    public:
        Summator(Value init=Value())
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

        Value v() const
        {
            return _sum;
        }

    private:
        Value _sum = Value();
        Value _error = Value();
    };
}

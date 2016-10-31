#include "basis.hpp"
#include "point.hpp"

namespace sp { namespace conv
{
    Basis::Basis()
    {
        if(1 == _amplitudeKAmount)
        {
            _amplitudeK0[0] = 1;
            _amplitudeK0[1] = 1;
        }
        else
        {
            _amplitudeK0[0] = 0;
            _amplitudeK0[1] = 1;
            _amplitudeK1[0] = 1;
            _amplitudeK1[1] = 0;
        }

        for(auto i = 0; i != _periodKAmount; ++i)
        {
            _periodK0[i] = real(1) + (real(i)-real(_periodKAmount/2))*_periodKStep;
            _periodK1[i] = real(1) - (real(i)-real(_periodKAmount/2))*_periodKStep;
        }
    }

    real Basis::eval(const Point &p, real period, real x, real wx)
    {
        real res{};

        for(std::size_t ia(0); ia<_amplitudeKAmount; ++ia)
        {
            for(std::size_t ip(0); ip<_periodKAmount; ++ip)
            {
                real dp = x*g_2pi/(period*_periodK0[ip]*wx + period*_periodK1[ip]*(1.0-wx));

                res +=
                        (
                            p._v[ia][ip].rotate(-dp).re()*_amplitudeK0[ia]*(real(1.0)-wx)+
                            p._v[ia][ip].rotate(-dp).re()*_amplitudeK1[ia]*(wx)
                        );
            }
        }

        real wnd = (cos((wx-0.5)*g_2pi)/2+0.5);
        return res * wnd;
    }

}}

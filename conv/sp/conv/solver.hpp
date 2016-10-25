#pragma once

#include "sp/math.hpp"

namespace sp { namespace conv
{
    class Point
    {
    public:
        Point()
        {
        }

        ~Point()
        {

        }

        void accumuleConv(real v, real period, real x, real wx)
        {
            for(std::size_t ia(0); ia<_amplitudeKAmount; ++ia)
            {
                for(std::size_t ip(0); ip<_periodKAmount; ++ip)
                {
                    real dp = x*g_2pi/(period*_periodK0[ip]*wx + period*_periodK1[ip]*(1.0-wx));

                    _v[ia][ip] +=
                            complex(v).rotate(dp)*_amplitudeK0[ia]*(real(1.0)-wx)+
                            complex(v).rotate(dp)*_amplitudeK1[ia]*(wx);
                }
            }
        }

        void finalizeConv(real amount)
        {
            for(std::size_t ia(0); ia<_amplitudeKAmount; ++ia)
            {
                for(std::size_t ip(0); ip<_periodKAmount; ++ip)
                {
                    _v[ia][ip] /= amount;
                }
            }
        }

        void add(const Point &p, real mult)
        {
            for(std::size_t ia(0); ia<_amplitudeKAmount; ++ia)
            {
                for(std::size_t ip(0); ip<_periodKAmount; ++ip)
                {
                    _v[ia][ip] += p._v[ia][ip] * mult;
                }
            }
        }

        real eval(real period, real x, real wx)
        {
            real res{};

            for(std::size_t ia(0); ia<_amplitudeKAmount; ++ia)
            {
                for(std::size_t ip(0); ip<_periodKAmount; ++ip)
                {
                    real dp = x*g_2pi/(period*_periodK0[ip]*wx + period*_periodK1[ip]*(1.0-wx));

                    res +=

                            _v[ia][ip].rotate(-dp).re()*_amplitudeK0[ia]*(real(1.0)-wx)+
                            _v[ia][ip].rotate(-dp).re()*_amplitudeK1[ia]*(wx);
                }
            }

            return res;
        }

        real maxA()
        {
            real maxA2 = 0;
            std::size_t maxIA = 0;
            std::size_t maxIP = 0;

            for(std::size_t ia(0); ia<_amplitudeKAmount; ++ia)
            {
                for(std::size_t ip(0); ip<_periodKAmount; ++ip)
                {
                    if(maxA2 < _v[ia][ip].a2())
                    {
                        maxA2 = _v[ia][ip].a2();
                        maxIA = ia;
                        maxIP = ip;
                    }
                }
            }

            return sqrt(maxA2);
        }

        Point getMax()
        {
            real maxA2 = 0;
            std::size_t maxIA = 0;
            std::size_t maxIP = 0;

            for(std::size_t ia(0); ia<_amplitudeKAmount; ++ia)
            {
                for(std::size_t ip(0); ip<_periodKAmount; ++ip)
                {
                    if(maxA2 < _v[ia][ip].a2())
                    {
                        maxA2 = _v[ia][ip].a2();
                        maxIA = ia;
                        maxIP = ip;
                    }
                }
            }

            Point res;
            res._v[maxIA][maxIP] = _v[maxIA][maxIP];
            return res;
        }

        real *data()
        {
            return &_v[0][0].re();
        }

        static std::size_t dataSize()
        {
            return 2 * _amplitudeKAmount * _periodKAmount;
        }

        friend std::ostream &operator<<(std::ostream &out, const Point &p)
        {
            for(std::size_t ia(0); ia<_amplitudeKAmount; ++ia)
            {
                for(std::size_t ip(0); ip<_periodKAmount; ++ip)
                {
                    out << p._v[ia][ip].re()<<", "<<p._v[ia][ip].im()<<", ";
                }
            }

            return out;
        }

    public:
        static constexpr std::size_t _amplitudeKAmount = 2;
        static constexpr real _amplitudeK0[_amplitudeKAmount] = {   real(0.0),  real(1.0)};
        static constexpr real _amplitudeK1[_amplitudeKAmount] = {   real(1.0),  real(0.0)};


        static constexpr std::size_t _periodKAmount = 15;
        static constexpr real _periodK0[_periodKAmount] = {
            real(0.3),
            real(0.4),
            real(0.5),
            real(0.6),
            real(0.7),
            real(0.8),
            real(0.9),
            real(1.0),
            real(1.1),
            real(1.2),
            real(1.3),
            real(1.4),
            real(1.5),
            real(1.6),
            real(1.7),
        };



        static constexpr real _periodK1[_periodKAmount] = {
            real(1.7),
            real(1.6),
            real(1.5),
            real(1.4),
            real(1.3),
            real(1.2),
            real(1.1),
            real(1.0),
            real(0.9),
            real(0.8),
            real(0.7),
            real(0.6),
            real(0.5),
            real(0.4),
            real(0.3),
        };


        complex _v[_amplitudeKAmount][_periodKAmount];
    };
    using TVPoint = std::vector<Point>;

    class Solver
    {
    public:
        Solver();
        ~Solver();

        void setup(
                real ppw,
                const TVReal &periods,
                real signalSampleStep,
                std::size_t signalBucketSize);

        void pushSignal(const real *signalBucketSamples);

        void update();
        void update2();

        const TVPoint &points();

    private:
        std::size_t signalSize4Period(real period);

        void fetchEcho(TVPoint &points);
        real applyEchoPart(TVPoint &echoPoints);

    private:
        real                            _ppw = 0;
        TVReal                          _periods;
        real                            _signalSampleStep = 0;
        std::size_t                     _signalSamplesPushed = 0;
        std::size_t                     _signalBucketSize = 0;
        TVReal                          _signal;

        TVPoint _points;
        TVReal  _coveredSignal;


    };

}}

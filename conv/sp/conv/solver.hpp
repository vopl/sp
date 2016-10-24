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
                            complex(v).rotate(dp)*_amplitudeK0[ia]*wx+
                            complex(v).rotate(dp)*_amplitudeK1[ia]*(real(1.0)-wx);
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

                            _v[ia][ip].rotate(-dp).re()*_amplitudeK0[ia]*wx+
                            _v[ia][ip].rotate(-dp).re()*_amplitudeK1[ia]*(real(1.0)-wx);
                }
            }

            return res;
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

    private:
        static constexpr std::size_t _amplitudeKAmount = 2;
        static constexpr real _amplitudeK0[_amplitudeKAmount] = {   real(0.0),  real(1.0)};
        static constexpr real _amplitudeK1[_amplitudeKAmount] = {   real(1.0),  real(0.0)};

//        static constexpr std::size_t _periodKAmount = 15;
//        static constexpr real _periodK0[_periodKAmount] = { real(0.79), real(0.82), real(0.85), real(0.88), real(0.91), real(0.94), real(0.97), real(1.00), real(1.03), real(1.06), real(1.09), real(1.12), real(1.15), real(1.18), real(1.21)};
//        static constexpr real _periodK1[_periodKAmount] = { real(1.00), real(1.00), real(1.00), real(1.00), real(1.00), real(1.00), real(1.00), real(1.00), real(1.00), real(1.00), real(1.00), real(1.00), real(1.00), real(1.00), real(1.00)};

        static constexpr std::size_t _periodKAmount = 15;
        static constexpr real _periodK0[_periodKAmount] = {
            real(0.65),
            real(0.70),
            real(0.75),
            real(0.80),
            real(0.85),
            real(0.90),
            real(0.95),
            real(1.00),
            real(1.05),
            real(1.10),
            real(1.15),
            real(1.20),
            real(1.25),
            real(1.30),
            real(1.35),
        };

//        static constexpr real _periodK1[_periodKAmount] = {
//            real(1.1627906976744186047),
//            real(1.1363636363636363636),
//            real(1.1111111111111111111),
//            real(1.0869565217391304348),
//            real(1.0638297872340425532),
//            real(1.0416666666666666667),
//            real(1.0204081632653061224),
//            real(1.00),
//            real(0.98),
//            real(0.96),
//            real(0.94),
//            real(0.92),
//            real(0.90),
//            real(0.88),
//            real(0.86)};

        static constexpr real _periodK1[_periodKAmount] = {
            real(1.0),
            real(1.0),
            real(1.0),
            real(1.0),
            real(1.0),
            real(1.0),
            real(1.0),
            real(1.0),
            real(1.0),
            real(1.0),
            real(1.0),
            real(1.0),
            real(1.0),
            real(1.0),
            real(1.0),
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

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
            {
                real dp0 = x*g_2pi/(period*wx + period*_periodK0*(1.0-wx));
                _v_0_01 += complex(v).rotate(dp0)*wx;
                _v_0_10 += complex(v).rotate(dp0)*(real(1.0)-wx);
            }

            {
                real dp1 = x*g_2pi/(period*wx + period*_periodK1*(1.0-wx));
                _v_1_01 += complex(v).rotate(dp1)*wx;
                _v_1_10 += complex(v).rotate(dp1)*(real(1.0)-wx);
            }

            {
                real dp2 = x*g_2pi/(period*wx + period*_periodK2*(1.0-wx));
                _v_2_01 += complex(v).rotate(dp2)*wx;
                _v_2_10 += complex(v).rotate(dp2)*(real(1.0)-wx);
            }
        }

        void finalizeConv(real amount)
        {
            _v_0_01 /= amount;
            _v_0_10 /= amount;

            _v_1_01 /= amount;
            _v_1_10 /= amount;

            _v_2_01 /= amount;
            _v_2_10 /= amount;
        }

        void add(const Point &p, real mult)
        {
            _v_0_01 += p._v_0_01 * mult;
            _v_0_10 += p._v_0_10 * mult;

            _v_1_01 += p._v_1_01 * mult;
            _v_1_10 += p._v_1_10 * mult;

            _v_2_01 += p._v_2_01 * mult;
            _v_2_10 += p._v_2_10 * mult;
        }

        real eval(real period, real x, real wx)
        {
            real dp0 = x*g_2pi/(period*wx + period*_periodK0*(1.0-wx));
            real dp1 = x*g_2pi/(period*wx + period*_periodK1*(1.0-wx));
            real dp2 = x*g_2pi/(period*wx + period*_periodK2*(1.0-wx));

            return
                    _v_0_01.rotate(-dp0).re() *wx +
                    _v_0_10.rotate(-dp0).re() *(1.0-wx) +

                    _v_1_01.rotate(-dp1).re() *wx +
                    _v_1_10.rotate(-dp1).re() *(1.0-wx) +

                    _v_2_01.rotate(-dp2).re() *wx +
                    _v_2_10.rotate(-dp2).re() *(1.0-wx) +

                    0;
        }


        friend std::ostream &operator<<(std::ostream &out, const Point &p)
        {
            out
                <<p._v_0_01.re()<<", "
                <<p._v_0_01.im()<<", "
                <<p._v_0_10.re()<<", "
                <<p._v_0_10.im()<<", "

                <<p._v_1_01.re()<<", "
                <<p._v_1_01.im()<<", "
                <<p._v_1_10.re()<<", "
                <<p._v_1_10.im()<<", "

                <<p._v_2_01.re()<<", "
                <<p._v_2_01.im()<<", "
                <<p._v_2_10.re()<<", "
                <<p._v_2_10.im();

            return out;
        }

    private:
        static constexpr real _periodK0 = 0.25;
        complex _v_0_01;
        complex _v_0_10;

        static constexpr real _periodK1 = 1.0;
        complex _v_1_01;
        complex _v_1_10;

        static constexpr real _periodK2 = 4.0;
        complex _v_2_01;
        complex _v_2_10;
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

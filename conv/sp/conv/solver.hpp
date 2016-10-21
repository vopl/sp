#pragma once

#include "sp/math.hpp"

namespace sp { namespace conv
{
    struct Point
    {
        complex _v;
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
        real applyEchoPart(TVPoint &points, real part);

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

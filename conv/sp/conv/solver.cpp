#include "sp/conv/solver.hpp"
#include <cassert>
#include <fstream>
#include <iostream>

namespace sp { namespace conv
{
    Solver::Solver()
    {

    }

    Solver::~Solver()
    {

    }

    void Solver::setup(
            real ppw,
            const TVReal &periods,
            real signalSampleStep,
            std::size_t signalBucketSize)
    {
        _ppw = ppw;
        _periods = periods;
        _signalSampleStep = signalSampleStep;
        _signalBucketSize = signalBucketSize;

        _signal.resize(signalSize4Period(periods.back()));

        _points.resize(_periods.size());
        _coveredSignal.resize(_signal.size());
    }

    void Solver::pushSignal(const real *signalBucketSamples)
    {
        std::move(
                    _signal.begin() + _signalBucketSize,
                    _signal.end(),
                    _signal.begin());

        std::copy(
                    signalBucketSamples,
                    signalBucketSamples+_signalBucketSize,
                    _signal.end() - _signalBucketSize);

        _signalSamplesPushed += _signalBucketSize;
    }

    void Solver::update()
    {
        std::fill(_coveredSignal.begin(), _coveredSignal.end(), real(0));

        for(std::size_t periodIndex(0); periodIndex<_periods.size(); ++periodIndex)
        {
            _points[periodIndex]._v = 0;
        }


        for(;;)
        {
            TVPoint points(_points.size());
            fetchEcho(points);

            //update covered signal
            real err = applyEchoPart(points, 0.4);
            std::cout<<err<<std::endl;

            {
                std::ofstream out("s2");

                for(std::size_t sampleIndex(0); sampleIndex<_signal.size(); ++sampleIndex)
                {
                    out<<_signal[sampleIndex]<<", "<<_coveredSignal[sampleIndex]<<std::endl;
                }
            }

            int k = 220;

        }

    }

    const TVPoint &Solver::points()
    {
        return _points;
    }

    std::size_t Solver::signalSize4Period(real period)
    {
        return std::size_t(period * _ppw / _signalSampleStep+0.5);
    }

    void Solver::fetchEcho(TVPoint &points)
    {
        std::vector<std::size_t> amounts(points.size());

        real xStart = _signalSamplesPushed*_signalSampleStep - _signal.size()*_signalSampleStep;

        std::size_t minPeriodIndex = _periods.size();

        for(std::size_t sampleIndex(0); sampleIndex<_signal.size(); ++sampleIndex)
        {
            while(minPeriodIndex>0 && sampleIndex > _signal.size() - signalSize4Period(_periods[minPeriodIndex-1]))
            {
                minPeriodIndex--;
            }

            real x = xStart + sampleIndex*_signalSampleStep;
            real v = _signal[sampleIndex] - _coveredSignal[sampleIndex];

            for(std::size_t periodIndex(minPeriodIndex); periodIndex<_periods.size(); ++periodIndex)
            {
                real period = _periods[periodIndex];

                real dp = x*g_2pi/period;

                points[periodIndex]._v += complex(v).rotate(dp);
                amounts[periodIndex]++;
            }
        }

        for(std::size_t periodIndex(0); periodIndex<_periods.size(); ++periodIndex)
        {
            std::size_t amount = amounts[periodIndex];
            if(amount)
            {
                points[periodIndex]._v /= amount;
            }
        }

        int k = 220;
    }

    real Solver::applyEchoPart(TVPoint &points, real part)
    {
        real res = 0;
        for(std::size_t periodIndex(0); periodIndex<_periods.size(); ++periodIndex)
        {
            _points[periodIndex]._v += points[periodIndex]._v * part;
        }

        real xStart = _signalSamplesPushed*_signalSampleStep - _signal.size()*_signalSampleStep;

        std::size_t minPeriodIndex = _periods.size();

        for(std::size_t sampleIndex(0); sampleIndex<_signal.size(); ++sampleIndex)
        {
            while(minPeriodIndex>0 && sampleIndex > _signal.size() - signalSize4Period(_periods[minPeriodIndex-1]))
            {
                minPeriodIndex--;
            }

            real x = xStart + sampleIndex*_signalSampleStep;
            real &v = _coveredSignal[sampleIndex];
            v = 0;

            for(std::size_t periodIndex(minPeriodIndex); periodIndex<_periods.size(); ++periodIndex)
            {
                real period = _periods[periodIndex];

                real dp = x*g_2pi/period;

                v += _points[periodIndex]._v.rotate(-dp).re();
            }

            res += fabs(_signal[sampleIndex] - v);
        }

        return res;
    }

}}

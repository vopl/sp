#include "sp/signalConvolver.hpp"
#include "sp/signalConvolverLevel.hpp"
#include "sp/math.hpp"
#include <cassert>
#include <numeric>
#include <boost/math/special_functions.hpp>

#include <iostream>

namespace sp
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolver::SignalConvolver()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolver::~SignalConvolver()
    {
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        real kaizerDenom(real beta)
        {
            return boost::math::cyl_bessel_i(0, beta);
        }

        real kaizer(real beta, int n, int N, real denom)
        {
            real nom = boost::math::cyl_bessel_i(0, beta*sqrt(1.0L-sqr(real(2*n-N+1)/(N-1))));

            return nom / denom;
        }

        real kaizer(real beta, int n, int N)
        {
            real nom = boost::math::cyl_bessel_i(0, beta*sqrt(1.0L-sqr(real(2*n-N+1)/(N-1))));

            return nom / kaizerDenom(beta);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void lowPassFir(
            real bndT,
            std::size_t n,
            TVReal &A)
        {
            real w = g_2pi/bndT;

            n+=2;
            n |= 1;
            A.resize(n-2);

            std::size_t mn = n/2-1;
//            real kaizerBeta = 10;
//            real kaizerDenominator = kaizerDenom(kaizerBeta);

            A[mn] = 1;// * kaizer(kaizerBeta, mn, n-2, kaizerDenominator);
            for(std::size_t k(1); k<n/2; k++)
            {
                A[mn+k] = A[mn-k] = boost::math::sinc_pi(k*w);// * kaizer(kaizerBeta, mn+k, n-2, kaizerDenominator);
            }

            real sum = std::accumulate(A.begin(), A.end(), real(0));
            for(real &v : A)
            {
                v /= sum;
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::setup(real pow, real maxPeriod, real sampleStep, std::size_t samplesPerPeriod)
    {
        _pow = pow;
        _signalSampleStep = sampleStep;
        _samplesPushed = 0;
        _samplesPerPeriod = samplesPerPeriod;

        _signal.clear();
        _signal.resize(std::size_t(maxPeriod*_pow*2/_signalSampleStep+0.5) + 10);

        _dirty = true;

        _firs.resize(std::size_t(_samplesPerPeriod*_pow + 0.5));

        for(std::size_t firIdx(0); firIdx<_firs.size(); ++firIdx)
        {
            std::size_t firLen = (firIdx*2)+3;

            real bndT = real(firLen-1)/(_pow/2)/2;

            lowPassFir(bndT, firLen, _firs[firIdx]);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::setup(real pow, const PeriodGrid &periodGrid, real sampleStep, std::size_t samplesPerPeriod)
    {
        setup(pow, periodGrid.grid().back(), sampleStep, samplesPerPeriod);

        _levels.resize(periodGrid.grid().size());

        for(std::size_t i(0); i<_levels.size(); ++i)
        {
            LevelPtr &l = _levels[i];

            l.reset(new SignalConvolverLevel(pow, periodGrid.grid()[i], _signalSampleStep, _samplesPerPeriod));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::pushSignal(const real *samples, std::size_t amount)
    {
        for(std::size_t i(0); i<amount; ++i)
        {
            _signal.push_front(samples[i]);
        }
        _samplesPushed += amount;
        _dirty = true;
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    real SignalConvolver::getTime() const
    {
        return _signalSampleStep * _samplesPushed;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    TVComplex /*echo*/ SignalConvolver::convolve()
    {
        /*
        что дальше

            обрезка по частоте, попробовать через сигмоид переключать сигнал-результат фильтра
        */

        prepareValues();

        TVComplex res(_levels.size());

        for(std::size_t i(0); i<_levels.size(); ++i)
        {
            res[i] = _levels[i]->convolve();
        }

        return res;
    }

    complex /*echo*/ SignalConvolver::convolve(real period)
    {
        const real *signal = _signal.linearize();
        std::size_t signalSize = _signal.size();

        SignalConvolverLevel level(_pow, period, _signalSampleStep, _samplesPerPeriod);
        level.update(signal, signalSize);
        level.filtrate(_firs);

        return level.convolve();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::prepareValues()
    {
        if(!_dirty)
        {
            return;
        }

        _dirty = false;

        const real *signal = _signal.linearize();
        std::size_t signalSize = _signal.size();

        for(std::size_t i(0); i<_levels.size(); ++i)
        {
            _levels[i]->update(signal, signalSize);
            _levels[i]->filtrate(_firs);
        }
    }
}

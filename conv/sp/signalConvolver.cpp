#include "signalConvolver.hpp"
#include "signalConvolverLevel.hpp"
#include <cassert>
#include <boost/math/special_functions.hpp>
#include "sp/math.hpp"

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

            real sum = std::accumulate(A.begin(), A.end(), 0.0L);
            for(real &v : A)
            {
                v /= sum;
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::setup(real pow, const PeriodGrid &periodGrid, real sampleStep, std::size_t samplesPerPeriod)
    {
        _sampleStep = sampleStep;
        _samplesPushed = 0;
        _signal.clear();
        _signal.resize(std::size_t(periodGrid.grid().back()*pow*2/_sampleStep+0.5) + 10);
        _dirty = true;

        _levels.resize(periodGrid.grid().size());

        for(std::size_t i(0); i<_levels.size(); ++i)
        {
            LevelPtr &l = _levels[i];

            l.reset(new SignalConvolverLevel(pow, periodGrid.grid()[i], sampleStep, samplesPerPeriod));
        }

        _firs.resize(std::size_t(samplesPerPeriod*pow + 0.5));

        for(std::size_t firIdx(0); firIdx<_firs.size(); ++firIdx)
        {
            std::size_t firLen = (firIdx*2)+3;

            real bndT = real(firLen-1)/(pow/2)/2;

            lowPassFir(bndT, firLen, _firs[firIdx]);
        }
        //exit(0);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::pushSignal(const real *samples, std::size_t amount)
    {
        for(std::size_t i(amount-1); i<amount; --i)
        {
            _signal.push_front(samples[i]);
        }
        _samplesPushed += amount;
        _dirty = true;
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    real SignalConvolver::getTime() const
    {
        return _sampleStep * _samplesPushed;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    TVComplex /*echo*/ SignalConvolver::convolve()
    {
        /*
        что дальше

            обрезка по частоте, попробовать через сигмоид переключать сигнал-результат фильтра
            свертка
        */

        prepareValues();

        TVComplex res(_levels.size());

        for(std::size_t i(0); i<_levels.size(); ++i)
        {
            res[i] = _levels[i]->convolve();
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::prepareValues()
    {
        if(!_dirty)
        {
            return;
        }

        _dirty = false;

        _signal.linearize();

        const real *first = _signal.array_one().first;
        std::size_t firstSize = _signal.array_one().second;

        for(std::size_t i(0); i<_levels.size(); ++i)
        {
            _levels[i]->update(first, firstSize);
            _levels[i]->filtrate(_firs);
        }
    }
}
